CC := gcc
CFLAGS :=
LDFLAGS :=

SRCDIR := c
BUILDDIR := ../../build/common

DIRECTORIES := $(addprefix $(BUILDDIR)/, $(dir $(wildcard $(SRCDIR)/*)))
CFILES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(addprefix $(BUILDDIR)/, $(CFILES:.c=.o))

.PHONY: directories

all: common

directories: $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $(DIRECTORIES)

$(BUILDDIR)/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

common: directories $(OBJECTS)

clean:
	rm -rf $(BUILDDIR)
