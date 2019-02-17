#include"wire.h"
void Wire::addPoint(unsigned x, unsigned y){
    allWire.append(x);
    allWire.append(y);
}
void Wire::End(unsigned x, unsigned y){
    addPoint(x,y);
    allWire.append(-1);
}
bool Wire::deleteWire(unsigned x, unsigned y){
    unsigned length = allWire.Length();
    unsigned i=0;
    unsigned x1,y1,x2,y2;
    x1 = allWire.getNextData()->data;
    y1 = allWire.getNextData()->data;
    for(;i<length;){
        x2 = allWire.getNextData()->data;
        if(x2==-1){
            x1 = allWire.getNextData()->data;
            y1 = allWire.getNextData()->data;
            i+=3;
            continue;
        }
        y2 = allWire.getNextData()->data;
        if(x1<=x&&y1<=y){
            if(x2>=x&&y2>=y)
                goto out;
        }
        else if(x1>=x&&y1>=y){
            if(x2<=x&&y2<=y)
                goto out;
        }
        x1 = x2;
        y1 = y2;
        i+=2;
    }
    return false;
    out:
    allWire.print();
    while(allWire.read(i)!=-1)
        i++;
    std::cout<<"Value of i: "<<i<<std::endl;
    allWire.remove(i);
    while(i>0){
        i--;
        std::cout<<"Inside "<<allWire.read(i)<<std::endl;
        if(allWire.read(i)==-1)
            break;
        allWire.remove(i);
    }
    return true;
}

void Wire::deleteLast(){
    allWire.remove(allWire.Length());
    allWire.remove(allWire.Length());
}

WireSelection Wire::getWireStatus(unsigned x, unsigned y){
    allWire.reset();
    unsigned length = allWire.Length();
    unsigned counter = 0;
    bool isFirst = false;
    for(unsigned i=0;i<length;){
        int x1 = allWire.getNextData()->data;
        if(x1==-1){
            i++;
            isFirst=true;
            continue;
        }
        unsigned y1 = allWire.getNextData()->data;
        unsigned x2 = allWire.getNextData()->data;
        unsigned y2 = allWire.getNextData()->data;
        if((x1<=x&&y1<=y)||(x1>=x&&y1>=y)){
            if((x2>=x&&y2>=y)||(x2<=x&&y2<=y))
                counter++;
        }
        if(counter>=1&&isFirst)
            return JOINT;
        isFirst=false;
        i+=2;
    }
    if(counter==0)
        return NOTHING;
    else if(counter==1)
        return SELECTABLE;
    else
        return CROSSOVER;
}
LinkedList<int> Wire::getJoinedPoints(){

}

void Wire::getLine(unsigned x, unsigned y, int* points, unsigned position, unsigned line[4]){
    unsigned k;
    for(k = position;points[k]!=-1;k++);

}


std::string Wire::getWireAsString(){
    std::ostringstream returner;
    allWire.reset();
    if(!allWire.Length())
        return "No Wires";
    unsigned counter = 1;
    int length = allWire.Length();
    if(length)
        returner<<"Wire"<<counter<<" ";
    for(unsigned i=0;i<length-1;){
        int x1 = allWire.getNextData()->data;
        if(x1==-1){
            counter++;
            returner<<std::endl;
            returner<<"Wire"<<counter<<" ";
            i++;
            continue;
        }
        unsigned y1 = allWire.getNextData()->data;
        returner<<"("<<x1<<","<<y1<<")";
        i+=2;
    }
    return returner.str();
}
