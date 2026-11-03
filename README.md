# primes_sieve_mpi

Sieve of Eratosthenes parallelised with MPI, written for the third assignment of a parallel-processing
course (task sheet: `Assignment No. 3.pdf`; write-up, in Persian: `report.pdf` / `report.docx`).

## What is in the repo

- `deliverables/parallel_primes.c` – the submitted program. Rank 0 reads `N` and a mode, broadcasts `N`,
  every process sieves the numbers `2..sqrt(N)` itself and then marks its own even share of the range
  `sqrt(N)+1..N`. The shares are sent back to rank 0, which prints the primes and then runs a serial sieve
  for comparison. `deliverables/parallel_primes` is a 32-bit Linux binary built from it and
  `deliverables/report.pdf` is identical to the top-level `report.pdf`.
- Reference code that the assignment built on:
  - `sieve1.c` – the classic block-decomposed MPI sieve from Quinn's textbook (modified by Ryan Holt and
    Nathan Dykhuis). Takes `<n>` and prints the prime count and run time.
  - `prime_mpi.cpp` – John Burkardt's `PRIME_MPI` (trial division, cyclic decomposition, LGPL). Counts
    primes for `N = 1, 2, 4, ... 262144`. It uses the C++ MPI bindings (`MPI::`), which were removed in
    MPI-3, so it only builds against an MPI library that still provides them.
  - `fastMarkigPrime.cpp` – a sequential "fast marking" sieve handed out with the course (by Ahmad
    Siavashi). As committed it does not compile (`sizeof char` on line 43 needs parentheses).
- `guide/` – a CUDA sieve (`kernel.cu`) and its report, supplied as a guide for the assignment.
- `Parallelization - Sieve of Eratosthenes.pdf` – background reading.

## Building and running

The `Makefile` builds the three MPI programs with `mpicc` / `mpicxx`:

```sh
make                 # parallel_primes, sieve1 (and prime_mpi if your MPI still has the C++ bindings)
make parallel_primes # just the assignment program
```

`parallel_primes` takes the upper bound `N` and a mode: `0` runs silently, `1` prints the primes found by
the parallel and the serial sieve, `2` prints the parallel time, serial time and speedup. With no
arguments it defaults to `N = 128`, mode `1`.

```sh
mpirun -np 4 ./parallel_primes 100000 2
mpirun -np 4 ./sieve1 100000
```

Notes: the number of processes must be small enough that each one gets at least one number above
`sqrt(N)`; the serial sieve in `parallel_primes.c` stops at `N-1`, so its output can differ from the
parallel one by the number `N` itself when `N` is prime.
