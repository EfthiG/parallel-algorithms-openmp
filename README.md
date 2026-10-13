# Parallel Algorithms with OpenMP

Parallel implementations of **Conway's Game of Life** and **Gaussian back substitution** in C, developed as a collaborative university assignment for a Parallel Systems course.

The project explores shared-memory parallel programming with **OpenMP**, compares serial and parallel implementations, and provides scripts and recorded experiments for performance analysis.

## Implemented algorithms

### 1. Conway's Game of Life

Simulates a two-dimensional cellular automaton over a configurable number of generations. Each cell's next state depends on its eight neighboring cells.

- **Serial implementation:** updates the grid cell by cell.
- **Parallel implementation:** uses `#pragma omp parallel for` with `collapse(2)` and `schedule(static)` to distribute the grid update across threads.
- Maintains separate current and next-generation grids and swaps them after each generation.
- Supports configurable grid dimensions, generation counts, and thread counts.
- Reads initial live-cell coordinates from `input_files/`, generating a random coordinate file when one is not already present.

### 2. Gaussian Back Substitution

Solves an upper-triangular linear system using two traversal strategies:

- **By line:** computes solution entries from the last row upward.
- **By column:** propagates each computed solution entry to preceding rows.

Both strategies have serial and OpenMP implementations. The parallel variants use OpenMP work-sharing loops and support **static**, **dynamic**, and **guided** scheduling. The by-line implementation uses an atomic update for concurrent contributions to a solution entry.

The program generates an upper-triangular integer matrix and a right-hand-side vector, then records execution time. It is intended for comparing parallelization strategies rather than as a general-purpose numerical solver.

## Technologies

- **Language:** C
- **Parallel programming:** OpenMP
- **Build:** GCC and Make
- **Benchmarking:** Bash scripts and Python plotting utilities
- **Environment:** Linux / WSL with OpenMP-enabled GCC

## Repository structure

```text
.
├── src/
│   ├── common.c
│   ├── game_of_life/
│   │   ├── grid_funcs.c
│   │   └── main.c
│   └── gauss/
│       ├── gauss_funcs.c
│       └── main.c
├── include/
│   ├── common.h
│   ├── game_of_life.h
│   └── gauss.h
├── input_files/
├── Makefile
├── tests.sh
└── graphs.py
```

The build creates `bin/`, `build/`, and `results/` directories as needed.

## Build and run

### Requirements

A Linux environment with `gcc`, `make`, and OpenMP support. For the plotting utility, install the Python packages required by `graphs.py`.

From the repository root:

```bash
make all
```

This builds `bin/game_of_life` and `bin/gauss`.

### Game of Life

```bash
./bin/game_of_life <generations> <rows> <cols> <parallel: 0|1> <threads>
```

Example — 100 generations on a 64×64 grid with four threads:

```bash
./bin/game_of_life 100 64 64 1 4
```

Serial example:

```bash
./bin/game_of_life 100 64 64 0 1
```

Execution times are appended to `results/game_of_life.txt`.

### Gaussian Back Substitution

```bash
./bin/gauss <size> <serial|parallel> <by_line|by_column> <threads> <schedule>
```

Example — parallel, by-column strategy, four threads, dynamic scheduling:

```bash
./bin/gauss 100 parallel by_column 4 dynamic
```

Serial example:

```bash
./bin/gauss 100 serial by_line 1 static
```

Execution times are appended to `results/gauss.txt`.

### Clean generated files

```bash
make clean
```

**Note:** This command removes the `bin/`, `build/`, and `results/` directories.

## Performance experiments

The project includes benchmarking logic in `tests.sh` for comparing serial and parallel execution over multiple problem sizes, thread counts, and scheduling policies. The script currently has its benchmark function calls commented out and runs the graph-generation function; review or enable the relevant calls before using it to reproduce benchmarks.

The `graphs.py` script is used to visualize benchmark results. Performance depends on the machine, workload size, scheduling policy, and number of threads; no universal speedup is assumed.

## Academic context

This repository contains a **collaborative university project** completed for a Parallel Systems course.