#include <env.h>
#include <pmap.h>
#include <printf.h>

// Trivial temporary clock interrupt handler,
// called from clock_interrupt in locore.S
/*void
clock(void)
{
	printf("*");
}*/


// The real clock interrupt handler,
// implementing round-robin scheduling
void
sched_yield(void)
{
	static int Next_EnvNum=0;
	int iTemp,i;
	//fmars for test
	static int Print_Envid = -1;
	
	int max;
	int flag,charter;
	max = 1;
	for(i = 0;i < NENV;i++)
		if(envs[i].env_status == ENV_RUNNABLE){
			if(envs[i].env_runs == 0)
				env_run(&envs[i]);//charter
			if(envs[i].env_runs > max)
				max = envs[i].env_runs;
	}
	//printf("max = %d\n",max);	 
	for(i = 0;i < NENV;i++)		//envs[0] must be runnable
	{
		if(envs[Next_EnvNum].env_status == ENV_RUNNABLE && envs[Next_EnvNum].env_runs == max)
		{
			iTemp = Next_EnvNum;
			Next_EnvNum++;
			Next_EnvNum = Next_EnvNum%NENV;
			if( envs[iTemp].env_runs > 1 )
				envs[iTemp].env_runs--;
			if(iTemp!=0)
			{
				//panic("BBBBBBBBBBBBBBBBBBBB id:%x",iTemp);
				//fmars for test 
				//if(envs[iTemp].env_id != Print_Envid){
					printf("Clock interrupt handler. Env %x will run ---level=%d\n",envs[iTemp].env_id,envs[iTemp].env_runs);
					Print_Envid = envs[iTemp].env_id;
				//}
				env_run(&envs[iTemp]);
				return;
			}
		}
		else
		{
			Next_EnvNum++;
                        Next_EnvNum = Next_EnvNum%NENV;
		}
	}
	//env_run(&envs[0]);	
	panic("There is no process to run ! haha !\n");
}

