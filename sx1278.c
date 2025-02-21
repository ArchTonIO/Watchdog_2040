#include "sx1278.h"
#include "hardware_config.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include <string.h>
#include <stdlib.h>

sx1278 *instance = NULL;

void spi_write_reg_single_byte(sx1278 *radio, uint8_t reg, uint8_t payload);
void spi_write_reg_multi_byte(sx1278 *radio, uint8_t reg, uint8_t *payload, size_t length);
uint8_t spi_read_reg_single_byte(sx1278 *radio, uint8_t reg);
void spi_read_reg_multi_byte(sx1278 *radio, uint8_t reg, uint8_t *buffer, size_t length);
bool wait_packet_sent(sx1278 *radio);
void irq_handler(uint gpio, uint32_t events);
void message_sent_callback();
void message_received_callback();

sx1278 *sx1278_init(
	pin mosi,
	pin miso,
	pin sck,
	pin cs,
	pin interrupt,
	uint8_t addr,
	spi_inst_t *spi_port,
	uint32_t baudrate,
	uint8_t tx_power)
{
	sx1278 *new_radio = (sx1278 *)malloc(sizeof(sx1278));
	new_radio->mosi = mosi;
	new_radio->miso = miso;
	new_radio->sck = sck;
	new_radio->cs = cs;
	new_radio->interrupt = interrupt;
	new_radio->addr = addr;
	new_radio->spi_port = spi_port;
	new_radio->baudrate = baudrate;
	new_radio->tx_power = tx_power;
	new_radio->mode = NULL;
	new_radio->irq_flags = NULL;
	new_radio->packet_sent_timeout_ms = 200;

	// interrupt set up
	gpio_init(interrupt);
	gpio_set_dir(interrupt, GPIO_IN);
	gpio_set_irq_enabled_with_callback(interrupt, GPIO_IRQ_EDGE_RISE, true, irq_handler);

	// spi setup
	spi_init(spi_port, baudrate);
	gpio_set_function(sck, GPIO_FUNC_SPI);
	gpio_set_function(mosi, GPIO_FUNC_SPI);
	gpio_set_function(miso, GPIO_FUNC_SPI);

	// cs setup
	gpio_init(cs);
	gpio_set_dir(cs, GPIO_OUT);
	gpio_put(cs, 1);

	// set mode
	spi_write_reg_single_byte(new_radio, REG_01_OP_MODE, MODE_SLEEP | LONG_RANGE_MODE);
	printf("1 - REG_01_OP_MODE: %d\n", spi_read_reg_single_byte(new_radio, REG_01_OP_MODE)); //! DEBUG SHIT
	sleep_ms(10);

	// check initialisation
	uint8_t reg_01_op_mode_content = spi_read_reg_single_byte(new_radio, REG_01_OP_MODE);
	if (reg_01_op_mode_content != (MODE_SLEEP | LONG_RANGE_MODE))
	{
		printf(
			"SX1278 initialization error, reading on REG_01_OP_MODE resulted in: %d, expecting: %d\n",
			reg_01_op_mode_content,
			MODE_SLEEP | LONG_RANGE_MODE);
		return new_radio;
	}

	spi_write_reg_single_byte(new_radio, REG_0E_FIFO_TX_BASE_ADDR, 0x00);
	printf("2 - REG_0E_FIFO_TX_BASE_ADDR: %d\n", spi_read_reg_single_byte(new_radio, REG_0E_FIFO_TX_BASE_ADDR)); //! DEBUG SHIT
	spi_write_reg_single_byte(new_radio, REG_0F_FIFO_RX_BASE_ADDR, 0x00);
	printf("3 - REG_0F_FIFO_RX_BASE_ADDR: %d\n", spi_read_reg_single_byte(new_radio, REG_0F_FIFO_RX_BASE_ADDR)); //! DEBUG SHIT
	sx1278_set_mode_idle(new_radio);

	// set modem config
	spi_write_reg_single_byte(new_radio, REG_1D_MODEM_CONFIG1, 0x72);
	printf("5 - REG_1D_MODEM_CONFIG1: %d\n", spi_read_reg_single_byte(new_radio, REG_1D_MODEM_CONFIG1)); //! DEBUG SHIT
	spi_write_reg_single_byte(new_radio, REG_1E_MODEM_CONFIG2, 0x74);
	printf("6 - REG_1E_MODEM_CONFIG2: %d\n", spi_read_reg_single_byte(new_radio, REG_1E_MODEM_CONFIG2)); //! DEBUG SHIT
	spi_write_reg_single_byte(new_radio, REG_26_MODEM_CONFIG3, 0x04);
	printf("7 - REG_26_MODEM_CONFIG3: %d\n", spi_read_reg_single_byte(new_radio, REG_26_MODEM_CONFIG3)); //! DEBUG SHIT

	// set preamble length
	spi_write_reg_single_byte(new_radio, REG_20_PREAMBLE_MSB, 0x00);
	printf("8 - REG_20_PREAMBLE_MSB: %d\n", spi_read_reg_single_byte(new_radio, REG_20_PREAMBLE_MSB)); //! DEBUG SHIT
	spi_write_reg_single_byte(new_radio, REG_21_PREAMBLE_LSB, 0x08);
	printf("9 - REG_21_PREAMBLE_LSB: %d\n", spi_read_reg_single_byte(new_radio, REG_21_PREAMBLE_LSB)); //! DEBUG SHIT

	// set frequency
	spi_write_reg_single_byte(new_radio, REG_06_FRF_MSB, 0x6c);
	printf("10 - REG_06_FRF_MSB: %d\n", spi_read_reg_single_byte(new_radio, REG_06_FRF_MSB)); //! DEBUG SHIT
	spi_write_reg_single_byte(new_radio, REG_07_FRF_MID, 0x40);
	printf("11 - REG_07_FRF_MID: %d\n", spi_read_reg_single_byte(new_radio, REG_07_FRF_MID)); //! DEBUG SHIT
	spi_write_reg_single_byte(new_radio, REG_08_FRF_LSB, 0x00);
	printf("12 - REG_08_FRF_LSB: %d\n", spi_read_reg_single_byte(new_radio, REG_08_FRF_LSB)); //! DEBUG SHIT

	// set tx power
	if (tx_power < 5)
		new_radio->tx_power = 5;
	if (tx_power > 23)
		new_radio->tx_power = 23;
	if (tx_power < 20)
	{
		spi_write_reg_single_byte(new_radio, REG_4D_PA_DAC, PA_DAC_ENABLE);
		printf("13 - REG_4D_PA_DAC: %d\n", spi_read_reg_single_byte(new_radio, REG_4D_PA_DAC)); //! DEBUG SHIT
		new_radio->tx_power -= 3;
	}
	else
	{
		spi_write_reg_single_byte(new_radio, REG_4D_PA_DAC, PA_DAC_DISABLE);
		printf("14 - REG_4D_PA_DAC: %d\n", spi_read_reg_single_byte(new_radio, REG_4D_PA_DAC)); //! DEBUG SHIT
	}
	spi_write_reg_single_byte(new_radio, REG_09_PA_CONFIG, PA_SELECT | (new_radio->tx_power - 5));
	printf("15 - REG_09_PA_CONFIG: %d\n", spi_read_reg_single_byte(new_radio, REG_09_PA_CONFIG)); //! DEBUG SHIT
	instance = new_radio;
	return new_radio;
}

void sx1278_sleep(sx1278 *radio)
{
	if (radio->mode != MODE_SLEEP)
	{
		spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_SLEEP);
		radio->mode = MODE_SLEEP;
	}
}

void sx1278_set_mode_tx(sx1278 *radio)
{
	if (radio->mode != MODE_TX)
	{
		spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_TX);
		spi_write_reg_single_byte(radio, REG_40_DIO_MAPPING1, 0x40);
		radio->mode = MODE_TX;
	}
}

void sx1278_set_mode_rx(sx1278 *radio)
{
	if (radio->mode != MODE_RXCONTINUOUS)
	{
		spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_RXCONTINUOUS);
		spi_write_reg_single_byte(radio, REG_40_DIO_MAPPING1, 0x00); // interrupt on rx done
		radio->mode = MODE_RXCONTINUOUS;
		printf("mode was set to rx");
	}
}

void sx1278_set_mode_idle(sx1278 *radio)
{
	if (radio->mode != MODE_STDBY)
	{
		spi_write_reg_single_byte(radio, REG_01_OP_MODE, MODE_STDBY);
		radio->mode = MODE_STDBY;
		printf("4 - REG_01_OP_MODE: %d\n", spi_read_reg_single_byte(radio, REG_01_OP_MODE)); //! DEBUG SHIT
	}
}

bool wait_packet_sent(sx1278 *radio)
{
	uint32_t start_ms = to_ms_since_boot(get_absolute_time());
	while ((to_ms_since_boot(get_absolute_time()) - start_ms) < radio->packet_sent_timeout_ms)
	{
		if (radio->mode != MODE_TX)
		{
			return true;
		}
	}
	return false;
}

void sx1278_send_str(sx1278 *radio, char *data)
{
	wait_packet_sent(radio);
	sx1278_set_mode_idle(radio);
	spi_write_reg_single_byte(radio, REG_0D_FIFO_ADDR_PTR, 0x00);
	spi_write_reg_multi_byte(radio, REG_00_FIFO, (uint8_t *)data, strlen(data));
	spi_write_reg_single_byte(radio, REG_22_PAYLOAD_LENGTH, strlen(data));
	sx1278_set_mode_tx(radio);
}

void spi_write_reg_single_byte(sx1278 *radio, uint8_t reg, uint8_t payload)
{
	uint8_t buffer[2] = {reg | 0x80, payload};
	printf("writing %d to register %d\n", payload, reg);
	gpio_put(radio->cs, 0);
	spi_write_blocking(radio->spi_port, buffer, 2);
	gpio_put(radio->cs, 1);
}

void spi_write_reg_multi_byte(sx1278 *radio, uint8_t reg, uint8_t *payload, size_t length)
{
	uint8_t buffer[length + 1];
	buffer[0] = reg | 0x80;
	memcpy(&buffer[1], payload, length);
	gpio_put(radio->cs, 0);
	spi_write_blocking(radio->spi_port, buffer, length + 1);
	gpio_put(radio->cs, 1);
}

uint8_t spi_read_reg_single_byte(sx1278 *radio, uint8_t reg)
{
	uint8_t tx_buffer = reg & 0x7F;
	uint8_t rx_buffer[2] = {0};
	gpio_put(radio->cs, 0);
	spi_write_read_blocking(radio->spi_port, &tx_buffer, rx_buffer, 2);
	gpio_put(radio->cs, 1);
	return rx_buffer[1];
}

void spi_read_reg_multi_byte(sx1278 *radio, uint8_t reg, uint8_t *buffer, size_t length)
{
	uint8_t tx_buffer = reg & 0x7F;
	uint8_t rx_buffer[length + 1];
	gpio_put(radio->cs, 0);
	spi_write_read_blocking(radio->spi_port, &tx_buffer, rx_buffer, length + 1);
	gpio_put(radio->cs, 1);
	memcpy(buffer, &rx_buffer[1], length);
}

void irq_handler(uint gpio, uint32_t event_mask)
{
	printf("@@@@@@@@@@@@@@@ IRQ! @@@@@@@@@@@@@@@\n");
	instance->irq_flags = spi_read_reg_single_byte(instance, REG_12_IRQ_FLAGS);
	spi_write_reg_single_byte(instance, REG_12_IRQ_FLAGS, instance->irq_flags);
	if (instance->mode == MODE_TX && instance->irq_flags & TX_DONE)
	{
		message_sent_callback();
	}
	else if (instance->mode == MODE_RXCONTINUOUS && instance->irq_flags & RX_DONE)
	{
		message_received_callback();
	}
}

void message_sent_callback()
{
	printf("TX done\n");
	sx1278_set_mode_idle(instance);
}

void message_received_callback()
{
	uint8_t length = spi_read_reg_single_byte(instance, REG_13_RX_NB_BYTES);
	spi_write_reg_single_byte(instance, REG_0D_FIFO_ADDR_PTR, spi_read_reg_single_byte(instance, REG_10_FIFO_RX_CURRENT_ADDR));
	uint8_t buffer[length];
	spi_read_reg_multi_byte(instance, REG_00_FIFO, buffer, length);
	printf("RX done, received: %s\n", buffer);
}