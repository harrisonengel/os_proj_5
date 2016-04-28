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
  bool receive_msg();
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
  char end_msg[] = "<END>";
  
  MPI_Init(&args, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Main Read loop
  if(rank == 0)
    {
      printf ("[0]: There are %d processes in the system\n", size-1);
      while(true)
	{
	  getline(cin,input);
	  if(string(input).compare("end") == 0)
	    break;

	  run_command(input);

	 
	}
      for(int i =1; i < size; i++)
	{
	  MPI_Send(&end_msg, 5, MPI_CHAR, i, 0, MPI_COMM_WORLD);
	}
      //      MPI_Bcast(&end_msg, 5,MPI_CHAR,0, MPI_COMM_WORLD);
    }
  else
    {
      myClock.init_clock(rank);
      while(myClock.receive_msg());

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
  string s = msg.substr(0,msg.find(":"));

  MPI_Send(msg.c_str(), msg.length(), MPI_CHAR, proc, 0, MPI_COMM_WORLD);
  printf("\t[%d]: Message Sent to %d: Messgage >%s<: Logical Clock = %d\n",rank,proc,s.c_str(),count);

}



bool compare_send(string s)
{
  return(s[0] == '<' && s[1] == 'S' && s[2] == 'E'
	 && s[3] == 'N' && s[4] == 'D' && s[5] == '>' && s[6] == ':');
  
}

bool compare_exec(string s)
{
  return(s[0] == '<' && s[1] == 'E' && s[2] == 'X'
	 && s[3] == 'E' && s[4] == 'C' && s[5] == '>');
}

bool compare_end(string s)
{
  if(s.length() >= 5)
    {
      return(s[0] == '<' && s[1] == 'E'
	     && s[2] == 'N' && s[3] == 'D'
	     && s[4] == '>');
    }
  else
    {
      return false;
    }
}
	   
int extract_count(string s)
{
  int l,r;
  string count;

  count = s.substr(s.find(":")+1);

  return atoi(count.c_str());

}

bool LamportClock::receive_msg()
{
  MPI_Status stat;
  char msg[256], temp_msg[256];
  char *send;
  int proc;
  string temp, temp_num, temp_count;
  MPI_Recv(&msg, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  //printf("Message Received!");
  if(compare_exec(string(msg)))
    {

      exec_event();
    }
  else
    {
      
      if(compare_send(string(msg)))
	{
	  count++;
	  
	  temp = string(msg);
	  temp = temp.substr(temp.find("\"") + 1);
	  temp = temp.substr(0, temp.find("\""));

	  temp_num = string(msg);
	  temp_num = temp_num.substr(temp_num.find_last_of("<")+1, temp_num.find_last_of(">"));

	  send = new char[256];
	  sprintf(send, "%s:%d", temp.c_str(), count);
	  send_msg(string(send), atoi(temp_num.c_str()));

	}
      else if(compare_end(string(msg)))
	{
	  printf("\t[%d]: Logical Clock = %d\n", rank, count);
	  //	  MPI_Finalize();
	  return false;
	}
      else
	{
	  count++;
	  
	  temp = string(msg);
	  temp = temp.substr(0, temp.find(":"));

	  proc = extract_count(string(msg));
	  if(proc > count)
	    {
	      count = proc;
	    }
	    printf("\t[%d]: Message Received from %d: Message >%s<: Logical Clock = %d\n",rank, stat.MPI_SOURCE, temp.c_str(), count);
	}
    }
  return true;

}
