#pragma once
template<class Type>
struct linkedVertexData{
	int vertex;
	Type weight;
	linkedVertexData<Type> *next;
};
template<class type>
class LinkedVertex{
	private:
		int id;
		int degree;
		linkedVertexData<type> *first;
		linkedVertexData<type> *current;
		linkedVertexData<type> *last;
		bool reachedLast;

	public:
		LinkedVertex(int id):	id(id),degree(0),reachedLast(false){}
		int getId(){	return id;}
		void addEdge(int to, type weight);
		void deleteEdgeTo(int to);
		linkedVertexData<type>* getNextVertex();
		int getDegree(){	return degree;}
		void reset(){	current=first; reachedLast = false;}
		~LinkedVertex();


};
template<class type>
void LinkedVertex<type>::addEdge(int to, type weight){
	if(degree==0){
		first = new linkedVertexData<type>;
		first->vertex = to;
		first->weight = weight;
		current = first;
		last = first;

	}
	else{
		last->next = new linkedVertexData<type>;
		last = last->next;
		last->vertex = to;
		last->weight = weight;
	}
	degree++;
}
template<class type>
void LinkedVertex<type>::deleteEdgeTo(int to){
	linkedVertexData<type> *tracker = first;
	linkedVertexData<type> *itemToDelete = first;

	if(degree<=0)
		return;

	if(first->vertex==to){
		if(degree>1)
			first=first->next;

	}
	else{
		for(;itemToDelete!=last&&itemToDelete->vertex!=to;){
			tracker = itemToDelete;
			itemToDelete=itemToDelete->next;
		}
		if(itemToDelete->vertex!=to)
			return;
		itemToDelete = tracker->next;
		tracker->next = itemToDelete->next;

	}
	if(current==itemToDelete){
		if(itemToDelete==last)
			current=first;
		else
			current = itemToDelete->next;
	}

	degree--;
	delete itemToDelete;

}
template<class type>
linkedVertexData<type>* LinkedVertex<type>::getNextVertex(){
	if(reachedLast)
        return 0;
    linkedVertexData<type>* sender = current;
	if(current==last){
        current = first;
        reachedLast = true;
	}else
        current = current->next;
	return sender;
}
template<class type>
LinkedVertex<type>::~LinkedVertex(){
	linkedVertexData<type> *itemToDelete = first;
	for(int i=0;i<degree;i++){
		if(degree>1)
			first = first->next;
		delete itemToDelete;
		itemToDelete = first;
		degree--;
	}
}
