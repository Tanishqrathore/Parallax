#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

#include "Task.hpp"
#include<queue>
#include<memory>
#include<atomic>
#include<sys/fcntl.h>
#include<semaphore.h>
#include <mutex>


//

class ProcessManagement{
    
   sem_t* itemsSemaphore;
   sem_t* emptySlotsSemaphore;

    public:
    ProcessManagement();
    ~ProcessManagement(); //destructor to free shared memory
    bool submitToQueue(std::unique_ptr<Task> task);
    void executeTasks();

    private:
    // std::queue<std::unique_ptr<Task>>taskQueue;  //queue stored unique ptr's pointing at task objects

    //we want it to emulate it as a shared queue now
    //this is blueprint for a circular queue that lies in shared memory segment
    struct SharedMemory{
         std::atomic<int> workerCount;
         std::atomic<int>size; // current size of queue
         char tasks[1000][256]; //can store upto 1000 tasks,each of 256bytes max(this 256 bytes is not file size, it is just serialization string size,file toh kitni bhi badi ho no worries: since shm mei toh we are storing the path na, not the file content)
         int front;
         int rear;
    };

    SharedMemory* sharedMem; //pointer that points at start of shared memory segment
    int shmFd;// holds fd to shared memory object
    const char* SHM_NAME = "/my_queue"; //name of shared memory object
    std::mutex queueLock;   //mutex that allows exlusive access to queue.
};
#endif

//here shared memory object doesnt refer to a c++ object, but object in terms of OS is a kernel managed resource
//via posix namin convention: shared memory objcet name starts from /: so here shared memory object name is "/queue"