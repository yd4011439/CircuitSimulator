#pragma once
#include<iostream>
template<class type>
struct linkedData{
    type data;
    linkedData<type>* nextData;
};

template<class Type>
class LinkedList{
private:
    unsigned length;
    linkedData<Type>* current;
    linkedData<Type>* first;
    linkedData<Type>* last;
public:
    LinkedList():length(0){}
    unsigned Length(){  return length;}
    void add(Type data,unsigned n);
    void addAtFront(Type data);
    void append(Type data);
    Type read(unsigned n);
    linkedData<Type>* getNextData();
    bool contains(Type data);
    Type* CopyToPointer();
    void appendList(LinkedList& list);
    void remove(unsigned n);
    void reset(){   current = first;}
    void print();
    void removeAll();
    ~LinkedList();
};

template<class Type>
class Stack{
private:
    linkedData<Type>* first;
    unsigned length;
public:
    Stack():length(0){}
    void push(Type);
    Type pop();
    bool contains(Type data);
    void print();
    Type readTop();
    Length(){   return length;}
    ~Stack();
};
