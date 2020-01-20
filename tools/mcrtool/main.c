#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "gme.h"
#include "mcr.h"

uint8_t flash[128*1024];

int s_mode = 0; // 0:MCR 1:GME
int s_blank = 1;
char s_mcs[256];
char s_mcr[256];
char s_mcq[256];

int main(int argc, char *argv[])
{
	if(argc < 3) {
		puts("usage: mcrtool [-GR] mcs=filename [if=filename] of=filename");
		
		puts("-G           : Output as DexFile GME format");
		puts("-R           : Output as PCSX MCR format(default)");
		
		puts("mcs=filename : (must be specified) Input MCS filename");
		puts("if=filename  : Input MCR/GME filename");
		puts("of=filename  : (must be specified) Output MCR/GME filename");
		
		return 1;
	}
	
	int spec_mcs = 0;
	int spec_mcr = 0;
	
	for(int i=1;i<argc;i++) {
		char *p = argv[i];
		
		if(*p == '-') {
			if(p[1] == 'G') s_mode = 1;
			else if(p[1] == 'R') s_mode = 0;
			else {
				printf("Unknown option \"%c\".",p[1]);
				return 2;
			}
		} else {
			if(!strncmp(p,"mcs=",4)) {
				strncpy(s_mcs,p+4,256);
				spec_mcs = 1;
			} else if(!strncmp(p,"of=",3)) {
				strncpy(s_mcr,p+3,256);
				spec_mcr = 1;
			} else if(!strncmp(p,"if=",3)) {
				strncpy(s_mcq,p+3,256);
				s_blank = 0;
			} else {
				printf("Unknown option \"%s\".\n",p);
				return 2;
			}
		}
	}
	
	if(!spec_mcs) {
		puts("mcs=filename must be specified");
		return 3;
	}
	
	if(!spec_mcr) {
		puts("of=filename must be specified");
		return 4;
	}
	
	if(s_blank) {
		memcpy(flash, mcr_superblock, 0x2000);
	} else {
		FILE *fp = fopen(s_mcq,"rb");
		if(!fp) {
			printf("Unabled to open \"%s\".",s_mcq);
			return 5;
		}
		
		uint8_t head[0xf40];
		
		fread(head,0xf40,1,fp);
		
		if(!memcmp(head,"MC",2)) {
			memcpy(flash,head,0xf40);
			fread(flash+0xf40,128*1024-0xf40,1,fp);
		} else {
			fread(flash,128*1024,1,fp);
		}
		
		fclose(fp);
	}
	
	FILE *mcs = fopen(s_mcs,"rb");
	
	if(!mcs) {
		printf("Unabled to open \"%s\".\n",s_mcs);
		return 6;
	}
	
	
	int slot = 0;
	int ofs_flash = 0x2000;
	
	if(!s_blank) {
		for(int i=0;i<15;i++) {
			if(flash[0x80+i*0x80] == 0xa0) {
				slot = i;
				break;
			} else {
				uint32_t b[4],n;
				b[0] = flash[0x80+i*0x80+4];
				b[1] = flash[0x80+i*0x80+5];
				b[2] = flash[0x80+i*0x80+6];
				b[3] = flash[0x80+i*0x80+7];
				
				n = b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
				
				ofs_flash += n;
				printf("Skipped Slot %d, 0x%04x bytes\n", i, n);
			}
		}
	}
	
	uint8_t mcs_head[128];
	
	fread(mcs_head,128,1,mcs);
	
	uint32_t len = mcs_head[4] | (mcs_head[5] << 8)  | (mcs_head[6] << 16)  | (mcs_head[7] << 24);
	
	printf("binary size : %d\n",len);
	
	memcpy(flash+0x80+slot*0x80,mcs_head,128);
	
	fread(flash+ofs_flash,len,1,mcs);
	
	fclose(mcs);
	
	int block = (len + 8191) / 8192;
	
	for(int i=1;i<block;i++) {
		uint8_t *p = flash+0x80+(slot+i)*0x80;
		
		p[0] = 0x52;
		
		p[127] = 0;
		
		p[8] = i+1;
		p[9] = 0;
		
		if(i == block - 1) {
			p[0] = 0x53;
			p[8] = 0xff;
			p[9] = 0xff;
		}
		
		for(int j=0;j<127;j++) p[127] ^= p[j];
	}
	
	FILE *mcr = fopen(s_mcr,"wb");
		
	if(!mcr) {
		printf("Unabled to write \"%s\".\n",s_mcr);
	}
	
	if(!s_mode) {
		fwrite(flash,128*1024,1,mcr);
		
		fclose(mcr);
	} else {
		fwrite(gme_header,0xf40,1,mcr);
		fwrite(flash,128*1024,1,mcr);
		
		fclose(mcr);
	}
}
