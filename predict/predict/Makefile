# Makefile For ZOJ judge system; drangon ; 2002-02-27

CC		= g++
CXX		= gcc
CFLAGS	= -Wall -fPIC -ggdb -I. -I./lib -I./scheduler -I./dataprocess -I/usr/include/libxml2 -I./libxml2
LFLAGS	= -L. -L/usr/lib -lxml2
#LIBS	= -lzojlib -lmysqlclient -lkmmon -lz

#################################
.SUFFIXES: .cpp .c 

.cpp.o:
	$(CXX) -c $(CFLAGS) -g -o $@ $<

.c.o:
	$(CXX) -c $(CFLAGS) -g -o $@ $<

#########################################
TARGET	= drsctrl

OBJECT	= drsctrl.o drscontroller.o ./dataprocess/xmlprocess.o ./dataprocess/generator.o ./lib/cls_host.o ./lib/util.o ./scheduler/scheduler.o ./scheduler/alg_initplace.o ./lib/cls_pm.o ./lib/cls_vm.o ./lib/cls_sys.o ./lib/cls_vmgroup.o ./lib/cls_frame.o ./lib/cls_pmgroup.o ./lib/cls_net.o

SOURCE	= drsctrl.c drscontroller.c  xmlprocess.c generator.c cls_host.c util.c scheduler.c alg_initplace.c cls_pm.c ./lib/cls_vm.c ./lib/cls_sys.c ./lib/cls_vmgroup.c ./lib/cls_frame.c ./lib/cls_pmgroup.c ./lib/cls_net.c

######################################

all: $(TARGET) 

drsmain: $(OBJECT)
	$(CXX) $(CFLAGS) $(LFLAGS)   -shared -o libdrsctrl_0731.so  $(OBJECT)


clean:
	rm -f $(OBJECT) $(TARGET) 

