IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -Wall -g

SRCDIR=.
ODIR=.
LIBS=-lm -lpthread

_OBJ = main.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

OUTPUTDIR = .

$(ODIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTPUTDIR)/cvrptw: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: $(OUTPUTDIR)/cvrptw

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUTPUTDIR)/cvrptw
