# Chess_Engine

A UCI-compliant chess engine written in C++, developed as a university exam project. 
The engine is built with a strong focus on performance and search optimization, utilizing modern computer chess techniques.

## Features

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

to compile the engine on linux/windows you'll need to change the CXX on the MAKEFILE

## perft results, while optimizing

Starting Point:
| Depth | Nodes | Time | NPS |
| Depth 1 | Nodes: 20 | Time: 1.146e-05 s | NPS: 1745200 |
| Depth 2 | Nodes: 400 | Time: 3.8624e-05 s | NPS: 10356255 |
| Depth 3 | Nodes: 8902 | Time: 0.000869083 s | NPS: 10242980 |
| Depth 4 | Nodes: 197281 | Time: 0.0186704 s | NPS: 10566495 |
| Depth 5 | Nodes: 4865609 | Time: 0.47753 s | NPS: 10189126 |
| Depth 6 | Nodes: 119060324 | Time: 11.2894 s | NPS: 10546196 |

---

after moving movegeneration, and implementing inline functions:

| Depth 1 | Nodes: 20 | Time: 6.98e-06 s | NPS: 2865329 |
| Depth 2 | Nodes: 400 | Time: 3.3108e-05 s | NPS: 12081672 |
| Depth 3 | Nodes: 8902 | Time: 0.00102368 s | NPS: 8696102 |
| Depth 4 | Nodes: 197281 | Time: 0.0162119 s | NPS: 12168899 |
| Depth 5 | Nodes: 4865609 | Time: 0.35752 s | NPS: 13609320 |
| Depth 6 | Nodes: 119060324 | Time: 7.18962 s | NPS: 16560024 |

---

after changing the single piece_variables to a unified array:

| Depth 1 | Nodes: 20 | Time: 3.582e-06 s | NPS: 5583472 |
| Depth 2 | Nodes: 400 | Time: 2.5007e-05 s | NPS: 15995521 |
| Depth 3 | Nodes: 8902 | Time: 0.000549997 s | NPS: 16185542 |
| Depth 4 | Nodes: 197281 | Time: 0.0123092 s | NPS: 16027086 |
| Depth 5 | Nodes: 4865609 | Time: 0.303075 s | NPS: 16054152 |
| Depth 6 | Nodes: 119060324 | Time: 7.1152 s | NPS: 16733227 |
