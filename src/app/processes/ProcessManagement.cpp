#include "ProcessManagement.hpp"
#include<iostream>
#include<cstring>
#include<sys/wait.h>
#include "../encryptDecrypt/Cryption.hpp"
#include<sys/mman.h>
#include<atomic>
#include<sys/fcntl.h>
#include<semaphore.h>

ProcessManagement::ProcessManagement(){
   this->itemsSemaphore = sem_open("/items_semaphore",O_CREAT,0666,0);
   this->emptySlotsSemaphore = sem_open("/empty_slots_semaphore",O_CREAT,0666,1000);





                     //name   //create|open for read write //permissions
    shmFd = shm_open(SHM_NAME,O_CREAT|O_RDWR,0666); //shm_open a function that creates or opens a shared memory object:shared memroy region
    ftruncate(shmFd,sizeof(SharedMemory)); //setting the size of shared memory region: to the size of an instance of struct SharedMemory


                                            //mmap maps the shared memory object to virtual addr space of process and returns a void* pointer, static cast type converts it to SharedMemory* type
    sharedMem=static_cast<SharedMemory *> (mmap(nullptr,sizeof(SharedMemory),PROT_READ|PROT_WRITE,MAP_SHARED,shmFd,0));
    sharedMem->front=0;
    sharedMem->rear=0;
    sharedMem->size.store(0);  //.store(): provided by atomic to set value
    sharedMem->workerCount.store(0);
};

//puts task in queue
bool ProcessManagement::submitToQueue(std::unique_ptr<Task>task){


    sem_wait(emptySlotsSemaphore);//to check if capacity is not allredy full
    std::unique_lock<std::mutex>lock(queueLock); //acquire lock and enter



    // if(sharedMem->size.load()>=1000){return false;}


    //sharedMem->tasks[sharedMem->rear]=task->toString().c_str(); c style array mei no direct use of =
    
    //strcpy(destination addr,src addr): does byte by byte copying of string
    strcpy(sharedMem->tasks[sharedMem->rear],task->toString().c_str()); 
        //2d array decays into a pointer                     //.c_str() returns a pointer to the internally managed char array of the string
        //destination addr                                   //src addr
    


     sharedMem->rear=(sharedMem->rear+1)%1000;
     sharedMem->size.fetch_add(1);
     
     lock.unlock();
     sem_post(itemsSemaphore);

    if(sharedMem->workerCount<8){
        int pid = fork();
        if(pid<0){
        return false; //child process couldn't spin up
        }
        else if(pid>0){
        std::cout<<"Entering the parent process"<<std::endl;
        }
        else{
        sharedMem->workerCount.fetch_add(1);    
        //EXECUTES IF WITHIN CHILD
        std::cout<<"Entering the Child Process"<<std::endl;
        executeTasks();
       }
    }
    
    return true;
}

//currently it consumes 1 task from queue at a time
void ProcessManagement::executeTasks(){
       while (true) {
        sem_wait(itemsSemaphore);

        std::unique_lock<std::mutex> lock(queueLock);

        char taskStr[256];
        strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
        sharedMem->front = (sharedMem->front + 1) % 1000;
        sharedMem->size.fetch_sub(1);

        lock.unlock();
        sem_post(emptySlotsSemaphore);

        
        std::cout << "Executing task in child (PID: " << getpid() << ")\n";
        int num = executeCryption(taskStr);
        if(num==-1){break;}
    }

    exit(0);  // exit only after poison pill
    
}

ProcessManagement::~ProcessManagement(){
    munmap(sharedMem,sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
}

