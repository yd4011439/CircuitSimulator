#pragma once
#include"lists.h"
#include"lists.cpp"
#include"component.h"
#include"LinkedGraph.cpp"
#include"enginemessage.h"
#include<math.h>
#include"RowEquivalent.h"

class NodalSolver{
private:
    /*--------------------------------ListOfComponents----------------------------------------*/
    LinkedList<Component*> resistors;
    LinkedList<Component*> inductors;
    LinkedList<Component*> capacitors;
    LinkedList<Component*> iVS;
    LinkedList<Component*> iCS;
    LinkedList<Component*> cCVS;
    LinkedList<Component*> cCCS;
    LinkedList<Component*> vCVS;
    LinkedList<Component*> vCCS;
    LinkedList<Component*> voltageComponents;
    LinkedList<Component*> currentComponents;
    LinkedList<Component*> listOfComponents;
    LinkedList<Component*> simulationList;
    /*--------------------------------NumberOf---------------------------------------*/
    int numOfNodes;
    int numOfComponents;

    /*-------------------------------SuperNodes--------------------------------------*/
    LinkedList<int> supNode;

    /*-------------------------------LinkedGraph------------------------------------*/
    LinkedGraph<Component*> circuit;

    /*------------------------------FalseFlag---------------------------------*/
    static bool errorFlag;

    //Constructor is made private so only one instance of the solver will be present i.e. the static member N Solver
    NodalSolver(){}

    /*------------------------------------PrivateFunctions------------------------------------*/
    void genVertices(LinkedList<Component*>&,int*, int*);
    void calcNumOfNodes();
    void genSuperNodes();
    void checkSuperNode();//Needs setGraph to be called
    bool checkLoopAt(int, Stack<int>&);//Needs setGraph to be called
    void checkSeriesCurrentSource();//Needs setGraph to be called
    void setVoltageComponents();
    void setCurrentComponents();
    void setGraph();
    bool isInvalidLoop(LinkedList<Component*>&);//Will find whether a loop is valid or not
    void setMatCoef();
    bool containsReference(LinkedList<int>&);
    void setCapacitors(LinkedList<int>&,LinkedList<Component*>&);
    void genSimulationList();
    void getSingleSourceVertex(LinkedList<int>&);

public:
    static NodalSolver NSolver;
    void setComponent(ComponentType,LinkedList<Component*>&);
    bool setup();
    void simulate();
    void printAllData();
};
