#include<iostream>
#include<filesystem>
#include <sys/wait.h>
#include <chrono>
#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/Task.hpp"

namespace fs = std::filesystem;

int main(int argc,char *argv[]){
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string directory;
    std::string action;

    std::cout<<"Enter the directory path:"<<std::endl;
    std::getline(std::cin,directory);

    std::cout<<"Enter the action (encrypt/decrypt)"<<std::endl;
    std::getline(std::cin,action);
    
    try{
        std::cout<<1;
        if(fs::exists(directory) && fs::is_directory(directory)){
            ProcessManagement processManagement;
           
            //rec iterator allows to iterate complete file strucutre of the given node(itself + its subdirectories)
            for(const auto&entry: fs::recursive_directory_iterator(directory)){
               
                
                if(entry.is_regular_file()){
                    std::string filePath = entry.path().string();
                    IO io(filePath);
                    std::fstream f_stream = std::move(io.getFileStream());
                    if(f_stream.is_open()){
                       
                        Action taskAction = (action=="encrypt"?Action::ENCRYPT:Action::DECRYPT);
                        auto task = std::make_unique<Task>(std::move(f_stream),taskAction,filePath);
                        processManagement.submitToQueue(std::move(task));
                    }
                    else{
                        std::cout<<"Unable to Open the file"<<std::endl;
                    }
                }
            }

            
           
        }
        else{
            std::cout<<"Invalid directory path: "<<directory<<std::endl;
        }
       
    }
    catch(const fs::filesystem_error &ex){
       std::cout<<"Filesystem error:"<<ex.what()<<std::endl;
    }

    int status;
    pid_t wpid;
    std::cout << "Parent waiting for children to finish..." << std::endl;
    // This loop will wait for all children forked by this parent process
    // that are still running or have exited but not yet reaped.
    // It blocks until at least one child exits.
    // wait() returns -1 when there are no more children to wait for.
    while ((wpid = wait(&status)) > 0) {
        std::cout << "Child with PID " << wpid << " exited with status " << status << std::endl;
    }
    // Check for errors or no children remaining
    if (wpid == -1 && errno == ECHILD) { // ECHILD means no children
        std::cout << "No more children to reap." << std::endl;
    } else if (wpid == -1) {
        perror("wait error"); // Other wait-related errors
    }


    // 3. Capture the end time
    auto end_time = std::chrono::high_resolution_clock::now();

    // 4. Calculate the duration -- THIS LINE MUST BE BEFORE YOU USE 'duration'
    std::chrono::duration<double> duration = end_time - start_time;

    // Output the duration
    std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;

}