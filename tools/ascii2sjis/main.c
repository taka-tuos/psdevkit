#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char ShiftJISpunc[] = ",.:;?!^_-/\\~|()[]{}+-=<>'\"$£%#&*@";
unsigned char ShiftJISpuncCode[] = {0x43,0x44,0x46,0x47,0x48,0x49,0x4F,0x51,0x5D,0x5E,0x5F,0x60,0x62,0x69,0x6A,0x6D,0x6E,0x6F,0x70,0x7B,0x7C,0x81,0x83,0x84,0x8C,0x8D,0x90,0x92,0x93,0x94,0x95,0x96,0x97};

void	ASCII2ShiftJIS(char *src, unsigned char *dst)
{
	while (*src)
	{
		if (*src == ' ')
		{
			*dst++ = 0x81;
			*dst++ = 0x40;
		}
		else if ((*src >= '0') && (*src <= '9'))
		{
			*dst++ = 0x82;
			*dst++ = 0x4F + ((*src) - '0');
		}
		else if ((*src >= 'A') && (*src <= 'Z'))
		{
			*dst++ = 0x82;
			*dst++ = 0x60 + ((*src) - 'A');
		}
		else if ((*src >= 'a') && (*src <= 'z'))
		{
			*dst++ = 0x82;
			*dst++ = 0x81 + ((*src) - 'a');
		}
		else
		{
			int	i;

			for (i = 0; i < sizeof(ShiftJISpuncCode); i++)
				if (*src == ShiftJISpunc[i])
				{
					*dst++ = 0x81;
					*dst++ = ShiftJISpuncCode[i];
					break;
				}
		}
		src++;
	}
}

unsigned char	buffer[64];

int	main(int argc, char *argv[])
{
	FILE	*f;
	int		size;

	printf("ASCII2ShiftJIS for PocketStation Lib - by Orion_ [2013]\n\n");

	if (argc > 2)
	{
		size = strlen(argv[1]);
		if (size > 32)
		{
			printf("Title too long (must be <= 32)\n");
			return (-1);
		}
		memset(buffer, 0, sizeof(buffer));
		ASCII2ShiftJIS(argv[1], buffer);

		f = fopen(argv[2], "wb");
		if (!f)
		{
			printf("Cannot create file %s\n", argv[2]);
			return (-1);
		}

		// Title
		fprintf(f, "\t/* %s */\r\n", argv[1]);	// Just to keep a record of the Original Title :)
		fprintf(f, "\t.ascii\t\"");
		fwrite(buffer, 1, size*2, f);
		fprintf(f, "\"\r\n");

		// Padding
		if (size < 32)
		{
			fprintf(f, "\t.hword\t");
			size = 32 - size;
			while (size)
			{
				fprintf(f, "0");
				size--;
				if (size)
					fprintf(f, ",");
			}
			fprintf(f, "\r\n");
		}

		fclose(f);
	}
	else
	{
		printf("Usage: ascii2sjis ASCII_Title file.sjis\n\n");
		printf("Ex: ascii2sjis \"Test Program\" title.sjis\n\n");
	}

	return (0);
}
