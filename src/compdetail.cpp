#include"compdetail.h"
#include<iostream>

void DerivedTypeInfo::loadCompInto(LinkedList<CompDetail*>& l, unsigned* nodes, unsigned& lastAvilableNode){
    //Code For Copying into the provided list

    unsigned length = components.Length();
    l.removeAll();
    components.reset();
    for(unsigned i=0;i<length;i++){
        CompDetail* traverser = components.getNextData()->data;
        if(traverser->Father==0)
            traverser->Father = this;
        l.append(traverser->getACopy());
    }

    //Code for creating an array of Nodes
    unsigned *newNodes = new unsigned[numOfNodes];
    unsigned a = 0;
    for(;a<numOfOutPins;a++){
        newNodes[a] = nodes[a];
    }
    for(;a<numOfNodes;a++,lastAvilableNode++){
        newNodes[a] = lastAvilableNode;
    }
    reAssignNode(newNodes,l);
    delete[] newNodes;
    //Reassignment
    reAssignOrientation(l);

}
void DerivedTypeInfo::reAssignNode(unsigned *newNode,LinkedList<CompDetail*>& l){
    unsigned length = l.Length();
    l.reset();
    for(unsigned i=0;i<length;i++){
        CompDetail* comp = l.getNextData()->data;
        comp->nodes[0] = newNode[comp->nodes[0]];
        comp->nodes[1] = newNode[comp->nodes[1]];
    }
}

void DerivedTypeInfo::reAssignOrientation(LinkedList<CompDetail*>& l){
    l.reset();
    unsigned length = l.Length();
    components.reset();
    for(unsigned i=0;i<length;i++){
        CompDetail* created = l.getNextData()->data;
        CompDetail* original = components.getNextData()->data;
        if((created->nodes[0]>created->nodes[1]&&original->nodes[0]<original->nodes[1])||(created->nodes[0]<created->nodes[1]&&original->nodes[0]>original->nodes[1])){
            if(created->orient){
                created->orient = false;
            }
            else
                created->orient = true;
        }
    }
}

//Given Nodes in an Order it will be able to generate a List of CompDetail objects with appropriate nodes and orientation
//Will Also Assign the Component Provided the function are loaded
DerivedTypeInfo::DerivedTypeInfo(){
    valuerange=defvalue=0;
    pinpoints=0;
    pinLabels=valueLabels=0;
    imgx=imgy=numOfOutPins=numOfNodes=numOfComp=numOfFunction=0;
    numOfOutPins=numOfNodes=numOfValueLabel=numOfComp=0;
}
std::string DerivedTypeInfo::getErrorString(){
    std::string a = "";
    if(numOfOutPins==0)
        a+="Number of outpin not defined";
    if(numOfNodes==0)
        a+="Number of nodes not defined";
    if(numOfComp==0)
        a+="Number of Component not defined";
    if(imgx==0||imgy==0)
        a += "Image Coordinates not defined\n";
    if(!(valuerange==0&&defvalue==0&&valueLabels==0)){
        if(valuerange==0)
            a += "Value Range Not Defined\n";
        if(defvalue==0)
            a += "Default Value Not Defined\n";
        if(valueLabels==0)
            a += "Value Labels Not Defined\n";
    }
    if(pinLabels==0)
        a += "Pin Labels Not Defined\n";
    if(pinpoints==0)
        a += "Pin Points Not Defined\n";
    if(sizeof(valueLabels)/sizeof(std::string)!=sizeof(defvalue)/sizeof(double))
        a += "Number of valuelabel doesn't match with the number of defvalue\n";
    if(2*sizeof(valueLabels)/sizeof(std::string)!=sizeof(valuerange)/sizeof(double))
        a += "Number of valuelabel doesn't match with the number of value range\n";
    if(2*sizeof(defvalue)/sizeof(std::string)!=sizeof(valuerange)/sizeof(double))
        a += "Number of defvalue doesn't match with the number of valuerange\n";
    return a;
}
void DerivedTypeInfo::print(){
    std::cout<<"                                         "<<typeLabel<<std::endl;
    std::cout<<"Number of Value Labels: "<<numOfValueLabel<<std::endl;
    if(valueLabels!=0&&valuerange!=0&&defvalue!=0){
        for(unsigned i=0;i<numOfValueLabel;i++){
            std::cout<<"Value["<<i<<"] = "<<defvalue[i]<<" ValueLabel["<<i<<"] = "<<valueLabels[i]<<" ValueRange["<<i<<"] = ("<<valuerange[2*i]<<","<<valuerange[2*i+1]<<")"<<std::endl;
        }
    }else
        std::cout<<"Values not defined"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Number of OutPins:"<<numOfOutPins<<" Number of Nodes:"<<numOfNodes<<" Number of Components:"<<numOfComp<<std::endl;
    if(pinLabels!=0&&pinpoints!=0){
        for(unsigned i=0;i<numOfOutPins;i++){
            std::cout<<"pinpoint["<<i<<"] = ("<<pinpoints[2*i]<<","<<pinpoints[2*i+1]<<") pinLabel["<<i<<"] = "<<pinLabels[i]<<std::endl;
        }
    }
    if(numOfFunction!=0){
        std::cout<<"Function Labels: ";
        for(unsigned i=0;i<numOfFunction;i++){
            std::cout<<functionNames[i]<<" ";
        }
        std::cout<<std::endl;
    }else
        std::cout<<"No functions defined"<<std::endl;
    std::cout<<"ImageX:"<<imgx<<" ImageY:"<<imgy<<std::endl;

    std::cout<<"\n\\-----------------------------------------------Component Defination------------------------------------------------\\\n"<<std::endl;
    unsigned length = components.Length();
    components.reset();
    for(unsigned i = 0; i<length;i++){
        std::cout<<"Component: "<<i+1<<std::endl;
        components.getNextData()->data->print();
        std::cout<<std::endl;
    }
}
