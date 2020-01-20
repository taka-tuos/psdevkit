/****************************************/
/* PocketStation Lib - by Orion_ [2013] */
/****************************************/
// Based on Miko Hoshina & Martin Korth work.
// If you use it, please give credits !
// v3.6

// Optional Functions, don't define this in your main .c file do reduce size
//#define	USE_SPRITE
//#define	USE_RANDOM
//#define	USE_SOUND

#ifndef	NULL
#define	NULL	((void*)0)
#endif

#ifndef	CLOCK_SPEED
#define	CLOCK_SPEED	CLOCK_4M
#endif

#ifndef	FPS
#define	FPS			64
#endif

#ifndef	SOUND_FREQ
#define	SOUND_FREQ	4000	// If 8000Hz, then use CLOCK_8M !
#endif

/**/

#define PAD_BUTTON	0x1
#define PAD_RIGHT	0x2
#define PAD_LEFT	0x4
#define PAD_DOWN	0x8
#define PAD_UP		0x10

#define	PAD_STATUS	((IRQ_STATUS_RAW) & 0x1F)

int		PadOnRelease, PadOnPress;

/**/

#define	LCD_ON		0x40
#define	LCD_OFF		0x00
#define	LCD_64HZ	0x10
#define	LCD_32HZ	0x20
#define	LCD_16HZ	0x30
#define	LCD_CPEN	0x08

#define LCD_MODE				*((volatile unsigned int *)0xD000000)
#define LCD_VRAM(_line_)		*((volatile unsigned int *)(0xD000100+(_line_ << 2)))
#define LCD_VRAM_ADRS			(volatile unsigned int *)0xD000100

/**/

#define CLOCK_62_5K				0x1
#define CLOCK_125K				0x2
#define CLOCK_250K				0x3
#define CLOCK_500K				0x4
#define CLOCK_1M				0x5
#define CLOCK_2M				0x6
#define CLOCK_4M				0x7
#define CLOCK_8M				0x8

/**/

#define TIMER0_START()			*((volatile unsigned int *)(0xA800008)) |=  0x4
#define TIMER1_START()			*((volatile unsigned int *)(0xA800018)) |=  0x4
#define TIMER2_START()			*((volatile unsigned int *)(0xA800028)) |=  0x4
#define TIMER0_STOP()			*((volatile unsigned int *)(0xA800008)) &= ~0x4
#define TIMER1_STOP()			*((volatile unsigned int *)(0xA800018)) &= ~0x4
#define TIMER2_STOP()			*((volatile unsigned int *)(0xA800028)) &= ~0x4
#define TIMER0_SETCOUNT(_x_)	*((volatile unsigned int *)(0xA800000)) = (_x_)
#define TIMER1_SETCOUNT(_x_)	*((volatile unsigned int *)(0xA800010)) = (_x_)
#define TIMER2_SETCOUNT(_x_)	*((volatile unsigned int *)(0xA800020)) = (_x_)

/**/

#define FIQ_COMM			0x40
#define IRQ_TIMER0			0x80
#define IRQ_TIMER1			0x100
#define IRQ_RTC				0x200
#define IRQ_BATTERY			0x400
#define IRQ_PSCOMM			0x800
#define IRQ_INFRARED		0x1000
#define FIQ_TIMER2			0x2000

#define IRQ_STATUS			*((volatile unsigned int *)(0xA000000))
#define IRQ_STATUS_RAW		*((volatile unsigned int *)(0xA000004))
#define IRQ_ENABLE			*((volatile unsigned int *)(0xA000008))
#define IRQ_DISABLE			*((volatile unsigned int *)(0xA00000c))
#define IRQ_CLEAR			*((volatile unsigned int *)(0xA000010))

#define IRQ_DISABLE_ALL()	IRQ_DISABLE = 0x3FFF;

/**/

#define IOCTL_CONFIG			*((volatile unsigned int *)0xD800000)
#define IOCTL_OFF				*((volatile unsigned int *)0xD800004)
#define IOCTL_ON				*((volatile unsigned int *)0xD800008)
#define IOCTL_DAC				*((volatile unsigned int *)0xD800010)
#define IOCTL_DASOUND			*((volatile unsigned int *)0xD800014)
#define IOCTL_BATTERY			*((volatile unsigned int *)0xD800020)

#define IOCTL_LED				0x02
#define IOCTL_SPEAKER			0x20
#define IOCTL_IRDA				0x40

/**/

#define ROMFONT_LARGE		((unsigned int *)0x04003b6c)		// 8x8dot ROM Font (partial)

/**/

/****************************************/

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed int s32;
typedef signed short s16;
typedef signed char s8;


/****************************************/
// From header.S

// Bios Calls
void	*SetCallbacks(int index, void (*function));
int		SetCpuSpeed(int speed);
int		PrepareExecute(int flag, int dir_index, int param);
void	DoExecute(int snapshot_flag);
void	SetComOnOff(int flag);
int		GetDirIndex(void);

// IRQ/FIQ Asm
void	IntIRQ(void);
void	IntFIQ(void);

void	(*UserIRQ)(void);
int		(*UserFIQ)(void);	// Return 1 to Skip Library Sound Processing, else return 0



/****************************************/
// Utils

u8	ROMByteAccess(unsigned char *addr)
{
	u16 data = *(u16*)(((u32)addr) & ~1);

	if (((u32)addr) & 1)
		return (data >> 8);
	return (data & 0xFF);
}


/****************************************/
// IRQ/Timer0 Handler (for VSync)

volatile int	VCount;

void	IRQ_Handler(void)
{
	int	status;

	status = IRQ_STATUS;
	status &= IRQ_ENABLE;
	status &= ~(FIQ_TIMER2 | FIQ_COMM);	// Clear FIQ Bits

	if (status & IRQ_TIMER0)
		VCount++;

	if (UserIRQ)
		UserIRQ();

	IRQ_CLEAR = status;
}


/****************************************/
// Sound

#ifdef	USE_SOUND

unsigned char	*SndPtr, *SndAdrs;
int				SndSize, SndCnt;

void	SoundStop(void)
{
	IOCTL_OFF = IOCTL_SPEAKER;
	IOCTL_DAC = 0;
	TIMER2_STOP();
}

void	SoundStart(void)
{
	IOCTL_ON = IOCTL_SPEAKER;
	IOCTL_DAC = 1;

	TIMER2_SETCOUNT((15625 << CLOCK_SPEED) / SOUND_FREQ);
	TIMER2_START();
}

void	PlaySound(void *adrs, int size, int loop)
{
	SoundStop();

	SndAdrs = SndPtr = (unsigned char *)adrs;
	SndCnt = size;
	SndSize = loop ? size : 0;

	SoundStart();
}

void	PlaySoundChain(void *adrs1, int size1, void *adrs2, int size2)
{
	SoundStop();

	SndPtr = (unsigned char *)adrs1;
	SndAdrs = (unsigned char *)adrs2;
	SndCnt = size1;
	SndSize = size2;

	SoundStart();
}

#endif

/****************************************/
// FIQ/Timer2 Handler (for Sound)

void	FIQ_Handler(void)
{
	if (UserFIQ)
		if (UserFIQ())
			goto skipsound;

#ifdef	USE_SOUND

	if (SndCnt == 0)
	{
		if (SndSize > 0)	// Loop
		{
			SndCnt = SndSize;
			SndPtr = SndAdrs;
		}
		else
		{
			SoundStop();
			SndCnt--;
		}
	}

	if (SndCnt > 0)
	{
		u32	data = ROMByteAccess(SndPtr++);
		data <<= 8;	// 10bits signed dac (upper 10bits used only)
		IOCTL_DASOUND = data;
		SndCnt--;
	}

#endif

skipsound:
	IRQ_CLEAR = FIQ_TIMER2;
}


/****************************************/
// Fast Graphic Functions

u32		LCDbuffer[32];

#define	FastLCDClear()		FastScreenClear(LCDbuffer, 0, 32)
void	FastScreenClear(u32 *scr, int offset, int line)
{
	scr += offset;

	while (line)
	{
		*scr++ = 0;
		line--;
	}
}

#define	DrawLCDPix(_x_, _y_)	DrawPixPtr(LCDbuffer, _x_, _y_)
void	DrawPixPtr(u32 *scr, int x, int y)
{
	scr += y;
	*scr |= (1 << x);
}

#define	ClearLCDPix(_x_, _y_)	DrawPixPtr(LCDbuffer, _x_, _y_)
void	ClearPixPtr(u32 *scr, int x, int y)
{
	scr += y;
	*scr &= ~(1 << x);
}

#define	FastLCDInvert(_o_, _l_)	FastLCDXor(_o_, _l_, 0xFFFFFFFF)
void	FastLCDXor(int offset, int line, u32 xordata)
{
	u32	*ptr = LCDbuffer;

	ptr += offset;
	while (line)
	{
		*ptr ^= xordata;
		ptr++;
		line--;
	}
}

void	FastLCDCopy(u32 *data, int offset, int line)
{
	u32	*ptr = LCDbuffer;

	ptr += offset;
	while (line)
	{
		*ptr++ = *data++;
		line--;
	}
}

void	FastLCDCombine(u32 *data, int offset, int line)
{
	u32	*ptr = LCDbuffer;

	ptr += offset;
	while (line)
	{
		*ptr++ |= *data++;
		line--;
	}
}

void	FastLCDMask(u32 *data, u32 *mask, int offset, int line)
{
	u32	*ptr = LCDbuffer;
	u32	ldata;

	ptr += offset;
	while (line)
	{
		ldata = *ptr;
		ldata &= *mask++;
		ldata |= *data++;
		*ptr++ = ldata;
		line--;
	}
}

void	FastDrawLCD(void)
{
	u32				*src = LCDbuffer;
	volatile u32	*dst = LCD_VRAM_ADRS;
	int				i = 32;

	while (i)
	{
		*dst++ = *src++;
		i--;
	}
}


/****************************************/
// Sprites Functions

#ifdef	USE_SPRITE

// Show a 32xN Full Clipped Sprite
void	Sprite32(int x, int y, int height, unsigned int *data)
{
	unsigned int	*screen = LCDbuffer;

	// Trivial X/Y/Height Clipping
	if ((y + height < 0) || (y >= 32) || (x <= -32) || (x >= 32) || (height == 0))
		return;

	// Y Clipping
	if (y < 0)
	{
		data -= y;
		height += y;
	}
	else
	{
		screen += y;
		if ((y + height) > 32)
			height -= (y + height) - 32;
	}

	// Left clipping
	if (x < 0)
	{
		while (height--)
			*(screen++) |= *(data++) >> (-x);
	}
	else
	{
		while (height--)
			*(screen++) |= *(data++) << x;
	}
}

/**/

// Show a 32xN Full Clipped and Masked Sprite
void	Sprite32m(int x, int y, int height, unsigned int *data, unsigned int *mask)
{
	unsigned int	*screen = LCDbuffer;
	unsigned int	fixmask;

	// Trivial X/Y/Height Clipping
	if ((y + height < 0) || (y >= 32) || (x <= -32) || (x >= 32) || (height == 0))
		return;

	// Y Clipping
	if (y < 0)
	{
		data -= y;
		mask -= y;
		height += y;
	}
	else
	{
		screen += y;
		if ((y + height) > 32)
			height -= (y + height) - 32;
	}

	// Left clipping
	if (x < 0)
	{
		fixmask = 0xFFFFFFFF << (32 + x);
		while (height--)
		{
			*(screen) &= fixmask | (*(mask++) >> (-x));
			*(screen++) |= *(data++) >> (-x);
		}
	}
	else
	{
		if (x)
			fixmask = 0xFFFFFFFF >> (32 - x);
		else
			fixmask = 0;
		while (height--)
		{
			*(screen) &= fixmask | (*(mask++) << x);
			*(screen++) |= *(data++) << x;
		}
	}
}

#endif

/****************************************/
// Draw Text & Number Functions

extern unsigned int TinyFont[];

void	DrawChar(int x, int y, int c)
{
	unsigned int *f;
	unsigned int mask;
	unsigned int d;
	int i;
	int off;

	c &= 0x7f;
	c -= ' ';
	if ((c < 0) || (c >= 96))
		return;

	off = -(c & 7) * 4;
	off += x;
	mask = 0xF << x;
	f = &TinyFont[(c >> 3) * 5];

	for (i = 0; i < 5; i++)
	{
		if (y >= 32)
			break;
		d = LCDbuffer[y];
		d &= ~mask;
		if (off < 0)
			d |= ((*f) >> (-off)) & mask;
		else
			d |= ((*f) <<   off ) & mask;
		LCDbuffer[y++] = d;
		f++;
	}
}

void	DrawString(int x, int y, char *str)
{
	u32	c;

	while (42)
	{
		c = ROMByteAccess(str);
		if (!c)
			return;
		DrawChar(x, y, c);
		x += 4;
		if (x >= 32)
			return;
		str++;
	}
}

void	DrawNumber(int x, int y, int n, int ndigit)
{
	int	num;
	x += ndigit * 4;

	do
	{
		x -= 4;
		num = n / 10;
		DrawChar(x, y, (n - (num * 10)) + '0');
		n = num;
		ndigit--;
	} while (ndigit);
}

void	DrawHex(int x, int y, u32 hex)
{
	int		i;
	char	b;

	for (i = 0; i < 8; i++)
	{
		b = (hex >> (28 - (i * 4))) & 0xF;
		if (b >= 0xA)
			b += ('A' - 0xA);
		else
			b += '0';
		DrawChar(x, y, b);
		x += 4;
	}
}


/****************************************/
// Sync Functions

void	VSync(int copylcd)
{
	register int	cnt = VCount;
	register int	PadState = PAD_STATUS;

	PadOnRelease = PadState;
	PadOnRelease ^= PadOnPress;
	PadOnRelease &= PadOnPress;
	PadOnPress = PadState;

	while (VCount == cnt);	// Wait For IRQ Timer0

	if (copylcd)
		FastDrawLCD();
}

void	WaitForButtonPress(void)
{
	do
	{
		VSync(1);
	} while (!(PadOnRelease & PAD_BUTTON));
}


/****************************************/
// Random Function

#ifdef	USE_RANDOM

u8	random8[64];		// In Ram (because we modify the values)
u8	random8Init[64] =	// In Rom
{
	215,25,169,8,231,201,91,15,156,5,152,30,159,50,22,49,
	28,148,157,123,58,3,156,236,46,78,50,157,186,170,233,138,
	49,98,215,231,0,249,233,254,194,12,110,115,122,166,15,2,163,
	237,143,30,54,5,45,199,201,106,184,136,44,175,197,11
};

u16	seed8_1;
u16	seed8_2;

void	InitRandom(void)
{
	u8	i;

	seed8_1 = 29;
	seed8_2 = 63;
	for (i = 0; i < 64; i++)
		random8[i] = ROMByteAccess(&random8Init[i]);
}

u8		random(int max)
{
	u8	dta;

	random8[seed8_2] = random8[seed8_1] + random8[seed8_2];
	seed8_1 = (seed8_1 + 1) & 63;
	seed8_2 = (seed8_2 + 1) & 63;
	dta = random8[seed8_2];
	while (dta >= max)
		dta -= max;
	return (dta);
}

#endif


/****************************************/
// Init, need to be called at first !

void	PocketInit(void)
{
	SetCpuSpeed(CLOCK_SPEED);
	IRQ_DISABLE_ALL();

	UserIRQ = NULL;
	UserFIQ = NULL;

	VCount = 0;
	PadOnRelease = 0;
	PadOnPress = 0;
#ifdef	USE_RANDOM
	InitRandom();
#endif

	SetCallbacks(1, IntIRQ);	// VSync Interrupt
	SetCallbacks(2, IntFIQ);	// Audio Interrupt

	IRQ_CLEAR = IRQ_TIMER0 | FIQ_TIMER2;
	IRQ_ENABLE = IRQ_TIMER0 | FIQ_TIMER2;

	// Start VSync Timer
	TIMER0_SETCOUNT((15625 << CLOCK_SPEED) / FPS);
	TIMER0_START();

	while (PAD_STATUS & PAD_BUTTON);	// Wait for Button Release (Button pressed when comming from Menu)
}


/****************************************/
// Exit, need to be called to exit app !

void	PocketExit(void)
{
	SetComOnOff(0);
	TIMER0_STOP();
	TIMER2_STOP();
	IRQ_DISABLE_ALL();
	IRQ_ENABLE = IRQ_RTC | IRQ_PSCOMM;
	PrepareExecute(1, 0, GetDirIndex() | 0x30);	// Return to GUI on our Program Icon
	DoExecute(0);
}

/****************************************/
