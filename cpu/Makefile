# Linux Makefile for Mace
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

INCLUDES=-I/usr/include -I../

OBJS=	Code1.o	Code2.o	Code3.o	Code4.o	Code5.o Code6.o	Code7.o	Code8.o	Code9.o	Run.o Utils.o	cpu.o

all: libCPU.a

libCPU.a: $(OBJS)
	(rm -f libCPU.a; ar rcv libCPU.a $(OBJS); ranlib libCPU.a)

clean:
	rm -f *.o
	rm -f libCPU.a
	rm -f *~

.m.o:
	$(CC) $(INCLUDES) -c $(INCDIRS) $(CFLAGS) $(DEBUGFLAGS) $*.m
.c.o:
	$(CC) $(INCLUDES) -c $(INCDIRS) $(CFLAGS) $(DEBUGFLAGS) $*.c -o $@
.c.s:
	$(CC) $(INCLUDES) -S $(INCDIRS) $(CFLAGS) $*.c -o $@
.c.i:
	$(CC) $(INCLUDES) -E $(INCDIRS) $(CFLAGS) $*.c > $@
.S.o:
	$(CC) $(INCLUDES) -c $(INCDIRS) $(CFLAGS) $(DEBUGFLAGS) $*.S -o $@
.s.o:
	$(CC) $(INCLUDES) -c $(INCDIRS) $(CFLAGS) $(DEBUGFLAGS) $*.s -o $@

