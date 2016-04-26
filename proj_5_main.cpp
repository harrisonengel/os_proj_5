#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;

// Functions
void parse_command(char *com);

int main(int args, char* argv[])
{
  int numProc, rank, size;
  char command[256];



  MPI_Init(&args, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Main Read loop
  if(rank == 0)
    {
      cin >> numProc;
      printf ("[0]: There are %d processes in the system\n", numProc);
      /*      while(!string("end").compare(command))
	{
	  cin >> command;
	  
	  parse_command(command);

	  }*/
    }
  else
    {
      
      // Message-Passing process code
      printf("Hello World! I'm process %d\n", rank);

    }

  // Finalize MPI
  MPI_Finalize();

}


void parse_command(char *com)
{

  // Do some command parsing

}
