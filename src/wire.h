#pragma once
#include"lists.h"
#include<sstream>
#include<string>
enum WireSelection{
    SELECTABLE, NOTHING, JOINT, CROSSOVER
};
class Wire{
private:
    LinkedList<int> allWire;
    LinkedList<unsigned>* currentWire;
    bool alreadySet;
    unsigned zoomLevel;
    bool checkLine(unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned x, unsigned y);
    void getLine(unsigned x, unsigned y, int* points, unsigned position, unsigned line[4]);
public:
    /*----------------------------------FunctionsNotTypicalToGUI------------------------------------*/
    Wire(){}
    void addPoint(unsigned, unsigned);
    void End(unsigned, unsigned);
    void addLabel(){    allWire.append(-1);}
    LinkedList<int> getJoinedPoints();
    std::string getWireAsString();

    /*-----------------------------------FunctionsTypicalToGUI-------------------------------------*/
    void setZoomLevel(unsigned z);
    void deleteLast();
    bool deleteWire(unsigned x, unsigned y);
    WireSelection getWireStatus(unsigned,unsigned);

};
