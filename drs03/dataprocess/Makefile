# Makefile For ZOJ judge system; longman ; 2002-02-27

CC		= g++
CXX		= gcc
CFLAGS	= -Wall -ggdb 
LIBS	= -L/usr/lib/mysql -lmysqlclient
INCS	= -I../inc -I/use/include

#################################
# .SUFFIXES: .o .c
# .c.o:	;$(CC) $(CFLAGS) $(INCS) -c $*.c
.SUFFIXES: .cpp .c 

.cpp.o:
	$(CXX) -c $(CFLAGS) $(INCS) -o $@ $<

.c.o:
	$(CXX) -c $(CFLAGS) $(INCS) -o $@ $<

####################################
# SOBJS=db_lib.o test.o utils.o
# TARGET=test
TEST	= test

GENLIB	= libzojlib.a

OBJS	= utils.o contest.o

SOURCE	= utils.c contest.c

##################################
all : $(TEST) $(GENLIB)
	@echo "test and lib done!"

$(TEST) : test.o $(GENLIB)
	$(CC) $(CFLAGS) test.o -o $(TEST) -L. -lzojlib $(LIBS)

$(GENLIB) : $(OBJS)
	ar crv $(GENLIB) $(OBJS)
	
clean : 
	rm -f $(OBJS) core $(TEST) $(GENLIB)
	
# test: $(SOBJS)
#	$(CC) $(CFLAGS) $(SOBJS) $(LIBS) -o $(TARGET)


# clean:
#	rm -f *.o core $(TARGET) 
