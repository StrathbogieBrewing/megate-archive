CC         = gcc
PROGRAM    = datalog
CSRC       = logger.c log.c udp.c mkdir.c
BUILDDIR   = bin
SOURCEDIR  = src
EXTDIR     = ext

# include directories
INCLUDE    = -I../ -I../../
VPATH      = ../../ ../

CFLAGS     =  $(INCLUDE)
LDFLAGS    =

COBJ := $(notdir $(CSRC:.c=.o) )
OBJS := $(patsubst %.o, $(BUILDDIR)/%.o, $(COBJ) )

all: dir $(BUILDDIR)/$(PROGRAM)
	# rsync bin/datalog johny@lunar:cgi-bin/api
	# rsync www/index.html johny@lunar:index.html
	# rsync www/dygraph.css johny@lunar:dygraph.css
	# rsync www/dygraph.js johny@lunar:dygraph.js
	# sftp bin/datalog Johny@lunar/home/johny/cgi-bin/api

dir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(BUILDDIR)/$(PROGRAM)

# clean files
clean:
	rm $(BUILDDIR)/*.o
