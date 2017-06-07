#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FCFS 0				//Firstcome-Firtstservice
#define nonSJF 1			//Non-preemptive SJF
#define SJF 2				//Preemptive SJF
#define nonPRIORITY 3			//Non-preemptive PRIORITY
#define PRIORITY 4			//preemptive PRIORITY
#define RR 5				//RoundRobin
#define nonLIF 6			//Non-preemptive Longest I/O first
#define LIF 7				//Preemptive Longest I/O first
#define nonLISC 8			//Non-preemptive Longest I/O Shortest CPU First
#define LISC 9				//Preemptive Longest I/O Shortest CPU First

#define TIME_QUANTUM 3

#define MAX_PROCESS_NUM 20
#define MAX_ARRIVALTIME 100

int count = 0;
int endTime = 0;
int idle = 0;

FILE *fp;

typedef struct Proc			//Process
{
	int pid;
	int CPUburst;
	int IOburst;
	int ArrivalTime;
	int priority;
	int reCPUburst;
	int reIOburst;
	int WaitingTime;
	int TurnaroundTime;
	int ActiveTime;
	int IOstartTime;
}Proc;
typedef struct Proc* ProcPtr;
/*
typedef struct Eval
{
	int Avg_WaitingTime;
	int Avg_TurnaroundTime;
	double CPU_Utilization;
}Eval;
typedef struct Eval* EvalPtr;

EvalPtr EvalAlg[10];
int EvalAlg_num = 0;
*/
ProcPtr JobQ[MAX_PROCESS_NUM];		//Job Queue
int JobQ_num = 0;

ProcPtr tempJobQ[MAX_PROCESS_NUM];	//Temporary Job Queue
int tempJobQ_num = 0;

ProcPtr ReadyQ[MAX_PROCESS_NUM];	//Ready Queue
int ReadyQ_num = 0;

ProcPtr WaitQ[MAX_PROCESS_NUM];		//Waiting Queue
int WaitQ_num = 0;

ProcPtr TermQ[MAX_PROCESS_NUM];		//Terminated Queue
int TermQ_num = 0;

ProcPtr RunningProc = NULL;		//Running State Process

ProcPtr FCFS_alg();
ProcPtr nonSJF_alg();
ProcPtr SJF_alg();
ProcPtr nonPrty_alg();
ProcPtr Prty_alg();
ProcPtr RR_alg();
ProcPtr nonLIF_alg();
ProcPtr LIF_alg();
ProcPtr nonLISC_alg();
ProcPtr LISC_alg();

void error(char* msg)			//Print Error Message
{
	perror(msg);
	exit(0);
}

void Init_JobQ()			//Initialize Job Queue
{
	JobQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
		JobQ[i] = NULL;
}

void Clear_JobQ()			//Clear Job Queue
{
	JobQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
	{
		JobQ[i] = NULL;
		free(JobQ[i]);
	}
}

void Init_tempJobQ()			//Initialize Temporary Queue
{
	tempJobQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
		tempJobQ[i] = NULL;
}

void Clear_tempJobQ()			//Clear Temporary Queue
{
	tempJobQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
	{
		tempJobQ[i] = NULL;
		free(tempJobQ[i]);
	}
}

void Init_ReadyQ()			//Initialize Ready Queue
{
	ReadyQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
		ReadyQ[i] = NULL;
}

void Clear_ReadyQ()			//Clear Ready Queue
{
	ReadyQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
	{
		ReadyQ[i] = NULL;
		free(ReadyQ[i]);
	}
}

void Init_WaitQ()			//Initialize Waiting Queue
{
	WaitQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
	{
		WaitQ[i] = NULL;
	}
}

void Clear_WaitQ()			//Clear Waiting Queue
{
	WaitQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
	{
		WaitQ[i] = NULL;
		free(WaitQ[i]);
	}
}

void Init_TermQ()			//Initialize Terminated Queue
{
	TermQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
	{
		TermQ[i] = NULL;
	}
}

void Clear_TermQ()			//Clear Terminated Queue
{
	TermQ_num = 0;
	for(int i=0; i<MAX_PROCESS_NUM; i++)
	{
		TermQ[i] = NULL;
		free(TermQ[i]);
	}
}

void Insert_JobQ(ProcPtr proc)				//Insert process into Job Queue
{
	if( proc == NULL )
		error("Fail Insert into JobQ\n");

	if( JobQ_num >= MAX_PROCESS_NUM )
		error("Queue is already full\n");

	for(int i=0; i<JobQ_num; i++)
	{
		if( proc->pid == JobQ[i]->pid )
			fprintf(fp,"The process(%d) already exists in Job Queue\n", proc->pid);
	}
	
	JobQ[JobQ_num++] = proc;
}

void Insert_tempJobQ(ProcPtr proc)				//Insert process into Temporary Queue
{
	if( proc == NULL )
		error("Fail Insert into temporary Queue\n");

	if( tempJobQ_num >= MAX_PROCESS_NUM )
		error("Queue is already full\n");

	for(int i=0; i<tempJobQ_num; i++)
	{
		if( proc->pid == tempJobQ[i]->pid )
			fprintf(fp,"The process(%d) already exists in temp Queue\n", proc->pid);
	}
	
	tempJobQ[tempJobQ_num++] = proc;
}

void Insert_ReadyQ(ProcPtr proc)				//Insert process into Job Queue
{
	if( proc == NULL )
		error("Fail Insert into Ready Queue\n");

	if( ReadyQ_num >= MAX_PROCESS_NUM )
		error("Queue is already full\n");

	for(int i=0; i<ReadyQ_num; i++)
	{
		if( proc->pid == ReadyQ[i]->pid )
			fprintf(fp,"The process(%d) already exists in Ready Queue\n", proc->pid);
	}
	
	ReadyQ[ReadyQ_num++] = proc;
}

void Insert_WaitQ(ProcPtr proc)				//Insert process into Waiting Queue
{
	if( proc == NULL )
		error("Fail Insert into Waiting Queue\n");

	if( WaitQ_num >= MAX_PROCESS_NUM )
		error("Queue is already full\n");

	for(int i=0; i<WaitQ_num; i++)
	{
		if( proc->pid == WaitQ[i]->pid )
			fprintf(fp,"The process(%d) already exists in Queue\n", proc->pid);
	}
	
	WaitQ[WaitQ_num++] = proc;
}

void Insert_TermQ(ProcPtr proc)				//Insert process into Job Queue
{
	if( proc == NULL )
		error("Fail Insert into Terminated Queue\n");

	if( TermQ_num >= MAX_PROCESS_NUM )
		error("Queue is already full\n");

	for(int i=0; i<TermQ_num; i++)
	{
		if( proc->pid == TermQ[i]->pid )
			fprintf(fp,"The process(%d) already exists in Queue\n", proc->pid);
	}
	
	TermQ[TermQ_num++] = proc;
}

ProcPtr removefrom_JobQ(ProcPtr proc)			//Bring Process from Job Queue
{
	ProcPtr result = NULL;

	int i;

	if( JobQ_num <= 0 )
	{
		fprintf(fp,"Job Queue is empty\n");
		return NULL;
	}
	
	for(i=0; i<JobQ_num; i++)
	{
		if( proc->pid == JobQ[i]->pid )
		{	
			result = JobQ[i];
			for(int j=i; j<JobQ_num; j++)
				JobQ[j] = JobQ[j+1];
			JobQ[JobQ_num-1] = NULL;
			JobQ_num--;
			return result;
		}
	}
	
	if( i == JobQ_num )
		fprintf(fp,"The process(%d) dosen't exist in Queue\n", proc->pid);
	return NULL;
}

ProcPtr removefrom_ReadyQ(ProcPtr proc)			//Bring Process from Ready Queue
{
	ProcPtr result = NULL;

	int i;

	if( ReadyQ_num <= 0 )
	{
		//fprintf(fp,"Ready Queue is empty\n");
		return NULL;
	}
	
	for(i=0; i<ReadyQ_num; i++)
	{
		if( proc->pid == ReadyQ[i]->pid )
		{	
			result = ReadyQ[i];
			for(int j=i; j<ReadyQ_num; j++)
				ReadyQ[j] = ReadyQ[j+1];
			ReadyQ[ReadyQ_num-1] = NULL;
			ReadyQ_num--;
			return result;
		}
	}
	
	if( i == ReadyQ_num )
		fprintf(fp,"The process(%d) dosen't exist in Queue\n", proc->pid);
	return NULL;
}

ProcPtr removefrom_WaitQ(ProcPtr proc)			//Bring Process from Waiting Queue
{
	ProcPtr result = NULL;

	int i;

	if( WaitQ_num <= 0 )
	{
		fprintf(fp,"Waiting Queue is empty\n");
		return NULL;
	}
	
	for(i=0; i<WaitQ_num; i++)
	{
		if( proc->pid == WaitQ[i]->pid )
		{	
			result = WaitQ[i];
			for(int j=i; j<WaitQ_num; j++)
				WaitQ[j] = WaitQ[j+1];
			WaitQ[WaitQ_num-1] = NULL;
			WaitQ_num--;
			return result;
		}
	}
	
	if( i == WaitQ_num )
		fprintf(fp,"The process(%d) dosen't exist in Queue\n", proc->pid);
	return NULL;
}

void print_JobQ()					//Print Process in Job Queue
{
	fprintf(fp,"Total Process : %d\n", JobQ_num);
	fprintf(fp,"PID\tPriority\tArrival Time\tCPU burst\tI/O burst\tI/O start\n");
	fprintf(fp,"----------------------------------------------------------------------------------\n");
	for(int i=0; i<JobQ_num; i++)
	{
		fprintf(fp,"%3d\t%8d\t%12d\t%9d\t%9d\t%9d\n",JobQ[i]->pid,JobQ[i]->priority,JobQ[i]->ArrivalTime,JobQ[i]->CPUburst,JobQ[i]->IOburst,JobQ[i]->IOstartTime);
	}
	fprintf(fp,"----------------------------------------------------------------------------------\n");
}

void print_tempJobQ()					//Print Process in Job Queue
{
	fprintf(fp,"Total Process : %d\n", tempJobQ_num);
	fprintf(fp,"PID\tPriority\tArrival Time\tCPU burst\tI/O burst\tI/O start\n");
	fprintf(fp,"----------------------------------------------------------------------------------\n");
	for(int i=0; i<tempJobQ_num; i++)
	{
		printf("%3d\t%8d\t%12d\t%9d\t%9d\t%9d\n",tempJobQ[i]->pid,tempJobQ[i]->priority,tempJobQ[i]->ArrivalTime,tempJobQ[i]->CPUburst,tempJobQ[i]->IOburst,tempJobQ[i]->IOstartTime);
	}
	fprintf(fp,"----------------------------------------------------------------------------------\n");
}

void print_ReadyQ()					//Print Process in Ready Queue
{
	fprintf(fp,"Total Process : %d\n", ReadyQ_num);
	fprintf(fp,"PID\tPriority\tArrival Time\tCPU burst\tI/O burst\tI/O start\n");
	fprintf(fp,"----------------------------------------------------------------------------------\n");
	for(int i=0; i<ReadyQ_num; i++)
	{
		fprintf(fp,"%3d\t%8d\t%12d\t%9d\t%9d\t%9d\n",ReadyQ[i]->pid,ReadyQ[i]->priority,ReadyQ[i]->ArrivalTime,ReadyQ[i]->CPUburst,ReadyQ[i]->IOburst,ReadyQ[i]->IOstartTime);
	}
	fprintf(fp,"----------------------------------------------------------------------------------\n");
}

void print_TermQ()					//Print Process in Terminated Queue
{
	fprintf(fp,"Total Process : %d\n", TermQ_num);
	fprintf(fp,"PID\tPriority\tArrival Time\tCPU burst\tI/O burst\tI/O start\n");
	fprintf(fp,"----------------------------------------------------------------------------------\n");
	for(int i=0; i<TermQ_num; i++)
	{
		fprintf(fp,"%3d\t%8d\t%12d\t%9d\t%9d\t%9d\n",TermQ[i]->pid,TermQ[i]->priority,TermQ[i]->ArrivalTime,TermQ[i]->CPUburst,TermQ[i]->IOburst,TermQ[i]->IOstartTime);
	}
	fprintf(fp,"----------------------------------------------------------------------------------\n");
}

void sort_JobQ()		//Sorting Process in Job Queue with arrivaltime(Insertion Sort)
{
	ProcPtr temp;
	int i,j;
	int smallest = 0;

	for(i=1; i<JobQ_num; i++)
	{
		temp = JobQ[i];
		j = i-1;
		while(j>=0 && temp->ArrivalTime < JobQ[j]->ArrivalTime)
		{
			JobQ[j+1] = JobQ[j];
			j--;
		}
		JobQ[j+1] = temp; 
	}

	smallest = JobQ[0]->ArrivalTime;				//ArrivalTime start from 0
	if( JobQ[0]->ArrivalTime > 0 )
	{
		smallest = JobQ[0]->ArrivalTime;
		for(i=0; i<JobQ_num; i++)
			JobQ[i]->ArrivalTime = JobQ[i]->ArrivalTime - smallest;	
	}
}

ProcPtr Init_Proc(int pid, int CPUburst, int IOburst, int ArrivalTime, int priority, int IOstartTime)		//Initialize new Process
{
	if(CPUburst <= 0 || IOburst < 0)
	{
		fprintf(fp,"Wrong BurstTime\n");
		return NULL;
	}

	if(ArrivalTime < 0 || ArrivalTime > MAX_ARRIVALTIME)
	{
		fprintf(fp,"Wrong ArrivalTime\n");
		return NULL;
	}
	
	ProcPtr newProc = (ProcPtr)malloc(sizeof(struct Proc));
	newProc->pid = pid;
	newProc->CPUburst = CPUburst;
	newProc->IOburst = IOburst;
	newProc->ArrivalTime = ArrivalTime;
	newProc->priority = priority;
	newProc->reCPUburst = CPUburst;
	newProc->reIOburst = IOburst;
	newProc->WaitingTime = 0;
	newProc->TurnaroundTime = 0;
	newProc->ActiveTime = 0;
	newProc->IOstartTime = IOstartTime;
	
	//debug
	fprintf(fp,"%d %d %d %d %d %dcreated\n", newProc->pid, newProc->CPUburst, newProc->IOburst, newProc->ArrivalTime, newProc->priority, newProc->IOstartTime);
	
	return newProc;
}

void CreateProcess(int totalnum)
{
	int i;
	int CPUburst;
	int IOburst = 0;
	int ArrivalTime;
	int priority;
	int IOstartTime = 0;

	srand(time(NULL));

	for(i=0; i<totalnum; i++)
	{
		CPUburst = rand() % 11 + 5;		//CPU burst : 5 ~ 15
		if(((rand() + i) % 4) == 0)			//1 of 4 Process has I/Oburst
			IOburst = (rand() + i) % 10 + 1;	//I/O burst : 1 ~ 10
		ArrivalTime = rand() % (totalnum * 3);	//ArrivalTime : 0 ~ 3 times the number of Processes
		priority = (rand() + i) % (totalnum + i) + 1;	//Priority : 1 ~ 2 times the number of Processes
		if(IOburst != 0)
			IOstartTime = ((rand() + i) % (CPUburst - 1)) + 1;		
		Insert_JobQ(Init_Proc(i+1,CPUburst,IOburst,ArrivalTime,priority,IOstartTime));
	}
	//print_JobQ();
	sort_JobQ();
	print_JobQ();

	for(i=0; i<totalnum; i++)
		Insert_tempJobQ(JobQ[i]);

	//print_tempJobQ(JobQ,&JobQ_num);
}

void Copy_JobQ()					//Copy tempJobQ into JobQ
{
	Clear_JobQ();

	for(int i=0; i<tempJobQ_num; i++)
	{
		ProcPtr tempProc = (ProcPtr)malloc(sizeof(struct Proc));
		tempProc->pid = tempJobQ[i]->pid;
		tempProc->CPUburst = tempJobQ[i]->CPUburst;
		tempProc->IOburst = tempJobQ[i]->IOburst;
		tempProc->ArrivalTime = tempJobQ[i]->ArrivalTime;
		tempProc->priority = tempJobQ[i]->priority;
		tempProc->reCPUburst = tempJobQ[i]->CPUburst;
		tempProc->reIOburst = tempJobQ[i]->IOburst;
		tempProc->WaitingTime = 0;
		tempProc->TurnaroundTime = 0;
		tempProc->ActiveTime = 0;
		tempProc->IOstartTime = tempJobQ[i]->IOstartTime;

		Insert_JobQ(tempProc);
	}
	print_JobQ();
}

ProcPtr schedule(int algorithm)				//Select next running Process
{
	ProcPtr NextProc = NULL;

	switch(algorithm)
	{
		case FCFS:
			NextProc = FCFS_alg();
			break;
		case nonSJF:
			NextProc = nonSJF_alg();
			break;
		case SJF:
			NextProc = SJF_alg();
			break;
		case nonPRIORITY:
			NextProc = nonPrty_alg();
			break;
		case PRIORITY:
			NextProc = Prty_alg();
			break;
		case RR:
			NextProc = RR_alg();
			break;
		case nonLIF:
			NextProc = nonLIF_alg();
			break;
		case LIF:
			NextProc = LIF_alg();
			break;
		case nonLISC:
			NextProc = nonLISC_alg();
			break;
		case LISC:
			NextProc = LISC_alg();
			break;
		default:
			return NULL;
	}
	
	return NextProc;
}

void progress(int algorithm)			//Realtime State and progress one step
{
	int i;
	
	for(i=0; i<JobQ_num; i++)		//해당 시간에 도착한 Process를 JobQ에서 ReadyQ로 적재한다.
	{
		if(JobQ[i]->ArrivalTime == count)
			Insert_ReadyQ(removefrom_JobQ(JobQ[i--]));
	}
	
	ProcPtr prevProc = RunningProc;
	RunningProc = schedule(algorithm);

	fprintf(fp,"%d: ", count++);

	if( (prevProc != NULL) && (prevProc != RunningProc))
	{
		prevProc->ActiveTime = 0;
		Insert_ReadyQ(prevProc);
		fprintf(fp,"[pid: %d] -> Preemptive, ", prevProc->pid);
	}

	if( ReadyQ_num > 0)
	{
		for(i=0; i<ReadyQ_num; i++)
		{
			ReadyQ[i]->WaitingTime++;
			ReadyQ[i]->TurnaroundTime++;
		}
	}

	if(RunningProc == NULL)
	{
		fprintf(fp,"idle");
		idle++;
	}
	else
	{
		RunningProc->reCPUburst--;
		RunningProc->TurnaroundTime++;
		RunningProc->ActiveTime++;
		fprintf(fp,"[pid: %d] -> Running", RunningProc->pid);
		if(RunningProc->reCPUburst == 0)
		{
			Insert_TermQ(RunningProc);
			RunningProc = NULL;
			fprintf(fp," -> terminated");
		}
		else
		{
			if((RunningProc->CPUburst - RunningProc->reCPUburst) == RunningProc->IOstartTime)
			{
				RunningProc->ActiveTime = 0;
				Insert_WaitQ(RunningProc);
				RunningProc = NULL;
				fprintf(fp," -> I/O Request");
			}
		}
	}

	if( WaitQ_num > 0)
	{
		for(i=0; i<WaitQ_num; i++)
		{
			WaitQ[i]->WaitingTime++;
			WaitQ[i]->TurnaroundTime++;
			WaitQ[i]->reIOburst--;

			if(WaitQ[i]->reIOburst == 0)
			{
				fprintf(fp,", [pid: %d] -> IO complete", WaitQ[i]->pid);
				Insert_ReadyQ(removefrom_WaitQ(WaitQ[i]));
			}
		}
	}
	fprintf(fp,"\n");
}

void Report(int algorithm)				//Report result
{
	int avg_waittime = 0;
	int avg_turntime = 0;
	int sum_waittime = 0;
	int sum_turntime = 0;
	double cpu_util = 0;

	ProcPtr termProc = NULL;

	if(TermQ_num <= 0)
		error("Terminated Processes are not Existed\n");
	fprintf(fp,"-------------------------------------------------------------------\n");
	for(int i=0; i<TermQ_num; i++)
	{
		termProc = TermQ[i];
		fprintf(fp,"(pid: %d)\n", termProc->pid);
		fprintf(fp,"Waiting Time = %d, ", termProc->WaitingTime);
		fprintf(fp,"Turnaround Time = %d\n", termProc->TurnaroundTime);		
		//fprintf(fp,"Remaining CPU bursttime = %d, ", termProc->reCPUburst);
		//fprintf(fp,"Remaining I/O bursttime = %d, ", termProc->reIOburst);
		fprintf(fp,"-------------------------------------------------------------------\n");
		sum_waittime += termProc->WaitingTime;
		sum_turntime += termProc->TurnaroundTime;
	}
	
	avg_waittime = sum_waittime/TermQ_num;
	avg_turntime = sum_turntime/TermQ_num;
	cpu_util = (double)(endTime-idle)/endTime*100;
	fprintf(fp,"End time: %d / CPU Utilization : %.2lf%% \n", endTime, cpu_util);

	fprintf(fp,"Average Waiting Time : %d\n", avg_waittime);
	fprintf(fp,"Average Turnaround Time : %d\n", avg_turntime);
	fprintf(fp,"Idle Time = %d\n", idle);
}

void simulate(int algorithm, int totalnum)			//Simulate algorythm
{
	if( JobQ_num != totalnum )
		error("Fail Process Create\n");

	//print_tempJobQ();

	while( TermQ_num != totalnum )
		progress(algorithm);

	endTime = --count;

	Report(algorithm);

	//print_TermQ();

	count = 0;
	endTime = 0;
	idle = 0;
	
	RunningProc = NULL;

	Init_TermQ();

	Copy_JobQ();
}

ProcPtr FCFS_alg()
{
	ProcPtr NextProc = ReadyQ[0];

	if( RunningProc != NULL)
		return RunningProc;
	else
	{
		if ( NextProc != NULL)
			return removefrom_ReadyQ(ReadyQ[0]);
		else
			return NULL;
	}
}

ProcPtr nonSJF_alg()
{
	int i;

	ProcPtr NextProc = ReadyQ[0];
	
	if( RunningProc == NULL )
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if(NextProc->reCPUburst > ReadyQ[i]->reCPUburst)
					NextProc = ReadyQ[i];
			}
			return removefrom_ReadyQ(NextProc);
		}
		else
			return NULL;
	}
	else
		return RunningProc;
}

ProcPtr SJF_alg()
{
	int i;

	ProcPtr NextProc = ReadyQ[0];

	if( RunningProc == NULL )
		return nonSJF_alg();
	else
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if(NextProc->reCPUburst > ReadyQ[i]->reCPUburst)
					NextProc = ReadyQ[i];
			}
			if( NextProc->reCPUburst < RunningProc->reCPUburst )
				return removefrom_ReadyQ(NextProc);
			else 
				return RunningProc;
		}
		else
			return RunningProc;
	}
}

ProcPtr nonPrty_alg()
{
	int i;

	ProcPtr NextProc = ReadyQ[0];
	
	if( RunningProc == NULL )
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if(NextProc->priority > ReadyQ[i]->priority)
					NextProc = ReadyQ[i];
			}
			return removefrom_ReadyQ(NextProc);
		}
		else
			return NULL;
	}
	else
		return RunningProc;
}

ProcPtr Prty_alg()
{
	int i;

	ProcPtr NextProc = ReadyQ[0];

	if( RunningProc == NULL )
		return nonPrty_alg();
	else
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if( NextProc->priority > ReadyQ[i]->priority )
					NextProc = ReadyQ[i];
			}
			if( NextProc->priority < RunningProc->priority )
				return removefrom_ReadyQ(NextProc);
			else 
				return RunningProc;
		}
		else
			return RunningProc;
	}
}

ProcPtr RR_alg()
{
	ProcPtr NextProc = ReadyQ[0];

	if( RunningProc == NULL )
		return removefrom_ReadyQ(ReadyQ[0]);
	else
	{
		if( NextProc != NULL )
		{
			if( RunningProc->ActiveTime == TIME_QUANTUM )
				return removefrom_ReadyQ(ReadyQ[0]);
			else
				return RunningProc;
		}
		else
			return RunningProc;
	}
}

ProcPtr nonLIF_alg()
{
	int i;
	ProcPtr NextProc = ReadyQ[0];
	
	if( RunningProc == NULL )
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if(NextProc->reIOburst < ReadyQ[i]->reIOburst)
					NextProc = ReadyQ[i];
			}
			return removefrom_ReadyQ(NextProc);
		}
		else
			return NULL;
	}
	else
		return RunningProc;

}

ProcPtr LIF_alg()
{
	int i;

	ProcPtr NextProc = ReadyQ[0];

	if( RunningProc == NULL )
		return nonLIF_alg();
	else
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if(NextProc->reIOburst < ReadyQ[i]->reIOburst)
					NextProc = ReadyQ[i];
			}
			if( NextProc->reIOburst > RunningProc->reIOburst )
				return removefrom_ReadyQ(NextProc);
			else 
				return RunningProc;
		}
		else
			return RunningProc;
	}
}

ProcPtr nonLISC_alg()
{
	int i;
	
	ProcPtr NextProc = ReadyQ[0];

	if( RunningProc == NULL )
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if(NextProc->reIOburst < ReadyQ[i]->reIOburst)
					NextProc = ReadyQ[i];
				else if(NextProc->reIOburst == ReadyQ[i]->reIOburst)
				{
					if(NextProc->reCPUburst > ReadyQ[i]->reCPUburst)
						NextProc = ReadyQ[i];
				}
			}
			return removefrom_ReadyQ(NextProc);
		}
		else
			return NULL;
	}
	else
		return RunningProc;
}

ProcPtr LISC_alg()
{
	int i;

	ProcPtr NextProc = ReadyQ[0];

	if( RunningProc == NULL )
		return nonLISC_alg();
	else
	{
		if( NextProc != NULL)
		{
			for(i=0; i<ReadyQ_num; i++)
			{
				if(NextProc->reIOburst < ReadyQ[i]->reIOburst)
					NextProc = ReadyQ[i];
				else if(NextProc->reIOburst == ReadyQ[i]->reIOburst)
				{
					if(NextProc->reCPUburst > ReadyQ[i]->reCPUburst)
						NextProc = ReadyQ[i];
				}
			}
			if( NextProc->IOburst > RunningProc->IOburst )
				return removefrom_ReadyQ(NextProc);
			else 
				return RunningProc;
		}
		else
			return RunningProc;
	}
}

void main(int argc, char* argv[])
{
	int totalnum = 0;

	fp = fopen("/home/project/osproject/output.txt", "w");

	if(argc != 2)
		error("Enter the number of process\n");
	
	totalnum = atoi(argv[1]);
	if(totalnum <= 0)
		error("Wrong Number\n");

	fprintf(fp,"totalnum : %d\n", totalnum);	

	Init_JobQ();
	Init_tempJobQ();
	Init_ReadyQ();
	Init_WaitQ();
	Init_TermQ();

	CreateProcess(totalnum);

	fprintf(fp,"<FCFS Algorithm>\n");
	simulate(0,totalnum);
	fprintf(fp,"<Non Preemptive SJF Algorithm>\n");
	simulate(1,totalnum);
	fprintf(fp,"<Preemptive SKF Algorithm>\n");
	simulate(2,totalnum);
	fprintf(fp,"<Non Preemptive Priority Algorithm>\n");
	simulate(3,totalnum);
	fprintf(fp,"<Preemptive Priority Algorithm>\n");
	simulate(4,totalnum);
	fprintf(fp,"<RoundRobin Algorithm>\n");
	simulate(5,totalnum);
	fprintf(fp,"<Non Preemptive LIF Algorithm>\n");
	simulate(6,totalnum);
	fprintf(fp,"<Preemptive LIF Algorithm>\n");
	simulate(7,totalnum);
	fprintf(fp,"<Non Preemptive LISC Algorithm>\n");
	simulate(8,totalnum);
	fprintf(fp,"<Preemptvie LISC Algorithm>\n");
	simulate(9,totalnum);
	
	Clear_ReadyQ();
	Clear_tempJobQ();
	Clear_WaitQ();
	Clear_TermQ();

	fclose(fp);
}
