CC=g++
IDIR=./include
ODIR=./obj

VPATH=./src

TARGET=clp_dist

#_DEPS = 
#DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o #
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CFLAGS=-pedantic -g -Wall -I$(IDIR)

.PHONY: all clean

all: $(TARGET)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ $(TARGET)