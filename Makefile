all: nbody nbody_pt nbody_omp

nbody: nbody.c timer.c
	gcc nbody.c timer.c -o nbody -lm
nbody_pt: nbody_pt.c timer.c
	gcc -pthread nbody_pt.c timer.c -o nbody_pt -lm
nbody_omp: nbody_omp.c timer.c
	gcc -fopenmp nbody_omp.c timer.c -o nbody_omp -lm

clean:
	rm -f *.o nbody nbody_pt nbody_omp