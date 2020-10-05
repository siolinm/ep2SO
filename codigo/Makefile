DIRNAME    = ep2-lucas-marcos
EPOBJS     = ep2.o
CFLAGS     = -Wall -g -pedantic -Wno-unused-result
LIBS_EP2   = -lpthread

default: ep2

ep2: $(EPOBJS)
	gcc $(CFLAGS) $(EPOBJS) -o ep2 $(LIBS_EP2)

%.o: %.c # %.h
	gcc $(CFLAGS) -c $<

tar:
	rm -Rf ../$(DIRNAME)
	rm -f ../$(DIRNAME).tar.gz
	mkdir ../$(DIRNAME)

	# copia os slides, o LEIAME e o Makefile
	cp ../LEIAME ../$(DIRNAME)/LEIAME
	cp Makefile ../$(DIRNAME)/Makefile
	cp ../slides/slides.pdf ../$(DIRNAME)/slides.pdf

	# copia o codigo fonte
	for a in *.c; do \
		cp $$a ../$(DIRNAME)/$$a; \
	done

	# (se houverem .h)
	for a in *.h; do \
		cp $$a ../$(DIRNAME)/$$a; \
	done

	# gera ../$(DIRNAME).tar.gz
	tar -czvf ../$(DIRNAME).tar.gz ../$(DIRNAME)
	rm -Rf ../$(DIRNAME)


clean:
	rm *.o ep2 *~
