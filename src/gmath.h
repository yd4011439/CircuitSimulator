#pragma once
#include<math.h>

class LID{
private:
    double (*f)(double,double*);
    double position;
    double *valuePointer;
    double threshold;
    double getAppropriate(double x){
        while(x>threshold){
            x = x/2.0;
        }
        return x;
    }
public:
    LID(double (*fun)(double,double*),double t):position(0.0),threshold(t){ f = fun;}
    void setMembers(double (*fun)(double,double*),double* valPointer,double t){
        f = fun;
        threshold = t;
        valuePointer = valPointer;
    }
    LID(){}
    double getSlope(){

        double a = (f(position+10e-6,valuePointer) - f(position,valuePointer))/(10e-6);
        std::cout<<"slope: "<<a<<std::endl;
        return a;
    }
    double getConst(){
        float c = f(position,valuePointer) - getSlope()*position;
        std::cout<<"Constant: "<<c<<std::endl;
        return c;
    }
    bool doesFit(double x, double y){
        if(abs(y-f(x,valuePointer))<0.00000001)
            return true;
        if(x>threshold)
           x = getAppropriate(x);
        position = x;
    }
    static double abs(double val){   if(val<0) return -val; return val;}

};
