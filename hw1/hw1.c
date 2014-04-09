/*
 ============================================================================
 Name        : hw1.c
 Author      : Mahir Ozturk
 Version     :
 Copyright   : 
 Description : Parallelization of dot product operation using MPI 
 								libraries, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "mpi.h"

#define DEF_ARRAY_LENGTH	10000000

/* argv[1]: array size */
int main(int argc, char **argv)
{
	int node;
	int size;
	int i;
	int rval;
	long szVecLen;
	long szVecParLen;

	double *pdVecA;
	double *pdVecB;

	double *pdParVecA;
	double *pdParVecB;

	double T1, T2, T3, T4, T5;

	double dDotProduct = 0;
	double dGlbDotProduct;


	MPI_Init(&argc,&argv);


	MPI_Comm_rank(MPI_COMM_WORLD, &node);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	T1 = MPI_Wtime();

	if(argc > 1) {
		szVecLen = atoi(argv[1]);
	} else {
		szVecLen = DEF_ARRAY_LENGTH;
	}

	if(node == 0) {

		/* Create vectors A & B */
		pdVecA = (double *)malloc(szVecLen*sizeof(double));
		pdVecB = (double *)malloc(szVecLen*sizeof(double));

		if(pdVecA == 0 || pdVecB == 0) {
			printf("Memory allocation has failed!!!\n");
			return 0;
		}

		// Seed the random number generator to get different results each time
		srand(time(NULL));

		/* fill the vectors with random values */
		for (i = 0; i < szVecLen; i++) {
			pdVecA[i] = ((double)rand()/(double)RAND_MAX);
			pdVecB[i] = ((double)rand()/(double)RAND_MAX);
		}
	}

	szVecParLen = szVecLen / size;

	pdParVecA = (double *)malloc(szVecParLen*sizeof(double));
	pdParVecB = (double *)malloc(szVecParLen*sizeof(double));

	if(pdParVecA == 0 || pdParVecB == 0) {
		printf("Memory allocation has failed!!!\n");
		return 0;
	}

	T2   = MPI_Wtime();

	rval = MPI_Scatter((void *)pdVecA, szVecParLen, MPI_DOUBLE, (void *)pdParVecA, szVecParLen, MPI_DOUBLE,
			0, MPI_COMM_WORLD);
	if (rval) {
		printf("MPI_Scatter() error %d\n", rval);
		return 0;
	}

	rval = MPI_Scatter((void *)pdVecB, szVecParLen, MPI_DOUBLE, (void *)pdParVecB, szVecParLen, MPI_DOUBLE,
			0, MPI_COMM_WORLD);
	if (rval) {
		printf("MPI_Scatter() error %d\n", rval);
		return 0;
	}

	T3   = MPI_Wtime();

	/* vector multiplication */
	for (i = 0; i < szVecParLen; i++) {
		dDotProduct += pdParVecA[i]*pdParVecB[i];
	}
	//printf("A.B = %.3f from process %d of %d\n", dDotProduct, node, size);

	T4   = MPI_Wtime();

	MPI_Reduce(&dDotProduct, &dGlbDotProduct, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	T5   = MPI_Wtime();

	// Print timings
	if (node == 0) {
		printf("Initialization : %f seconds from process %d of %d\n", T2-T1, node, size);
		printf("MPI_Scatter    : %f seconds from process %d of %d\n", T3-T2, node, size);
		printf("Dot Product    : %f seconds from process %d of %d\n", T4-T3, node, size);
		printf("MPI_Reduce     : %f seconds from process %d of %d\n", T5-T4, node, size);
		printf("DP+MPI_Reduce  : %f seconds from process %d of %d\n", T5-T3, node, size);
		printf("Result = %.3f\n", dGlbDotProduct);
	}

	MPI_Finalize();

	if (node == 0) {
		free(pdVecA);
		free(pdVecB);
	}
	free(pdParVecA);
	free(pdParVecB);

	return 0;
}

