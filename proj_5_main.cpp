#include <iostream>
#include <mpi.h>

using namespace std;

// Classes
class LamportClock{

public:
  int count, rank;
  void init_clock(int r);
  void exec_event();
  void send_msg(string msg, int proc);
  void receive_msg();
};

// Constant variabls
const string EXEC_MSG = "<EXEC>";
const string SEND_MSG = "<SEND>: <%s> <%d>";

// Functions
void run_command(string s);
void send_exec(int proc);

int main(int args, char* argv[])
{
  int numProc, rank, size;
  LamportClock proc_clock;
  string command;
  
  MPI_Init(&args, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Main Read loop
  if(rank == 0)
    {
      printf ("[0]: There are %d processes in the system\n", size - 1);
      while(!command.compare("end"))
	{
	  cin >> command;
	  
	  run_command(command);
	  printf("%s\n", command.c_str());
	}
    }
  else
    {
      


    }

  MPI_Finalize();

}


void run_command(string s)
{
  string command, temp, message;
  int p1, p2;
  
  command = s.substr(0,s.find(" "));
  if(command.compare("exec") == 0)
    {
      p1 = stoi(s.substr(s.find(" ") + 1));
     
    }
  else if(command.compare("send") == 0)
    {
      temp = s.substr(s.find(" ") + 1);
      p1 = stoi(temp.substr(0,temp.find(" ")));
      temp = temp.substr(temp.find(" ") + 1);
      p2 = stoi(temp.substr(0, temp.find(" ")));
      message = temp.substr(temp.find(" ") + 1);

      string final_msg = to_string(sprintf("<SEND>: <%s> <%d>",message.c_str(), p2));
      
      MPI_Send(final_msg.c_str(),final_msg.length(), MPI_CHAR,p1,0, MPI_COMM_WORLD);
    }
  else
    {
      printf("[0]: ERROR: >%s< is not a command.\n", s.c_str());
    }
	     
}


void send_exec(int proc)
{
  MPI_Send(EXEC_MSG.c_str(), EXEC_MSG.length(), MPI_CHAR, proc, 0, MPI_COMM_WORLD);
}

void LamportClock::init_clock(int r)
{
  count = 0;
  rank = r;
}

void LamportClock::exec_event()
{
  count++;
  printf("[%d]: Execution Event: Logical Clock = %d",rank, count);
}

void LamportClock::send_msg(string msg, int proc)
{

  count++;
  MPI_Send(msg.c_str(), msg.length(), MPI_CHAR, proc, 0, MPI_COMM_WORLD);
  printf("[%d]: Message Sent to %d: Messgage >%s<: Logical Clock = %d",rank,proc,msg.c_str(),count);

}

void LamportClock::receive_msg()
{
  MPI_Status stat;
  char msg[256];
  int proc;
  MPI_Recv(&msg, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  
  if(strcmp(msg, EXEC_MSG.c_str()))
    {
      exec_event();
    }
  else if(strcmp(msg, SEND_MSG.c_str()))
    {
      
    }
  else
    {
      printf("[%d]: Message Received from %d: Message >%s<: Logical Clock = %d",rank,proc,msg,count);
    }
}
