# Tetris Engine

## Overview

Processes sequences of pre-defined Tetris pieces dropped into a 10-unit wide grid. Calculates the final height of remaining blocks after placement and row-clearing.

## Key Features

- **Templatized Architecture** — single engine logic supporting multiple board implementations via compile-time polymorphism
- **Bitset Board** — better-performance implementation using bitwise operations for O(1) collision detection and placement
- **Two-Pointer Row Clearing** — stable read/write compaction for O(N) clearing with zero extra allocations
- **Zero-Allocation Parsing** — `std::string_view` throughout to eliminate new heap allocations and in the long run heap fragmentation
- **Compile-Time Lookup tables** — all piece geometries stored in `static constexpr` lookup tables, no runtime hashing

## Project Structure

```
src/TetrisEngine.hpp                   # Templatized engine logic
src/TetrisTypes.hpp                    # Standard coordinate-based board and piece types
src/TetrisTypesBitset.hpp              # Optimized bitset board and piece types
test/TetrisEngineTest.cpp              # Full engine tests (GoogleTest)
test/TetrisEngineFunctionLevelTest.cpp # Function-level unit tests (GoogleTest)
test/TetrisEnginePerfTest.cpp          # Performance benchmark
test/drw_test_input.txt                # Test input
test/drw_test_output.txt               # Test output
```

## Building and Running

**Requirements:** C++20 compiler (GCC 10+), GoogleTest

```bash
make test   # build and run unit tests
make perf   # build and run performance benchmark
make clean  # clean build artifacts
```

## Output for the given input

```
Height = [2] after line = Q0
Height = [4] after line = Q0,Q1
Height = [0] after line = Q0,Q2,Q4,Q6,Q8
Height = [2] after line = Q0,Q2,Q4,Q6,Q8,Q1
Height = [4] after line = Q0,Q2,Q4,Q6,Q8,Q1,Q1
Height = [1] after line = I0,I4,Q8
Height = [0] after line = I0,I4,Q8,I0,I4
Height = [2] after line = L0,J2,L4,J6,Q8
Height = [2] after line = L0,Z1,Z3,Z5,Z7
Height = [2] after line = T0,T3
Height = [1] after line = T0,T3,I6,I6
Height = [1] after line = I0,I6,S4
Height = [4] after line = T1,Z3,I4
Height = [3] after line = L0,J3,L5,J8,T1
Height = [1] after line = L0,J3,L5,J8,T1,T6
Height = [2] after line = L0,J3,L5,J8,T1,T6,J2,L6,T0,T7
Height = [1] after line = L0,J3,L5,J8,T1,T6,J2,L6,T0,T7,Q4
Height = [8] after line = S0,S2,S4,S6
Height = [8] after line = S0,S2,S4,S5,Q8,Q8,Q8,Q8,T1,Q1,I0,Q4
Height = [0] after line = L0,J3,L5,J8,T1,T6,S2,Z5,T0,T7
Height = [3] after line = Q0,I2,I6,I0,I6,I6,Q2,Q4
```

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

## Performance

Benchmark: 1 million iterations(each iteration of 10 lines)  over standard test cases.

| Engine            | Time     |   
|-------------------|----------|   
| StandardEngine    | 2526 ms  |   
| BitsetEngine      | 1190 ms  |   
| **Speedup**       | **2.12x**|   

The bitset implementation doubles throughput via bitwise parallelism and L1 cache residency.

