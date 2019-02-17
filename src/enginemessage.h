#pragma once
#include<iostream>
#include<string>
#include<sstream>
class EngineMessage{
private:
    EngineMessage(){};
public:
    static std::ostringstream message;
    static std::ostringstream simulationTree;
    static std::string getMessage();
    static void printMessageToConsole();
    static void printSTree();
};

