# Makefile for Mace
#

CC        = gcc
CPP       = gcc -E
LDFLAGS   = 
CFLAGS    = 
X_CFLAGS  = 
LIBRARIES = 
MATHLIB   = -lm

INSTALL         = @INSTALL@
INSTALL_PROGRAM = @INSTALL_PROGRAM@
INSTALL_DATA    = @INSTALL_DATA@
prefix          = /usr/local
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
libdir          = ${exec_prefix}/lib
sysconfdir      = ${prefix}/etc

.SUFFIXES: .o .c .h .m .i .S

INCLUDES=-I/usr/local/include

OBJS =	File.o Unix.o Main.o Mem.o Res.o Toolbox.o

all: Mace

clean:
	rm -f Mace
	rm -f core
	rm -f *.o
	rm -f *~
	cd cpu && $(MAKE) clean

cpu/libCPU.a:
	cd cpu && $(MAKE)
	cd ..

Mace: $(OBJS) cpu/libCPU.a
	$(CC) $(CFLAGS) $(OBJS) -o Mace -Lcpu -lCPU $(LDFLAGS) $(LIBRARIES) $(MATHLIB)

.m.o:
	$(CC) $(INCLUDES) -c $(CFLAGS) $*.m
.c.o:
	$(CC) $(INCLUDES) -c $(CFLAGS) $*.c -o $@
.c.s:
	$(CC) $(INCLUDES) -S $(CFLAGS) $*.c
.c.i:
	$(CC) $(INCLUDES) -E $(CFLAGS) $*.c > $@
.S.o:
	$(CC) $(INCLUDES) -c $(CFLAGS) $*.S
.s.o:
	$(CC) $(INCLUDES) -c $(CFLAGS) $*.s
