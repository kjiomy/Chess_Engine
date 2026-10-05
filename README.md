# Chess_Engine

A UCI-compliant chess engine written in C++, developed as a university exam project. 
The engine is built with a strong focus on performance and search optimization, utilizing modern computer chess techniques.

## Features

<<<<<<< HEAD
- Board Representation: Bitboards
- Protocol: Fully supports the UCI (Universal Chess Interface) protocol.
- Search Algorithms:
  - Negamax search framework
  - Iterative Deepening
  - Quiescence Search (to mitigate the horizon effect)
- Move Ordering & Optimizations:
  - MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
  - Transposition Tables (for caching previously searched positions)

## Compilation
=======
- **Board Representation:** Bitboards
- **Protocol:** Fully supports the UCI (Universal Chess Interface) protocol.
- **Search Algorithms:**
  - Negamax search framework
  - Iterative Deepening
  - Quiescence Search (to mitigate the horizon effect)
- **Move Ordering & Optimizations:**
  - MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
  - Transposition Tables (for caching previously searched positions)
 
## Compilation

To compile the engine on Linux/Windows you'll need to change the CXX on the MAKEFILE.

## Perft results, while optimizing

Starting Point:

| Depth | Nodes | Time (s) | NPS |
|:---:|---:|---:|---:|
| 1 | 20 | 1.146e-05 | 1,745,200 |
| 2 | 400 | 3.8624e-05 | 10,356,255 |
| 3 | 8,902 | 0.000869083 | 10,242,980 |
| 4 | 197,281 | 0.0186704 | 10,566,495 |
| 5 | 4,865,609 | 0.47753 | 10,189,126 |
| 6 | 119,060,324 | 11.2894 | 10,546,196 |
>>>>>>> 71cdff254a726cc6d6de2e037ac9d13dc047dd47

To compile the engine with maximum performance optimizations (-O3 and Link-Time Optimization -flto), run the following command in your terminal:

<<<<<<< HEAD
=======
After moving movegeneration and implementing inline functions:

| Depth | Nodes | Time (s) | NPS |
|:---:|---:|---:|---:|
| 1 | 20 | 6.98e-06 | 2,865,329 |
| 2 | 400 | 3.3108e-05 | 12,081,672 |
| 3 | 8,902 | 0.00102368 | 8,696,102 |
| 4 | 197,281 | 0.0162119 | 12,168,899 |
| 5 | 4,865,609 | 0.35752 | 13,609,320 |
| 6 | 119,060,324 | 7.18962 | 16,560,024 |

---

After changing the single piece_variables to a unified array:

| Depth | Nodes | Time (s) | NPS |
|:---:|---:|---:|---:|
| 1 | 20 | 3.582e-06 | 5,583,472 |
| 2 | 400 | 2.5007e-05 | 15,995,521 |
| 3 | 8,902 | 0.000549997 | 16,185,542 |
| 4 | 197,281 | 0.0123092 | 16,027,086 |
| 5 | 4,865,609 | 0.303075 | 16,054,152 |
| 6 | 119,060,324 | 7.1152 | 16,733,227 |
>>>>>>> 71cdff254a726cc6d6de2e037ac9d13dc047dd47
