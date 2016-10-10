#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include "shmdata.h"

int main()
{
	int running = 1;//程序是否继续运行的标志
	void *shm = NULL;//分配的共享内存的原始首地址
	struct shared_use_st *shared;//指向shm
	int shmid;//共享内存标识符
	//创建共享内存
	shmid = shmget((key_t)1234,sizeof(struct shared_use_st),0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
	 //将共享内存连接到当前进程的地址空间
	shm = shmat(shmid,0,0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\nMemory attached at %x\n",int(shm));
	shared = (struct shared_use_st*)shm;
	shared->written = 0;
	while(running)
	{
		if(shared->written != 0)
		{
			printf("You wrote:%s", shared->text);
			sleep(rand() % 3);
			shared->written = 0;
			if(strncmp(shared->text,"end",3) == 0)
			{
				running = 0;
			}
		}else{
			sleep(1);
		}
	}
	//把共享内存从当前进程中分离  
	if(shmdt(shm) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	//删除共享内存
	if(shmctl(shmid,IPC_RMID,0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}