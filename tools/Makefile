SUBDIRS = ascii2sjis bin2mcs bmp2ps1b mcpad mcrtool tim2mcicon

all: 
	@for i in $(SUBDIRS) ; \
	do \
	$(MAKE) -C $$i ; \
	done \

fclean: 
	@for i in $(SUBDIRS) ; \
	do \
	$(MAKE) -C $$i fclean ; \
	done \
