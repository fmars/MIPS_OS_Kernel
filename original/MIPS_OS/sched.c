#include "include/env.h"
#include "include/printf.h"

int SCHED_N;

extern void env_idle_run();

extern int Next_EnvNum;

extern void print_num_hex2(unsigned int *n);


void
sched_yield(void)  // 进程调度函数
{
	int iTemp,i;
	int id;
	
	printf("Enter Schedule Env  ");
	
	for(i = 0;i < NENV;i++)		//envs[0] must be runnable
	{
		if(envs[Next_EnvNum].env_status == ENV_RUNNABLE)
		{
			iTemp = Next_EnvNum;
			Next_EnvNum++;
			Next_EnvNum = Next_EnvNum%NENV;
			if(iTemp!=0)
			{
				printf("Schedule Env ");
				id=ENVX((&envs[iTemp])->env_id);
				print_num_hex2(&id);
				printf(" to run...\n");
				
				env_run(&envs[iTemp]);
			}
		}
		else
		{
					Next_EnvNum++;
            		Next_EnvNum = Next_EnvNum%NENV;
		}
	}
	
	printf("Schedule end. There is no process to run...\n");

	env_idle_run();

}

