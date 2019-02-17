#include"nodalsolver.h"
NodalSolver NodalSolver::NSolver;
bool NodalSolver::errorFlag = false;
/*----------------------------------------------------ForFalseFlagGeneration---------------------------------*/
void genFalseFlag(bool* pointer, int length){
    for(int i=0;i<length;i++)
        pointer[i] = false;
}
void NodalSolver::setComponent(ComponentType ctype,LinkedList<Component*>& receivedList){
        switch(ctype){
        case RESISTOR:
            resistors = receivedList;
        break;
        case INDUCTOR:
            inductors = receivedList;
        break;
        case CAPACITOR:
            capacitors = receivedList;
        break;
        case IVS:
            iVS = receivedList;
        break;
        case ICS:
            iCS = receivedList;
        break;
        case CCCS:
            cCCS = receivedList;
        break;
        case VCCS:
            vCCS = receivedList;
        break;
        case VCVS:
            vCVS = receivedList;
        break;
        case CCVS:
            cCVS = receivedList;
        break;
        default:
            std::cout<<"Component not Identified";
        }
        ////std::cout<<"Number of Components added:"<<receivedList.Length()<<std::endl;
}

void NodalSolver::genVertices(LinkedList<Component*>& list, int *snode, int *lnode){
    int numOfComponents = list.Length();
    ////std::cout<<"From gen vertex"<<std::endl;
    for(int i=0;i<numOfComponents;i++){
        Component* dummy = list.getNextData()->data;
        snode[i] = dummy->sNode;
        lnode[i] = dummy->lNode;
        ////std::cout<<"SNode: "<<snode[i]<<"LNode: "<<lnode[i]<<std::endl;
    }
}

void NodalSolver::setAllComponents(){
    //Voltage Components
    voltageComponents.appendList(capacitors);
    voltageComponents.appendList(iVS);
    voltageComponents.appendList(cCVS);
    voltageComponents.appendList(vCVS);

    //Current Components
    currentComponents.appendList(resistors);
    currentComponents.appendList(inductors);
    currentComponents.appendList(iCS);
    currentComponents.appendList(cCCS);
    currentComponents.appendList(vCCS);

    //ListOfAllComponents
    listOfComponents.appendList(currentComponents);
    listOfComponents.appendList(voltageComponents);

    //ControlledComponents
    controlledComponents.appendList(cCVS);
    controlledComponents.appendList(vCVS);
    controlledComponents.appendList(cCCS);
    controlledComponents.appendList(vCCS);

}
int* genunseparatedNodes(int *data1, int *data2){
    LinkedList<int> data;
    bool *flag = new bool[65536];
    unsigned i;
    for(i=0;i<sizeof(data1);i++){
        int lnode = data1[i];
        int snode = data2[i];
        if(!flag[snode]){
            flag[snode] = true;
            data.append(snode);
        }
        if(!flag[lnode]){
            flag[lnode] = true;
            data.append(lnode);
        }
    }
    int* datatosend = data.CopyToPointer();
    return datatosend;
}
void NodalSolver::genSuperNodes(){
    //HouseKeeping Stuffs
    int numOfComponents = voltageComponents.Length();
    int *sNode = new int[numOfComponents];
    int *lNode = new int[numOfComponents];
    genVertices(voltageComponents,sNode,lNode);
    //int *unOrganizedSupNodes = genunseparatedNodes(sNode,lNode);
    //Flag
    bool *flag = new bool[numOfNodes+1];
    genFalseFlag(flag,numOfNodes+1);
    //flag[sNode[0]] = true;

    std::cout<<"We entered into genSuperNode"<<std::endl;
    //WorkingList/
    LinkedList<int> workingList;
    workingList.append(sNode[0]);

    while(true){
        int currentVertex = workingList.read(0);
        for(int i=0;i<numOfComponents;i++){
            int snode = sNode[i];
            int lnode = lNode[i];
            std::cout<<"Super Node"<<std::endl;
            supNode.print();
            if(snode==currentVertex&&flag[lnode]==false){
                supNode.append(lnode);
                workingList.append(lnode);
                flag[lnode] = true;
            }
            else if(lnode==currentVertex&&flag[snode]==false){
                supNode.append(snode);
                workingList.append(snode);
                flag[snode] = true;
            }
        }
        //std::cout<<"Infinite Loop"<<std::endl;
        workingList.remove(0);
        if(workingList.Length()==0){
            supNode.append(-1);
            // -1 will be flag for separating the superNodes
            int i;
            for(i=0;i<numOfComponents;i++){
                if(!flag[sNode[i]])
                    break;
            }
            if(i==numOfComponents){
                delete[] flag;
                delete[] sNode;
                delete[] lNode;
                return;
            }
            //flag[i] = true;
            workingList.append(sNode[i]);

        }

    }
 }

 void NodalSolver::calcNumOfNodes(){
    listOfComponents.reset();
    numOfComponents = listOfComponents.Length();
    Component* traveller;
    bool *flag = new bool[65536];
    genFalseFlag(flag,65536);
    for(int i=0;i<numOfComponents;i++){
        traveller = listOfComponents.getNextData()->data;
        flag[traveller->sNode] = true;
        flag[traveller->lNode] = true;
    }
    int i;
    for(i=0;flag[i];i++);
    numOfNodes = i;
    delete[] flag;
}

void NodalSolver::checkSuperNode(){
    if(supNode.Length()==0)
        return;
    supNode.reset();
    int numOfVert = supNode.Length();
    Stack<int> track;
    int i=0;
    while(i<numOfVert-1){
        int num = supNode.getNextData()->data;
        ////std::cout<<"Next vertex being checked "<<num<<std::endl;
        if(checkLoopAt(num,track)){
            EngineMessage::message<<"There is a Loop of ideal voltage sources/capacitors as follow:\n";
            int top = track.readTop();
            EngineMessage::message<<top<<" ";
            track.pop();
            int reader =-1;
            while(reader!=top){
                reader = track.readTop();
                track.pop();
                EngineMessage::message<<reader<<" ";
            }
            EngineMessage::message<<"\nNote: \n1. An ideal voltage source cannot be shorted.\n"
            "2. An ideal voltage source cannot form loop with another ideal voltage source or capacitor"
            " if the loop doesn't contain any inductor, resistor or current source"<<std::endl;
            if(!errorFlag)
                errorFlag=true;

        }
        for(int j=-2;j!=-1;i++)
            j = supNode.getNextData()->data;
        //supNode.getNextData()->data;
        i++;
        ////std::cout<<"Value of i"<<i<<std::endl;
    }
    ////std::cout<<"We are at the end of checkSupNode at"<<std::endl;
}
bool NodalSolver::checkLoopAt(int startingVertex,Stack<int>& track){
    linkedVertexData<Component*>* currentVertexPointer=0;
    Component* dummy;
    ComponentType type;

    int nextVertex = -1;
    LinkedList<Component*> visitedComponents;
    LinkedList<Component*> loopComponents;

    track.push(startingVertex);
    ////std::cout<<"Value of starting vertex "<<startingVertex<<std::endl;

    while(track.Length()){
        currentVertexPointer=circuit.getNextVertexFrom(track.readTop());
        if(currentVertexPointer == 0){
            ////std::cout<<"Has been found"<<std::endl;
            loopComponents.remove(0);
            track.pop();
            continue;
        }
        ////std::cout<<"Inside checkLoop at "<<std::endl;
        //track.print();
        nextVertex = currentVertexPointer->vertex;
        ////std::cout<<"Next vertex is:"<<nextVertex<<std::endl;
        ////std::cout<<"Number of visited components is:"<<visitedComponents.Length()<<std::endl;

        dummy=currentVertexPointer->weight;
        type = dummy->getType();
        ////std::cout<<"Type of comp "<<dummy->value<<std::endl;
        if((type==CAPACITOR || type == IVS || type == CCVS || type == VCVS)&&!visitedComponents.contains(dummy)){
            visitedComponents.append(dummy);
            loopComponents.addAtFront(dummy);
            if(track.contains(nextVertex)){
                track.push(nextVertex);
                if(isInvalidLoop(loopComponents)){
                    ////std::cout<<"Gets it valid"<<std::endl;
                    //track.print();
                    return true;
                }
            }else
                track.push(nextVertex);
        }
    }
    ////std::cout<<"We are at the end of checkLoop at"<<std::endl;
    return false;
}

bool NodalSolver::isInvalidLoop(LinkedList<Component*>& list){
    list.reset();
    Component* dummy=0;
    Component* checker = list.getNextData()->data;
    while(dummy!=checker){
        dummy = list.getNextData()->data;
        ComponentType type = dummy->getType();
        if(type==IVS||type==CCVS||type==VCVS){
            //std::cout<<"sNode: "<<checker->sNode<<" lNode: "<<checker->lNode<<std::endl;
            return true;
        }
    }
    return false;
}

void NodalSolver::setGraph(){
    circuit.addVertex(numOfNodes);
    listOfComponents.reset();
    for(int i=0;i<numOfComponents;i++){
        Component *dummy = listOfComponents.getNextData()->data;
        int sNode = dummy->sNode;
        int lNode = dummy->lNode;
        circuit.addSymmetricEdge(sNode,lNode,dummy);
    }
}

void NodalSolver::checkSeriesCurrentSource(){

    circuit.resetAll();
    for(int i=0;i<numOfNodes;i++){
        LinkedVertex<Component*>* currentVertex = circuit.gotoNextVertex();
        int numberOfEdges = currentVertex->getDegree();
        int numOfOtherComponents = 0;
        int numOfCurrentSources = 0;
        int numOfInductors = 0;
        for(int j=0;j<numberOfEdges;j++){
            Component* dummy = currentVertex->getNextVertex()->weight;
            ComponentType type = dummy->getType();
            if(type==ICS||type==CCCS||type==VCCS)
                numOfCurrentSources++;
            else if(type==INDUCTOR)
                numOfInductors++;
            else
                numOfOtherComponents++;
        }
        if(numOfOtherComponents==0&&(numOfCurrentSources>=2||(numOfInductors>=1&&numOfCurrentSources>=1))){
            EngineMessage::message<<"\nThere are current source(s) or inductor(s) and current source(s) in series at node: "<<currentVertex->getId();
            if(!errorFlag)
                errorFlag=true;
        }
    }
}

void NodalSolver::setMatCoef(){
    supNode.reset();
    std::cout<<"Super nodes"<<std::endl;
    supNode.print();
    circuit.resetAll();
    int numOfElements = supNode.Length();
    LinkedList<int> singleSupNode;
    LinkedList<Component*> visitedComponents;
    LinkedList<Component*> capacitorFlag;
    linkedVertexData<Component*>* currentVertex;
    //int firstVertex;
    int i=0;
    while(i<numOfElements){

        for(int j=-900;j!=-1;i++){
            j = supNode.getNextData()->data;

            if(j!=-1)
            singleSupNode.append(j);
        }
        i++;
        int j = 0;
        int numOfEleInSSN = singleSupNode.Length();


        int superNodalCoefficient = -100;
        if(!containsReference(singleSupNode)){
            superNodalCoefficient = singleSupNode.getNextData()->data;
        }
        //std::cout<<"SuperNode after checking"<<std::endl;
        //singleSupNode.print();
        int vertex = singleSupNode.read(0);
        while(j<numOfEleInSSN){

            currentVertex = circuit.getNextVertexFrom(vertex);
            if(currentVertex==0){
                j++;
                vertex = singleSupNode.read(j);
                ////std::cout<<"Value of vertex: "<<vertex<<"Value of j: "<<j<<std::endl;
                continue;
            }
            Component* dummy = currentVertex->weight;
            ComponentType type = dummy->getType();
            if((type == IVS||type==CCVS||type==VCVS)&&!visitedComponents.contains(dummy)){
                int constraintRow = singleSupNode.getNextData()->data;
                dummy->setMatCoef(dummy->sNode, constraintRow);
                visitedComponents.append(dummy);
                //std::cout<<"Value of Constraint row"<<constraintRow<<std::endl;
            }else if(type==CAPACITOR&&!capacitorFlag.contains(dummy)){
                capacitorFlag.append(dummy);
            }
            else if(type==RESISTOR||type==INDUCTOR||type==ICS||type==CCCS||type==VCCS){
                if(superNodalCoefficient!=-100)
                    dummy->setMatCoef(vertex, superNodalCoefficient);
                else
                    dummy->setMatCoef(vertex,-1);
            }

        }
        if(capacitorFlag.Length())
            setCapacitors(singleSupNode,capacitorFlag);
        capacitorFlag.removeAll();
        singleSupNode.removeAll();
    }
}

bool NodalSolver::containsReference(LinkedList<int>& list){
    int numOfComp = list.Length();
    bool contains = false;
    for(int i=0; i<numOfComp; i++){
       if(list.getNextData()->data == numOfNodes-1){
            contains = true;
            list.remove(i);
       }
    }
    return contains;
}
void NodalSolver::setCapacitors(LinkedList<int>& sSupNode,LinkedList<Component*>& capacitor){
    int numOfCapacitors = capacitor.Length();
    ////std::cout<<"The number of capacitors is:"<<numOfCapacitors<<std::endl;
    LinkedList<Component*> parallelCaps;
    LinkedList<Component*> visitedComponent;
    for(int i=0;i<numOfCapacitors;i++){
        Component* traveller = capacitor.getNextData()->data;
        if(visitedComponent.contains(traveller))
            continue;
        double parallelCapacitance = traveller->value;
        parallelCaps.append(traveller);
        visitedComponent.append(traveller);
        for(int j=0;j<numOfCapacitors;j++){
            Component* debugger = capacitor.getNextData()->data;
            if(debugger->sNode==traveller->sNode&&debugger->lNode==traveller->lNode&&!visitedComponent.contains(debugger)){
                parallelCaps.append(debugger);
                visitedComponent.append(debugger);
                parallelCapacitance += debugger->value;
            }
        }

        int numOfParallelcaps = parallelCaps.Length();
        ////std::cout<<"The number of parallel capacitors is inside:"<<numOfParallelcaps<<std::endl;
        int nextRow = sSupNode.getNextData()->data;
        //std::cout<<"The nextRow is"<<nextRow<<std::endl;
        //sSupNode.print();
        for(int k=0; k<numOfParallelcaps;k++){
            traveller = parallelCaps.getNextData()->data;
            traveller->setMatCoef(traveller->sNode,nextRow);
            traveller->setMatCoef(-1,traveller->value/parallelCapacitance);
        }
        parallelCaps.removeAll();
    }
}

void NodalSolver::genSimulationList(){
    int numOfElements = supNode.Length();
    LinkedList<int> singleSupNode;
    LinkedList<Component*> voltageSimulationList;
    LinkedList<Component*> nextComps;
    LinkedList<Component*> visitedComponent;
    //int firstVertex;
    int i=0;
    while(i<numOfElements){
        for(int j=-900;j!=-1;i++){
            j = supNode.getNextData()->data;
            if(j!=-1)
            singleSupNode.append(j);
        }
        i++;
        int j = 0;
        int currentVertex = singleSupNode.read(0);
        circuit.reset(currentVertex);
        int numOfSources = 0;
        int counter = 0;
        int numOfEdge = circuit.getDegreeOf(currentVertex);
        int nextCapVertex = -1;
        //std::cout<<" Next SuperNode:"<<std::endl;
        singleSupNode.print();
        while(singleSupNode.Length()>1){
            linkedVertexData<Component*>* edge = circuit.getNextVertexFrom(currentVertex);
            if(edge==0||numOfSources>1){
                if(j>=singleSupNode.Length()-1)
                    j=0;
                else
                    j++;
                currentVertex = singleSupNode.read(j);
                circuit.reset(currentVertex);
                numOfSources = counter = 0;
                numOfEdge = circuit.getDegreeOf(currentVertex);
                nextCapVertex = -1;
                nextComps.removeAll();
                continue;
            }
            Component* traveller = edge->weight;
            int nextVertex = edge->vertex;
            ComponentType type= traveller->getType();
            if(!voltageSimulationList.contains(traveller)){
                if(type==IVS||type==VCVS||type==CCVS){
                    numOfSources++;
                    nextComps.append(traveller);
                }else if(type==CAPACITOR){
                    if(nextCapVertex==-1){
                        numOfSources++;
                        nextCapVertex = nextVertex;
                    }

                    if(nextVertex!= nextCapVertex)
                        numOfSources++;
                    nextComps.append(traveller);
                }
            }
            counter++;
            if(counter==numOfEdge&&numOfSources==1){
                std::cout<<"Components with following id have been added with current seeking node equals to:"<<currentVertex<<std::endl;
                for(unsigned i = 0;i<nextComps.Length();i++){
                    Component* setter = nextComps.getNextData()->data;
                    std::cout<<setter<<std::endl;
                    setter->setMatCoef(setter->lNode,currentVertex);
                    std::cout<<setter->id<<std::endl;
                    voltageSimulationList.append(setter);
                }
                nextComps.removeAll();
                //std::cout<<"The value of j is: "<<j<<" and the length of singleSupNode is: "<<singleSupNode.Length()<<std::endl;
                singleSupNode.remove(j);
            }

        }
        singleSupNode.removeAll();
    }
    simulationList.appendList(currentComponents);
    simulationList.appendList(voltageSimulationList);
}

void NodalSolver::getSingleSourceVertex(LinkedList<int>& superNodes){
    int length = superNodes.Length();
    superNodes.reset();
    int numOfSources = 0;
    for(int j=0;j<length;j++){
        int vertex = superNodes.getNextData()->data;
        LinkedVertex<Component*>* currentVertex = circuit.returnVertexAt(vertex);
        currentVertex->reset();
        numOfSources = 0;
        //int nextCapVertex=-1;

        if(numOfSources==1){
            superNodes.remove(j);
            superNodes.addAtFront(vertex);
            return;
        }
    }
}

void NodalSolver::printAllData(){
    int len = numOfComponents;
    simulationList.reset();
    for(;len>0;len--){
        Component* traveller = simulationList.getNextData()->data;
        traveller->printData();
    }

}

void NodalSolver::simulate(){
    Component::copyMatrix();
    MatrixSolver::mSolver.getSolution();
    bool solved = false;
    unsigned int i = 0;
    while(!solved){
        for(int i=0;i<numOfComponents;i++)
            simulationList.getNextData()->data->setConstant();
        if(Component::InaccurateFlag){
            std::cout<<"\nIteration"<<i<<std::endl;
            printAllData();
            setDeMatrix();
            Component::reverseCopy();
            MatrixSolver::mSolver.getSolution();
        }
        else{
            solved = true;
        }
        i++;
    }
}

void NodalSolver::setDeMatrix(){
    int numOfco = controlledComponents.Length();
    for(int i=0;i<numOfco;i++)
        controlledComponents.getNextData()->data->setMatrix();
}

void NodalSolver::setConstantForCurrentControlledSources(){
    int numOfco = cCVS.Length();
    for(int i=0;i<numOfco;i++)
        cCVS.getNextData()->data->setConstant();
}

bool NodalSolver::setup(){
    setAllComponents();
    calcNumOfNodes();
    setGraph();
    genSuperNodes();
    checkSuperNode();
    checkSeriesCurrentSource();
    capacitors.print();
    resistors.print();
    iVS.print();

    if(errorFlag)
        return false;

    setMatCoef();
    genSimulationList();
    simulationList.reset();
    Component::setInitials(numOfComponents,numOfNodes);
    //std::cout<<"Simulation List: "<<std::endl;
    //printAllData();
    for(int i=0;i<numOfComponents;i++){
        simulationList.getNextData()->data->setMatrix();
    }
    Component::copyMatrix();
    MatrixSolver::mSolver.setMatrix(Component::matrix,numOfNodes-1,Component::nodalVoltage);
    return true;
}
