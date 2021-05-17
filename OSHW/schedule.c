#include<stdio.h>
#include<stdlib.h>
#include"schedule.h"
typedef struct PR
{

	int ProcID;			
	int ArrivalTime;	
	int BurstTime;		
	int RemainingTime;	
	int FinishTime;		
	int FirstAllocatedTime;
	int TurnAroundTime, WaitingTime, ResponseTime;
	int Terminated;		//Check this process is terminated
	int IsInQueue;		//Process is in ready queue or CPU, it is 1. nor 0 
	int Procmem;		//Process's memory address, when it comes in order, it's value is 1,2,3.....proccnt-1


	



}Proc;

struct PR *PRList;			//read process from txt for dynamic allocate struct PR

int schedulingmethod;		//scheduling method
int proccnt;				//numbers of process
int curprocessing = -1;			//curprocessing has -1 value no process is in cpu, when process comes to cpu, this value has process's memory(Procmem) 
int TQ = 2;					//Time Quantum for RR


//scheduling method functions, these functions return 0 when all process are terminated, return 1 when any processes remain
int SJF(int tick);		//
int FCFS(int tick);		//
int SRTF(int tick);		//
int RR(int tick);		//


void sortingPRList();		//Sorting PRList on the basis of Arrival time
//#####################Queue
//RR is easier when using Queue then not
int front = -1;
int rear = -1;
int *queue;

int IsEmpty(void) {
	if (front == rear)
		return 1;
	else return 0;
}
int IsFull(void) {
	int tmp = (rear + 1) % proccnt; 
	if (tmp == front)
		return 1;
	else
		return 0;
}

void addq(int value) {
	if (IsFull())
		printf("Queue is Full.\n");
	else {
		rear = (rear + 1) % proccnt;
		queue[rear] = value;
	}

}
int deleteq() {
	if (IsEmpty())
		printf("Queue is Empty.\n");
	else {
		front = (front + 1) % proccnt;
		return queue[front];
	}
}


//####################Queue






void read_proc_list(const char* file_name)	
{
	
	FILE* fp = fopen(file_name, "r");
	fscanf(fp, "%d", &proccnt);
	PRList = malloc(sizeof(struct PR)* proccnt);
	queue = malloc(sizeof(int)* proccnt);
	for (int i = 0; i < proccnt; i++)
	{
		PRList[i].ArrivalTime = 0;
		PRList[i].BurstTime = 0;
		PRList[i].ProcID = -1;
		PRList[i].RemainingTime = 1000;
		PRList[i].FinishTime = 0;
		PRList[i].FirstAllocatedTime = 0;
		PRList[i].ResponseTime = 0;
		PRList[i].TurnAroundTime = 0;
		PRList[i].WaitingTime = 0;
		PRList[i].Terminated = 0;
		PRList[i].IsInQueue = 0;
		PRList[i].Procmem = -1;

	}
	for (int i = 0; i < proccnt; i++)		//PRList initiate
	{
		for (int j = 0; j < 3; j++)
		{
			int temp;
			fscanf(fp, "%d", &temp);
			switch (j)
			{
			case 0:
				PRList[i].ProcID = temp;
				PRList[i].Procmem = i;
			case 1:
				PRList[i].ArrivalTime = temp;
			case 2:
				PRList[i].BurstTime = temp;
				PRList[i].RemainingTime = temp;

			}


		}


	}
	sortingPRList();

	for (int i = 0; i < proccnt; i++)
	{
		queue[i] = -1;
	}

}



//Pass method to Global variable schedulingmethod 
void set_schedule(int method)
{
	
	printf("Scheduling method:");	
	schedulingmethod = method;

	if (schedulingmethod == 1)
		printf("FCFS: First Come First Served(Non - preemptive) \n");
	else if (schedulingmethod == 2)
		printf("SJF: Shortest Job First (Non-Preemptive) \n");
	else if (schedulingmethod == 3)
		printf("SRTF: Shrotest Remaining Time First (Preemptive) \n");
	else if (schedulingmethod == 4)
		printf("RR: Round Robin (Preemptive) \n");
	
}


int do_schedule(int tick)
{


	int time = tick;

	if (schedulingmethod == 1)
	{
		return FCFS(time);
	}


	else if (schedulingmethod == 2)	
	{
		return SJF(time);
	}

	else if (schedulingmethod == 3)
	{
		return SRTF(time);
	}
	else if (schedulingmethod == 4)

	{
		return RR(time);
	}


}
void print_performance()
{
	for (int i = 0; i < proccnt; i++)
	{
		PRList[i].TurnAroundTime = PRList[i].FinishTime - PRList[i].ArrivalTime;
		PRList[i].WaitingTime = PRList[i].FinishTime - (PRList[i].ArrivalTime + PRList[i].BurstTime);
		PRList[i].ResponseTime = PRList[i].FirstAllocatedTime - PRList[i].ArrivalTime;
	}


	printf("========================================================================================\n");
	printf(" PID\tarrival\tfinish\tburst\tTurn arount time\tWaiting time\tResponse time\n");
	printf("========================================================================================\n");
	for (int i = 0; i < proccnt; i++)
	{
	printf(" %d \t  %d   \t  %d   \t  %d   \t     %d      \t              %d    \t       %d             \t  \n",PRList[i].ProcID, PRList[i].ArrivalTime, PRList[i].FinishTime, PRList[i].BurstTime, PRList[i].TurnAroundTime,PRList[i].WaitingTime,PRList[i].ResponseTime );


	}
	float aver1=0, aver2=0, aver3=0;
	for (int i = 0; i < proccnt; i++)
	{
		aver1 += PRList[i].TurnAroundTime;
		aver2 += PRList[i].WaitingTime;
		aver3 += PRList[i].ResponseTime;
	}
	aver1 = aver1 / (float)proccnt;
	aver2 = aver2 / (float)proccnt;
	aver3 = aver3 / (float)proccnt;
	printf("----------------------------------------------------------------------------------------\n");
	printf("average:                            %.2f                     %.2f             %.2f                \n",aver1,aver2,aver3);
	printf("========================================================================================\n");

}



int SJF(int tick)
{

	//Block that check all process is terminated.
	int IsFinished = 0;
	for (int i = 0; i < proccnt; i++)
	{
		IsFinished = PRList[i].RemainingTime;
	}
	if (IsFinished == 0)
	{
		printf("[tick: %d] All Processes are terminated. \n", tick);
		return 0;
	}

	for (int i = 0; i < proccnt; i++)
	{
		if (PRList[i].ArrivalTime == tick)
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			int check = 0;
			PRList[i].IsInQueue = 1;
			for (int j = 0; j < proccnt; j++)
			{
				if (PRList[j].IsInQueue == 1)
					check++;
			}
			if (curprocessing == -1 && check == 1)		//need to dispatch - New process arrives and one process is in RQ(Ready queue)
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].Procmem;
		
			}
			else if (curprocessing == -1 && check > 1)	//new process arrives and two or more processes are in RQ
			{
				int temp = 100000;
				for (int j = 0; j < proccnt; j++)
				{
					if (temp > PRList[j].RemainingTime && PRList[j].IsInQueue == 1)		//Processes have same time, first come process has priority
					{
						temp = PRList[j].RemainingTime;
						curprocessing = PRList[j].Procmem;

					}

				}
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[curprocessing].ProcID);
				PRList[curprocessing].FirstAllocatedTime = tick;
			}
			
		}

	}
	if (curprocessing == -1)//When CPU is empty
	{
		int temp = 100000;
		int check = 0;		//variable that checks processes in CPU or RQ
		for (int i = 0; i < proccnt; i++)			
		{ 
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)		//processes in RQ
			{
				if (temp > PRList[i].BurstTime)		//allocate shortest Bursttime(non-preemptive, so it is same as remaining time)
				{
					temp = PRList[i].BurstTime;
					curprocessing = PRList[i].Procmem;
					check++;
				}


			}

		}
		if (check == 0)		//RQ has no process but in txt, some processes will arrive so return 1
			return 1;
		//else dispatch 
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		PRList[curprocessing].FirstAllocatedTime = tick;
	}




	PRList[curprocessing].RemainingTime--;				//process 1 tick


	if (PRList[curprocessing].RemainingTime == 0)			//allocating process is terminated, return cpu
	{
		PRList[curprocessing].IsInQueue = 0;
		PRList[curprocessing].Terminated = 1;
		PRList[curprocessing].FinishTime = tick + 1;
		curprocessing = -1;

	}

	return 1;
}


int FCFS(int tick)
{
	int IsFinished = 0;
	for (int i = 0; i < proccnt; i++)
	{
		IsFinished = PRList[i].RemainingTime;
	}
	if (IsFinished == 0)
	{
		printf("[tick: %d] All Processes are terminated. \n", tick);
		return 0;
	}

	for (int i = 0; i < proccnt; i++)
	{
		if (PRList[i].ArrivalTime == tick)
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			int check = 0;
			PRList[i].IsInQueue = 1;
			for (int j = 0; j < proccnt; j++)
			{
				if (PRList[j].IsInQueue == 1)
					check++;
			}
			if (curprocessing == -1 && check == 1)		//need to dispatch - New process arrives and one process is in RQ(Ready queue)
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].Procmem;

				
			}
			else if (curprocessing == -1 && check > 1)		//new process arrives and two or more processes are in RQ
			{
				
				for (int j = 0; j < proccnt; j++)
				{
					if (PRList[j].IsInQueue == 1)		//allocate first of all
					{
						curprocessing = PRList[j].Procmem;
						break;

					}
				}
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[curprocessing].ProcID);
				PRList[curprocessing].FirstAllocatedTime = tick;


			}

		}

	}
	if (curprocessing == -1)
	{
		int temp = 100000;
		int check = 0;
		for (int i = 0; i < proccnt; i++)			//allocate process when RQ is empty
		{
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)		//Process in RQ
			{
				if (temp > PRList[i].ArrivalTime)						//Allocate First of all
				{
					temp = PRList[i].ArrivalTime;
					curprocessing = PRList[i].Procmem;
					check++;
				}

			}

		}
		if (check == 0)
			return 1;
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		PRList[curprocessing].FirstAllocatedTime = tick;
	}




	PRList[curprocessing].RemainingTime--;				//process 1 tick


	if (PRList[curprocessing].RemainingTime == 0)			//return cpu
	{
		PRList[curprocessing].IsInQueue = 0;
		PRList[curprocessing].Terminated = 1;
		PRList[curprocessing].FinishTime = tick + 1;
		curprocessing = -1;

	}


	return 1;
}

int SRTF(int tick)
{

	int IsFinished = 0;
	for (int i = 0; i < proccnt; i++)
	{
		IsFinished += PRList[i].RemainingTime;
	}
	if (IsFinished == 0)
	{
		printf("[tick: %d] All Processes are terminated. \n", tick);
		return 0;
	}

	for (int i = 0; i < proccnt; i++)
	{
		if (PRList[i].ArrivalTime == tick)					//algorithm when process arrives
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			PRList[i].IsInQueue = 1;
			int check = 0;
			for (int j = 0; j < proccnt; j++)
			{
				check += PRList[j].IsInQueue;
			}
			if (curprocessing == -1 && check == 1)			//need to dispatch - New process arrives and one process is in RQ(Ready queue)
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				curprocessing = PRList[i].Procmem;
				//SRTF is preemptive solution, so dispatch in the middle. using this if block need to first allocated time 
				if(PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)		
				{
					PRList[curprocessing].FirstAllocatedTime = tick;
				}



			}
			else if (check > 1)		//RQ > 1
			{
				int temp=100000;
				int change = 0;	//check process change
				for (int j = 0; j < proccnt; j++)
				{
					//가장 Remaining time이 작은 프로세스를 CPU에 할당한다, 큐에 프로세스가 없으면 프로세스 변화가 없다
					//Allocate shortest Remaining time process, RQ has no process, CPU's allocated process has no change
					if(PRList[j].IsInQueue == 1 && temp > PRList[j].RemainingTime)		
					{
						temp = PRList[j].RemainingTime;
						curprocessing = PRList[j].Procmem;
						change++;

					}


				}

				if (change > 0)		//when 'change' is changed, context-switching occured
				{
					printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[curprocessing].ProcID);
					if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)						//if block to get first allocated time
					{
						PRList[curprocessing].FirstAllocatedTime = tick;
					}

				}

			}
			
			
		}

	}


	if (curprocessing == -1)
	{
		int temp = 100000;
		int check = 0;
		for (int i = 0; i < proccnt; i++)		//CPU is empty, shortest remaining time process is allocated
		{
			if (PRList[i].IsInQueue == 1)
			{
				if (temp > PRList[i].RemainingTime)
				{
					temp = PRList[i].RemainingTime;
					curprocessing = PRList[i].Procmem;
					check++;
				}


			}

		}
		if (check == 0)
			return 1;
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //if block to get first allocated time
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}
	}




	PRList[curprocessing].RemainingTime--;				


	if (PRList[curprocessing].RemainingTime == 0)			
	{
		PRList[curprocessing].IsInQueue = 0;
		PRList[curprocessing].Terminated = 1;
		PRList[curprocessing].FinishTime = tick + 1;
		curprocessing = -1;

	}

	return 1;
}



void sortingPRList()
{
	
	for (int i = proccnt-1; i >0; i--)
	{
		for (int j = 0; j < i; j++)
		{
			if (PRList[j].ArrivalTime > PRList[j + 1].ArrivalTime)
			{
				Proc temp = PRList[j];
				PRList[j] = PRList[j + 1];
				PRList[j + 1] = temp;
			}
		}
		
	}
	for (int i = 0; i < proccnt; i++)
	{
		PRList[i].Procmem = i;
	}

}	


//build RR using Queue
int RR(int tick)
{
	int IsFinished = 0;
	for (int i = 0; i < proccnt; i++)
	{
		IsFinished += PRList[i].RemainingTime;
	}
	if (IsFinished == 0)
	{
		printf("[tick: %d] All Processes are terminated. \n", tick);
		return 0;
	}


	int cnt = 0;

	for (int i = 0; i < proccnt; i++)
	{
		if (PRList[i].ArrivalTime == tick)					//when process arrived
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			addq(i);
			PRList[i].IsInQueue = 1;
			int check = 0;
			for (int j = 0; j < proccnt; j++)
			{
				check += PRList[j].IsInQueue;
			}
			if (curprocessing == -1 && check == 1)			//RQ has one process and CPU isn't allocated process
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				deleteq();
				TQ = 2;
				curprocessing = PRList[i].Procmem;
				if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)		
				{
					PRList[curprocessing].FirstAllocatedTime = tick;
				}



			}			
		}

	}
	int check = 0;
	for (int i = 0; i < proccnt; i++)
	{
		if (PRList[i].IsInQueue == 1)
			check++;

	}
	
	if (curprocessing == -1 && check > 0)		//no process in CPU, RQ has process
	{
		curprocessing = deleteq();
		if (check == 0)
			return 1;
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		TQ = 2;
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //if block to calculate First allocated time
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}
	}
	
	else if (check == 1 && TQ == 0)
	{
		
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		TQ = 2;
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //if block to calculate First allocated time
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}

	}
	else if (check >1 && TQ == 0)	//RQ has two or more process, and Time quantum is 0, context switching ocuurs
	{
		
		TQ = 2;
		addq(curprocessing);
		curprocessing = deleteq();
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //if block to calculate First allocated time
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}

	}
	




	PRList[curprocessing].RemainingTime--;				
	TQ--;

	if (PRList[curprocessing].RemainingTime == 0)			
	{
		PRList[curprocessing].IsInQueue = 0;
		PRList[curprocessing].Terminated = 1;
		PRList[curprocessing].FinishTime = tick + 1;
		curprocessing = -1;

	}

	return 1;


}