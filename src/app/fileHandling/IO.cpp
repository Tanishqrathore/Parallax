#include <iostream>
#include "IO.hpp"
#include<fstream>


//IO class ka constructor yehan implement hora hai
IO::IO(const std:: string &file_path){
   file_stream.open(file_path,std::ios::in | std::ios::out | std::ios::binary);  //mode of file opening: reading mode wala bit OR writing mode wala bit OR content shud be in biinary : these are ORed bit flags
                              
   if(!file_stream.is_open()){
    std::cout<<"Unable to Open the file: "<<file_path<<std::endl;
   }
}
//OS allows each process to only open a fixed amount of file handle's,so in order to avoid duplicacy and open a fresh new handle to the file
//just move the ownership to th desrired stream: genius.
std::fstream IO::getFileStream(){
    return std::move(file_stream);
}

IO::~IO(){
    if(file_stream.is_open()){
        file_stream.close();
    }
}