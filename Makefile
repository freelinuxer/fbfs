ODIR=./obj
BINDIR=./bin
CC=gcc
CFLAGS=-I. -g -Wall

all: directories $(BINDIR)/fbfs 

directories: 
	mkdir -p ${ODIR} ${BINDIR}

$(BINDIR)/fbfs: $(ODIR)/fbfs.o
		gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/fbfs.o: fbfs.c
		$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
		rm -f $(ODIR)/* *~ core $(INCDIR)/*~ 
cleanall:
		rm -f $(ODIR)/* *~ core $(INCDIR)/*~ $(BINDIR)/*

