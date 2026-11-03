MPICC   ?= mpicc
MPICXX  ?= mpicxx
CFLAGS  ?= -O2
LDLIBS  ?= -lm

all: parallel_primes sieve1 prime_mpi

parallel_primes: deliverables/parallel_primes.c
	$(MPICC) $(CFLAGS) -o $@ $< $(LDLIBS)

sieve1: sieve1.c
	$(MPICC) $(CFLAGS) -o $@ $< $(LDLIBS)

# Uses the C++ MPI bindings (MPI::), which need an MPI library that still ships them.
prime_mpi: prime_mpi.cpp
	$(MPICXX) $(CFLAGS) -o $@ $<

clean:
	rm -f parallel_primes sieve1 prime_mpi

.PHONY: all clean
