#ifndef IO_HPP   //this is a include guard:since this is a header file, we only want its content included once in a single compiled file, so it does what it says
#define IO_HPP

#include<fstream> //provides classes for file input and output operation
#include<string>
#include<iostream>

//:fstream:can read write both on file
//gives ifstream and ofstream class too: can only write and read respectively from 'em


class IO{
    std::fstream file_stream;

    public:
    IO(const std::string &file_path);  //const so no messup with file path
    ~IO();
    std::fstream getFileStream();      //fstream type ka object is returned, meaning, caller of this fn can read/write both on file with ease

};
#endif


//this is header