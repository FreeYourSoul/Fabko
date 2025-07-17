# CNF

Those are CNF files representing probles that must be runneable from the CLI.
Those are used for testing purposes and validation of the SAT solver which is part of the fabko solution

## Simple Straightforward formula

This is a simple formula that is used to test the most basic SAT solver.

```cnf
p cnf 3 2
1 -3  0	
2  3 -1  0 
```

This problem has many solutions and should be solved by the SAT solver even by accident.

## propagation chain

A CNF that resolve itself at the first propagation step.

```cnf
p cnf 4  4
-1         0  
 1 -2      0
 1  2  -3  0
 3  4      0
```

[cnf file](propagation-chain.cnf)

- the first clause is a unit clause that will be propagated,
- the second clause will be resolved with the first one,
- and the third clause will be resolved with the second one,
- and the fourth clause will be resolved with the third one.

## Conflict

A CNF that has a conflict, it is used to test the SAT solver.

```cnf
p cnf 4 4
-1  4       0
-1  2       0
-1  3       0
-4 -2       0
```

This CNF should produce at least one conflict at resolution time.

[cnf file](simple_conflict.cnf)

## 4 queen problem

Simple 4 queen problem that is used to test the SAT solver.
The problem is to place 4 queens on a 4x4 chessboard such that no two queens threaten each other. This is a problem that
contains 16 variables. one for each square of the 4x4 chessboard. Each variable represents whether a queen is placed on
that square or not. The constraints ensure that no two queens are in the same row, column, or diagonal.

There are 2 solutions to this problem, and the CNF file represents one of them.

```txt

cnf to be found in the file (too big to be included here)

result SAT 1  : ~1 2 ~3 ~4 ~5 ~6 ~7 8 9 ~10 ~11 ~12 ~13 ~14 15 ~16

 0 1 0 0 
 0 0 0 1
 1 0 0 0
 0 0 1 0
 
 result SAT 2 : ~1 ~2 3 ~4 5 ~6 ~7 ~8 ~9 ~10 ~11 12 ~13 14 ~15 ~16
 
 0 0 1 0
 1 0 0 0 
 0 0 0 1
 0 1 0 0
```

- [cnf file](4-queen-problem.cnf)
