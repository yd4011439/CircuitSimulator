#include"component.h"
#include<cmath>
//-----------------------------------Component--------------------------------------------//
float* Component::constMatrix = 0;
float* Component::matrix = 0;
float* Component::nodalVoltage = 0;
float* Component::nodalExcessCurrent = 0;
float* Component::componentVoltage = 0;
float* Component::componentCurrent = 0;
int Component::numOfNodes = 0;
int Component::numOfComponents = 0;
float Component::deltaT = 0;

void Component::setMatCoef(int vertex, float row){
    //std::cout<<"Setting Matrix for: Id("<<id<<") vertex: "<<vertex<<" row "<<row<<std::endl;
    if(vertex==sNode)
        sRow = row;
    else if(vertex==lNode)
        lRow = row;
    else if(vertex == -1)
        magFact = row;
}

void Component::setDeltaT(float t){
    deltaT = t;
}

void Component::printData(){
    //std::cout<<"Id: "<<id<<" sRow: "<<sRow<<" lRow: "<<lRow<<std::endl;
    std::cout<<"Id: "<<id<<" Voltage: "<<componentVoltage[id]<<" Current: "<<componentCurrent[id]<<std::endl;
}

void Component::copyMatrix(){
    //std::cout<<"Matrix Coefficients: "<<std::endl;
    for(int i=0;i<numOfNodes*(numOfNodes-1);i++){
            matrix[i] = constMatrix[i];
            //std::cout<<matrix[i]<<"\t";
            //if(i%(numOfNodes-1)==0&&i!=0)
                //std::cout<<std::endl;

    }
    //std::cout<<"Nodal Excess Currents:"<<std::endl;
    for(int i=0;i<numOfNodes;i++){
        //std::cout<<nodalExcessCurrent[i]<<" --> ";
        nodalExcessCurrent[i] = 0;
        //std::cout<<nodalExcessCurrent[i]<<std::endl;
    }
}

void Component::setInitials(int numOfComps, int numOfNod){
    numOfComponents = numOfComps;
    numOfNodes = numOfNod;
    constMatrix = new float[numOfNodes*(numOfNodes-1)];
    matrix = new float[numOfNodes*(numOfNodes-1)];
    for(int i=0;i<numOfNodes*(numOfNodes-1);i++)
        matrix[i] = constMatrix[i] = 0;
    nodalVoltage = new float[numOfNodes];
    nodalExcessCurrent = new float[numOfNodes];
    for(int i=0; i<numOfNodes;i++)
        nodalVoltage[i] = nodalExcessCurrent[i] = 0;
    componentCurrent = new float[numOfComponents];
    componentVoltage = new float[numOfComponents];
    for(int i=0;i<numOfComponents;i++)
        componentCurrent[i] = componentVoltage[i] = 0;
}

Component::~Component(){
    delete constMatrix;
    delete matrix;
    delete nodalVoltage;
    delete nodalExcessCurrent;
    delete componentVoltage;
    delete componentCurrent;
}

//------------------------------------Resistor----------------------------------------------//
void Resistor::setMatrix(){

    if(sRow!=-1){
        Component::constMatrix[sRow*numOfNodes + sNode] += 1/value;
        if(lNode<numOfNodes-1)
            Component::constMatrix[sRow*numOfNodes + lNode] -= 1/value;
        //std::cout<<"Id: "<<id<<"Value is: "<<Component::constMatrix[sRow*numOfNodes + lNode]<<std::endl;
    }
    if(lRow!=-1&&lNode<numOfNodes-1){
        Component::constMatrix[lRow*numOfNodes + lNode] += 1/value;
        Component::constMatrix[lRow*numOfNodes + sNode] -= 1/value;
    }
    //std::cout<<"The matrix has been set and the coefficients are: "<<Component::constMatrix[sRow*numOfNodes + sNode]<<" "<<std::endl;
}
void Resistor::setConstant(){
    Component::componentVoltage[id] = nodalVoltage[sNode] - nodalVoltage[lNode];
    Component::componentCurrent[id] = Component::componentVoltage[id]/value;
    Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];
    //std::cout<<"RES: NEC: sNODE "<<Component::nodalExcessCurrent[sNode]<<", lNODE: "<<Component::nodalExcessCurrent[lNode]<<" COMP I: "<<Component::componentCurrent[id]<<std::endl;
}

//----------------------------Inductor-----------------------------------------------//
void Inductor::setConstant(){
    Component::componentVoltage[id] = nodalVoltage[sNode] - nodalVoltage[lNode];
    Component::componentCurrent[id] = Component::componentCurrent[id] + deltaT*Component::componentVoltage[id]/value;
    //std::cout<<"Voltage Across Inductor "<<componentVoltage[id]<<" Current through inductor "<<componentCurrent[id]<<std::endl;
    Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];

    if(sRow!=-1)
        Component::constMatrix[sRow*numOfNodes + numOfNodes - 1] -= deltaT*Component::componentVoltage[id]/value;
    if(lRow!=-1&&lNode<numOfNodes-1)
        Component::constMatrix[lRow*numOfNodes + numOfNodes - 1] += deltaT*Component::componentVoltage[id]/value;
}
void Inductor::setMatrix(){
    if(sRow!=-1)
        Component::constMatrix[sRow*numOfNodes + numOfNodes - 1] -= Component::componentCurrent[id];
    if(lRow!=-1&&lNode<numOfNodes-1)
        Component::constMatrix[lRow*numOfNodes + numOfNodes - 1] += Component::componentCurrent[id];
}


//------------------------Capacitor-------------------------------------------------//
void Capacitor::setConstant(){
    if(lRow==sNode){
        Component::componentCurrent[id] = magFact*Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];
    }
    else{
        Component::componentCurrent[id] = -magFact*Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    }


    Component::componentVoltage[id] = Component::componentVoltage[id] + componentCurrent[id]*deltaT/value;
    Component::constMatrix[sRow*numOfNodes+numOfNodes-1] = Component::componentVoltage[id];

    //std::cout<<"CAP: NEC: sNODE "<<Component::nodalExcessCurrent[sNode]<<", lNODE: "<<Component::nodalExcessCurrent[lNode]<<" COMP I: "<<Component::componentCurrent[id]<<std::endl;
}
void Capacitor::setMatrix(){
    Component::constMatrix[sRow*numOfNodes + numOfNodes-1] = Component::componentVoltage[id];
    Component::constMatrix[sRow*numOfNodes + sNode] = 1;
    if(lNode<numOfNodes-1)
        Component::constMatrix[sRow*numOfNodes+ lNode] = -1;
}


/*---------------------------CurrentSource-------------------------------------------*/
void CurrentSource::setMatrix(){
    if(sRow!=-1)
        Component::constMatrix[sRow*numOfNodes + numOfNodes - 1] -= value;
    if(lRow!=-1&&lNode<numOfNodes-1)
        Component::constMatrix[lRow*numOfNodes + numOfNodes - 1] += value;
}
void CurrentSource::setConstant(){
    Component::componentVoltage[id] = nodalVoltage[sNode] - nodalVoltage[lNode];
    Component::componentCurrent[id] = value;//Need Not be placed at all because it is always constant
    Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];
}


/*---------------------------VoltageSource-------------------------------------------*/
void VoltageSource::setConstant(){
    if(lRow==sNode){
        Component::componentCurrent[id] = Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];
    }
    else{
        Component::componentCurrent[id] = -Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    }
    //std::cout<<"IVS: NEC: sNODE "<<Component::nodalExcessCurrent[sNode]<<", lNODE: "<<Component::nodalExcessCurrent[lNode]<<" COMP I: "<<Component::componentCurrent[id]<<std::endl;
}
void VoltageSource::setMatrix(){
    Component::constMatrix[sRow*numOfNodes + numOfNodes-1] = value;
    Component::constMatrix[sRow*numOfNodes + sNode] = 1;
    if(lNode<numOfNodes-1)
        Component::constMatrix[sRow*numOfNodes+ lNode] = -1;
    Component::componentVoltage[id] = value;
}

//------------------------------------------------FinishedTheIndependentComponents----------------------------------------------------//

