#include <iostream>
#include <mpi.h>
#include <string>


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
void parse_command(char com[]);
void receive_command();


int main(int args, char* argv[])
{
  int  rank, size, from, to, status;
  string command,input;
  LamportClock myClock;
  
  
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
	  run_command(input);
	}
    }
  else
    {
      myClock.init_clock(rank);
      while(true)
	{
	  myClock.receive_msg();

	}

    }

  // Finalize MPI
  MPI_Finalize();

}


void run_command(string s)
{
  string command, temp, message;
  int p1, p2;
  char *final_msg;
  
  command = s.substr(0,s.find(" "));

  if(command.compare("exec") == 0)
    {
      
      p1 = atoi(s.substr(s.find(" ") + 1).c_str());


      MPI_Send("<EXEC>",6, MPI_CHAR,p1,0, MPI_COMM_WORLD);
    }
  else if(command.compare("send") == 0)
    {
      temp = s.substr(s.find(" ") + 1);

      p1 = atoi(temp.substr(0,temp.find(" ")).c_str());

      temp = temp.substr(temp.find(" ") + 1);
      p2 = atoi(temp.substr(0, temp.find(" ")).c_str());
      printf("p1 : %d , p2 : %d \n", p1, p2);
      message = temp.substr(temp.find(" ") + 1);

      final_msg = new char[256];
      sprintf(final_msg,"<SEND>: <%s> <%d>",message.c_str(), p2);

     
      MPI_Send(final_msg,string(final_msg).length(), MPI_CHAR, p1,0, MPI_COMM_WORLD);

    }
  else
    {
      printf("[0]: ERROR: >%s< is not a command.\n", s.c_str());
    }
	     
}


void LamportClock::init_clock(int r)
{
  count = 0;
  rank = r;
}

void LamportClock::exec_event()
{
  count++;
  printf("\t[%d]: Execution Event: Logical Clock = %d\n",rank, count);
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
  string temp, temp_msg;
  MPI_Recv(&msg, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  //printf("Message Received!");
  if(EXEC_MSG.compare(msg) == 0)
    {
      exec_event();
    }
  else
    {
      temp = string(msg).substr(0,string(msg).find(":") );
      if(temp.compare("<SEND>"))
	{
	  temp_msg = string(msg).substr(string(msg).find("\""));
	  temp_msg = temp_msg.substr(0,temp_msg.find("\""));

	  send_msg(temp.substr(0,string(msg).find("\"")), 1);

	}
      else
	{
	  printf("\tMessage Received from %d: Message >%s<: Logical Clock = %d", stat.MPI_SOURCE, msg, count);
	}
    }

}
