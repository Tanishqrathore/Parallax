#ifndef TASK_HPP
#define TASK_HPP
#include<string>
#include<fstream>
#include<iostream>
#include<sstream>
#include "../fileHandling/IO.hpp"


enum class Action{
    ENCRYPT,DECRYPT
};

struct Task{
  std::string filePath;
  std::fstream f_stream;
  Action action;

  Task(std::fstream &&stream,Action act,std::string filePath):f_stream(std::move(stream)),action(act),filePath(filePath){}
  
  //serialization: conversion of a task object to a loggable string
  std::string toString(){
    std::ostringstream oss;
    oss<<filePath<<","<<(action==Action::ENCRYPT?"ENCRYPT":"DECRYPT");
    return oss.str();
  }
  
  //deserializtion of string to a task object
  static Task fromString(const std::string &taskData){
    std::istringstream iss(taskData);
    std::string filePath;
    std::string actionStr;
    
    if(std::getline(iss,filePath,',') && std::getline(iss,actionStr)){
        Action action = (actionStr=="ENCRYPT")?Action::ENCRYPT:Action::DECRYPT;
        IO io(filePath);
        std::fstream f_stream=std::move(io.getFileStream());
        if(f_stream.is_open()){
            return Task(std::move(f_stream),action,filePath);
        }
        else{
            throw std::runtime_error("Failed to open file:"+filePath);
        }
    }
    else{
        throw std::runtime_error("Invalid task data format");
    }

  }
};

#endif

