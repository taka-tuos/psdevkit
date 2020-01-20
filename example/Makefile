PROG	= test

SRCS	= ${PROG}.c
OBJS	= header.o ${SRCS:.c=.o}

CC	= arm-none-eabi-gcc
LD	= arm-none-eabi-ld
AS	= arm-none-eabi-as
OBJCOPY	= arm-none-eabi-objcopy
OBJDUMP	= arm-none-eabi-objdump

LDFLAGS	= #-Map ${PROG}.map
CFLAGS	= -Os -msoft-float
ASFLAGS	=

TITLE = "PocketStation Lib Test by Orion_"

all:	${PROG}.bin

.PHONY: data

${PROG}.bin: ${PROG}
	${OBJCOPY} -O binary ${PROG} ${PROG}.bin
	../tools/mcpad/mcpad ${PROG}.bin

${PROG}: ${OBJS}
	${LD} -o ${PROG} ${OBJS} -Tldscript ${LDFLAGS}

clean:
	rm -f ${OBJS} ${PROG} ${PROG}.map

fclean:	clean
	rm -f ${PROG}.bin
	rm -f ${PROG}.mcs

disasm:
	${OBJDUMP} --disassemble ${PROG} > ${PROG}.DISASM.S

data: Makefile data/icon.tim data/icon.bmp
	../tools/ascii2sjis/ascii2sjis $(TITLE) data/title.txt
	../tools/tim2mcicon/tim2mcicon data/icon.tim data/psicon.bin
	../tools/bmp2ps1b/bmp2ps1b b data/icon.bmp

tool:
	$(MAKE) -C ../tools

mcs: ${PROG}.bin
	../tools/bin2mcs/bin2mcs BESNESP00000GAMETEST ${PROG}.bin ${PROG}.mcs

mcr: mcs
	../tools/mcrtool/mcrtool of=${PROG}.mcr mcs=${PROG}.mcs

gme: mcs
	../tools/mcrtool/mcrtool -G of=${PROG}.gme mcs=${PROG}.mcs
