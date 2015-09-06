ODIR=./obj
BINDIR=./bin
CC=gcc
CFLAGS=-I. -g -Wall
MKDIR_P=mkdir -p

all: ${ODIR} ${BINDIR} $(BINDIR)/fbfs 

${ODIR}:
	${MKDIR_P} ${ODIR}

${BINDIR}:
	${MKDIR_P} ${BINDIR}

$(BINDIR)/fbfs: $(ODIR)/fbfs.o
		${CC} -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/fbfs.o: fbfs.c
		$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean directories

clean:
		rm -f $(ODIR)/* *~ core $(INCDIR)/*~ 
cleanall:
		rm -f $(ODIR)/* *~ core $(INCDIR)/*~ $(BINDIR)/*

