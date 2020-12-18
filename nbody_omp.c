#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define SOFTENING 1e-9f

typedef struct
{
    float x, y, z;
    float vx, vy, vz;
} Particle;

void ran_init(float *data, int n);

/* Add new argument: thread number */
void calc_force(Particle *p, float dt, int n, int nt);
/* -------------------------------- */

int main(const int argc, const char **argv)
{
    FILE *datafile = NULL;
    int nParticles = 3000;

    /* Total thread num */
    int nThreads = 1;
    if (argc > 1)
        nThreads = atoi(argv[1]);
    /* ----------------- */

    if (argc > 2)
        nParticles = atoi(argv[2]);

    const float dt = 0.01f;
    const int nIters = 200;

    float *buf = malloc(nParticles * sizeof(Particle));
    Particle *p = (Particle *)buf;

    ran_init(buf, 6 * nParticles);

    double totalTime = 0.0;

    datafile = fopen("particles.dat", "w");
    fprintf(datafile, "%d %d %d\n", nParticles, nIters, 0);

    /* ------------------------------*/
    /*     MAIN LOOP                 */
    /* ------------------------------*/
    for (int iter = 1; iter <= nIters; iter++)
    {
        printf("iteration:%d\n", iter);

        for (int i = 0; i < nParticles; ++i)
            fprintf(datafile, "%f %f %f \n", p[i].x, p[i].y, p[i].z);

        StartTimer();

        calc_force(p, dt, nParticles,nThreads);

        for (int i = 0; i < nParticles; i++)
        {
            p[i].x += p[i].vx * dt;
            p[i].y += p[i].vy * dt;
            p[i].z += p[i].vz * dt;
        }

        const double tElapsed = GetTimer() / 1000.0;
        if (iter > 1)
        {
            totalTime += tElapsed;
        }
    }

    fclose(datafile);
    double avgTime = totalTime / (double)(nIters - 1);

    printf("avgTime: %f   totTime: %f \n", avgTime, totalTime);
    free(buf);
}

void ran_init(float *data, int n)
{
    for (int i = 0; i < n; i++)
    {
        data[i] = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
    }
}

void calc_force(Particle *p, float dt, int n, int nt)
{
    /* Omp calculation */
#pragma omp parallel default(none) shared(p, dt, n) num_threads(nt)
    {
#pragma omp for
    /* ---------------- */
        for (int i = 0; i < n; i++)
        {
            float Fx = 0.0f;
            float Fy = 0.0f;
            float Fz = 0.0f;

            for (int j = 0; j < n; j++)
            {
                float dx = p[j].x - p[i].x;
                float dy = p[j].y - p[i].y;
                float dz = p[j].z - p[i].z;
                float distSqr = dx * dx + dy * dy + dz * dz + SOFTENING;
                float invDist = 1.0f / sqrtf(distSqr);
                float invDist3 = invDist * invDist * invDist;

                Fx += dx * invDist3;
                Fy += dy * invDist3;
                Fz += dz * invDist3;
            }
            p[i].vx = dt * Fx;
            p[i].vy = dt * Fy;
            p[i].vz = dt * Fz;
        }
    }
}