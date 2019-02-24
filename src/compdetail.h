#pragma once
#include"lists.h"
#include"lists.cpp"
#include"component.h"

enum CType{
    WIRED, NODED
};

enum Orientation{
    N, N90, N180, R270, F, F90, F180, F270
};
struct CompDetail;
struct DerivedTypeInfo{
    LinkedList<CompDetail*> components;
    double* valuerange;
    double* defvalue;
    unsigned *pinpoints;
    unsigned imgx, imgy;
    std::string *functionNames;
    unsigned numOfFunction;

    unsigned numOfOutPins;
    unsigned numOfNodes;
    unsigned numOfValueLabel;
    unsigned numOfComp;

    std::string typeLabel;
    std::string *pinLabels;
    std::string *valueLabels;
    //Two functions need to be rewritten
    void loadCompInto(LinkedList<CompDetail*>& l, unsigned* nodes, unsigned& lastAvilableNode);
    void reAssignNode(unsigned* newNode,LinkedList<CompDetail*>& l);
    void reAssignOrientation(LinkedList<CompDetail*>& l);
    DerivedTypeInfo();
    std::string getErrorString();
    void print();

};
struct CompDetail{

    int id;
    unsigned actualId;
    unsigned posx, posy;
    double *values;
    double threshold;
    std::string cType;
    DerivedTypeInfo* Father;

    std::string func;
    std::string label1;
    std::string label2;
    int prnode, nrnode;

    bool orient;
    Orientation *orientation;
    unsigned *nodes;
    unsigned numOfNodes;

    CompDetail():values(0),orientation(0),Father(0){
        prnode=-1;
        nrnode=-1;
        threshold = -10e90;
    }

    void reset(){
        if(values!=0){
            delete values;
            values = 0;
        }
        if(nodes!=0){
            delete nodes;
            nodes = 0;
        }
    }
    bool checkValue(std::string str){
        unsigned i=0;
        for(;i<Father->numOfValueLabel;i++){
            if(str==Father->valueLabels[i])
                break;
        }
        if(i==Father->numOfValueLabel)
            return false;
        return true;
    }
    bool setValue(std::string str,double value){
        unsigned i=0;
        for(;i<Father->numOfValueLabel;i++){
            if(str==Father->valueLabels[i])
                break;
        }
        if(i==Father->numOfValueLabel)
            return false;
        values[i] = value;
        return true;
    }
    ~CompDetail(){
        reset();
    }
    std::string getErrorStringForDefination(){
        std::string errorString = "";
        if(cType==""){
            errorString += "Type of Component not defined";
            return errorString;
        }
        if(cType=="VCCS"||cType=="VCVS"){
            if(prnode==-1)
                errorString += "Positive Reference Node for Voltage Controlled Component not defined\n";
            if(nrnode==-1)
                errorString += "Negatice Reference Node for Voltage Controlled Component not defined\n";
            if(threshold==10e-90)
                errorString += "Thershold for Voltage Controlled Component not defined\n";
        }else if(values==0)
            errorString += "Values not set";
        if(nodes==0)
            errorString += "Nodes not set";
        return errorString;
    }
    CompDetail* getACopy(){
        CompDetail* c = new CompDetail;
        c->id = id;
        c->actualId = actualId;
        c->cType = cType;
        c->Father = Father;
        c->func = func;
        if(nodes!=0){
            unsigned s;
           if(Father!=0)
            s = Father->numOfNodes;
           else
            s = 2;
            c->nodes = new unsigned[s];
           for(unsigned i=0;i<s;i++){
            c->nodes[i] = nodes[i];
           }
        }
        c->label1 = label1;
        c->label2 = label2;
        c->nrnode = nrnode;
        c->prnode = prnode;
        c->numOfNodes = numOfNodes;
        c->orient = orient;
        {
            if(cType=="VCCS"||cType=="TCVS"||cType=="VCVS"||cType=="TCCS"){
                c->values = new double[Father->numOfValueLabel];
                for(unsigned i=0;i<Father->numOfValueLabel;i++){
                    c->values[i] = Father->defvalue[i];
                }
            }else{
                c->values = new double;
                *c->values = *values;
            }
        }
        c->posx = posx;
        c->posy = posy;
        c->threshold = threshold;
        return c;
    }
    void print(){
        std::cout<<"Type: "<<cType<<std::endl;
        if(nodes!=0){
           unsigned s;
           if(Father!=0){
                s = Father->numOfNodes;
           }
            else
                s=2;
           std::cout<<"Nodes: ";
           for(unsigned i=0;i<s;i++){
                std::cout<<nodes[i]<<" ";
           }
           std::cout<<std::endl;
        }else{
            std::cout<<"Nodes Not Defined"<<std::endl;
        }
        if(values!=0){
            unsigned s;
            if(Father!=0)
                s = Father->numOfValueLabel;
            else
                s = 1;
            std::cout<<"Value: ";
            for(unsigned i=0;i<s;i++){
                std::cout<<values[i]<<" ";
            }
            std::cout<<std::endl;
        }
        std::cout<<"Orientation: "<<orient<<" prnode: "<<prnode<<" nrnode "<<nrnode<<std::endl;
        if(cType=="VCCS"||cType=="VCVS"||cType=="TCVS"||cType=="TCCS")
            std::cout<<"Function Label: "<<func<<std::endl;
    }
};



