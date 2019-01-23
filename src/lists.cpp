#include"lists.h"
/*-------------------------------------------------------LinkedList's Functions-------------------------------------------------------------------*/
template<class Type>
void LinkedList<Type>::add(Type data,unsigned n){
    if(n==0)
        addAtFront(data);
    else if(n>=length){
        append(data);
    }else{
        linkedData<Type>* traverser = first;
        for(unsigned i = 0;i<n-1;i++)
            traverser = traverser->nextData;
        linkedData<Type>* store = traverser->nextData;
        traverser->nextData = new linkedData<Type>;
        traverser->nextData->data = data;
        traverser->nextData->nextData = store;
    }

}

template<class Type>
void LinkedList<Type>::addAtFront(Type data){
    if(length==0){
        first = new linkedData<Type>;
        first->data = data;
        last = first;
        current = first;
    }
    else{
        linkedData<Type>* holder = first;
        first = new linkedData<Type>;
        first->data = data;
        first->nextData = holder;
    }
    length++;
}

template<class Type>
void LinkedList<Type>::append(Type data){
    if(length==0){
        first = new linkedData<Type>;
        first->data = data;
        last = first;
        current = first;
    }
    else{
        last->nextData = new linkedData<Type>;
        last = last->nextData;
        last->data = data;
    }
    length++;
}

template<class Type>
Type LinkedList<Type>::read(unsigned n){
    linkedData<Type>* traverser = first;
    if(n>=length||length==0)
        return 0;
    else if(n==length-1)
        return last->data;
    for(unsigned i=0;i<n;i++)
        traverser = traverser->nextData;
    return traverser->data;
}

template<class Type>
linkedData<Type>* LinkedList<Type>::getNextData(){
    if(length==0)
        return 0;
    linkedData<Type>* sender = current;
    if(current == last)
        current = first;
    else
        current = current->nextData;
    return sender;
}

template<class Type>
bool LinkedList<Type>::contains(Type data){
    linkedData<Type>* traverser = first;
    for(unsigned i=0;i<length;i++){
        if(traverser->data==data)
            return true;
        traverser = traverser->nextData;
    }
    return false;
}

template<class Type>
Type* LinkedList<Type>::CopyToPointer(){
    if(length==0)
        return 0;
    linkedData<Type>* traverser = first;
    Type* pointer = new Type[length];
    for(unsigned i=0;i<length;i++){
        pointer[i] = traverser->data;
        traverser = traverser->nextData;
    }
}

template<class Type>
void LinkedList<Type>::remove(unsigned n){
    if(n>=length)
        return;
    linkedData<Type>* itemTodelete = first;
    if(n==0){
        if(length>1)
            first = first->nextData;
        if(current == itemTodelete)
            current = first;
        delete itemTodelete;
        length--;
        return;
    }
    else{
        linkedData<Type>* traverser = first;
        for(unsigned i=0;i<n-1;i++)
            traverser = traverser->nextData;
        itemTodelete = traverser->nextData;
        if(n<length-1){
            traverser->nextData = itemTodelete->nextData;
            if(current==itemTodelete)
                current = traverser->nextData;
        }
        else{
            last = traverser;
            current = first;
        }
    }
    delete itemTodelete;
    length--;
}

template<class Type>
void LinkedList<Type>::removeAll(){
    if(length==0)
        return;
    linkedData<Type>* itemTodelete = first;
    for(unsigned i = 0;i<length;i++){
        if(i<length-1)
            first = first->nextData;
        delete itemTodelete;
        itemTodelete = first;
    }
    length=0;
}

template<class Type>
LinkedList<Type>::~LinkedList(){
    removeAll();
}

template<class Type>
void LinkedList<Type>::appendList(LinkedList& list){
    unsigned len = list.Length();
    list.reset();
    for(;len>0;len--)
        this->append(list.getNextData()->data);
}

template<class Type>
void LinkedList<Type>::print(){
    linkedData<Type>* traverser = first;
    for(unsigned i=0;i<length;i++){
        std::cout<<traverser->data<<std::endl;
        traverser = traverser->nextData;
    }
}

/*---------------------------------------------------------------Stack's Functions-------------------------------------------------------------*/
template<class Type>
Type Stack<Type>::readTop(){
    if(length==0)
        return 0;
    return first->data;
}
template<class Type>
void Stack<Type>::push(Type data){
    if(length==0){
        first = new linkedData<Type>;
        first->data = data;
    }
    else{
        linkedData<Type>* holder = first;
        first = new linkedData<Type>;
        first->data = data;
        first->nextData = holder;
    }
    length++;
}

template<class Type>
void Stack<Type>::pop(){
    if(length==0)
        return;
    linkedData<Type>* itemToDelete = first;
    first = first->nextData;
    delete itemToDelete;
    length--;
}

template<class Type>
bool Stack<Type>::contains(Type data){
    linkedData<Type>* traverser = first;
    for(unsigned i=0; i<length;i++){
        if(traverser->data==data)
            return true;
        traverser = traverser->nextData;
    }
    return false;
}

template<class Type>
void Stack<Type>::print(){
    linkedData<Type>* traverser = first;
    for(unsigned i=0;i<length;i++){
        std::cout<<traverser->data<<std::endl;
        traverser = traverser->nextData;
    }
}

template<class Type>
Stack<Type>::~Stack(){
    for(unsigned i = 0; i<length; i++)
        pop();
    length=0;
}
