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

#include "shmSegment.h"

int main( int argc , char *argv[] )
{
   int        shmid ;
   key_t      shmkey;
   int        shmflg;
   shmData    *p;

   shmkey = ftok("shmSegment.h" , 0 ) ;
   shmflg = IPC_CREAT | S_IRUSR | S_IWUSR ;

   shmid  = shmget( shmkey , SHMEM_SIZE , shmflg ) ;
   if ( shmid == -1 ) {
      printf ("\nFailed to get shared memory id=%d\n" , shmid );
      perror("Reason: ");
      exit( -1 ) ;
   }

   p  = (shmData *) shmat( shmid , NULL , 0 ); // Attach for R/W
   if ( p == (shmData *) -1 ) {
      printf ("\nFailed to attach shared memory id=%d\n" , shmid );
      perror("Reason: ");
      exit( -1 ) ;
   }

   /* Initialize data in the shared memory  */ 
   p->counter = 0;  p->p1Done  = 0;   p->p1Started = 1 ; 
   
   printf("P1 started. LARGEINT = %10lu\n" , LARGEINT );
   printf("Waiting for P2 to start, too.\n");
   /* Wait for Other Process to start   */
   while ( p-> p2Started != 1 ) ;

   printf("P1 now will increment the counter\n");
   for( unsigned i=1 ; i <= LARGEINT ; i++ )
        p->counter ++;

   p->p1Done  = 1; 
   printf("P1 is done. Waiting for P2 to finish, too.\n");
   while ( p-> p2Done != 1 ) ;

   unsigned long expected = LARGEINT << 1 ;  // 2*LARGEINT
   printf("Final counter value = %10u  Expecting: %10lu" 
          , p->counter , expected );

   if ( p->counter == expected )    printf("    CORRECT\n") ;
   else  printf("    NOT CORRECT\n") ;
       
   shmdt( p ) ;

   shmctl( shmid , IPC_RMID , NULL );
   
   return 0;
}

