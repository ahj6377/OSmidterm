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
	int Terminated;
	int IsInQueue;

	int CurProc;



}Proc;

struct PR *PRList;

int schedulingmethod;
int proccnt;
int curprocessing = -1;

int SJF(int tick);
int FCFS(int tick);
int SRTF(int tick);
//int RR();


void read_proc_list(const char* file_name)
{
	
	FILE* fp = fopen(file_name, "r");
	fscanf(fp, "%d", &proccnt);
	PRList = malloc(sizeof(struct PR)* proccnt);
	for (int i = 0; i < proccnt; i++)
	{
		PRList[i].ArrivalTime = 0;
		PRList[i].BurstTime = 0;
		PRList[i].CurProc = 0;
		PRList[i].ProcID = -1;
		PRList[i].RemainingTime = 1000;
		PRList[i].FinishTime = 0;
		PRList[i].FirstAllocatedTime = 0;
		PRList[i].ResponseTime = 0;
		PRList[i].TurnAroundTime = 0;
		PRList[i].WaitingTime = 0;
		PRList[i].Terminated = 0;
		PRList[i].IsInQueue = 0;

	}
	for (int i = 0; i < proccnt; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int temp;
			fscanf(fp, "%d", &temp);
			switch (j)
			{
			case 0:
				PRList[i].ProcID = temp;
			case 1:
				PRList[i].ArrivalTime = temp;
			case 2:
				PRList[i].BurstTime = temp;
				PRList[i].RemainingTime = temp;

			}


		}


	}
	

}




void set_schedule(int method)
{
	
	printf("Scheduling method:");
	/*
	if(method == 1)
		printf("FCFS: First Come First Served(Non - preemptive) \n");
	else if (method == 2)
		printf("SJF: Shortest Job First (Non-preemptive) \n");
	else if (method == 3)
		prinf("SRTF: Shortest Remaining Time First (Preemptive) \n");
	else if (method == 4)
		printf("4: RR: Round Robin (Preemptive) \n");
	*/
	
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


	else if (schedulingmethod == 2)					//SJF 구현
	{
		return SJF(time);
	}

	else if (schedulingmethod == 3)
	{
		return SRTF(time);
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
			PRList[i].IsInQueue = 1;
			if (curprocessing == -1)		//디스패치가 필요한 경우 - 현재 아무 프로세스도 실행중이지 않을때 새로운 프로세스가 들어옴
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].ProcID;
				PRList[i].CurProc = 1;
			}
			
		}

	}
	if (curprocessing == -1)
	{
		int temp = 100000;
		for (int i = 0; i < proccnt; i++)			//레디큐가 비었을경우 새 프로세스 할당, 이 부분주로 수정
		{ 
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)
			{
				if (temp > PRList[i].BurstTime)
				{
					temp = PRList[i].BurstTime;
					curprocessing = PRList[i].ProcID;
				}


			}

		}
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, curprocessing);
		PRList[curprocessing - 1].FirstAllocatedTime = tick;
	}




	PRList[curprocessing - 1].RemainingTime--;				//현재 작업중인 프로세스를 1초 진행시킨다


	if (PRList[curprocessing - 1].RemainingTime == 0)			//작업중인 프로세스가 완료되면 CPU반환
	{
		PRList[curprocessing - 1].Terminated = 1;
		PRList[curprocessing - 1].FinishTime = tick + 1;
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
			if (curprocessing == -1)		//디스패치가 필요한 경우 - 현재 아무 프로세스도 실행중이지 않을때 새로운 프로세스가 들어옴
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].ProcID;
				PRList[i].CurProc = 1;

				
			}

		}

	}
	if (curprocessing == -1)
	{
		int temp = 100000;
		for (int i = 0; i < proccnt; i++)			//레디큐가 비었을경우 새 프로세스 할당
		{
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)
			{
				if (temp > PRList[i].ArrivalTime)
				{
					temp = PRList[i].ArrivalTime;
					curprocessing = PRList[i].ProcID;
				}

			}

		}
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, curprocessing);
		PRList[curprocessing - 1].FirstAllocatedTime = tick;
	}




	PRList[curprocessing - 1].RemainingTime--;				//현재 작업중인 프로세스를 1초 진행시킨다


	if (PRList[curprocessing - 1].RemainingTime == 0)			//작업중인 프로세스가 완료되면 CPU반환
	{
		PRList[curprocessing - 1].Terminated = 1;
		PRList[curprocessing - 1].FinishTime = tick + 1;
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
		if (PRList[i].ArrivalTime == tick)					//Process가 들어왔을때의 알고리즘
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			PRList[i].IsInQueue = 1;
			int check = 0;
			for (int j = 0; j < proccnt; j++)
			{
				check += PRList[j].IsInQueue;
			}
			if (curprocessing == -1 && check == 1)			//아무 프로세스도 할당되지 않고, 레디큐에 하나의 프로세스가 있는경우
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				curprocessing = PRList[i].ProcID;
				if(PRList[curprocessing-1].BurstTime == PRList[curprocessing-1].RemainingTime)		//SRTF는 non-preemptive이므로 중간에 디스패치되는 경우가있다. firstalloctedtime을 구하기 위한 조건문
				{
					PRList[curprocessing-1].FirstAllocatedTime = tick;
				}



			}
			else if (check > 1)		//레디큐에 두개이상의 프로세스가 있는경우
			{
				int temp=100000;
				int change = 0;
				for (int j = 0; j < proccnt; j++)
				{
					if(PRList[j].IsInQueue == 1 && temp > PRList[j].RemainingTime)
					{
						temp = PRList[j].RemainingTime;
						curprocessing = PRList[j].ProcID;
						change++;

					}


				}

				if (change > 0)
				{
					printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[curprocessing-1].ProcID);
					if (PRList[curprocessing-1].BurstTime == PRList[curprocessing-1].RemainingTime)
					{
						PRList[curprocessing-1].FirstAllocatedTime = tick;
					}

				}

			}

		}

	}


	if (curprocessing == -1)
	{
		int temp = 100000;
		for (int i = 0; i < proccnt; i++)			//레디큐가 비었을경우 새 프로세스 할당, 이 부분주로 수정
		{
			if (PRList[i].IsInQueue == 1)
			{
				if (temp > PRList[i].RemainingTime)
				{
					temp = PRList[i].RemainingTime;
					curprocessing = PRList[i].ProcID;
				}


			}

		}
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, curprocessing);
		if (PRList[curprocessing-1].BurstTime == PRList[curprocessing-1].RemainingTime)
		{
			PRList[curprocessing-1].FirstAllocatedTime = tick;
		}
	}




	PRList[curprocessing - 1].RemainingTime--;				//현재 작업중인 프로세스를 1초 진행시킨다


	if (PRList[curprocessing - 1].RemainingTime == 0)			//작업중인 프로세스가 완료되면 CPU반환
	{
		PRList[curprocessing - 1].IsInQueue = 0;
		PRList[curprocessing - 1].Terminated = 1;
		PRList[curprocessing - 1].FinishTime = tick + 1;
		curprocessing = -1;

	}

	return 1;
}