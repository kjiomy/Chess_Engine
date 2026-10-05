# Chess_Engine

A UCI-compliant chess engine written in C++, developed as a university exam project. 
The engine is built with a strong focus on performance and search optimization, utilizing modern computer chess techniques.

## Features

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

To compile the engine with maximum performance optimizations (-O3 and Link-Time Optimization -flto), run the following command in your terminal:

