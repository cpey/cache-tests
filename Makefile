IDIR = ./include
ODIR = out
SDIR = src
CC = gcc
CFLAGS = -I$(IDIR) -O0 -DMUTE

_OBJ_01 = 01.cache.o helper.o
OBJ_01 = $(patsubst %,$(ODIR)/%,$(_OBJ_01))

_OBJ_02 = 02.cache.o helper.o
OBJ_02 = $(patsubst %,$(ODIR)/%,$(_OBJ_02))

_OBJ_03 = 03.cache.o helper.o
OBJ_03 = $(patsubst %,$(ODIR)/%,$(_OBJ_03))

_OBJ_04 = 04.cache.o helper.o
OBJ_04 = $(patsubst %,$(ODIR)/%,$(_OBJ_04))

_OBJ_05 = 05.cache.o helper.o
OBJ_05 = $(patsubst %,$(ODIR)/%,$(_OBJ_05))

.PHONY: all
all: $(ODIR)/01.cache
all: $(ODIR)/02.cache
all: $(ODIR)/03.cache
all: $(ODIR)/04.cache
all: $(ODIR)/05.cache

$(ODIR)/%.o: $(SDIR)/%.c include/helper.h
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/01.cache: $(OBJ_01)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/02.cache: $(OBJ_02)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/03.cache: $(OBJ_03)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/04.cache: $(OBJ_04)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/05.cache: $(OBJ_05)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(ODIR)/01.cache $(OBJ_01)
	rm -rf $(ODIR)/02.cache $(OBJ_02)
	rm -rf $(ODIR)/03.cache $(OBJ_03)
	rm -rf $(ODIR)/04.cache $(OBJ_04)
	rm -rf $(ODIR)/05.cache $(OBJ_05)
