#pragma once
#include"compdetail.h"
#include"lists.h"
#include"lists.cpp"
#include"enginemessage.h"
#include<string>
#include<fstream>
#include<stdlib.h>
#include<dlfcn.h>
#include"nodalsolver.h"

#define ReturnError(); {if(error) return;}
#define ReturnZeroError(); {if(error) return 0;}

typedef double (*relFunc)(double, double*);

class Parser{
private:
    LinkedList<DerivedTypeInfo*> loadedComponents;
    LinkedList<CompDetail*> allComponent;
    LinkedList<void*> dllLinks;

    //Buffers
    std::string travelBuffer;
    unsigned position;
    std::string stringBuffer[128];
    double doubleBuffer[128];
    bool error;
    unsigned messageCount;
    Stack<std::string> itemToLoad;
    DerivedTypeInfo* returner;
    CompDetail* component;
    LinkedList<CompDetail*> compBuffer;
    unsigned nodeCount;

    //Interfacing variables:
    unsigned varx1;
    unsigned varx2;
    unsigned vary1;
    unsigned vary2;
    double exitTime;


    std::string NextString(); //Will extract a string skipping separator token until another separator is found
    unsigned readOrderedDoubles();//Will read arbitarary number of Doubles inside ( ) separated by Commas and return the number of them
    unsigned readOrderedStrings();//Will read arbitarary number of Strings inside ( ) separated by Commas and return the number of them
    double NextNumber(); //Will Extract a Number without skipping a separator until another separator is found
    bool loaded();//Will search for a component in the list of loadedComponents and return false or true accordingly

    void _loadComponent();
    /**
     *  Will be a helper for DerivedTypeInfo* loadComponent() function
     *  Will Require travelBuffer be set
     */

    void NextComponent();
    //Will return a Basic ComponentDetail Object and will only be used by _loadComponent() function

    unsigned createDefination();
    //Will Create a DerivedTypeInfo object

    bool isLoaded(std::string compName);
    //This function will tell whether the component represented by compName has been loaded or not

    std::string loadFile(std::string fName);
    //Will return the contents of file defined by fName

    void skip();
    //Will Skip skipable separators

    void skipInclude();
    //Will Skip all the includes in a file

    void loadDerived();
    //This will first search derived components in the loadedComponents and then call function from compdetail class to create the component

    void createFunction();
    //This will create appropriate dll files for the desired functions

    void createAllComponents();
    //This Function will Fill the lists of NodalSolver Class;

    void loadComponentFromFile(std::string compName,bool isCircuit=false);
    //This will all components included in a file

    void createDependency(Stack<std::string>& dependencyList, std::string compName);
    //Creates a dependency stack for loading components

    void addMessage(std::string m, bool dispCount=true);
    void addMessageWithLabel(std::string m);

public:
    Parser():error(false),position(0),messageCount(0){}
    bool setComponent(std::string s, bool isFile=false);
    DerivedTypeInfo* loadComponent(std::string compName);
    void createCircuit(std::string str, bool isFile=true);
    //This function will create a circuit and load all the circuit into the NodalSolver class
    void CommandHandler();
};
