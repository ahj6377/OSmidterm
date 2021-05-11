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
	int Terminated;		//�� ���μ����� ReadyQueue�� ���ͼ�, �����°��� �Ǻ��ϴ� ����
	int IsInQueue;		//���μ����� ReadyQueue�� CPU�� �Ҵ���ִ°��� �Ǻ��ϴ� ���� ������ 1 �ƴϸ� 0
	int Procmem;		//���μ����� �޸��ּ�, �� �ùķ����Ϳ��� Arrivaltime�� �������� ���� ������ ���μ��� ������ 0,1,2.....Process�Ǽ�-1 �� �ο���


	



}Proc;

struct PR *PRList;			//txt���� �о�� ���μ������� �����Ҵ��ϱ����� PR ������

int schedulingmethod;		//�����층 ����� ���� ��������
int proccnt;				//���μ����� ��
int curprocessing = -1;			//curProcessing�� ���� CPU�� �Ҵ�� ���μ����޸𸮰��� ������. �ƹ� ���μ����� �Ҵ���� ������ -1���� ������.
int TQ = 2;					//RR�� ����� Time Quantum


//�����층 �Լ� ����������� ��� ���μ����� terminate �Ǿ����� 0 �׷��������� 1�� ������
int SJF(int tick);		//SFJ����
int FCFS(int tick);		//FCFS����
int SRTF(int tick);		//SRTF����
int RR(int tick);		//RR����


void sortingPRList();		//txt���Ͽ� Arrival time������ ���ĵ����� �ʴ��� �������ִ� �Լ�
//#####################Queue �� ���õ� ���� �� �Լ�
//RR�� queue�� �̿��� �����ϴ°� �� ������ Queue �ڷᱸ���� ����߽��ϴ�.
int front = -1;
int rear = -1;
int *queue;

int IsEmpty(void) {
	if (front == rear)//front�� rear�� ������ ť�� ����ִ� ���� 
		return 1;
	else return 0;
}
int IsFull(void) {
	int tmp = (rear + 1) % proccnt; //���� ť���� rear+1�� MAX�� ���� ����������
	if (tmp == front)//front�� ������ ť�� ������ �ִ� ���� 
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


//####################ť ���� ��






void read_proc_list(const char* file_name)		//���μ��� ����Ʈ�� ���Ͽ��� �о� tx
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
	for (int i = 0; i < proccnt; i++)		//PRList �ʱ�ȭ
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



//schedulingmethod ���������� method�� �����ϴ� �Լ�
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

//�� tick���� schedulingmethod�� ���� �����층 ����
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

	//��� ���μ����� terminate �Ǿ����� Ȯ���ϴ� ���
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
			if (curprocessing == -1 && check == 1)		//����ġ�� �ʿ��� ��� - ���ο� ���μ����� �������� ����ť�� �ϳ��� ���μ����� �ִ� ���
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].Procmem;
		
			}
			else if (curprocessing == -1 && check > 1)	//���ο� ���μ����� �������� ����ť�� �� �̻��� ���μ����� �ִ� ���
			{
				int temp = 100000;
				for (int j = 0; j < proccnt; j++)
				{
					if (temp > PRList[j].RemainingTime && PRList[j].IsInQueue == 1)		//Remaingtime�� ������ ���� ���� ���μ����� �켱������ ������.
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
	if (curprocessing == -1)//CPU�� ��������
	{
		int temp = 100000;
		int check = 0;		//����ť�� ���μ����� � �ִ��� Ȯ���ϴ� ����
		for (int i = 0; i < proccnt; i++)			
		{ 
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)		//����ť�� �ִ� ���μ�������
			{
				if (temp > PRList[i].BurstTime)		//Bursttime(non-preemptive�̹Ƿ� RemainingTime���� �ص� ����)�� ���� ª�� ���μ����� �����Ѵ�.
				{
					temp = PRList[i].BurstTime;
					curprocessing = PRList[i].Procmem;
					check++;
				}


			}

		}
		if (check == 0)		//����ť�� �ƹ� ���μ����� ���� ���߿� ���μ����� �� ���� ������ ��� ���μ����� terminate�Ȱ� �ƴϹǷ� process�� �������� �ʰ� �����Ѵ�.
			return 1;
		//�׿ܿ� ����ť�� ���μ����� �ְ� ���������� ����ġ�� ����ȴ�.
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		PRList[curprocessing].FirstAllocatedTime = tick;
	}




	PRList[curprocessing].RemainingTime--;				//���� �۾����� ���μ����� 1�� �����Ų��


	if (PRList[curprocessing].RemainingTime == 0)			//�۾����� ���μ����� �Ϸ�Ǹ� CPU��ȯ
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
			if (curprocessing == -1 && check == 1)		//����ġ�� �ʿ��� ��� - ���� �ƹ� ���μ����� ���������� ������ ���ο� ���μ����� ����
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				PRList[i].FirstAllocatedTime = tick;
				curprocessing = PRList[i].Procmem;

				
			}
			else if (curprocessing == -1 && check > 1)		//���� �ƹ� ���μ����� CPU�� �Ҵ������ �ʰ� ����ť�� �ΰ��̻��� ���μ����� �ִ°��
			{
				
				for (int j = 0; j < proccnt; j++)
				{
					if (PRList[j].IsInQueue == 1)		//ť���ִ� ���μ����� ���� ������ ���μ����� �Ҵ���
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
		for (int i = 0; i < proccnt; i++)			//����ť�� �������� �� ���μ��� �Ҵ�
		{
			if (tick > PRList[i].ArrivalTime && PRList[i].Terminated == 0)		//Terminate ���� �ʾҰ�, �̹� �������ִ� ���μ�����
			{
				if (temp > PRList[i].ArrivalTime)						//���� ���� ���� ���μ������� �Ҵ��Ѵ�. ���� �켱������ ��쿣 ���� ���� job�� ó���Ѵ�.
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




	PRList[curprocessing].RemainingTime--;				//���� �۾����� ���μ����� 1�� �����Ų��


	if (PRList[curprocessing].RemainingTime == 0)			//�۾����� ���μ����� �Ϸ�Ǹ� CPU��ȯ
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
		if (PRList[i].ArrivalTime == tick)					//Process�� ���������� �˰���
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			PRList[i].IsInQueue = 1;
			int check = 0;
			for (int j = 0; j < proccnt; j++)
			{
				check += PRList[j].IsInQueue;
			}
			if (curprocessing == -1 && check == 1)			//�ƹ� ���μ����� �Ҵ���� �ʰ�, ����ť�� �ϳ��� ���μ����� �ִ°��
			{
				printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[i].ProcID);
				curprocessing = PRList[i].Procmem;
				if(PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)		//SRTF�� non-preemptive�̹Ƿ� �߰��� ����ġ�Ǵ� ��찡�ִ�. firstalloctedtime�� ���ϱ� ���� ���ǹ�
				{
					PRList[curprocessing].FirstAllocatedTime = tick;
				}



			}
			else if (check > 1)		//����ť�� �ΰ��̻��� ���μ����� �ִ°��
			{
				int temp=100000;
				int change = 0;
				for (int j = 0; j < proccnt; j++)
				{
					if(PRList[j].IsInQueue == 1 && temp > PRList[j].RemainingTime)		//���� Remaining time�� ���� ���μ����� CPU�� �Ҵ��Ѵ�, ť�� ���μ����� ������ ���μ��� ��ȭ�� ����
					{
						temp = PRList[j].RemainingTime;
						curprocessing = PRList[j].Procmem;
						change++;

					}


				}

				if (change > 0)		//change�� �ٲ������쿣 Contextswitching�� �Ͼ��
				{
					printf("[tick: %d ] Dispatch to Process (ID : %d)\n", tick, PRList[curprocessing].ProcID);
					if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)						//FirstAllocatedTime ���ϱ� ���� ���ǹ�
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
		for (int i = 0; i < proccnt; i++)			//����ť�� �������� �� ���μ��� �Ҵ� ���� Remaining time�� ���� ���μ����� �Ҵ�
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
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //FirstAllocatedTime ���ϱ� ���� ���ǹ�
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}
	}




	PRList[curprocessing].RemainingTime--;				//���� �۾����� ���μ����� 1�� �����Ų��


	if (PRList[curprocessing].RemainingTime == 0)			//�۾����� ���μ����� �Ϸ�Ǹ� CPU��ȯ
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


//RR�� queue �ڷᱸ���� ���ؼ� �����߽��ϴ�
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
		if (PRList[i].ArrivalTime == tick)					//Process�� ���������� �˰���
		{
			printf("[tick: %d ] New Process (ID: %d) newly joins to ready queue \n", tick, PRList[i].ProcID);
			addq(i);
			PRList[i].IsInQueue = 1;
			int check = 0;
			for (int j = 0; j < proccnt; j++)
			{
				check += PRList[j].IsInQueue;
			}
			if (curprocessing == -1 && check == 1)			//�ƹ� ���μ����� �Ҵ���� �ʰ�, ����ť�� �ϳ��� ���μ����� �ִ°��
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
	
	if (curprocessing == -1 && check > 0)		//�ƹ��� ���μ����� �Ҵ���� ������, �׸��� ����ť�� ���μ����� ������
	{
		curprocessing = deleteq();
		if (check == 0)
			return 1;
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		TQ = 2;
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //FirstAllocatedTime ���ϱ� ���� ���ǹ�
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}
	}

	else if (check >1 && TQ == 0)			//Ready Queue�� �ΰ� �̻��� ���μ����� �ְ�, Time Quantum�� 0�� �Ǿ������ context switching�� �Ͼ��
	{
		
		TQ = 2;
		addq(curprocessing);
		curprocessing = deleteq();
		printf("[tick: %d ] Dispatch to Process (ID : %d) \n", tick, PRList[curprocessing].ProcID);
		if (PRList[curprocessing].BurstTime == PRList[curprocessing].RemainingTime)   //FirstAllocatedTime ���ϱ� ���� ���ǹ�
		{
			PRList[curprocessing].FirstAllocatedTime = tick;
		}

	}
	




	PRList[curprocessing].RemainingTime--;				//���� �۾����� ���μ����� 1�� �����Ų��
	TQ--;

	if (PRList[curprocessing].RemainingTime == 0)			//�۾����� ���μ����� �Ϸ�Ǹ� CPU��ȯ
	{
		PRList[curprocessing].IsInQueue = 0;
		PRList[curprocessing].Terminated = 1;
		PRList[curprocessing].FinishTime = tick + 1;
		curprocessing = -1;

	}

	return 1;


}