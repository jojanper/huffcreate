#*************************************************************************
#  Makefile  - Makefile for Huffman code construction routine.
#
#*************************************************************************

# compiler

CC = gcc


# defaults

INCL =
LIBS += -lm
CFLAGS +=  -ansi -pedantic -Wall -g
LDFLAGS += $(LIBPATH)

EXEC_NAME = huffcreate
PROGS = $(EXEC_NAME)


# set paths and directories

prefix = .
BINDIR = $(prefix)
SRCDIR = $(prefix)
OBJDIR = $(prefix)/obj
LIBDIR = $(prefix)/lib

LIBPATH += -L$(LIBDIR)

#vpath %.c $(SRCDIR)


# rules

all : $(PROGS)

ENC_OBJS = bintree.o list.o huffman.o main.o

ENCODER_OBJS = $(addprefix $(OBJDIR)/,$(ENC_OBJS))

# link

$(BINDIR)/$(EXEC_NAME): $(ENCODER_OBJS)
	@echo "*****" linking $@ ...
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

# compile

$(OBJDIR)/%.o : %.c
	@echo "*****" compiling $< ...
	$(CC) -c $(CFLAGS) $< -o $@

# dependencies
# the rules are generated automatically from the source files using
# command gcc -MM and including the output files *.d to this main makefile.

$(OBJDIR)/%.d : %.c
	@echo "*****" generating dependencies for $< ...
	$(SHELL) -c "$(CC) -MM $(CPPFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/.\/obj\/$*.o .\/obj\/$*.d : /g' > $@"

-include $(ENCODER_OBJS:.o=.d)

# other targets

clean :
	@echo "*****" cleaning ...
	-rm -f core $(ENCODER_OBJS) $(ENCODER_OBJS:.o=.d)
