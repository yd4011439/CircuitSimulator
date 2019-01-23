#pragma once
#include<iostream>
class NodalSolver;

/*-------------------------------------ComponentType------------------------------------------------*/
enum ComponentType{
	RESISTOR, CAPACITOR, INDUCTOR, ICS, IVS, CCVS, VCVS, CCCS, VCCS, NAC
};



/*-------------------------------------ComponentClass------------------------------------------------*/
class Component{
protected:
    /*---------------------------------------Constants------------------------------------------------*/
    int id;
    float value;
	int sRow;
	int lRow;//Current seekingNode for voltage Sources
	float magFact;
	static float deltaT;
	//const int sNode;
    //const int lNode;
    static int numOfNodes;
    static int numOfComponents;
public:
    /*--------------------------------------Matrices-------------------------------------------------*/
    const int sNode;
    const int lNode;
    static float *constMatrix;//Matrix for keeping like the state
    static float *matrix;//Matrix for solving
    static float *nodalVoltage;//NodalVoltage Which is obtained after nodalAnalysis
    static float *nodalExcessCurrent;//Excess Current in a node useful for voltage sources and capacitors set by the setConstant() function
    static float *componentVoltage;//Component Voltages set by the setConstant() function
    static float *componentCurrent;//Component Current set by the setConstant() function

    /*------------------------Functions Typical to Component Class-----------------------------------*/
    Component(int a,int b, int id):id(id),sRow(a),lRow(b),magFact(0),sNode(a),lNode(b){}
    static void setDeltaT(float val);
	void setMatCoef(int vertex, float row);
	static void copyMatrix();
	static void setInitials(int , int);
	~Component();
	//For debug purposes only:
	void printData();

    /*-----------------------Functions That need to be Overloaded-----------------------------------*/
	virtual void setConstant(){}//Will set the voltage across the component and the current through the component inside the componentVoltage and componentCurrent matrices respectively
	virtual void setMatrix(){}//Will set the coefficient of matrices according to the value of sRow and lRow
    virtual ComponentType getType(){    return NAC;}//Will return the type of Component
    /*Friended Class*/
    friend class NodalSolver;

};


/*------------------------------------ResistorClass-------------------------------------------------*/
class Resistor:public Component{
public:
	Resistor(int id, int sNode, int lNode, float val):Component(sNode, lNode, id){ Component::value = val;}
	void setMatrix();
	void setConstant();
	ComponentType getType(){    return RESISTOR;}
};


/*-----------------------------------InductorClass---------------------------------------------------*/
class Inductor:public Component{
public:
    Inductor(int id, int sNode, int lNode, float val):Component(sNode, lNode, id){ Component::value = val;}
    void setMatrix();
    void setConstant();
    ComponentType getType(){    return INDUCTOR;}
};


/*-------------------------------------CapacitorClass-------------------------------------------------*/
class Capacitor:public Component{
//lRow is the multiplicationFactor
public:
    Capacitor(int id, int sNode, int lNode, float val):Component(sNode, lNode, id){ Component::value = val;  lRow = 1.0;}
    void setMatrix();
    void setConstant();
    ComponentType getType(){    return CAPACITOR;}
};


/*-------------------------------------CurrentSourceClass-------------------------------------------------*/
class CurrentSource:public Component{
public:
    CurrentSource(int id, int sNode, int lNode, float val):Component(sNode, lNode, id){ Component::value = val;}
    void setMatrix();
    void setConstant();
    ComponentType getType(){    return ICS;}
};


/*-------------------------------------VoltageSourceClass-------------------------------------------------*/
class VoltageSource:public Component{
public:
    VoltageSource(int id, int sNode, int lNode, float val):Component(sNode, lNode, id){ Component::value = val;}
    void setMatrix();
    void setConstant();
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
