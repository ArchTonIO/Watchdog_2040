#include "sd_card.h"
#include "ff.h"
#include "sdcard.h"
#include "data_structures/string_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

sdcard *sdcard_init()
{
	if (!sd_init_driver())
	{
		printf("ERROR: Could not initialize SD card\r\n");
	}
	sdcard *sd = (sdcard *)malloc(sizeof(sdcard));
	return sd;
}

uint8_t sdcard_mount(sdcard *sd)
{
	sd->fr = f_mount(&sd->fs, "0:", 1);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not mount filesystem (%d)\r\n", sd->fr);
		return 0;
	}
	return 1;
}

void sdcard_unmount(sdcard *sd)
{
	f_unmount("0:");
}

void sdcard_write_file(sdcard *sd, char *filename, char *content, char mode)
{
	if (mode == 'w')
		sd->fr = f_open(&sd->fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
	else if (mode == 'a')
		sd->fr = f_open(&sd->fil, filename, FA_WRITE | FA_OPEN_APPEND);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not open file (%d)\r\n", sd->fr);
		return;
	}
	sd->ret = f_printf(&sd->fil, content);
	if (sd->ret < 0)
	{
		printf("ERROR: Could not write to file (%d)\r\n", sd->ret);
		f_close(&sd->fil);
		return;
	}
	sd->fr = f_close(&sd->fil);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not close file (%d)\r\n", sd->fr);
		return;
	}
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
	}
	sd->fr = f_closedir(&dir);
	if (sd->fr != FR_OK)
	{
		printf("ERROR: Could not close directory (%d)\r\n", sd->fr);
		return files;
	}
	return files;
}
