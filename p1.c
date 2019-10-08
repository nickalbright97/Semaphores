/*-------------------------------------------------------------------------------
Demo: IPC using Shared Memory
Written By: 
     1- Dr. Mohamed Aboutabl
-------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include "shmSegment.h"

int main( int argc , char *argv[] )
{
   int shmkey, shmid, shmflg; 
   int semflg, semmode;
   sem_t *mutex;
   sem_t *p1Started, *p1Done, *p2Started, *p2Done;
   shmData    *p;

   shmflg = IPC_CREAT | S_IRUSR | S_IWUSR ;
   semflg = O_CREAT;
   semmode = S_IRUSR | S_IWUSR;
   mutex = sem_open("/mutex_albrigne", semflg, semmode, 1);
   if (mutex == SEM_FAILED) { perror("sem_open failed"); exit(-1); }
   
   p1Started = sem_open("/p1Started_albrigne", semflg, semmode, 1);
   if (p1Started == SEM_FAILED) { perror("sem_open failed"); exit(-1); }
   
   p1Done = sem_open("/p1Done_albrigne", semflg, semmode, 1);
   if (mutex == SEM_FAILED) { perror("sem_open failed"); exit(-1); }
   
   p2Started = sem_open("/p2Started_albrigne", semflg, semmode, 1);
   if (p1Started == SEM_FAILED) { perror("sem_open failed"); exit(-1); }
   
   p2Done = sem_open("/p2Done_albrigne", semflg, semmode, 1);
   if (p1Started == SEM_FAILED) { perror("sem_open failed"); exit(-1); }

   shmkey =  ftok("shmSegment.h", 0);
   shmid = shmget (shmkey, SHMEM_SIZE, shmflg);
   if (shmid == 01) {
       printf("\nFailed to get shared memory id = %d\n", shmid);
       perror("Reason: ");
       exit(-1);
   }

   p = (shmData *) shmat(shmid, NULL, 0);
   if (p == (shmData*) -1) {
       printf ("\nFailed to attach shared memory id=%d\n", shmid);
       perror("Reason: ");
       exit(-1);
   }

   p->counter = 0;
   
   if (sem_post(p1Started) != 0) { perror("p1_start post failed"); exit(-1); }

   printf("P1 started. LARGEINT = %10lu\n" , LARGEINT );
   printf("Waiting for P2 to start, too.\n");
   if (sem_wait(p2Started) != 0) { perror("p2_started_wait failed"); exit(-1); };

   printf("P1 now will increment the counter\n");
   for( unsigned i=1 ; i <= LARGEINT ; i++ )
        p->counter ++;

   if (sem_post(p1Done) != 0) { perror("p1_done post failed"); exit(-1); }
   printf("P1 is done. Waiting for P2 to finish, too.\n");
   if (sem_wait(p2Done) != 0) { perror("p2_done wait failed"); exit(-1); }

   unsigned long expected = LARGEINT << 1 ;  // 2*LARGEINT
   printf("Final counter value = %10u  Expecting: %10lu" 
          , p->counter , expected );

   if ( p->counter == expected )    printf("    CORRECT\n") ;
   else  printf("    NOT CORRECT\n") ;
       
   shmdt( p ) ;

   shmctl( shmid , IPC_RMID , NULL );
   
   return 0;
}

