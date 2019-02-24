#pragma once
#include<iostream>
#include"gmath.h"
#include"enginemessage.h"

class NodalSolver;
class Parser;

/*-------------------------------------ComponentType------------------------------------------------*/
enum ComponentType{
	RESISTOR, CAPACITOR, INDUCTOR, ICS, IVS, CCVS, VCVS, CCCS, VCCS, TCVS, TCCS, DERIVED, NAC
};



/*-------------------------------------ComponentClass------------------------------------------------*/
class Component{
protected:
    /*---------------------------------------Constants------------------------------------------------*/
    int id;
    double value;//nNode for voltageControlled Sources, value for passive independent components
	int sRow;//matrixRow for voltageSources and currentSources towards sNode side
	int lRow;//Current seekingNode for voltage Sources and matrixRow for currentSources towards lNode
	double magFact;//Useful for capacitor and for voltageControlled sources for storing pNode
	static double deltaT;//stepping time
	static double timeElapsed;
    static int numOfNodes;
    static int numOfComponents;
    static bool InaccurateFlag;
    static int idCounter;
public:
    /*-----------------------------------PublicVariables---------------------------------------------*/
    int sNode;
    int lNode;
    static bool errorFlag;
    /*--------------------------------------Matrices-------------------------------------------------*/
    static double *constMatrix;//Matrix for keeping like the state
    static double *matrix;//Matrix for solving
    static double *nodalVoltage;//NodalVoltage Which is obtained after nodalAnalysis
    static double *nodalExcessCurrent;//Excess Current in a node useful for voltage sources and capacitors set by the setConstant() function
    static double *componentVoltage;//Component Voltage set by the setConstant() function
    static double *componentCurrent;//Component Current set by the setConstant() function
    static double *copyOfNodalVoltage;//Copy is needed if our solution yields inaccurate result
    static double *copyOfComponentVoltage;//Copy is needed if our solution yields inaccurate result
    static double *copyOfComponentCurrent;//Copy is needed if our solution yields inaccurate result

    /*------------------------Functions Typical to Component Class-----------------------------------*/
    Component(int a,int b, int i=0):magFact(0){
        if(a>b){
            lRow = lNode = a;
            sRow = sNode = b;
        }else{
            sRow = sNode = a;
            lRow = lNode = b;
        }
        id = idCounter++;
        std::cout<<"sNode = "<<sNode<<" lNode = "<<lNode<<std::endl;
        std::cout<<"Id = "<<id<<std::endl;
    }
    static void setDeltaT(double val);
	void setMatCoef(int vertex, double row);
	static void copyMatrix();
	static void setInitials(int , int);
	static void reset();
    int getId(){    return id;}
	static void reverseCopy();
	~Component();
	//For debug purposes only:
	void printData();

    /*-----------------------Functions That need to be Overloaded-----------------------------------*/
	virtual void setConstant(){}//Will set the voltage across the component and the current through the component inside the componentVoltage and componentCurrent matrices respectively
	virtual void setMatrix(){}//Will set the coefficient of matrices according to the value of sRow and lRow
    virtual ComponentType getType(){    return NAC;}//Will return the type of Component

    /*Friended Class*/
    friend class NodalSolver;
    friend class Parser;

};


/*------------------------------------ResistorClass-------------------------------------------------*/
class Resistor:public Component{
public:
	Resistor(int id, int sNode, int lNode, double val):Component(sNode, lNode, id){ Component::value = val;}
	void setMatrix();
	void setConstant();
	ComponentType getType(){    return RESISTOR;}
};


/*-----------------------------------InductorClass---------------------------------------------------*/
class Inductor:public Component{
public:
    Inductor(int id, int sNode, int lNode, double val):Component(sNode, lNode, id){ Component::value = val;}
    void setMatrix();
    void setConstant();
    ComponentType getType(){    return INDUCTOR;}
};


/*-------------------------------------CapacitorClass-------------------------------------------------*/
class Capacitor:public Component{
public:
    Capacitor(int id, int sNode, int lNode, double val):Component(sNode, lNode, id){ Component::value = val;  lRow = 1.0;}
    void setMatrix();
    void setConstant();
    ComponentType getType(){    return CAPACITOR;}
};


/*-------------------------------------CurrentSourceClass-------------------------------------------------*/
class CurrentSource:public Component{
public:
    CurrentSource(int id, int sNode, int lNode, double val):Component(sNode, lNode, id){ Component::value = val;}
    virtual void setMatrix();
    virtual void setConstant();
    ComponentType getType(){    return ICS;}
};


/*-------------------------------------VoltageSourceClass-------------------------------------------------*/
class VoltageSource:public Component{
public:
    VoltageSource(int id, int sNode, int lNode, double val):Component(sNode, lNode, id){ Component::value = val;
        std::cout<<"From Voltage Source: ID: "<<Component::id<<" sNode: "<<Component::sNode<<"lNode = "<<Component::lNode<<std::endl;
    }
    virtual void setMatrix();
    virtual void setConstant();
    ComponentType getType(){    return IVS;}
};

/*----------------------------------NotA ComponentClass--------------------------------------------------*/
//Will be used as a flag to say there is not a component
class NaC:public Component{
private:
    NaC():Component(0,0,0){}
public:
    static NaC nac;
    ComponentType getType(){    return NAC;}
};

/*----------------------------------------------------VCVS---------------------------------------------------------------------------------*/
//Can raise the flag InaccurateFlag
class VcvS: public Component{
protected:
    LID lid;
    bool orientation;
public:
    VcvS(int id, int sNode, int lNode, int pNode, int nNode,double (*f)(double,double*),double *valPointer, double t,bool Orientation):Component(sNode,lNode,id){
        if(pNode==nNode||(pNode==sNode&&nNode==lNode)||(pNode==lNode&&nNode==sNode)){
            Component::errorFlag = true;
            EngineMessage::message<<"Component Id: "<<id<<" Type: VCVS:- Invalid Reference\n"
            <<"A voltage controlled source can't have its own nodes as reference node\n\n";
        }
        orientation = Orientation;
        lid.setMembers(f,valPointer,t);
        Component::magFact = pNode;
        Component::value = nNode;
    }
    void setMatrix();
    void setConstant();//Will raise the flag InaccurateFlag if Necessary
    ComponentType getType(){    return VCVS;}
};

/*-------------------------------------------------------voltageControlledCurrentSource--------------------------------------------------*/
class VccS:public Component{
protected:
    LID lid;
    double preSlope;
    double preConst;
    bool orientation;
public:
    VccS(int id, int sNode, int lNode, int pNode, int nNode,double (*f)(double,double*),double *valPointer, double t,bool Orientation):Component(sNode,lNode,id){
        if(pNode==nNode){
            Component::errorFlag = true;
            EngineMessage::message<<"Component Id: "<<id<<" Type: VCCS:- Invalid Reference\n"
            <<"Reference nodes should be different\n\n";
        }
        preSlope = 0;
        preConst = 0;
        lid.setMembers(f,valPointer,t);
        orientation = Orientation;
        Component::magFact = pNode;
        Component::value = nNode;
    }
    void setMatrix();
    void setConstant();
    ComponentType getType(){    return VCCS;}
};

/*-------------------------------------------------TimeDependentSources----------------------------------------------------------------*/
class TimeDependentVoltageSource:public VoltageSource{
private:
    double (*f)(double,double*);
    double* values;
public:
    TimeDependentVoltageSource(int id, int sNode, int lNode,double (*fun)(double,double*), double* val):VoltageSource(id,sNode,lNode,value){
        f = fun;
        values = val;
    }
    void setMatrix();
    void setConstant();
};

/*-------------------------------------------------TimeDependentCurrentSource----------------------------------------------------------------*/
class TimeDependentCurrentSource:public CurrentSource{
private:
    double preValue;
    double (*f)(double,double*);
    double *values;
public:
    TimeDependentCurrentSource(int id, int sNode, int lNode,double (*fun)(double,double*), double* val):CurrentSource(id,sNode,lNode,value){
        f = fun;
        values = val;
    }
    void setMatrix();
    void setConstant();
};
