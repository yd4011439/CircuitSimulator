#include"parser.h"

void Parser::_loadComponent(){
    position = 0;
    skipInclude();
    ReturnError();
    returner = new DerivedTypeInfo;
    createDefination();
    ReturnError();
    if(error){
        delete returner;
        ReturnError();
    }
    nodeCount = returner->numOfNodes;
    for(unsigned i=0;i<returner->numOfComp;i++){
        NextComponent();
        ReturnError();
        compBuffer.reset();
        for(unsigned i=0;i<compBuffer.Length();i++){
            CompDetail* d = compBuffer.getNextData()->data;
            if((d->cType=="VCCS"||d->cType=="VCVS"||d->cType=="TCVS"||d->cType=="TCCS")&&d->Father==0){
                d->Father = returner;
                std::cout<<"Assigned Father: "<<returner->typeLabel;
            }
            returner->components.append(d->getACopy());
        }
         compBuffer.removeAll();
    }
    loadedComponents.append(returner);
    returner->print();
    createFunction();
}

void Parser::createFunction(){
    //Code for checking whether the file is already created or not
    std::string fileName = "defbin\\" + returner->typeLabel +".dll";
    std::ifstream a;
    a.open(fileName.c_str());
    if(a){
        std::cout<<"The file "<<fileName<<" has already been created"<<std::endl;
        a.close();
        return;
    }
    a.close();
    //Code for generating DLL file
    std::string nextString = "";
    std::string compileString =
    "#include\"tmp.hpp\"\n"
    "#include<cmath>\n";
    std::string headerString =
        "#pragma once\n"
        "#ifdef  __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n";
    for(unsigned i = 0;i<returner->numOfFunction;i++){
        nextString = NextString();
        unsigned j = 0;
        for(; j<returner->numOfFunction;j++){
            if(nextString==returner->functionNames[j])
                break;
        }
        if(j==returner->numOfFunction){
            error = true;
            addMessageWithLabel("Unidentified parameter " + nextString);
            return;
        }
        compileString += "double " + returner->typeLabel +returner->functionNames[j]+"(double x,double* value){";
        headerString += "double " + returner->typeLabel + returner->functionNames[j]+"(double x,double* value);\n";
        while(travelBuffer[position+2]!=0){
            compileString += travelBuffer[position];
            if(travelBuffer[position+1]=='#'&&travelBuffer[position+2]=='e')
                break;
            position++;
        }
        compileString += "}\n";
        NextString();
    }
    headerString +=
        "#ifdef  __cplusplus\n"
        "}\n"
        "#endif";
    std::ofstream file;
    file.open("defbin\\tmp.hpp");
    file<<headerString.c_str();
    file.close();
    file.open("defbin\\tmp.cpp");
    file<<compileString.c_str();
    file.close();
    std::string command = "g++ defbin\\tmp.cpp -o defbin\\" + returner->typeLabel +".dll -shared";
    //system("cls");
    system(command.c_str());
    std::cin.get();
    //std::cout<<"The Compile String is: \n"<<compileString<<std::endl;
    //std::cout<<"The Header String is: \n"<<headerString<<std::endl;
}

void Parser::NextComponent(){
    component = new CompDetail;
    std::string nextString = "";
    while(nextString!="#enddef"&&travelBuffer[position]!=0){
        nextString = NextString();
        ReturnError();
        if(nextString=="#component")
            continue;
        else if(nextString=="type"){
            readOrderedStrings();
            ReturnError();
            component->cType = stringBuffer[0];
        }else if(nextString=="node"){
            unsigned n = readOrderedDoubles();
            ReturnError();
            component->nodes = new unsigned[n];
            for(unsigned i=0;i<n;i++)
                component->nodes[i] = (unsigned)doubleBuffer[i];
        }else if(nextString=="orientation"){
            readOrderedDoubles();
            ReturnError();
            component->orient = (bool)doubleBuffer[0];
        }else if(nextString=="value"){
            unsigned n = readOrderedDoubles();
            ReturnError();
            component->values = new double[n];
            for(unsigned i=0;i<n;i++)
                component->values[i] = doubleBuffer[i];
        }else if(nextString=="threshold"){
            readOrderedDoubles();
            ReturnError();
            component->threshold = doubleBuffer[0];
        }else if(nextString=="func"){
            readOrderedStrings();
            ReturnError();
            component->func = returner->typeLabel + stringBuffer[0];
        }else if(nextString=="prnode"){
            readOrderedDoubles();
            ReturnError();
            component->prnode = (unsigned)doubleBuffer[0];
        }else if(nextString=="nrnode"){
            readOrderedDoubles();
            ReturnError();
            component->nrnode = (unsigned)doubleBuffer[0];
        }else if(nextString=="#enddef")
            break;
        else if(travelBuffer[position==0]){
            delete component;
            return;
        }else{
            error = true;
            addMessageWithLabel("Unidentified Parameter ");
            return;
        }
    }
    if(component->cType!="RESISTOR"&&component->cType!="CAPACITOR"&&component->cType!="INDUCTOR"&&component->cType!="IVS"&&component->cType!="ICS"&&component->cType!="VCCS"&&component->cType!="VCVS"&&component->cType!="TCCS"&&component->cType!="TCVS"){
        loadDerived();
        ReturnError();
    }else{
        std::string a = component->getErrorStringForDefination();
        if(a!=""){
            error = true;
            addMessage("Error Creating Component\n"+a);
            return;
        }
        compBuffer.append(component);
    }
}

void Parser::loadDerived(){
    unsigned length=loadedComponents.Length();
    loadedComponents.reset();
    for(unsigned i = 0; i<length; i++){
        DerivedTypeInfo* traverser = loadedComponents.getNextData()->data;
        if(traverser->typeLabel==component->cType){
            traverser->loadCompInto(compBuffer,component->nodes,nodeCount);
            return;
        }
    }
    addMessage("Component of type: "+component->cType + "Not Found");
    error = true;
}

unsigned Parser::createDefination(){
    std::string nextString="";
    returner = new DerivedTypeInfo;
    returner->numOfOutPins = 0;
    returner->numOfValueLabel = 0;
    while(nextString!="#enddef"&&!error){
        nextString = NextString();
        ReturnZeroError();
        if(nextString=="#defcomp")
            continue;
        else if(nextString=="type"){
            readOrderedStrings();
            ReturnZeroError();
            returner->typeLabel = stringBuffer[0];
        }else if(nextString=="node"){
            readOrderedDoubles();
            ReturnZeroError();
            returner->numOfNodes = (unsigned)doubleBuffer[0];
        }else if(nextString=="comp"){
            readOrderedDoubles()
            ReturnZeroError();
            returner->numOfComp = (unsigned)doubleBuffer[0];
        }else if(nextString=="outpin"){
            readOrderedDoubles()
            ReturnZeroError();
            returner->numOfOutPins = (unsigned)doubleBuffer[0];
        }else if(nextString=="pinlabel"){
            unsigned n = readOrderedStrings();
            ReturnZeroError();
            returner->pinLabels = new std::string[n];
            for(unsigned i=0;i<n;i++){
                returner->pinLabels[i] = stringBuffer[i];
            }
        }else if(nextString=="pinpoint"){
            if(returner->numOfOutPins==0){
                error = true;
                addMessageWithLabel("pinpoint defined before defining outpin");
                return 0;
            }else{
                returner->pinpoints = new unsigned[returner->numOfOutPins*2];
                for(unsigned i=0;i<2*returner->numOfOutPins;i+=2){
                    readOrderedDoubles();
                    ReturnZeroError();
                    returner->pinpoints[i] = (unsigned)doubleBuffer[0];
                    returner->pinpoints[i+1] = (unsigned)doubleBuffer[1];
                }
            }
        }else if(nextString=="imgsize"){
            readOrderedDoubles();
            ReturnZeroError();
            returner->imgx = (unsigned)doubleBuffer[0];
            returner->imgy = (unsigned)doubleBuffer[1];
        }else if(nextString=="func"){
            unsigned n = readOrderedStrings();
            ReturnZeroError();
            returner->numOfFunction = n;
            returner->functionNames = new std::string[n];
            for(unsigned i=0;i<n;i++)
                returner->functionNames[i] = stringBuffer[i];
        }else if(nextString=="valuelabel"){
            unsigned n = readOrderedStrings();
            ReturnZeroError();
            returner->numOfValueLabel = n;
            returner->valueLabels = new std::string[n];
            for(unsigned i=0;i<n;i++)
                returner->valueLabels[i] = stringBuffer[i];
        }else if(nextString=="value"){
            unsigned n = readOrderedDoubles();
            ReturnZeroError();
            returner->defvalue = new double[n];
            for(unsigned i=0;i<n;i++)
                returner->defvalue[i] = doubleBuffer[i];
        }else if(nextString=="valuerange"){
            if(returner->numOfValueLabel==0){
                error = true;
                addMessageWithLabel("valueRange defined before defining valueLabel");
                return 0;
            }else{
                returner->valuerange = new double[2*returner->numOfValueLabel];
                for(unsigned i=0;i<2*returner->numOfValueLabel;i+=2){
                    readOrderedDoubles();
                    ReturnZeroError();
                    returner->valuerange[i] = doubleBuffer[0];
                    returner->valuerange[i+1] = doubleBuffer[1];
                }
            }
        }
        else if(nextString=="#enddef")
            break;
        else{
            error = true;
            addMessageWithLabel("UnIdentified parameter"+nextString);
            return 0;
        }
    }
    std::string a = returner->getErrorString();
    if(a!=""){
        error = true;
        addMessage(a);
        return 0;
    }
}

void Parser::skipInclude(){
    while(1){
        if((travelBuffer[position]=='n'&&travelBuffer[position+1]=='o')||travelBuffer[position]=='#'&&travelBuffer[position+1]=='d'&&travelBuffer[position+2]=='e'||travelBuffer[position]==0)
            break;
        position++;
    }
    if(travelBuffer[position]==0)
        error = true;
}

DerivedTypeInfo* Parser::loadComponent(std::string compName){
    //Code for creating the component
    unsigned l = loadedComponents.Length();
    loadedComponents.reset();
    for(unsigned i=0;i<l;i++){
        DerivedTypeInfo* d = loadedComponents.getNextData()->data;
        if(compName==d->typeLabel)
            return d;
    }
    loadComponentFromFile("def\\"+compName+".def");
    ReturnZeroError();
    return returner;
}

void Parser::loadComponentFromFile(std::string fileName,bool isCircuit){
    //Code for generating the component from files
    Stack<std::string> dependencyStack;
    createDependency(dependencyStack,fileName);
    std::cout<<"Number Of Dependent Components: "<<dependencyStack.Length()<<std::endl;
    dependencyStack.print();
    unsigned length = dependencyStack.Length();
    if(isCircuit)
        length--;
    Stack<std::string> visited;
    while(length>0){
        std::cout<<"Next Component to be loaded: "<<dependencyStack.readTop()<<std::endl;
        if(!visited.contains(dependencyStack.readTop())){
            travelBuffer = loadFile(dependencyStack.readTop());
            _loadComponent();
            ReturnError();
            visited.push(dependencyStack.readTop());
        }
        dependencyStack.pop();
        length--;
    }
}

void Parser::createCircuit(std::string str, bool isFile){
    if(isFile){
        std::string nextString;
        loadComponentFromFile(str,true);
        travelBuffer = loadFile(str);
        position=0;
        skipInclude();
        nextString = NextString();
        if(nextString=="node"){
            std::cout<<"Next String is: "<<nextString<<std::endl;
            readOrderedDoubles();
            std::cout<<"Number of Nodes is: "<<doubleBuffer[0]<<std::endl;
            ReturnError();
            nodeCount = (unsigned)doubleBuffer[0];
        }else{
            addMessageWithLabel("Expected number of nodes");
            return;
        }
        allComponent.removeAll();
        while(travelBuffer[position]!=0){
            NextComponent();
            ReturnError();
            compBuffer.reset();
            for(unsigned i=0;i<compBuffer.Length();i++){
                CompDetail* d = compBuffer.getNextData()->data;
                allComponent.append(d->getACopy());
            }
            compBuffer.removeAll();
        }
        createAllComponents();
    }
}

void Parser::createAllComponents(){
    unsigned length=allComponent.Length();
    allComponent.reset();
    //Will require every components be assigned nodes
    Component* comp;
    for(unsigned i=0;i<length;i++){
        component = allComponent.getNextData()->data;
        if(component->cType=="RESISTOR"){
            comp = new Resistor(0,component->nodes[0],component->nodes[1],*(component->values));
            component->actualId = comp->getId();
            NodalSolver::NSolver.resistors.append(comp);
            std::cout<<"Resistor("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
        }else if(component->cType=="CAPACITOR"){
            comp = new Capacitor(0,component->nodes[0],component->nodes[1],*(component->values));
            component->actualId = comp->getId();
            NodalSolver::NSolver.capacitors.append(comp);
            std::cout<<"Capacitor("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
        }else if(component->cType=="INDUCTOR"){
            comp = new Inductor(0,component->nodes[0],component->nodes[1],*(component->values));
            component->actualId = comp->getId();
            NodalSolver::NSolver.inductors.append(comp);
            std::cout<<"Inductor("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
        }else if(component->cType=="IVS"){
            comp = new VoltageSource(0,component->nodes[0],component->nodes[1],*(component->values));
            component->actualId = comp->getId();
            NodalSolver::NSolver.iVS.append(comp);
            std::cout<<"VoltageSource("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
        }
        else if(component->cType=="ICS"){
            comp = new CurrentSource(0,component->nodes[0],component->nodes[1],*(component->values));
            component->actualId = comp->getId();
            NodalSolver::NSolver.iCS.append(comp);
            std::cout<<"CurrentSource("<<comp->getId()<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
        }else if(component->cType=="VCVS"||component->cType=="VCCS"||component->cType=="TCVS"||component->cType=="TCCS"){
            //Code for loading function from the dll files
            std::string fileLoc = "defbin\\"+component->Father->typeLabel+".dll";
            void* handle = dlopen(fileLoc.c_str(),0);
            if(!handle){
                error = true;
                addMessage("Can't load the dll "+fileLoc);
                return;
            }
            dllLinks.append(handle);
            relFunc func = reinterpret_cast<relFunc>(dlsym(handle,component->func.c_str()));
            if(!func){
                error = true;
                addMessage("Can't load function: " + component->func + " parent = " + component->Father->typeLabel);
                return;
            }
            //This Code Copies the default values for the component
            if(component->cType=="VCVS"){
                comp = new VcvS(0,component->nodes[0],component->nodes[1],component->prnode,component->nrnode,func,component->values,component->threshold,component->orient);
                component->actualId = comp->getId();
                NodalSolver::NSolver.vCVS.append(comp);
                std::cout<<"VcvS("<<comp->getId()<<component->nodes[0]<<","<<component->nodes[1]<<","<<component->prnode<<","<<component->nrnode<<","<<func<<","<<component->values<<","<<component->threshold<<","<<component->orient<<")"<<std::endl;
            }else if(component->cType=="VCCS"){
                comp = new VccS(0,component->nodes[0],component->nodes[1],component->prnode,component->nrnode,func,component->values,component->threshold,component->orient);
                component->actualId = comp->getId();
                NodalSolver::NSolver.vCCS.append(comp);
                std::cout<<"VccS("<<comp->getId()<<","<<component->nodes[0]<<","<<component->nodes[1]<<","<<component->prnode<<","<<component->nrnode<<","<<func<<","<<component->values<<","<<component->threshold<<","<<component->orient<<")"<<std::endl;
            }else if(component->cType=="TCVS"){
                comp = new TimeDependentVoltageSource(0,component->nodes[0],component->nodes[1],func,component->values);
                component->actualId = comp->getId();
                NodalSolver::NSolver.iVS.append(comp);
                std::cout<<"TimeDependentVoltageSource("<<comp->getId()<<","<<component->nodes[0]<<","<<component->nodes[1]<<","<<func<<","<<component->values<<")"<<std::endl;
            }else if(component->cType=="TCCS"){
                comp = new TimeDependentCurrentSource(0,component->nodes[0],component->nodes[1],func,component->values);
                component->actualId = comp->getId();
                NodalSolver::NSolver.iCS.append(comp);
                std::cout<<"TimeDependentVoltageSource("<<comp->getId()<<","<<component->nodes[0]<<","<<component->nodes[1]<<","<<func<<","<<component->values<<")"<<std::endl;
            }
        }else{
            error = true;
            addMessage("UnIdentified Component " + component->cType);
            return;
        }
    }
}

void Parser::createDependency(Stack<std::string>& dependencyStack, std::string fileName){
    Stack<std::string> workingStack;
    dependencyStack.push(fileName);
    workingStack.push(fileName);
    while(workingStack.Length()){
        travelBuffer = loadFile(workingStack.pop());
        ReturnError();
        position = 0;
        while(travelBuffer[position]!=0){
            std::string header = NextString();
            ReturnError();
            if(header=="#include"){
                unsigned n = readOrderedStrings();
                ReturnError();
                for(unsigned i = 0;i<n;i++){
                    workingStack.push("def\\"+stringBuffer[i]+".def");
                    dependencyStack.push("def\\"+stringBuffer[i]+".def");
                }
            }else
                break;
        }
    }
}

std::string Parser::loadFile(std::string fName){
     std::string returner = "";
     std::ifstream  file(fName.c_str());
     if(!file){
        addMessage("Can't Open File: " + fName +"\n");
        error = true;
        return returner;
     }
     std::string a;
     while(!file.eof()){
        getline(file,a);
        returner+=a + '\n';
     }
     file.close();
     addMessage("Loaded File: " +fName);
     return returner;
}

std::string Parser::NextString(){
    std::string returner = "";
    skip();
    for(;travelBuffer[position]!=','&&travelBuffer[position]!='('&&travelBuffer[position]!=')'&&travelBuffer[position]!=' '&&travelBuffer[position]!='\t'&&travelBuffer[position]!='\n';position++){
        returner+=travelBuffer[position];
        if(travelBuffer[position]==0)
            break;
    }
    return returner;
}

void Parser::skip(){
    for(;travelBuffer[position]==' '||travelBuffer[position]=='\t'||travelBuffer[position]=='\n';position++){
        if(travelBuffer[position]==0)
            break;
    }
}

unsigned Parser::readOrderedStrings(){
    skip();
    if(travelBuffer[position]!='('){
        addMessageWithLabel("Expected an \"(\"");
        error = true;
        return 0;
    }else
        position++;
    unsigned i = 0;
    while(travelBuffer[position]!=')'&&travelBuffer[position]!='('&&travelBuffer[position]!=0){
        stringBuffer[i] = NextString();
        i++;
        skip();
        if(travelBuffer[position]==')')
            break;
        if(travelBuffer[position]==',')
            position++;
        else{
            addMessageWithLabel("Expected an \",\"");
            error = true;
            return 0;
        }
    }
    if(travelBuffer[position]==')'){
        position++;
        return i;
    }
    else
        return 0;
}

void Parser::addMessage(std::string m, bool dispCount){
    if(dispCount)
        EngineMessage::message<<++messageCount<<". "<<m<<std::endl;
    else
        EngineMessage::message<<m<<std::endl;
}

void Parser::addMessageWithLabel(std::string m){
    unsigned lineNo=0,column=0;
    for(unsigned i=0;i<position;i++){
        if(travelBuffer[i]=='\n'){
            lineNo++;
            column=0;
        }
        if(travelBuffer[i]=='\t')
            column+=4;
        else
            column++;
    }
    std::string line ="";
    unsigned i = position;
    for(;travelBuffer[i]!='\n'&&i!=0;i--);
    i++;
    for(;i<position;i++){
        if(travelBuffer[i]!='\t')
         line +=travelBuffer[i];
    }
    EngineMessage::message<<++messageCount<<". At Line line "<<lineNo<<" and column: "<<column<<" "<<line<<std::endl;
    EngineMessage::message<<"   "<<m<<std::endl;
}

unsigned Parser::readOrderedDoubles(){
    unsigned n = readOrderedStrings();
    ReturnZeroError();
    for(unsigned i=0;i<n;i++){
        double d;
        if(sscanf(stringBuffer[i].c_str(), "%lf", &d) != 1){
            error = true;
            addMessageWithLabel("Can't read the number");
            return 0;
        }
        doubleBuffer[i] = d;
    }
    return n;
}
