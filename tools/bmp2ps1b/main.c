#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  FILE *in, *out;
  unsigned short x, y;
  unsigned int a, b;
  char n[255], *p;
  int i, j, k, s;

  printf("2 Colors BMP - to - PocketStation Binary data -- by Orion_ [2007-2013]\n\n");
  s = 0;

  if (argc > 2)
  {
   if ((argv[1][0] == 's') || (argv[1][0] == 'h') || (argv[1][0] == 'b'))
   {
    in = fopen(argv[2], "rb");
	if (in)
	{
	  strcpy(n, argv[2]);
	  p = strstr(n, ".bmp");
	  if (p)
	  {
		if (argv[1][0] == 'b')
		{
			strcpy(p + 1, "bin");
			out = fopen(n, "wb");
		}
		else
		{
			strcpy(p + 1, argv[1]);
			out = fopen(n, "w");
		}
		if (out)
		{
		 fseek(in,18,SEEK_SET);
		 fread(&x,2,1,in);
		 x /= 32;
		 if (x >= 1)
		 {
		  fseek(in,22,SEEK_SET);
		  fread(&y,2,1,in);
		  *(p - 2) = '\0';
		  if (argv[1][0] == 'h')
			fprintf(out, "const u32\t%s_gfx[] = {\n", n);
		  for (k = 0; k < y; k++)
		  {
		    fseek(in,-((k*x*4)+x*4),SEEK_END);
			if (argv[1][0] == 's')
				fprintf(out, "\t.word\t");
		    for (i = 0; i < x; i++)
		    {
			  fread(&a,4,1,in);
			  b = 0;
			  for (j = 0; j < 32; j++)
			    b |= (((a >> (31-j)) & 1) << j);
			  b ^= 0xFFFFFFFF;
			  b = (b >> 24) | (((b >> 16) & 0xFF) << 8) | (((b >> 8) & 0xFF) << 16) | ((b & 0xFF) << 24);
			  if (argv[1][0] == 'b')
				fwrite(&b, 1, 4, out);
			  else
				fprintf(out, "0x%x", b);
			  if ((argv[1][0] == 'h') || ((argv[1][0] == 's') && (i != x - 1)))
				fprintf(out, ",");
		    }
		    if (argv[1][0] != 'b')
				fprintf(out, "\n");
		  }
		  if (argv[1][0] == 'h')
			fprintf(out, "};\n");
		  s = 1;
		 }
		 else
		   printf("Width must be at least 32\n");
		}
		else
		  printf("Cannot create output file\n");
	  }
	  else
	    printf("Bad filename\n");
	}
	else
	  printf("Cannot load input file\n");
   }
   else
     printf("First argument must be 'h', 's', or 'b' (H = C Header File, S = ASM File, B = Binary File)\n");
  }

  if (s)
	printf("done...\n");
  else
  {
	printf("\nUsage: bmp2ps1b [h|s|b] file.bmp\n\n");
  	printf("First argument must be 'h', 's', or 'b' (H = C Header File, S = ASM File, B = Binary File)\n");
  	printf("BMP MUST BE 1BITS !! (Black and White 2 colors palette)\n\n");
  }

  return 0;
}
