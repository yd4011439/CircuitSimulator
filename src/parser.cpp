#include"parser.h"
bool Parser::setDimension(){
    if(goAfter("#grid")==false)
        fmessage("Grid not defined")
    if(!readNOrderPair(2)){
        fmessage("Grid dimensions not defined")
    }
    width = (unsigned)doubleBuffer[0];
    height = (unsigned)doubleBuffer[1];
    sTree("Set Grid: Width = "<<width<<" and Height = "<<height);
    return true;
}

bool Parser::setCType(){
    if(goAfter("#ctype")==false)
        fmessage("File empty or ctype not defined")
    unsigned position = bufferOffset;
    setline()
    bool setctype = false;
    if(goAfter("\"NODED\"")==false)
        bufferOffset = position;
    else{
        cType=NODED;
        setctype = true;
        sTree("Set CType: NODED")
    }
    if(!setctype){
        if(goAfter("\"WIRED\"")==false){
            eline()
            fmessage("Expected type of circuit \"NODED\" or \"WIRED\"")
        }
        else{
            cType = WIRED;
            sTree("Set CType: WIRED")
        }
    }
    return true;
}

bool Parser::compString(char* sc){
    for(int i=0;sc[i]!=0;i++,bufferOffset++){
        if(charBuffer[bufferOffset]!=sc[i])
            return false;
        upDatePosition();
    }
    return true;
}

bool Parser::goAfter(char *s,bool neglectAll){
    if(neglectAll){
        while(charBuffer[bufferOffset]){
            if(charBuffer[bufferOffset]==s[0]){
                if(compString(s))
                    return true;
            }
            if(charBuffer[bufferOffset]==';')
                return false;
            bufferOffset++;
            upDatePosition();
        }
    }else{
        while(charBuffer[bufferOffset]){
            if(charBuffer[bufferOffset]==' '||charBuffer[bufferOffset]=='\t'||charBuffer[bufferOffset]=='\n'){
                bufferOffset++;
                upDatePosition();
            }else if(charBuffer[bufferOffset]==s[0]){
                if(compString(s))
                    return true;
            }else
                break;
            if(charBuffer[bufferOffset]==';')
                return false;
        }
    }
    return false;
}

void Parser::upDatePosition(){
    if(charBuffer[bufferOffset]=='\n'){
        lineNo++;
        column=0;
    }else if(charBuffer[bufferOffset]=='\t')
        column+=4;
    else
        column++;
}

bool Parser::readNOrderPair(unsigned n){
    setline()
    if(!goAfter("(")){
        eline()
        fmessage("Syntax Error: Expected an \"(\".")
    }
    else{
        unsigned i = 0;
        while(i<n){
            if(!readNum(doubleBuffer[i]))
                return false;
            setline()
            i++;
            if(i<n&&n>1){
                if(!goAfter(",")){
                    eline()
                    fmessage("Expected a \",\"")
                }
            }
        }
        setline()
        if(!goAfter(")")){
            eline();
            fmessage("Syntax Error: Expected an \")\"")
        }
    }
    return true;
}

bool Parser::readNum(double& number){
    LinkedList<double> num;
    setline()
    for(;charBuffer[bufferOffset]==' '||charBuffer[bufferOffset]=='\t'||charBuffer[bufferOffset]=='\n';bufferOffset++){
        if(charBuffer[bufferOffset]==0){
            eline()
            fmessage("Expected a constant")
        }
        upDatePosition();
    }

    unsigned bDecimal = 0;
    unsigned aDecimal = 0;
    bool isADecimal = false;
    bool isNegative = false;
    for(;;bufferOffset++)
    {
        if(charBuffer[bufferOffset]>=0x30&&charBuffer[bufferOffset]<=0x39){
            if(!isADecimal)
                bDecimal++;
            else
                aDecimal++;
            num.append(charBuffer[bufferOffset]);
        }
        else if(charBuffer[bufferOffset]=='.'){
            isADecimal=true;
        }else if(charBuffer[bufferOffset]=='-'){
            isNegative=true;
            continue;
        }
        else
            break;
    }
    if(bDecimal==0&&aDecimal==0){
        fmessage("At Line: "<<l<<" Column: "<<c<<"\nExpected a Constant")
    }

    number = 0;
    num.reset();

    for(int i=bDecimal;i>0;i--){
        if(isNegative)
            number = number - (num.getNextData()->data-48)*pow(10,i-1);
        else
            number = number + (num.getNextData()->data-48)*pow(10,i-1);
    }
    for(int i=0;i<aDecimal;i++){
        if(isNegative)
            number = number - (num.getNextData()->data-48)*pow(10,-i-1);
        else
            number = number + (num.getNextData()->data-48)*pow(10,-i-1);
    }
    if(charBuffer[bufferOffset]=='f'){
        if(isADecimal){
            bufferOffset++;
            upDatePosition();
        }else{
            setline()
            eline()
            fmessage("\"f\" after a non-float constant")
        }
    }
    return true;
}

bool Parser::genList(){
    bool error = false;
    if(!setCType()){
        error = true;
        bufferOffset = 0;
    }
    if(cType==WIRED){
        if(!setDimension()){
            error = true;
            bufferOffset = 0;
        }
        if(!setWire()){
            error = true;
            bufferOffset = 0;
        }
    }
    else if(cType==NODED){
        if(!setComponent())
            error = true;
    }else
        error = true;
    if(error)
        return false;
    return true;
}


bool Parser::setWire(){
    if(goAfter("#wire")==false)
        fmessage("No Wire Defined")
    bool finished = false;
    unsigned counter = 1;
    std::ostringstream str;
    str<<"   Wire"<<counter<<" ";
    while(!finished){
        if(readNOrderPair(2)==false)
            fmessage("Can't read parameters of wire object")
        else{
            wire.addPoint(doubleBuffer[0],doubleBuffer[1]);
            str<<"("<<doubleBuffer[0]<<","<<doubleBuffer[1]<<")";
        }
        unsigned position = bufferOffset;
        if(goAfter("(",false)==false){
            position = bufferOffset;
            if(goAfter("#wire",false)==false){
                finished = true;
            }else{
                wire.addLabel();
                str<<std::endl;
                counter++;
                str<<"   Wire"<<counter<<" ";
            }
        }
        bufferOffset = position;
    }
    wire.addLabel();
    sTree("Added Following Wires\n"<<str.str())
    return true;
}

bool Parser::setComponent(){
    unsigned componentCount = 0;
    while(charBuffer[bufferOffset]!=0){
        component = new CompDetail;
        if(!goAfter("#component")&&!componentCount)
            fmessage("No Component Defined")
        else
            componentCount++;
        print("#Component Found");
        unsigned position = bufferOffset;
        setline()
        if(!goAfter("type",true)){
            eline()
            fmessage("Type of not found")
        }

        //Code for finding out the type
        else
        {
            print("type found");
            setline()
            if(!goAfter("=")){
                eline()
                fmessage("Expected an \"=\" after type")
            }else{
                setline()
                if(!goAfter("\"")){
                    eline()
                    fmessage("Expected an \"");
                }
                if(!extractStringTill('\"'))
                    return false;
                else{
                    print("stringextracted found");
                    setline()
                    if(extractedString=="RESISTOR")
                        component->cType = RESISTOR;
                    else if(extractedString=="INDUCTOR")
                        component->cType = INDUCTOR;
                    else if(extractedString == "CAPACITOR")
                        component->cType = CAPACITOR;
                    else if(extractedString=="IVS")
                        component->cType = IVS;
                    else if(extractedString == "ICS")
                        component->cType = ICS;
                    else if(extractedString=="VCVS")
                        component->cType = VCVS;
                    else if(extractedString == "VCCS")
                        component->cType = VCCS;
                    else if(extractedString == "TCVS")
                        component->cType = TCVS;
                    else if(extractedString == "TCCS")
                        component->cType = TCCS;
                    else if(extractedString == "DERIVED"){
                        component->cType = DERIVED;
                    }
                    else{
                        eline()
                        fmessage("Unidentified type")
                    }
                }
            }
        }

        //Code for finding out the nodes
        bufferOffset = position;
        if(goAfter("node",true)){
            print("node Found");
            setline()
            if(goAfter("=")){
                if(readNOrderPair(2)){
                    component->numOfNodes = 2;
                    component->nodes = new unsigned[2];
                    component->nodes[0] = (unsigned)doubleBuffer[0];
                    component->nodes[1] = (unsigned)doubleBuffer[1];
                }else
                    return false;
            }else{
                eline()
                fmessage("Expected an \"=\"")
            }
        }
        else
        {
            fmessage("Can' t find node assignment")
        }
        bufferOffset=position;
        //Code for finding out the value
        if(goAfter("value",true)){
            print("value Found");
            setline()
            if(goAfter("=")){
                if(readNOrderPair(1)){
                    component->values = new double;
                    *component->values = doubleBuffer[0];
                }else
                    return false;
            }else{
                eline()
                fmessage("Expected an \"=\"")
            }
        }else{
            fmessage("Can' t find value assignment")
        }
        bufferOffset = position;
        if(goAfter("label",true)){
            setline()
            if(goAfter("=")){
                setline()
                if(goAfter("\"")){
                    if(extractStringTill('\"')){
                        component->label1 = extractedString;
                    }
                    else
                        return false;
                }else{
                    eline()
                    fmessage("Expected an \"")
                }
            }else{
                eline()
                fmessage("Expected an \"=\"")
            }
        }else{
            fmessage("Label not defined")
        }
        //Checking for the semiColon
        setline()
        if(goAfter(";")==false){
            print("; Found");
            eline()
            fmessage("Expected an \";\"")
        }
        createComponent();
        componentList.append(component);
    }
    return true;
}

bool Parser::extractStringTill(char s){
    extractedString = "";
    print("Extracting string");
    for(;charBuffer[bufferOffset]=='\b'||charBuffer[bufferOffset]=='\t'||charBuffer[bufferOffset]=='\n';bufferOffset++);
    while(charBuffer[bufferOffset]!=s&&charBuffer[bufferOffset]!=0){
        upDatePosition();
        if(charBuffer[bufferOffset]!='\n')
            extractedString += charBuffer[bufferOffset];
        else{
            setline()
            eline()
            fmessage("Expected a "<<s)
        }
        bufferOffset++;
    }
    if(charBuffer[bufferOffset]!=0)
        return true;
    else{
        setline()
        eline()
        fmessage("Expected a "<<s)
    }
}

void Parser::CommandHandler(){
    std::string command = "";
    std::cout<<"                                    Circuit Simulator"<<std::endl;
    do{
        std::cout<<">> ";
        EngineMessage::message.str("");
        std::getline(std::cin,command);
        setWithString(command);
        if(goAfter("#component")){
            print("SetComponent");
            bufferOffset=0;
            if(!setComponent()){
                std::cout<<EngineMessage::message.str()<<std::endl;
                delete component;
            }
            continue;
        }
        bufferOffset = 0;
        if(goAfter("set")){
            if(!setHandler())
                std::cout<<EngineMessage::message.str()<<std::endl;
            else
                std::cout<<"No setError"<<std::endl;
            continue;
        }
        bufferOffset=0;
        if(goAfter("timestep")){
            if(readNOrderPair(1)){
                Component::setDeltaT(doubleBuffer[0]);
            }else{
                std::cout<<EngineMessage::message.str()<<std::endl;
            }
            continue;
        }
        bufferOffset=0;
        if(goAfter("exittime")){
            if(readNOrderPair(1)){
                exitTime = doubleBuffer[0];
            }else{
                std::cout<<EngineMessage::message.str()<<std::endl;
            }
             continue;
        }
        bufferOffset=0;
        if(goAfter("savefile")){
            if(goAfter("(")){
                if(extractStringTill(')')){
                    file.close();
                    file.open(extractedString.c_str());
                    if(!file)
                        std::cout<<"Can't create the file"<<std::endl;
                    saveTofile=true;
                    continue;
                }else{
                    std::cout<<EngineMessage::message.str()<<std::endl;
                }
            }
            else{
                std::cout<<"Expected an \"(\"."<<std::endl;
            }
            continue;
        }
        bufferOffset=0;
        if(goAfter("simulate")){
            if(simulate()==false)
                std::cout<<EngineMessage::message.str()<<std::endl;
            continue;
        }
        bufferOffset=0;
        if(goAfter("quit"))
            return;
        bufferOffset=0;
        if(goAfter("cls")){
            system("cls");
            continue;
        }
        std::cout<<"Can't identify the command"<<std::endl;
    }while(1);
}

bool Parser::setHandler(){
    if(charBuffer[bufferOffset]=='x'){
       if(setHandlerHelper(1))
            return true;
   }else if(charBuffer[bufferOffset]=='y'){
        if(!setHandlerHelper(2))
            return true;
   }
   return false;
}

bool Parser::setHandlerHelper(unsigned fid){
    bufferOffset++;
    if(charBuffer[bufferOffset]=='v'){
           if(readNOrderPair(2)){
               //Write a function that checks whether the node is present or not
               if(!nodePresent()){
                fmessage("One or both of the nodes are not present in the circuit")
               }
               if(fid==1){
                   varx1 = (unsigned)doubleBuffer[0];
                   varx2 = (unsigned)doubleBuffer[1];
                   xSetV=true;
                   xSetI=false;
               }else{
                    vary1 = (unsigned)doubleBuffer[0];
                    vary2 = (unsigned)doubleBuffer[1];
                    ySetV=true;
                    ySetI=false;
               }
           }
       }
       else if(charBuffer[bufferOffset]=='i'){
           if(goAfter("(")){
               if(extractStringTill(')')){
                   //Write a function which will search for the id of the required labels
                   int id = extractID(extractedString);
                   if(id!=-1){
                       if(fid==1){
                        varx1 = (unsigned)id;
                        xSetI=true;
                        xSetV=false;
                       }
                       else{
                        vary1 = (unsigned)id;
                        ySetI=true;
                        ySetV=false;
                       }
                   }else{
                       fmessage("Can't find the id or the device may not be two terminal device")
                   }
               }
           }
       }
       else{
        fmessage("Unappropriate Defination")
       }
}

bool Parser::nodePresent(){
    componentList.reset();
    int length = componentList.Length();
    if(!length)
        return false;
    while(length>0){
        CompDetail *comp = componentList.getNextData()->data;
        for(unsigned i=0;i<comp->numOfNodes;i+=2){
            if((unsigned)doubleBuffer[0]==comp->nodes[i]&&(unsigned)doubleBuffer[1]==comp->nodes[i+1])
                return true;
        }
        length--;
    }
    return false;
}

int Parser::extractID(std::string a){
    int length = componentList.Length();
    if(length==0)
        return -1;
    componentList.reset();
    for(int i=0;i<length;i++){
        CompDetail* comp = componentList.getNextData()->data;
        if(comp->label1==a&&comp->cType!=DERIVED)
            return comp->actualId;
    }
    return -1;
}

bool Parser::createComponent(){
    Component* comp;
    switch(component->cType){
    case RESISTOR:
        comp = new Resistor(0,component->nodes[0],component->nodes[1],*(component->values));
        component->actualId = comp->getId();
        NodalSolver::NSolver.resistors.append(comp);
        std::cout<<"Resistor("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
    break;
    case INDUCTOR:
        comp = new Inductor(0,component->nodes[0],component->nodes[1],*(component->values));
        component->actualId = comp->getId();
        NodalSolver::NSolver.inductors.append(comp);
        std::cout<<"Inductor("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
    break;
    case CAPACITOR:
        comp = new Capacitor(0,component->nodes[0],component->nodes[1],*(component->values));
        component->actualId = comp->getId();
        NodalSolver::NSolver.capacitors.append(comp);
        std::cout<<"Capacitor("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
    break;
    case IVS:
        comp = new VoltageSource(0,component->nodes[0],component->nodes[1],*(component->values));
        component->actualId = comp->getId();
        NodalSolver::NSolver.iVS.append(comp);
        std::cout<<"VoltageSource("<<comp->getId()<<","<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
    break;
    case ICS:
        comp = new CurrentSource(0,component->nodes[0],component->nodes[1],*(component->values));
        component->actualId = comp->getId();
        NodalSolver::NSolver.iCS.append(comp);
        std::cout<<"CurrentSource(0,"<<(component->nodes[0])<<","<<(component->nodes[1])<<","<<*(component->values)<<")"<<std::endl;
    break;
    default:
        std::cout<<"Couldn't recognize the components"<<std::endl;
    }
}

bool Parser::simulate(){
    if(componentList.Length()==0)
        fmessage("No Component set.")
    if(NodalSolver::NSolver.setup()==false)
        return false;
    while(exitTime>=Component::timeElapsed){
        NodalSolver::NSolver.simulate();
        if(xSetV)
            xBuffer.append(Component::nodalVoltage[varx1]-Component::nodalVoltage[varx2]);
        else if(xSetI)
            xBuffer.append(Component::componentCurrent[varx1]);
        else if(ySetV||ySetI)
            xBuffer.append(Component::timeElapsed);

        if(ySetV)
            yBuffer.append(Component::nodalVoltage[vary1]-Component::nodalVoltage[vary2]);
        else if(ySetI)
            yBuffer.append(Component::componentCurrent[vary1]);

    }
    if(saveTofile){
        std::cout<<"Yes Save to file"<<std::endl;
        unsigned numOfData = xBuffer.Length();
        file<<"X Coordinate:\n[";
        xBuffer.reset();
        for(unsigned i=0;i<numOfData-1;i++){
            file<<xBuffer.getNextData()->data<<", ";
        }
        file<<xBuffer.getNextData()->data<<"]\nY Coordinate:\n[";
        yBuffer.reset();
        for(unsigned i=0;i<numOfData-1;i++){
            double d = yBuffer.getNextData()->data;
            file<<d<<", ";
            std::cout<<"Data: "<<d<<" I: "<<i<<std::endl;
            if(!file){
                i=numOfData;
                std::cout<<"Error writting to file"<<std::endl;
            }
        }
        file<<yBuffer.getNextData()->data<<"]";
    }
}
