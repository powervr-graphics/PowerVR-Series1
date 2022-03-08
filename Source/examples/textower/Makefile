#
# Makefile for example PROG
#
CC = gcc
INCLUDES = -I../..
SGLLIB  = ../../sgl.a


PROG = textower
SRC  = textower.c

#
# Build the program
#
$(PROG): $(SRC) $(SGLLIB)
	$(CC) -o $(PROG) $(INCLUDES) $(SRC) $(SGLLIB) -lm

#
# release
#
release:    expected.bmp


expected.bmp: $(PROG)
	$(PROG) 0 0
	mv result00.bmp expected.bmp


#
# End of makefile
#
