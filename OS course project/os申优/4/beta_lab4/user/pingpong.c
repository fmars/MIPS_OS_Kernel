// Ping-pong a counter between two processes.
// Only need to start one of these -- splits into two with fork.

#include "lib.h"

void
umain(void)
{
	u_int who, i, j;

	writef("pingpong is running\n");
	syscall_set_prio(0,1000);

	if ((who = fork()) != 0) {
		//fmars
		writef("Fmars : parent env\n");
		// get the ball rolling
		//writef("\n@@@@@  send 0 from %x to %x\n", syscall_getenvid(), who);
		writef("Parent begins to send\n");
		ipc_send(who, 7777, 0, 0);
		i=1;
	}else{
		writef("Child begins \n");
		//fmars
		/*	
		writef("Fmars : child env\n");
		writef("child begins to rec\n");
		i = ipc_recv(&who, 0, 0);
		writef("Fmars ipc_recvvvvvvvvvvvvvvvvvv = %d\n",i);
		*/
		i=0;	
	}
/*
	//fmars
	//writef("In user PingPang parent_envid=%x child_envid=%x\n",syscall_getenvid(),who);
	for(;;){
	//for (i=4;i<7;i++) {
		//writef("%x is waiting.....\n",syscall_getenvid());
		j = ipc_recv(&who, 0, 0);
		
		writef("%x got %d from %x\n", syscall_getenvid(), j, who);
	
		//user_panic("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
		if( i==1)
			return;
		i++;
		writef("\n@@@@@send %x from %x to %x\n", i, syscall_getenvid(), who);
		ipc_send(who, i, 0, 0);
		if (i == 1)
			return;
	}
*/
		
}

