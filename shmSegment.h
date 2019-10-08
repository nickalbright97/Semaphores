/*-------------------------------------------------------------------------------
IPC using Shared Memory
Written By: 
     1- Dr. Mohamed Aboutabl
-------------------------------------------------------------------------------*/

#ifndef SHMEM_SEGMENT
#define SHMEM_SEGMENT


typedef struct {
    unsigned    counter ;
} shmData ;

#define SHMEM_SIZE sizeof(shmData)
#define LARGEINT   2000000000L

#endif
