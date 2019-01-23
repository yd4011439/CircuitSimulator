#include<iostream>
//#include"nodalsolver.h"
#include"lists.h"
#include"lists.cpp"
int main(){
    LinkedList<float*> floats;
    float* data1 = new float(100);
    float* data2 = new float(1007);
    float* data3 = new float(1006);
    float* data4 = new float(1050);
    float* data5 = new float(1400);

    floats.append(data1);
    floats.append(data2);
    floats.append(data3);
    floats.append(data4);
    floats.append(data5);

    floats.remove(0);
    std::cout<<"Value of data1: "<<*data1<<std::endl;

    floats.print();

}
