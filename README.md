# Tetris Engine

## Overview

Processes sequences of pre-defined Tetris pieces dropped into a 10-unit wide grid. Calculates the final height of remaining blocks after placement and row-clearing.

## Key Features

- **Templatized Architecture** — single engine logic supporting multiple board implementations via compile-time polymorphism
- **Bitset Board** — high-performance implementation using bitwise operations for O(1) collision detection and placement
- **Two-Pointer Row Clearing** — stable read/write compaction for O(N) clearing with zero extra allocations
- **Zero-Allocation Parsing** — `std::string_view` throughout to eliminate heap fragmentation
- **Compile-Time LUTs** — all piece geometries stored in `static constexpr` lookup tables, no runtime hashing

## Project Structure

```
src/TetrisEngine.hpp          # Templatized engine logic
src/TetrisTypes.hpp           # Standard coordinate-based board and piece types
src/TetrisTypesBitset.hpp     # Optimized bitset board and piece types
test/TetrisEngineTest.cpp     # Unit tests (GoogleTest)
test/drw_test_input.txt       # Unit Test input
test/drw_test_output.txt      # Unit Test output
test/TetrisEnginePerfTest.cpp # Performance benchmark
```

## Building and Running

**Requirements:** C++20 compiler (GCC 10+), GoogleTest

```bash
make test   # build and run unit tests
make perf   # build and run performance benchmark
make clean  # clean build artifacts
```

## Performance

Benchmark: 1 million iterations over standard test cases.

| Engine         | Time     |
|----------------|----------|
| StandardEngine | 2526 ms  |
| BitsetEngine   | 1190 ms  |
| **Speedup**    | **2.12x**|

The bitset implementation doubles throughput via bitwise parallelism and L1 cache residency.

## Implementation Details

### Bitset Math

The board is an array of `uint16_t` where each element is a 10-bit row mask.

```
Collision:      (rows[y + i] & (piece_mask << x)) != 0
Placement:      rows[y + i] |= (piece_mask << x)
Full row check: row == 0x3FF
```

### Row Compaction

Single pass through the board. Non-full rows are moved down to `writePtr`, full rows are skipped. `height = writePtr` at the end drops the tail.
