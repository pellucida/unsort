
#
#	License: Creative Commons CC0 
#		http://creativecommons.org/publicdomain/zero/1.0/legalcode
#	Author:	James Sainsbury
#		toves@sdf.lonestar.org

CFLAGS	= -g -O -DHAVE_CLOCK_GETTIME
LDFLAGS	= -lrt
#CFLAGS	= -g -O 
#LDFLAGS	= 

OBJ	= unsort.o line.o 
PROG	= unsort

$(PROG):	$(OBJ)

unsort.o:	unsort.c line.h constant.h 
line.o:		line.c line.h constant.h

clean:
	rm -f $(PROG) $(OBJ) core
