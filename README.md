# Assignment 2 --- String Matching

## Build

Run these commands from the project folder using a C++17 compiler such as MinGW GCC. Each command builds one program:

```powershell
g++ -std=c++17 -O2 "Knuth-Morris-Pratt Implementation/KMP Main Algorithm.cpp" -o kmp.exe
g++ -std=c++17 -O2 "Rabin-Karp Implementation/RK Main Algorithm.cpp" -o rabin-karp.exe
g++ -std=c++17 -O2 "Benchmark String Matching.cpp" -o benchmark.exe
g++ -std=c++17 -O2 "Correctness Checker.cpp" -o correctness-checker.exe
```

## Run The Algorithms

Run either standalone program, then enter the text and pattern on separate lines:

```powershell
.\kmp.exe
.\rabin-karp.exe
```

Rabin-Karp accepts an optional modulus, for example `.\rabin-karp.exe 1000000007`.

## Run The Benchmark

Pass one test file and optionally the number of repetitions (default: 10):

```powershell
.\benchmark.exe ".\Test Cases\Weak Modulus Test.txt" 10
```

Each test file starts with the number of cases. Each case then has three lines: modulus, pattern, and text. For example:

```text
2
101
aba
ababa
2
aaa
aaaaa
```

Patterns and texts must each fit on one line; embedded newline bytes are not supported.

## Run The Correctness Checker

Run with defaults (10,000 random cases), or supply optional values in this order: number of cases, maximum text length, maximum pattern length, random seed, and Rabin-Karp modulus.

```powershell
.\correctness-checker.exe
.\correctness-checker.exe 10000 256 64 12345 2
```

