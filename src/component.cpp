#include"component.h"
#include<cmath>
//-----------------------------------Component--------------------------------------------//
double* Component::constMatrix = 0;
double* Component::matrix = 0;

double* Component::nodalVoltage = 0;
double* Component::nodalExcessCurrent = 0;
double* Component::copyOfNodalVoltage = 0;

double* Component::componentVoltage = 0;
double* Component::componentCurrent = 0;
double* Component::copyOfComponentVoltage = 0;
double* Component::copyOfComponentCurrent = 0;

int Component::numOfNodes = 0;
int Component::numOfComponents = 0;
double Component::deltaT = 0.00001;
double Component::timeElapsed = 0;
int Component::idCounter = 0;

bool Component::InaccurateFlag = false;
bool Component::errorFlag = false;

void Component::setMatCoef(int vertex, double row){
    //std::cout<<"Setting Matrix for: Id("<<id<<") vertex: "<<vertex<<" row "<<row<<std::endl;
    if(vertex==sNode)
        sRow = row;
    else if(vertex==lNode)
        lRow = row;
    else if(vertex == -1)
        magFact = row;
}

void Component::setDeltaT(double t){
    deltaT = t;
}

void Component::printData(){
    std::cout<<"Id: "<<id<<" sRow: "<<sRow<<" lRow: "<<lRow<<std::endl;
    std::cout<<"Id: "<<id<<" Voltage: "<<componentVoltage[id]<<" Current: "<<componentCurrent[id]<<std::endl;
}

void Component::copyMatrix(){
    timeElapsed +=deltaT;
    for(int i=0;i<numOfNodes*(numOfNodes-1);i++){
            matrix[i] = constMatrix[i];
    }
    for(int i=0;i<numOfNodes;i++){
        nodalExcessCurrent[i] = 0;
        copyOfNodalVoltage[i] = nodalVoltage[i];
    }
    for(int i=0; i<numOfNodes;i++){
        copyOfComponentCurrent[i] = componentCurrent[i];
        copyOfComponentVoltage[i] = componentVoltage[i];
    }
}

void Component::reverseCopy(){
    for(int i=0;i<numOfNodes*(numOfNodes-1);i++){
            matrix[i] = constMatrix[i];
    }
    for(int i=0;i<numOfNodes;i++){
        nodalExcessCurrent[i] = 0;
        nodalVoltage[i] = copyOfNodalVoltage[i];
    }
    for(int i=0; i<numOfNodes;i++){
        componentCurrent[i] = copyOfComponentCurrent[i];
        componentVoltage[i] = copyOfComponentVoltage[i];
    }
}

void Component::setInitials(int numOfComps, int numOfNod){
    numOfComponents = numOfComps;
    numOfNodes = numOfNod;
    constMatrix = new double[numOfNodes*(numOfNodes-1)];
    matrix = new double[numOfNodes*(numOfNodes-1)];

    nodalVoltage = new double[numOfNodes];
    copyOfNodalVoltage = new double[numOfNodes];
    nodalExcessCurrent = new double[numOfNodes];
    nodalExcessCurrent[numOfNodes-1]=0;

    componentCurrent = new double[numOfComponents];
    copyOfComponentCurrent = new double[numOfComponents];
    componentVoltage = new double[numOfComponents];
    copyOfComponentVoltage = new double[numOfComponents];
    reset();
}

void Component::reset(){
    for(int i=0;i<numOfNodes*(numOfNodes-1);i++)
            constMatrix[i]=matrix[i] = constMatrix[i] = 0;
    for(int i=0; i<numOfNodes;i++)
        copyOfNodalVoltage[i]=nodalVoltage[i] = nodalExcessCurrent[i] = 0;
    for(int i=0;i<numOfComponents;i++)
       copyOfComponentVoltage[i] = copyOfComponentCurrent[i] = componentCurrent[i] = componentVoltage[i] = 0;
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


/*-----------------------------------------------------------DependentSources----------------------------------------------------------------*/
/*------------------------------VCVS----------------------------*/
void VcvS::setConstant(){
    if(lRow==sNode){
        Component::componentCurrent[id] = Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];
    }
    else{
        Component::componentCurrent[id] = -Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    }
    Component::componentVoltage[id] = (Component::nodalVoltage[(int)magFact]-Component::nodalVoltage[(int)value])*lid.getSlope() + lid.getConst();

    if(lid.doesFit(Component::nodalVoltage[(int)magFact]-Component::nodalVoltage[(int)value],Component::componentVoltage[id])==false)
        Component::InaccurateFlag = true;
    else
        Component::InaccurateFlag = false;

    if(orientation==false)
        Component::componentCurrent[id] = -Component::componentVoltage[id];
}

void VcvS::setMatrix(){
    double slope = lid.getSlope();
    double constTerm = lid.getConst();
    if(orientation == false){
        slope = -slope;
        constTerm = -constTerm;
    }
    Component::constMatrix[sRow*numOfNodes + numOfNodes-1] = lid.getConst();
    Component::constMatrix[sRow*numOfNodes + sNode] = 1;
    if(lNode<numOfNodes-1)
        Component::constMatrix[sRow*numOfNodes+ lNode] = -1;

    if(magFact<numOfNodes-1)
        Component::constMatrix[sRow*numOfNodes + (int)magFact] = -lid.getSlope();
    if(value<numOfNodes-1)
        Component::constMatrix[sRow*numOfNodes + (int)value] = +lid.getSlope();
}

/*----------------------------------------------------VCCS---------------------------------------------------------------------------*/
//Will Model Diode in the future
void VccS::setConstant(){
    Component::componentVoltage[id] = nodalVoltage[sNode] - nodalVoltage[lNode];
    Component::componentCurrent[id] = (Component::nodalVoltage[(int)magFact]-Component::nodalVoltage[(int)value])*lid.getSlope() + lid.getConst();

    if(lid.doesFit(Component::nodalVoltage[(int)magFact]-Component::nodalVoltage[(int)value],Component::componentCurrent[id])==false)
        Component::InaccurateFlag = true;
    else
        Component::InaccurateFlag = false;

    if(orientation==false)
        Component::componentCurrent[id] = -Component::componentCurrent[id];

    std::cout<<"Id: "<<id<<" voltage: "<<Component::componentVoltage[id]<<" current: "<<Component::componentCurrent[id]<<" Reference Voltage: "<<Component::nodalVoltage[(int)magFact]-Component::nodalVoltage[(int)value]<<std::endl;
    Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];
}

void VccS::setMatrix(){

    double slope = lid.getSlope()-preSlope;
    double constantTerm = lid.getConst()-preConst;

    if(orientation==false){
        slope = -slope;
        constantTerm = -constantTerm;

    }

    if(sRow!=-1){
        Component::constMatrix[sRow*numOfNodes + numOfNodes - 1] -= constantTerm;
        if(magFact<numOfNodes-1)
            Component::constMatrix[sRow*numOfNodes + (int)magFact] += slope;
        if(value<numOfNodes-1)
            Component::constMatrix[sRow*numOfNodes + (int)value] -= slope;
    }
    if(lRow!=-1&&lNode<numOfNodes-1){
        Component::constMatrix[lRow*numOfNodes + numOfNodes - 1] += constantTerm;

        if(magFact<numOfNodes-1)
            Component::constMatrix[lRow*numOfNodes + (int)magFact] -= slope;
        if(value<numOfNodes-1)
            Component::constMatrix[lRow*numOfNodes + (int)value] += slope;
    }
    preSlope = lid.getSlope();
    preConst = lid.getConst();
}


/*------------------------------------------------TimeDependentSources--------------------------------------------------*/
/*-------------------------------------------TimeDependentVoltageSource-------------------------------------------------*/
void TimeDependentVoltageSource::setConstant(){
     if(lRow==sNode){
        Component::componentCurrent[id] = Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];
    }
    else{
        Component::componentCurrent[id] = -Component::nodalExcessCurrent[lRow];
        Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    }
    Component::constMatrix[sRow*numOfNodes + numOfNodes-1] = f(Component::timeElapsed,values);
}


void TimeDependentVoltageSource::setMatrix(){
    Component::constMatrix[sRow*numOfNodes + numOfNodes-1] = f(Component::timeElapsed,values);
    Component::constMatrix[sRow*numOfNodes + sNode] = 1;
    if(lNode<numOfNodes-1)
        Component::constMatrix[sRow*numOfNodes+ lNode] = -1;
    Component::componentVoltage[id] = value;
}

/*------------------------------------TimeDependentCurrentSource--------------------------------------------------*/
void TimeDependentCurrentSource::setMatrix(){
    preValue = f(Component::timeElapsed,values);
    if(sRow!=-1)
        Component::constMatrix[sRow*numOfNodes + numOfNodes - 1] -= preValue;
    if(lRow!=-1&&lNode<numOfNodes-1)
        Component::constMatrix[lRow*numOfNodes + numOfNodes - 1] += preValue;
}

void TimeDependentCurrentSource::setConstant(){
    Component::componentVoltage[id] = nodalVoltage[sNode] - nodalVoltage[lNode];
    Component::componentCurrent[id] = f(Component::timeElapsed,values);
    Component::nodalExcessCurrent[sNode] -= Component::componentCurrent[id];
    Component::nodalExcessCurrent[lNode] += Component::componentCurrent[id];

    double val = f(Component::timeElapsed,values) - preValue;
    if(sRow!=-1)
        Component::constMatrix[sRow*numOfNodes + numOfNodes - 1] -= val;
    if(lRow!=-1&&lNode<numOfNodes-1)
        Component::constMatrix[lRow*numOfNodes + numOfNodes - 1] += val;

    preValue = value*f(Component::timeElapsed,values);
}
