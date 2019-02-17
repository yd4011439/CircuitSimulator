#include<iostream>
#include"lists.h"
#include"lists.cpp"
#include"enginemessage.h"
#include<math.h>
#include<cstring>
#include"wire.h"
#include<string>
#include<sstream>
#include"component.h"
#include<fstream>
#include"nodalsolver.h"
#include<stdlib.h>

#define fmessage(x) {   emNo++; EngineMessage::message<<emNo<<". "<<x<<std::endl;  return false;}
#define sTree(x) {  mNo++;  EngineMessage::simulationTree<<mNo<<". "<<x<<std::endl;}
#define eline() EngineMessage::message<<"At Line: "<<l<<" Column: "<<c<<"\n";
#define setline() {l = lineNo; c = column;}
#define print(x); //std::cout<<"At Label: "<<x<<std::endl;

enum CType{
    WIRED, NODED
};

enum Orientation{
    N, N90, N180, R270, F, F90, F180, F270
};

struct CompDetail{
    int id;
    std::ostringstream commandStream;
    unsigned actualId;
    unsigned numOfNodes;
    unsigned startingNode;
    unsigned numberOfValues;
    unsigned x,y;
    double *values;
    std::string label1;
    std::string label2;
    std::string *valueLabels;
    ComponentType cType;
    unsigned *pinNodes;
    Orientation orientation;
    unsigned* nodes;
    CompDetail():values(0),valueLabels(0),pinNodes(0),nodes(0){}
    void reset(){
        if(values!=0){
            delete values;
            values = 0;
        }
        if(valueLabels!=0){
            delete valueLabels;
            valueLabels = 0;
        }
        if(pinNodes!=0){
            delete pinNodes;
            pinNodes = 0;
        }
        if(nodes!=0){
            delete nodes;
            nodes = 0;
        }
    }
    bool checkValue(std::string str){
        unsigned i=0;
        for(;i<numberOfValues;i++){
            if(str==valueLabels[i])
                break;
        }
        if(i==numberOfValues)
            return false;
        return true;
    }
    bool setValue(std::string str,double value){
        unsigned i=0;
        for(;i<numberOfValues;i++){
            if(str==valueLabels[i])
                break;
        }
        if(i==numberOfValues)
            return false;
        valueLabels[i] = value;
        return true;
    }
    ~CompDetail(){
        reset();
    }
};

class Parser{
    /*--------------NotUsefulbutcan'tremove--------------------*/
    unsigned width;
    unsigned height;
    std::string compDefination;
    CType cType;


    /*--------------PositionStuffs----------------------------*/
    unsigned nodeCount;
    unsigned lineNo;
    unsigned column;
    unsigned l, c;
    unsigned bufferOffset;

    /*---------------Buffers------------------------------*/
    char* charBuffer;
    double doubleBuffer[128];
    std::string ifile;
    std::string extractedString;
    double constExtractor;
    CompDetail *component;
    LinkedList<CompDetail*> componentList;
    LinkedList<double> xBuffer;
    LinkedList<double> yBuffer;

    /*-----------MessageStuffs-------------------------*/
    unsigned mNo;
    unsigned emNo;

    /*-------------Observables--------------------------*/
    unsigned varx1;//Node1 for voltage reference
    unsigned varx2;//Node2 for voltage reference
    unsigned vary1;
    unsigned vary2;
    double s;
    double f;
    double exitTime;
    std::ofstream file;

    //Flags
    bool saveTofile;
    bool xSetI;
    bool xSetV;
    bool ySetI;
    bool ySetV;

    //Callsequence functions
    void (*f1)(void);
    void (*f2)(void);

    /*-----------------InterfaceFunctions--------------------*/
    //setxv(unsigned, unsigned);
    //setxi(std::string label);


    //void setyv();
    //void setyi();

    //void setconst(std::string label, double val);
    //void plotxy();

    bool setHandler();
    bool setHandlerHelper(unsigned fid);

    int extractID(std::string);
    bool nodePresent();

    bool setCType();
    bool setDimension();
    bool setComponent();
    bool compString(char *);
    bool extractStringTill(char s);
    bool goAfter(char* a,bool c=true);
    bool readNOrderPair(unsigned n);
    void upDatePosition();
    bool readNum(double&);
    bool setWire();
    bool createComponent();
    bool simulate();

public:
    void setWithString(std::string f){
        lineNo=column=bufferOffset=mNo=emNo=0;
        delete[] charBuffer;
        charBuffer = new char[f.length()+1];
        strcpy(charBuffer,f.c_str());
        charBuffer[f.length()] = 0;
    }
    Wire wire;
    bool genList();
    Parser(){
        charBuffer=new char[100];
        file.open("tmp.txt");
        xSetI=xSetV=ySetV=ySetI=saveTofile=false;
        exitTime = 1;
    }
    void CommandHandler();
};
