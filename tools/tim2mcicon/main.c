#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *in, *out;
	unsigned int a, b;
	int i;

	printf("16 Color TIM - to - MemoryCard Raw Icon -- by Orion_ [2013]\n\n");

	if (argc > 2)
	{
		in = fopen(argv[1], "rb");
		if (in)
		{
			fread(&a, 4, 1, in);	// TIM
			fread(&b, 4, 1, in);	// 4 Bits CLUT

			if ((a == 0x10) && (b == 0x08))
			{
				out = fopen(argv[2], "wb");
				if (out)
				{
					fseek(in, 3*4, SEEK_CUR);	// Skip header
					// CLUT
					for (i = 0; i < 16/2; i++)
					{
						fread(&a, 4, 1, in);
						fwrite(&a, 4, 1, out);
					}
					fseek(in, 3*4, SEEK_CUR);	// Skip header
					// 16x16 Pixels
					for (i = 0; i < 128/4; i++)
					{
						fread(&a, 4, 1, in);
						fwrite(&a, 4, 1, out);
					}
					fclose(in);
					fclose(out);
				}
				else
					printf("Cannot create output file\n");
			}
			else
				printf("Bad TIM file format, must be 4 bits CLUT\n");
		}
		else
			printf("Cannot load input file\n");
	}
	else
		printf("Usage: tim2mcicon file.tim file.bin\n\n");

	return (0);
}
