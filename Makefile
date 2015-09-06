ODIR=./obj
BINDIR=./bin

CC=gcc
CFLAGS=-I. -g -Wall

$(BINDIR)/fbfs: $(ODIR)/fbfs.o
		gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/fbfs.o: fbfs.c
		$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
		rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
cleanall:
		rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BINDIR)/*

