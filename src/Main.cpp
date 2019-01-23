#include<iostream>
#include"nodalsolver.h"
int main(){
    LinkedList<Component*> resistor;
    LinkedList<Component*> capacitor;
    LinkedList<Component*> voltageSource;
    LinkedList<Component*> inductor;

    Component* r1 = new Resistor(0,0,1,10);
    //Component* r2 = new Resistor(1,1,2,5);

    resistor.append(r1);
    //resistor.append(r2);

    Component* v1 = new VoltageSource(1, 0, 2, 10);

    voltageSource.append(v1);

    Component* c1 = new Capacitor(2, 1, 2, 0.001);
    //Component* c2 = new Capacitor(1, 1, 2, 0.002);
    capacitor.append(c1);
    //capacitor.append(c2);

    Component *in1 = new Inductor(3, 1, 2, 0.0001);

    inductor.append(in1);

    NodalSolver::NSolver.setComponent(RESISTOR,resistor);
    NodalSolver::NSolver.setComponent(CAPACITOR,capacitor);
    NodalSolver::NSolver.setComponent(IVS,voltageSource);
    NodalSolver::NSolver.setComponent(INDUCTOR,inductor);

    Component::setDeltaT(0.0001);

    if(NodalSolver::NSolver.setup()==false)
        EngineMessage::printMessageToConsole();
    else{
        for(int i=0;i<20000;i++)
            NodalSolver::NSolver.simulate();
        NodalSolver::NSolver.printAllData();
    }
}

