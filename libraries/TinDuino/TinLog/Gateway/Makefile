CC         = gcc

PROGRAM    = tinlog

BUILD       = $(shell date +'%y%m%d%H%M')

CSRC       = tinlog.c tinbus.c tinux.c msg.c

BUILDDIR   = bin
SOURCEDIR  = src
EXTDIR     = ext

# include directories
INCLUDE    = -I../ -I../../
VPATH      = ../../ ../ 

CFLAGS     = -DBUILD="$(BUILD)" -DPROGRAM="$(PROGRAM)" $(INCLUDE) -g

COBJ := $(notdir $(CSRC:.c=.o) )
OBJS := $(patsubst %.o, $(BUILDDIR)/%.o, $(COBJ) )

all: dir $(BUILDDIR)/$(PROGRAM)

run:
	-pkill tinlog
	nohup ./bin/tinlog /dev/ttyUSB0 </dev/null >/dev/null 2>&1 &

TARGET	= pi@raspberrypi:software
#TARGET	= pi@plw:software/monitor

upload:
	rsync -r ../ $(TARGET)/$(PROGRAM)
	# rsync Makefile $(TARGET)/$(PROGRAM)

dir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(BUILDDIR)/$(PROGRAM)
	rm $(BUILDDIR)/*.o
#	rm *.o
#	rm ../common/*.o
#	mv $(BUILDDIR)/$(PROGRAM) ../arduino/hardware/yellowbox/avr/tools/
#	git commit -m "Build $(PROGRAM) - $(BUILD)" -a
#	git push origin master
#  bin/readlog test.html /dev/ttyUSB0


# clean files
clean:
	rm -f $(BUILDDIR)/*.o
