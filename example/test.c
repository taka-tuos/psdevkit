/* PocketStation Lib Test Example by Orion_ [08/2013] */

#define	CLOCK_SPEED	CLOCK_8M
#define	USE_SOUND
#define	SOUND_FREQ	6000	// Up to 8000Hz, Use CLOCK_8M for > 4kHz !

#include "pocketlib.h"

/****************************************/

extern	unsigned char	SoundLoop[];
extern	unsigned char	SoundLoopE[];

void	main(void)
{
	int	speed, y;

	PocketInit();

	PlaySound(SoundLoop, SoundLoopE - SoundLoop, 1);

	y = 4;
	speed = 1;

	do
	{
		y += speed;
		if (y == 10)
			speed = -1;
		if (y == 4)
			speed = 1;

		FastLCDClear();
		DrawString(0, y, "Hello");
		DrawString(5, (32-6)-y, "World !");

		VSync(1);	// update LCD screen
		VSync(0);	// Wait a bit more to slow down animation
	} while (!(PadOnRelease & PAD_BUTTON));

	PocketExit();
}
