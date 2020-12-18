all: nbody_omp

nbody_omp: nbody_omp.c timer.c
	gcc -fopenmp nbody_omp.c timer.c -o nbody_omp -lm

clean:
	rm -f nbody_omp