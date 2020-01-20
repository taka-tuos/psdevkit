#include <stdio.h>
#include <stdlib.h>

unsigned char	data[128*1024];

int main(int argc, char *argv[])
{
	FILE *f;
	int	size, osize;

	printf("MemoryCard Block Padder & Adjust - by Orion_ [2013]\n\n");

	if (argc > 1)
	{
		f = fopen(argv[1], "rb");
		if (f)
		{
			memset(data, 0, sizeof(data));
			osize = size = fread(data, 1, sizeof(data), f);

			if ((data[0] == 'S') && (data[1] == 'C'))
			{
				fclose(f);

				size = (size + 8191) >> 13;	// One Block = 8k
				data[3] = size;	// Adjust Block Number
				size <<= 13;	// Pad to 8k

				f = fopen(argv[1], "wb");
				if (f)
				{
					fwrite(data, 1, size, f);
					fclose(f);
					printf("Free Space: %d\n", size - osize);
					printf("%d Block(s)\n", data[3]);
				}
				else
					printf("Cannot create output file\n");
			}
			else
				printf("Bad MemoryCard file format\n");
		}
		else
			printf("Cannot load input file\n");
	}
	else
		printf("Usage: mcpad mcfile.bin\n\n");

	return (0);
}
