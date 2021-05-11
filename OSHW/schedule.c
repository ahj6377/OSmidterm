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
	int Terminated;		//그 프로세스가 ReadyQueue에 들어와서, 끝났는가를 판별하는 변수
	int IsInQueue;		//프로세스가 ReadyQueue나 CPU에 할당되있는가를 판별하는 변수 있으면 1 아니면 0
	int Procmem;		//프로세스의 메모리주소, 이 시뮬레이터에선 Arrivaltime을 기준으로 먼저 도착한 프로세스 순으로 0,1,2.....Process의수-1 로 부여됨


	



}Proc;

struct PR *PRList;			//txt에서 읽어온 프로세스들을 동적할당하기위한 PR 포인터

int schedulingmethod;		//스케쥴링 방법을 받을 전역변수
int proccnt;				//프로세스의 수
int curprocessing = -1;			//curProcessing은 현재 CPU에 할당된 프로세스메모리값을 가진다. 아무 프로세스도 할당되지 않으면 -1값을 가진다.
int TQ = 2;					//RR에 사용할 Time Quantum


//스케쥴링 함수 공통사항으로 모든 프로세스가 terminate 되었으면 0 그렇지않으면 1을 리턴함
int SJF(int tick);		//SFJ구현
int FCFS(int tick);		//FCFS구현
int SRTF(int tick);		//SRTF구현
int RR(int tick);		//RR구현


void sortingPRList();		//txt파일에 Arrival time순으로 정렬되있지 않더라도 정렬해주는 함수
//#####################Queue 와 관련된 변수 및 함수
//RR은 queue를 이용해 구현하는게 더 쉬워서 Queue 자료구조를 사용했습니다.
int front = -1;
int rear = -1;
int *queue;

int IsEmpty(void) {
	if (front == rear)//front와 rear가 같으면 큐는 비어있는 상태 
		return 1;
	else return 0;
}
int IsFull(void) {
	int tmp = (rear + 1) % proccnt; //원형 큐에서 rear+1을 MAX로 나눈 나머지값이
	if (tmp == front)//front와 같으면 큐는 가득차 있는 상태 
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


//####################큐 구현 끝






void read_proc_list(const char* file_name)		//프로세스 리스트를 파일에서 읽어 tx
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
	for (int i = 0; i < proccnt; i++)		//PRList 초기화
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



//schedulingmethod 전역변수에 method를 전달하는 함수
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

//매 tick마다 schedulingmethod에 따라서 스케쥴링 진행
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

	//모든 프로세스가 terminate 되었는지 확인하는 블록
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
			if (curprocessing == -1 && check == 1)		//디스패치가 필요한 경우 - 새로운 프로세스가 들어왔을때 레디큐에 하나의 프로세스만 있는 경우
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].Procmem;
		
			}
			else if (curprocessing == -1 && check > 1)	//새로운 프로세스가 들어왔을때 레디큐에 둘 이상의 프로세스가 있는 경우
			{
				int temp = 100000;
				for (int j = 0; j < proccnt; j++)
				{
					if (temp > PRList[j].RemainingTime && PRList[j].IsInQueue == 1)		//Remaingtime이 같을땐 먼저 들어온 프로세스가 우선순위를 가진다.
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
	if (curprocessing == -1)//CPU가 비었을경우
	{
		int temp = 100000;
		int check = 0;		//레디큐에 프로세스가 몇개 있는지 확인하는 변수
		for (int i = 0; i < proccnt; i++)			
		{ 
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)		//레디큐에 있는 프로세스들중
			{
				if (temp > PRList[i].BurstTime)		//Bursttime(non-preemptive이므로 RemainingTime으로 해도 무관)이 가장 짧은 프로세스를 선택한다.
				{
					temp = PRList[i].BurstTime;
					curprocessing = PRList[i].Procmem;
					check++;
				}


			}

		}
		if (check == 0)		//레디큐에 아무 프로세스도 없고 나중에 프로세스가 더 오긴 하지만 모든 프로세스가 terminate된건 아니므로 process를 진행하지 않고 리턴한다.
			return 1;
		//그외엔 레디큐에 프로세스가 있고 정상적으로 디스패치가 진행된다.
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		PRList[curprocessing].FirstAllocatedTime = tick;
	}




	PRList[curprocessing].RemainingTime--;				//현재 작업중인 프로세스를 1초 진행시킨다


	if (PRList[curprocessing].RemainingTime == 0)			//작업중인 프로세스가 완료되면 CPU반환
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
			if (curprocessing == -1 && check == 1)		//디스패치가 필요한 경우 - 현재 아무 프로세스도 실행중이지 않을때 새로운 프로세스가 들어옴
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].Procmem;

				
			}
			else if (curprocessing == -1 && check > 1)		//현재 아무 프로세스도 CPU에 할당되있지 않고 레디큐에 두개이상의 프로세스가 있는경우
			{
				
				for (int j = 0; j < proccnt; j++)
				{
					if (PRList[j].IsInQueue == 1)		//큐에있는 프로세스중 가장 먼저온 프로세스를 할당함
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
		for (int i = 0; i < proccnt; i++)			//레디큐가 비었을경우 새 프로세스 할당
		{
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)		//Terminate 되지 않았고, 이미 도착해있는 프로세스중
			{
				if (temp > PRList[i].ArrivalTime)						//가장 빨리 들어온 프로세스에게 할당한다. 같은 우선순위의 경우엔 먼저 들어온 job을 처리한다.
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




	PRList[curprocessing].RemainingTime--;				//현재 작업중인 프로세스를 1초 진행시킨다


	if (PRList[curprocessing].RemainingTime == 0)			//작업중인 프로세스가 완료되면 CPU반환
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
				curprocessing = PRList[i].Procmem;
				if(PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)		//SRTF는 non-preemptive이므로 중간에 디스패치되는 경우가있다. firstalloctedtime을 구하기 위한 조건문
				{
					PRList[curprocessing].FirstAllocatedTime = tick;
				}



			}
			else if (check > 1)		//레디큐에 두개이상의 프로세스가 있는경우
			{
				int temp=100000;
				int change = 0;
				for (int j = 0; j < proccnt; j++)
				{
					if(PRList[j].IsInQueue == 1 && temp > PRList[j].RemainingTime)		//가장 Remaining time이 작은 프로세스를 CPU에 할당한다, 큐에 프로세스가 없으면 프로세스 변화가 없다
					{
						temp = PRList[j].RemainingTime;
						curprocessing = PRList[j].Procmem;
						change++;

					}


				}

				if (change > 0)		//change가 바뀌었을경우엔 Contextswitching이 일어난것
				{
					printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[curprocessing].ProcID);
					if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)						//FirstAllocatedTime 구하기 위한 조건문
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
		for (int i = 0; i < proccnt; i++)			//레디큐가 비었을경우 새 프로세스 할당 가장 Remaining time이 작은 프로세스를 할당
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
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //FirstAllocatedTime 구하기 위한 조건문
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}
	}




	PRList[curprocessing].RemainingTime--;				//현재 작업중인 프로세스를 1초 진행시킨다


	if (PRList[curprocessing].RemainingTime == 0)			//작업중인 프로세스가 완료되면 CPU반환
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


//RR은 queue 자료구조를 통해서 구현했습니다
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
		if (PRList[i].ArrivalTime == tick)					//Process가 들어왔을때의 알고리즘
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			addq(i);
			PRList[i].IsInQueue = 1;
			int check = 0;
			for (int j = 0; j < proccnt; j++)
			{
				check += PRList[j].IsInQueue;
			}
			if (curprocessing == -1 && check == 1)			//아무 프로세스도 할당되지 않고, 레디큐에 하나의 프로세스가 있는경우
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
	
	if (curprocessing == -1 && check > 0)		//아무런 프로세스도 할당되지 않을때, 그리고 레디큐에 프로세스가 있을때
	{
		curprocessing = deleteq();
		if (check == 0)
			return 1;
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		TQ = 2;
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //FirstAllocatedTime 구하기 위한 조건문
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}
	}

	else if (check >1 && TQ == 0)			//Ready Queue에 두개 이상의 프로세스가 있고, Time Quantum이 0이 되었을경우 context switching이 일어난다
	{
		
		TQ = 2;
		addq(curprocessing);
		curprocessing = deleteq();
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //FirstAllocatedTime 구하기 위한 조건문
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}

	}
	




	PRList[curprocessing].RemainingTime--;				//현재 작업중인 프로세스를 1초 진행시킨다
	TQ--;

	if (PRList[curprocessing].RemainingTime == 0)			//작업중인 프로세스가 완료되면 CPU반환
	{
		PRList[curprocessing].IsInQueue = 0;
		PRList[curprocessing].Terminated = 1;
		PRList[curprocessing].FinishTime = tick + 1;
		curprocessing = -1;

	}

	return 1;


}