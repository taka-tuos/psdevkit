#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned char	data[128*1025];

int	main(int argc, char *argv[])
{
	FILE *f;
	int	size, i;

	printf("MemoryCard BIN TO MCS - by Orion_ [2013]\n\n");

	if (argc > 3)
	{
		f = fopen(argv[2], "rb");
		if (f)
		{
			memset(data, 0, sizeof(data));
			size = fread(&data[128], 1, sizeof(data)-128, f);

			if ((data[128] == 'S') && (data[129] == 'C'))
			{
				fclose(f);

				size = (size + 8191) >> 13;	// One Block = 8k
				data[128+3] = size;	// Adjust Block Number
				size <<= 13;	// Pad to 8k

				f = fopen(argv[3], "wb");
				if (f)
				{
					// Generate MCS header
					data[0] = 0x51;
					data[4] = (size >>  0) & 0xFF;
					data[5] = (size >>  8) & 0xFF;
					data[6] = (size >> 16) & 0xFF;
					data[7] = (size >> 24) & 0xFF;
					data[8] = 1;	// next block
					strncpy(&data[10], argv[1], 20);

					for (i = 0; i < 127; i++)
						data[127] ^= data[i];

					fwrite(data, 1, 128+size, f);
					fclose(f);
				}
				else
					printf("Cannot create output file\n");
			}
			else
				printf("Bad MemoryCard BIN file format\n");
		}
		else
			printf("Cannot load input file\n");
	}
	else
	{
		printf("Usage: bin2mcs FileID file.bin file.mcs\n\n");
		printf("FileID Description: BcSlCTxCCCCCFILENAME (20 char max)\n");
		printf("                    Bc -> BI = Japan, BA = America, BE = Europe\n");
		printf("                      Sl -> SC = Sony Computer, SL = Sony Licensed\n");
		printf("                        C -> P = Japan, U = America, E = Europe\n");
		printf("                         T -> S = Game, D = Demo, M = ?\n");
		printf("                          x -> - = Normal Save, P = PocketStation\n");
		printf("                           CCCCC -> Game Digit Code\n");
		printf("                                FILENAME -> This Save Specific Filename\n");
		printf("\nFileID Example:\n");
		printf("BISLPS-00000GAMENAME Sony Licensed Japan,   Game Code:00000 Filename: GAMENAME\n");
		printf("BASCUS-21042SAVEFILE Sony Computer America, Game Code:21042 Filename: SAVEFILE\n");
		printf("BESCES-88888FILENAME Sony Computer Europe,  Game Code:88888 Filename: FILENAME\n");
		printf("BESLESP00000MINIGAME Sony Licensed Europe,  PocketStation   Filename: MINIGAME\n\n");
		getchar();
	}

	return (0);
}
