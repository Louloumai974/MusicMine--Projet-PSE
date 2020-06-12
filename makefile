# TP7 : Fichier Makefile
#
include ../Makefile.inc

EXE =  AltClient

all: ${EXE}

${EXE): ${PSE_LIB}

clean:
	rm -f *.o *~ ${EXE} journal.log

