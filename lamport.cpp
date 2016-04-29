/************************************
 *
 * Harrison Engel and Duy Dang
 * Project 4 part 1: Lamport Logical Clocks
 * Due April 29, 2016 
 *
 ************************************/
#include <iostream>
#include <mpi.h>
#include <string>


using namespace std;

/***********************************
 *
 *     LamportClock
 * 
 * Class used to keep track of
 * each logical clock. 
 *
 * Variabls
 * -------
 * lamport_clock: int 
 *    - Keeps track of the logical clock
 *
 * rank: int
 *    - Current rank of process with clock
 *
 * Functions
 * ---------
 * init_clock(int r):
 *     - creates a new clock with starting
 *       value of 0. r is its rank.
 *
 * exec_event():
 *     - Simulates executing an event.
 *
 * send_msg(string msg, int proc)
 *     - Simulates sending a message
 *   
 * receive_msg():
 *     - Simulates receiving a message
 ***********************************/
class LamportClock{

public:
  int lamport_clock, rank;
  void init_clock(int r);
  void exec_event();
  void send_msg(string msg, int proc);
  bool receive_msg();
};



/***********************************
 *   Function Declarations
 **********************************/

void run_command(string s);
void parse_command(char com[]);
void receive_command();



/*********************************
 *    Main
 *
 * Main function of the lamport 
 * logical clock simulation.
 * Uses MPI to pass messages between
 * processes and change clock values.
 *
 * Variables
 * ---------
 * rank: int
 *     - Rank of the process, set using MPI_Comm_rank
 * size: int
 *     - Number of total processes in the system.
 * input: string
 *     - Stores lines from cin.
 * myClock: LamportClock
 *     - LamportClock for each process
 * end_msg: char[]
 *     - Stores the message to send when simulation ends
 ********************************/
int main(int args, char* argv[])
{
  int  rank, size;
  string input;
  LamportClock myClock;
  char end_msg[] = "<END>";
  
  MPI_Init(&args, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Main Read loop
  if(rank == 0)
    {
      getline(cin,input);
      printf ("[0]: There are %d processes in the system\n", atoi(input.c_str()));
      while(true)
	{
	  getline(cin,input);
	  if(string(input).compare("end") == 0)
	    {
	      printf("[0]: Simulation Ending.\n");
	      break;
	    }
	  run_command(input);

	 
	}
      for(int i =1; i < size; i++)
	{
	  MPI_Send(&end_msg, 5, MPI_CHAR, i, 0, MPI_COMM_WORLD);
	}

    }
  else
    {
      myClock.init_clock(rank);
      while(myClock.receive_msg());

    }
  
  // Finalize MPI
  MPI_Finalize();

}


/***********************************
 *     Run Command
 * 
 * Method used to run a command from the
 * "orchestrator" (process 0) process.
 *
 * Variables
 * ---------
 * command: string
 *     - Stores the command (exec or send)
 * temp: string
 *     - String used for processing
 * message: string
 *     - String used to create final message to send
 *       to another process.
 * p1: int:
 *     - pid of the process to send from (in send)
 * p2: int:
 *     - pid of the process to send to (in send)
 * final_msg:
 *     - Final message to send because it must be
 *       sent as a char *. 
 **********************************/
void run_command(string s)
{
  string command, temp, message;
  int p1, p2;
  char *final_msg;
  MPI_Status status;
  
  command = s.substr(0,s.find(" "));

  if(command.compare("exec") == 0)
    {
      
      p1 = atoi(s.substr(s.find(" ") + 1).c_str());

      final_msg = new char[2];
      MPI_Send("<EXEC>",6, MPI_CHAR,p1,0, MPI_COMM_WORLD);
      // Wait for process to finish EXEC command.
      MPI_Recv(final_msg, 2, MPI_CHAR,p1, 0, MPI_COMM_WORLD, &status);
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
      MPI_Recv(&final_msg, 2, MPI_CHAR, p2, 0, MPI_COMM_WORLD, &status);
    }
  else
    {
      printf("[0]: ERROR: >%s< is not a command.\n", s.c_str());
    }
	     
}


/**************************
 *    Init Clock
 * Initializes the clock.
 * 
 * Parameters
 * ----------
 * r: int
 *     - The rank for MPI of this process
 *************************/
void LamportClock::init_clock(int r)
{
  lamport_clock = 0;
  rank = r;
}

/***************************
 *
 *     Exec Event
 * Simulates executing an event.
 * 
 **************************/
void LamportClock::exec_event()
{
  char msg[] = "OK";
  lamport_clock++;
  printf("\t[%d]: Execution Event: Logical Clock = %d\n",rank, lamport_clock);
  MPI_Send(&msg, 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
}

/****************************
 *     Send Message
 * Simulates sending a message from
 * one lamport clock process to another.
 *
 * Parameters
 * ----------
 * msg: string
 *     - The message to be sent (requires some processing)
 * proc: int
 *     - The process to send the message to
 ****************************/
void LamportClock::send_msg(string msg, int proc)
{
  string s = msg.substr(0,msg.find(":"));
  
  MPI_Send(msg.c_str(), msg.length(), MPI_CHAR, proc, 0, MPI_COMM_WORLD);
  printf("\t[%d]: Message Sent to %d: Messgage >%s<: Logical Clock = %d\n",rank,proc,s.c_str(),lamport_clock);


}

/***************************
 * Comparison Functions
 *
 * The compare_send, compare_exec,
 * compare_end, and 
 * extract_lamport_clock are
 * all utilty functions to
 * make processing messages easier.
 *
 **************************/


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
	   
int extract_lamport_clock(string s)
{
  int l,r;
  string lamport_clock;

  lamport_clock = s.substr(s.find(":")+1);

  return atoi(lamport_clock.c_str());

}

/************************
 *     Receive Message
 * 
 * Models receiving a message in the
 * lamport clock process. It must differentiate
 * between a message sent by the conductor and 
 * a message sent by another clock process. Then
 * it must decypher the message, and respond 
 * accordingly.
 *
 * Variables
 * ----------
 * stat: MPI_Status
 *     - Stores info about the sending process during a receive.
 * msg: char[256]
 *     - Used to store messages
 * temp_msg: char[256]
 *     - Used during message processing
 * send: char*
 *     - Character array that is ultimately sent
 * proc: int
 *     - The number of the procedure to send to
 * temp: string
 *     - Temporary string used for string processing
 * temp_num: 
 *     - Temp string used for string processing
 * temp_lamport:
 *     - Temp string used for string processing
 *
 ************************/
bool LamportClock::receive_msg()
{
  MPI_Status stat;
  char msg[256], temp_msg[256];
  char *send;
  int proc;
  string temp, temp_num, temp_lamport_clock;
  char ok[] = "OK";
  
  MPI_Recv(&msg, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  //printf("Message Received!");
  //  cout << string(msg) << endl;
  if(compare_exec(string(msg)))
    {

      exec_event();
    }
  else
    {
      
      if(compare_send(string(msg)))
	{
	  lamport_clock++;
	  
	  temp = string(msg);
	  temp = temp.substr(temp.find("\"") + 1);
	  temp = temp.substr(0, temp.find("\""));

	  temp_num = string(msg);
	  temp_num = temp_num.substr(temp_num.find_last_of("<")+1, temp_num.find_last_of(">"));

	  send = new char[256];
	  sprintf(send, "%s:%d", temp.c_str(), lamport_clock);
	  send_msg(string(send), atoi(temp_num.c_str()));

	}
      else if(compare_end(string(msg)))
	{
	  printf("\t[%d]: Logical Clock = %d\n", rank, lamport_clock);

	  return false;
	}
      else
	{
	  lamport_clock++;
	  
	  temp = string(msg);
	  temp = temp.substr(0, temp.find(":"));

	  proc = extract_lamport_clock(string(msg));
	  if(proc > lamport_clock)
	    {
	      lamport_clock = proc;
	    }
	    printf("\t[%d]: Message Received from %d: Message >%s<: Logical Clock = %d\n",rank, stat.MPI_SOURCE, temp.c_str(), lamport_clock);
	    MPI_Send(&ok, 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD); // Send message to get conductor running again
	}
    }
  return true;

}
 
