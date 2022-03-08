#
# Makefile for example PROG
#
CC = gcc
INCLUDES = -I../..

PROG = tower
SRC  = tower.c
SGLLIB = ../../sgl.a

#
# Build the program
#
$(PROG): $(SRC) $(SGLLIB)
	$(CC) -o $(PROG) $(INCLUDES) $(SRC) $(SGLLIB)  -lm

#
# release
#
release:    expected.bmp


expected.bmp: $(PROG)
	$(PROG)
	mv result.bmp expected.bmp


#
# End of makefile
#
