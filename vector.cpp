/************************************
 *
 * Harrison Engel and Duy Dang
 * Project 4 part 2: Vector Logical Clocks
 * Due April 29, 2016 
 *
 ************************************/
#include <iostream>
#include <mpi.h>
#include <string>


using namespace std;

/***********************************
 *
 *     VectorClock
 * 
 * Class used to keep track of
 * each logical clock. 
 *
 * Variabls
 * -------
 * vector_clock: int 
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
class VectorClock{

public:
  int* vector_clock;
  int rank, size;
  void init_clock(int r, int s);
  void exec_event(int rank, int size);
  void send_msg(string msg, int proc, int rank);
  bool receive_msg(int rank);
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
 * Main function of the vector 
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
 * myClock: VectorClock
 *     - VectorClock for each process
 * end_msg: char[]
 *     - Stores the message to send when simulation ends
 ********************************/
int main(int args, char* argv[])
{
  int  rank, size;
  string input;
  VectorClock myClock;
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
	    break;

	  run_command(input);


	 
	}
      for(int i =1; i < size; i++)
	{
	  MPI_Send(&end_msg, 5, MPI_CHAR, i, 0, MPI_COMM_WORLD);
	}

    }
  else
    {

      myClock.init_clock(rank, size);

      while(myClock.receive_msg(rank));

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
      // <SEND>: <"message"> <#>
      sprintf(final_msg,"<SEND>: <%s> <%d>",message.c_str(), p2);
           
      MPI_Send(final_msg,string(final_msg).length(), MPI_CHAR, p1,0, MPI_COMM_WORLD);

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
void VectorClock::init_clock(int r, int s)
{
  vector_clock = new int[s];

  for(int i =0; i < s; i++)
    {
      vector_clock[i] = 0;
    }

  rank = r;
  size = s;
}

/***************************
 *
 *     Exec Event
 * Simulates executing an event.
 * 
 **************************/
void VectorClock::exec_event(int rank, int size)
{
  printf("\t[%d]: Execution Event: Logical Clock = ",rank);


  //Print time stamp
  printf("[");

  for (int i = 0; i < size - 1; i++)
  {
    printf("%d, ",vector_clock[i]);
  }

  printf("%d]\n", vector_clock[size-1]);
  
}

/****************************
 *     Send Message
 * Simulates sending a message from
 * one vector clock process to another.
 *
 * Parameters
 * ----------
 * msg: string
 *     - The message to be sent (requires some processing)
 * proc: int
 *     - The process to send the message to
 ****************************/
void VectorClock::send_msg(string msg, int proc, int rank)
{


  string s = msg.substr(0,msg.find("["));

  MPI_Send(msg.c_str(), msg.length(), MPI_CHAR, proc, 0, MPI_COMM_WORLD);


  printf("\t[%d]: Message Sent to %d: Messgage >%s<: Logical Clock = ",rank,proc,s.c_str());

  //print vector clock
  printf("[");

  for (int i = 0; i < size - 1; i++)
  {
    printf("%d, ",vector_clock[i]);
  }

  printf("%d]\n", vector_clock[size-1]);

}

/***************************
 * Comparison Functions
 *
 * The compare_send, compare_exec,
 * compare_end, and 
 * extract_vector_clock are
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


/************************
 *     Receive Message
 * 
 * Models receiving a message in the
 * vector clock process. It must differentiate
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
 * temp_vector:
 *     - Temp string used for string processing
 *
 ************************/
bool VectorClock::receive_msg(int rank)
{

  vector_clock[rank]++;

  MPI_Status stat;
  char msg[256], temp_msg[256];
  char *send;
  int proc;
  string temp, temp_num, temp_vector_clock;
  MPI_Recv(&msg, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  //printf("Message Received!");
  if(compare_exec(string(msg)))
    {
      exec_event(rank, size);
    }
  else
    {
      
      if(compare_send(string(msg)))
	{
	  
    //temp store the message
	  temp = string(msg);
	  temp = temp.substr(temp.find("\"") + 1);
	  temp = temp.substr(0, temp.find("\""));

    //temp_num store the id of destination process
	  temp_num = string(msg);
	  temp_num = temp_num.substr(temp_num.find_last_of("<")+1, temp_num.find_last_of(">"));


    //sprintf(send, "%s:%d", temp.c_str(), vector_clock);

	  string send = string("");

    send+= temp;

    //Put time stamp at the end of message
    send+= "[";
    for (int i = 0; i < size - 1; i++)
    {
      send+= to_string(vector_clock[i]);
      send+= ",";
    }

    send+= to_string(vector_clock[size-1]);
    send+= "]";

	  send_msg(send, atoi(temp_num.c_str()), rank);

	}
      else if(compare_end(string(msg)))
	{

    vector_clock[rank]--;

    printf("\t[%d]: Vector Clock = ", rank);
    // TODO Big Loop
	  return false;
	}
      else //receive message from another process (not coordinator)
	{

	  temp = string(msg);
	  temp = temp.substr(0, temp.find("["));

	  //int timestamp[size] = extract_vector_clock(string(msg));



    int temp_int;
    string s = string(msg);

    

    int tempValue0 = atoi(s.substr(s.find("[")+1, s.find(",")).c_str());
    if (vector_clock[0] < tempValue0) {
          vector_clock[0] = tempValue0;
        }

    s = s.substr(s.find(",") +1);

    for (int i=1; i<size-1; i++)
      {

        s = s.substr(s.find(",") +1);
        int tempValue = atoi(s.substr(0, s.find(",")).c_str());

        if (vector_clock[i] < tempValue) {
          vector_clock[i] = tempValue;
        }

      }


    int tempValueLast = atoi(s.substr(0, s.find("]")).c_str());

    if (vector_clock[size-1] < tempValueLast) {
      vector_clock[size-1] = tempValueLast;
    }

	    printf("\t[%d]: Message Received from %d: Message >%s<: Logical Clock = ",rank, stat.MPI_SOURCE, temp.c_str());


      //print vector clock
      printf("[");

      for (int i = 0; i < size - 1; i++)
      {
        printf("%d, ",vector_clock[i]);
      }

      printf("%d]\n", vector_clock[size-1]);




	}
    }
  return true;

}
