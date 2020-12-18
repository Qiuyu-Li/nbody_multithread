# NBody in a multithreading style

## Table of Contents

- [The Problem](#the-problem)
- [Analysis of the problem](#analysis-of-the-problem)
- [How to run](#how-to-run)
- [Result](#result)

## The Problem

Calculate the combined effects of gravity each body received from every other bodies, and accelerate the main loop with Open MP.

## Analysis of the problem

The key is the tread-off between the time for creating threads and the time for the multithread schedule. Therefore, the optimal value of threads should be a function of nParticle.
The maximum number of threads is 32678 on my machine.
Complexity = O(n^2 * m), where n is the number of particles and m is the number of iteration times.
Since n^2 is larger than 32678, there's no need to consider m -- The best we can do is to optimize the n^2 schedule.
I expect that there's no need to consider n^2 -- that is to say, the nested loop. We can just consider allocating one layer of the loop into multiple threads, so that the calculation each thread will be responsible for is n/nThread * n, equal to n^2 / nThread. The equation is not satisfied only if nThread > (n=3000), but I expect that the optimal thread number won't be so large.
Therefore, the problem is even simpler than arr_sum -- there's no global variable in the multithread schedule.
The above analysis is only true when the loops are homogeneous process.

## How to run

```
$make
```

```
$./nbody_omp *the_number_of_threads*
```

## Result

By grid search, the optimal value of the number of Threads is between 64 ~ 544 when nParticle = 3000. 128 threads would be ideal enough.
Note: Number of threads = 0 refers to the serial performance

The detailed table:
| Number of threads	| avgTime | totTime |
| --- | --- | --- |
| 0	| 0.382315	| 76.080717 |
| 1	| 0.34724	| 69.100697 |
| 2	| 0.180746	| 35.968499 |
| 4	| 0.11973	| 23.826282 |
| 8	| 0.045128	| 8.980479 |
| 16	| 0.048504	| 9.652317 |
| 32	| 0.04583	| 9.120198 |
| 64	| 0.045462	| 9.046865 |
| 128	|  0.044204	| 8.796531 |
| 216	| 0.044041	| 8.764255 |
| 256	| 0.043649	| 8.686162 |
| 260	| 0.044641	| 8.883613 |
| 304	| 0.044071	| 8.770207 |
| 512	| 0.044388	| 8.833212 |
| 544	| 0.045275	| 9.009745 |
| 1024	| 0.049851	| 9.920265 |
