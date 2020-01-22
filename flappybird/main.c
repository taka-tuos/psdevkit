/* PocketStation Lib Test Example by Orion_ [08/2013] */

#define	CLOCK_SPEED	CLOCK_8M

#include "pocketlib.h"

/****************************************/

u8 __xor_seed;

u8 xorshift(void) {
	__xor_seed ^= __xor_seed >> 3;
	__xor_seed ^= __xor_seed << 5;
	__xor_seed ^= __xor_seed >> 4;
	return __xor_seed;
}

u8 h[2];
s8 scrl;
u8 birdy;

void DrawSticks()
{
	for(u8 i=0;i<2;i++) {
		for(u8 j=0;j<h[i];j++) {
			DrawLCDPix(i*18+scrl,j);
			DrawLCDPix(i*18+1+scrl,j);
		}
		
		for(u8 j=h[i]+12;j<32;j++) {
			DrawLCDPix(i*18+scrl,j);
			DrawLCDPix(i*18+1+scrl,j);
		}
	}
}

void DrawBird()
{
	DrawLCDPix(8,birdy);
	DrawLCDPix(9,birdy);
	DrawLCDPix(8,birdy+1);
	DrawLCDPix(9,birdy+1);
}

void main(void)
{
	PocketInit();
	
	__xor_seed = 3;
	
restart:
	birdy = 16;
	scrl = 24;
	
	u8 exitf = 0;
	
	s16 birdy4 = birdy << 4;
	
	s8 birdspd = 0;

	for(u8 i=0;i<2;i++) {
		h[i] = xorshift() % (32-2-12)+1;
	}
	
	u32 sctim = 0;

	for(u8 i=0;i<3;i++) {
		FastLCDClear();
		DrawSticks();
		DrawBird();
		DrawChar(0,0,'3'-i);
		VSync(1);
		for(u8 j=1;j<60;j++) {
			VSync(0);
			if(PadOnRelease & PAD_BUTTON) {
				exitf = 1;
				break;
			}
		}
		if(exitf) break;
	}

	u8 press = 0;
	u8 pth = 0, ptl = 0;

	while (!exitf) {
		if(PadOnPress & PAD_BUTTON) {
			if(press == 0) {
				birdspd = -16;
			}
			press = 1;
		} else {
			press = 0;
		}
		
		birdy4 = birdy4 + birdspd;
		
		if(birdy4 < 0) birdy4 = 0;
		
		if((birdy4 >> 4) >= 30) {
			goto restart;
		}
		
		birdy = birdy4 >> 4;
		birdspd += 1;
		for(u8 i=0;i<2;i++) {
			u8 refsc = i*18;
			if((scrl == 8+refsc || scrl == 9+refsc) && (birdy <= h[i] || birdy+1 >= h[i]+12)) {
				goto restart;
			}
		}
		
		FastLCDClear();
		DrawChar(0,0,'0'+pth);
		DrawChar(4,0,'0'+ptl);
		DrawSticks();
		DrawBird();
		VSync(1);
		VSync(0);
		sctim++;
		if(sctim == 3) {
			scrl--;
			sctim = 0;
			if(scrl == 8 || scrl == 18+8) {
				ptl++;
				if(ptl == 10) {
					pth++;
					ptl = 0;
					if(pth == 10) pth  -0;
				}
			}
			if(scrl == -2) {
				scrl = 16;
				for(u8 i=0;i<1;i++) {
					h[i] = h[i+1];
				}
				h[1] = xorshift() % (32-2-12)+1;
			}
		}
	}

	PocketExit();
}
