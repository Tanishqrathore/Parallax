#include "Cryption.hpp"
#include "../processes/Task.hpp"
#include "../fileHandling/ReadEnv.cpp"
#include<ctime>
#include<iomanip>

int executeCryption(const std::string &taskData){
    Task task = Task::fromString(taskData); //deserialization into Task object
    ReadEnv env;
    std::string envKey = env.getenv();
    int key = std::stoi(envKey);
    
    //encryption logic
    if(task.action == Action::ENCRYPT){
       char ch; //load each char, one by one in ch
       while(task.f_stream.get(ch)){
        if(ch=='*'){return -1;}
        //hello, if pointer is at h, h loads into ch, pointer moves to e, to write on h, we move pointer back and then we write

         ch=(ch+key)%256; //ch gets values bw 0 and 255
         task.f_stream.seekp(-1,std::ios::cur); //move pointer back
         task.f_stream.put(ch);//write on it
       }
       task.f_stream.close();
    }
    else{
        char ch; //decrypt
        while(task.f_stream.get(ch)){
            ch=(ch-key+256)%256;
            task.f_stream.seekp(-1,std::ios::cur);
            task.f_stream.put(ch);
        }
        task.f_stream.close();
    }

    //using time package: included 2 extra libs
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::cout << "Exiting the encryption/decryption at: " << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;



    return 0;
}