#include<iostream>
#include"nodalsolver.h"
#include<math.h>
#include"gmath.h"
#include"parser.h"

int main(){
    std::string a = "#ctype \"NODED\"\n"
    "#component type=\"RESISTOR\" nodes=(0,1) value=(100);"
    "#component type=\"IVS\" nodes=(0,1) value=(-100);"
    "#component type=\"RESISTOR\" nodes=(0,1) value=(1000);";


    /*"#grid(2000.0f,1000.0f)\n"
    "#wire (2,2)(2,1)(8,1)(8,2)"
    "#wire (5,2)(5,1)"
    "#wire (8,4)(8,5)(2,5)(2,4)"
    "#wire (5,4)(5,5)";*/

    Parser p;
    //p.setWithString(a);
    //if(p.genList()==false)
        //EngineMessage::printMessageToConsole();
    //EngineMessage::printSTree();
    p.CommandHandler();
    std::cout<<p.wire.getWireAsString()<<std::endl;
    return 0;

}

