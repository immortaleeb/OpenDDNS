CC := gcc
CFLAGS :=
LDFLAGS := -lpthread

export CC CFLAGS LDFLAGS

PROG := server
COMMON := common

SRCDIR := c
BUILDDIR := ../../build/$(PROG)
BINDIR := ../../bin

COMMON_SRCDIR := ../$(COMMON)
COMMON_BUILDDIR := ../../build/$(COMMON)/c
COMMON_OBJECTS := $(wildcard $(COMMON_BUILDDIR)/*.o) $(wildcard $(COMMON_BUILDDIR)/*/*.o)

DIRECTORIES := $(BINDIR) $(addprefix $(BUILDDIR)/, $(dir $(wildcard $(SRCDIR)/*))) \
	$(addprefix $(BUILDDIR)/, $(dir $(wildcard $(SRCDIR)/*/*)))
CFILES := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c)
OBJECTS := $(addprefix $(BUILDDIR)/, $(CFILES:.c=.o))

.PHONY: directories

all: directories $(BINDIR)/$(PROG)

directories: $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $(DIRECTORIES)

$(BUILDDIR)/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

$(BINDIR)/$(PROG): $(OBJECTS)
	make -C $(COMMON_SRCDIR)
	$(CC) -o $@ $(COMMON_OBJECTS) $^ $(LDFLAGS)

clean:
	rm -rf $(BUILDDIR)
	rm -f $(BINDIR)/$(PROG)
