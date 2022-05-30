#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "myProto.h"
#include <time.h>

int main(int argc, char *argv[])
{
   int size, rank, counter = 0, result, excepted = 0;
   double t1, t2;
   int *data;
   MPI_Status status;
   srand(time(NULL)); // Initialization, should only be called once.

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   if (size != 2)
   {
      printf("Run the example with two processes only\n");
      MPI_Abort(MPI_COMM_WORLD, __LINE__);
   }
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   // Divide the tasks between both processes
   if (rank == 0)
   {
      // Allocate memory for the whole array and send a half of the array to other process
      if ((data = (int *)malloc(FOUR_PARTS * sizeof(int))) == NULL)
         MPI_Abort(MPI_COMM_WORLD, __LINE__);

      for (int i = 0; i < FOUR_PARTS; i++)
         data[i] = rand();

      t1 = MPI_Wtime();
      MPI_Send(data + TWO_PARTS, TWO_PARTS, MPI_INT, 1, 0, MPI_COMM_WORLD); // send the second half array to process 1.
   }
   else
   {
      // Allocate memory and reieve a half of array from other process
      if ((data = (int *)malloc(TWO_PARTS * sizeof(int))) == NULL)
         MPI_Abort(MPI_COMM_WORLD, __LINE__);
      MPI_Recv(data, TWO_PARTS, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); // process 1 recieved his half array
   }

   // On each process - perform a first half of its task with OpenMP
#pragma omp parallel for reduction(+ \
                                   : counter)
   for (int i = 0; i < PART; i++)
      if (f(data[i]) > 0)
         counter++;

   // On each process - perform a second half of its task with CUDA
   if (computeOnGPU(data + PART, PART, &counter) != 0)
      MPI_Abort(MPI_COMM_WORLD, __LINE__);

   // Collect the result on one of processes
   MPI_Reduce(&counter, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // take the counter from each process and sum it and then put it at 'result'.

   // Perform a test just to verify that integration MPI + OpenMP + CUDA worked as expected
   if (rank == 0)
   {
      printf("parallel time is %f\n", MPI_Wtime() - t1);
      printf("parallel result is %d\n", result);

      t2 = MPI_Wtime();
      excepted = test(data, FOUR_PARTS);
      if (excepted = result)
         printf("test succeded!!!\n");
      else
         printf("test failed!!!\n");
      printf("sequential result is %d\n", excepted);
      printf("sequential time is %f\n", MPI_Wtime() - t2);
   }

   MPI_Finalize();
   return EXIT_SUCCESS;
}
