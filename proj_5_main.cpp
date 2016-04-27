#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;

// Functions
void parse_command(char com[]);
void receive_command();
bool compare_exec(char com[]);
bool compare_send(char com[]);

int main(int args, char* argv[])
{
  int  rank, size, clock_val, from, to, status;
  string command,input;
 
  clock_val = 0;


  MPI_Init(&args, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Main Read loop
  if(rank == 0)
    {
      printf ("[0]: There are %d processes in the system\n", size-1);
      while(string("end").compare(input))
	{
	  getline(cin,input);
	  command = input.substr(0, input.find(" "));
	  printf("%s\n", command.c_str());
	}
    }
  else
    {
      
      
      printf("Hello World! I'm process %d\n", rank);

    }

  // Finalize MPI
  MPI_Finalize();

}


void parse_command(char com[])
{
  if(compare_exec(com))
    {
      printf("EXEC CALLED");
    }
  else if(compare_send(com))
    {
      printf("Send Called");
    }
  else
    {
      printf("ERROR");
    }

}

bool compare_exec(char com[])
{
  return ((com[0] == 'e') && (com[1] == 'x')
	  && (com[2] == 'e') && (com[3] == 'c')
	  && (com[4] == ' '));

}

bool compare_send(char com[])
{
  return((com[0] == 's') && (com[1] == 'e')
	 && (com[2] == 'n') && (com[3] == 'd')
	 && (com[4] == ' '));


}
