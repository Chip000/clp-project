CC=g++
IDIR=./include
ODIR=./obj
VPATH=./src

CONCERTDIR = /usr/local/ilog/concert29
CPOPTIMIZERDIR = /usr/local/ilog/cpoptimizer23

TARGET=clp_dist

_DEPS = cp.h prob.h graph.h edge.h f_edge.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o cp.o prob.o graph.o edge.o f_edge.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CFLAGS=-DIL_STD -O -DNDEBUG -I$(IDIR) -I$(CPOPTIMIZERDIR)/include	\
-I$(CONCERTDIR)/include -fstrict-aliasing -pedantic -g -Wall		\
-fexceptions -ffloat-store -DILOUSEMT -D_REENTRANT -DILM_REENTRANT

LDFLAGS = -L$(CPOPTIMIZERDIR)/lib/x86-64_debian4.0_4.1/static_pic -lcp	\
-lcplex -L$(CONCERTDIR)/lib/x86-64_debian4.0_4.1/static_pic -lconcert	\
-lpthread

.PHONY: all clean

all: $(TARGET)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ $(TARGET)