/*
 ============================================================================
 Name        : hw2.c
 Author      : Mahir Ozturk
 Version     :
 Copyright   : 
 Description : Performance comparisons of different MPI_Bcast implemantations
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "mpi.h"

/*
 * Macros
 */
#define DEF_ARRAY_LENGTH	10000000

/*
 * Function declerations
 */
int My_Bcast_RecursiveDoubling(void *buffer, int count, MPI_Datatype datatype, int root,
        MPI_Comm comm );

int My_Bcast_Naive(void *buffer, int count, MPI_Datatype datatype, int root,
        MPI_Comm comm );


/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
	int node;
	int size;
	int i;
	long szDataLen;
	double *pdData;
	double T1, T2, T3, T4;

	MPI_Init(&argc,&argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &node);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	if(argc > 1) {
		szDataLen = atoi(argv[1]);
	} else {
		szDataLen = DEF_ARRAY_LENGTH;
	}

	pdData = (double *)malloc(szDataLen*sizeof(double));
	if(pdData == 0) {
		printf("Memory allocation has failed!!!\n");
		return 0;
	}

	if(node == 0) {

		// Seed the random number generator to get different results each time
		srand(time(NULL));

		/* fill the vectors with random values */
		for (i = 0; i < szDataLen; i++) {
			pdData[i] = ((double)rand()/(double)RAND_MAX);
		}
	}

	T1 = MPI_Wtime();
	MPI_Bcast((void *)pdData, szDataLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	T2 = MPI_Wtime();
	My_Bcast_Naive((void *)pdData, szDataLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	T3 = MPI_Wtime();
	My_Bcast_RecursiveDoubling((void *)pdData, szDataLen, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	T4 = MPI_Wtime();

	if(node == 0) {
		printf("MPI_Bcast()                  : %f seconds from process %d of %d\n", T2-T1, node, size);
		printf("My_Bcast_Naive()             : %f seconds from process %d of %d\n", T3-T2, node, size);
		printf("My_Bcast_RecursiveDoubling() : %f seconds from process %d of %d\n", T4-T3, node, size);
	}


	MPI_Finalize();

	free(pdData);

	return 0;
}

/**
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int My_Bcast_Naive(void *buffer, int count, MPI_Datatype datatype, int root,
        MPI_Comm comm ) {

	int node;
	int size;
	MPI_Comm_rank(comm, &node);
	MPI_Comm_size(comm, &size);

	if (node == root) {
		// If we are the root process, send our data to everyone
		int i;
		for (i = 0; i < size; i++) {
			if (i != node) {
				MPI_Send(buffer, count, datatype, i, 0, comm);
			}
		}
	} else {
		// If we are a receiver process, receive the data from the root
		MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
	}

	return 0;
}

/**
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int My_Bcast_RecursiveDoubling(void *buffer, int count, MPI_Datatype datatype, int root,
        MPI_Comm comm ) {

	int node;
	int size;
	MPI_Comm_rank(comm, &node);
	MPI_Comm_size(comm, &size);

	int xmit_node, recv_node;
	int nstep = (int)log2((double)size);
	int i, j;

	for (i = 0; i < nstep; i++) {
		for (j = 0; j < pow(2,i); j++) {

			xmit_node = j*(size/pow(2,i));
			recv_node = xmit_node + size/pow(2,i+1);

			if (node == xmit_node) {
				MPI_Send(buffer, count, datatype, recv_node, 0, comm);
			} else if (node == recv_node){
				// If we are a receiver process, receive the data from the root
				MPI_Recv(buffer, count, datatype, xmit_node, 0, comm, MPI_STATUS_IGNORE);
			}
		}
	}

	return 0;
}


