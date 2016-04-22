#include <mpi.h>
#include <stdio.h>
#include <string.h>

using namespace std;

// Functions
void parse_command(char *com);

int main(int args, char* argv[])
{
  int numProc, rank, size;
  char command[256]

  cin >> numProc;

  MPI_Init(&args, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Main Read loop
  if(rank == 0)
    {
      printf ("[0]: There are %d processes in the system\n", numProc);
      while(!"end".compare(command))
	{
	  cin >> command;
	  
	  parse_command(command);

	}
    }
  else
    {
      


    }

}
