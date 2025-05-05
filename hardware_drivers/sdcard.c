#include "sd_card.h"
#include "ff.h"
#include "sdcard.h"
#include "data_structures/string_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

sdcard *sdcard_init()
{
	if (!sd_init_driver())
	{
		printf("ERROR: Could not initialize SD card\r\n");
	}
	sdcard *sd = (sdcard *)malloc(sizeof(sdcard));
	sd->is_working = true;
	return sd;
}

bool sdcard_mount(sdcard *sd)
{
	sd->fr = f_mount(&sd->fs, "0:", 1);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not mount filesystem (%d)\r\n", sd->fr);
		sd->is_working = false;
		return false;
	}
	return true;
}

void sdcard_unmount(sdcard *sd)
{
	f_unmount("0:");
}

bool sdcard_write_file(sdcard *sd, char *filename, char *content, char mode)
{
	if (mode == 'w')
		sd->fr = f_open(&sd->fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
	else if (mode == 'a')
		sd->fr = f_open(&sd->fil, filename, FA_WRITE | FA_OPEN_APPEND);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not open file (%d)\r\n", sd->fr);
		return false;
	}
	sd->ret = f_printf(&sd->fil, content);
	if (sd->ret < 0)
	{
		printf("ERROR: Could not write to file (%d)\r\n", sd->ret);
		f_close(&sd->fil);
		return false;
	}
	sd->fr = f_close(&sd->fil);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not close file (%d)\r\n", sd->fr);
		return false;
	}
	return true;
}

str_list *sdcard_read_file(sdcard *sd, char *filename)
{
	str_list *lines = list();
	sd->fr = f_open(&sd->fil, filename, FA_READ);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not open file (%d)\r\n", sd->fr);
		return lines;
	}
	while (f_gets(sd->buf, sizeof(sd->buf), &sd->fil))
	{
		char *line = (char *)malloc(strlen(sd->buf) + 1);
		strcpy(line, sd->buf);
		lstappend(lines, line);
		free(line);
	}
	sd->fr = f_close(&sd->fil);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not close file (%d)\r\n", sd->fr);
		return lines;
	}
	return lines;
}

str_list *sdcard_list_files(sdcard *sd)
{
	str_list *files = list();
	DIR dir;
	FILINFO fno;
	sd->fr = f_opendir(&dir, "/");
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not open directory (%d)\r\n", sd->fr);
		return files;
	}
	for (;;)
	{
		sd->fr = f_readdir(&dir, &fno);
		if (sd->fr != FR_OK || fno.fname[0] == 0)
			break;
		char *file = (char *)malloc(strlen(fno.fname) + 1);
		strcpy(file, fno.fname);
		lstappend(files, file);
		free(file);
	}
	sd->fr = f_closedir(&dir);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not close directory (%d)\r\n", sd->fr);
		return files;
	}
	return files;
}

/**
 * @brief Write a key-value pair to a file in the format "key~value\n"
 *
 * @param sd The sdcard instance
 * @param filename The name of the file to write to
 * @param mode The mode to open the file in ('w' for write, 'a' for append)
 * @param key The key to write
 * @param value The value to write
 * @return true if the write was successful, false otherwise
 *
 * @note it is assumed that the key and value do not contain the separator character '~'
 */
bool sdcard_write_key_value_to_file(sdcard *sd, char *filename, char mode, char *key, char *value)
{
	char sep = '~';
	size_t total_len = strlen(key) + strlen(value) + 3;
	char *content = (char *)malloc(total_len);
	snprintf(content, total_len, "%s%c%s\n", key, sep, value);
	bool res = sdcard_write_file(sd, filename, content, mode);
	free(content);
	return res;
}

/**
 * @brief Read a value from a file given a key in the format "key~value\n"
 *
 * @param sd The sdcard instance
 * @param filename The name of the file to read from
 * @param key The key to search for
 * @return char* The value associated with the key, or NULL if not found
 *
 * @note it is assumed that the key and value do not contain the separator character '~'
 */
char *sdcard_read_value_from_file(sdcard *sd, char *filename, char *key)
{
	char sep = '~';
	str_list *lines = sdcard_read_file(sd, filename);
	for (uint16_t i = 0; i < lines->len; i++)
	{
		char *line = lstget(lines, i);
		char *delimiter = strchr(line, sep);
		*delimiter = '\0';
		if (strcmp(line, key) == 0)
		{
			char *value = (char *)malloc(strlen(delimiter + 1) + 1);
			strcpy(value, delimiter + 1);
			lstdel(lines);
			return value;
		}
	}
	lstdel(lines);
	return NULL;
}
