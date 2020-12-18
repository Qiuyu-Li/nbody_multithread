#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include <pthread.h>

#define SOFTENING 1e-9f

typedef struct
{
    float x, y, z;
    float vx, vy, vz;
} Particle;

/* Define the Param struct for calc_force */
typedef struct
{
    Particle *p;
    float dt;
    int n;
    int nl;
    int start;
}Params;
/* ------------------------ */

void ran_init(float *data, int n);

/* Replace the argument declaration in calc_force function by void* args */
void* calc_force(void* args);
/* ------------------------ */

int main(const int argc, const char **argv)
{
    int i,iter;
    FILE *datafile = NULL;
    int nParticles = 3000;

    /* Total thread num */
    int nThreads = 1;
    if (argc > 1)
        nThreads = atoi(argv[1]);
    pthread_t t[nThreads];
    void* status;
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
    for (iter = 1; iter <= nIters; iter++)
    {
        printf("iteration:%d\n", iter);

        for (i = 0; i < nParticles; ++i)
            fprintf(datafile, "%f %f %f \n", p[i].x, p[i].y, p[i].z);

        StartTimer();

        /* The O(n^2) loop */
        Params param[nThreads];
        for (i=0; i < nThreads; ++i)
        {
            param[i].p = p;
            param[i].dt = dt;
            param[i].n = nParticles;
            param[i].nl = nParticles/nThreads;
            param[i].start = param[i].nl * i;
            pthread_create(&t[i],NULL,calc_force,(void*) &param[i]);
        }

        for (i=0;i<nThreads;i++) {
            pthread_join(t[i], &status);
        }
        /* ----------------------- */

        for (i = 0; i < nParticles; i++)
        {
            p[i].x += p[i].vx * dt;
            p[i].y += p[i].vy * dt;
            p[i].z += p[i].vz * dt;
        }
        const double tElapsed = GetTimer() / 1000.0;
        //printf("\n");
        if (iter > 1)
        {
            totalTime += tElapsed;
        }
    }
    fclose(datafile);
    double avgTime = totalTime / (double)(nIters - 1);

    printf("avgTime: %f   totTime: %f \n", avgTime, totalTime);
    free(buf);
    pthread_exit(NULL);
}

void ran_init(float *data, int n)
{
    for (int i = 0; i < n; i++)
    {
        data[i] = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
    }
}

void* calc_force(void* args)
{   
    /* Decalre the parameters */
    Params* param = (Params*) args;
    Particle* p = param->p;
    float dt = param->dt;
    int n = param->n;
    int nl = param->nl;
    int start = param->start;
    int end = start + param->nl;
    /* ------------------------ */

    /* Try only re-allocate the first layer of the loop */
    for (int i = start; i < end; i++)
    /* ------------------------ */
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