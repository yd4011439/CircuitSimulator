#pragma once
#include"lists.h"
#include"LinkedVertex.h"
template<class Type>
class LinkedGraph{
	private:
		int numOfVertex;
		LinkedList<LinkedVertex<Type>*> vertexList;
		bool created=false;

	public:
		LinkedGraph():numOfVertex(0){}
		void addVertex(int, int*, Type*);
		LinkedVertex<Type>* gotoNextVertex(){   return vertexList.getNextData()->data;}
		LinkedVertex<Type>* returnVertexAt(int i){   return vertexList.read(i);}
		void addVertex(int n);
		void addFromMatrix(Type* data, int noOfVertices);
		linkedVertexData<Type>* getNextVertexFrom(int vertex);
		int getDegreeOf(int vertex);
		void addEdge(int from, int to, Type weight);
		void addSymmetricEdge(int from, int to, Type weight);
		void deleteEdge(int from, int to);
		void deleteVertex(int vertex);
		void reset(){   vertexList.reset();}
		void reset(int i);
		void resetAll();
		Type* getAsMatrix();
		int numOfVert(){	return numOfVertex;}

};
template<class Type>
void LinkedGraph<Type>::reset(int vertex){
    LinkedVertex<Type>* vert = vertexList.read(vertex);
    if(vert!=0)
        vert->reset();
}

template<class Type>
void LinkedGraph<Type>::resetAll(){
    vertexList.reset();
    for(int i=0;i<numOfVertex;i++){
        LinkedVertex<Type>* vert = vertexList.getNextData()->data;
        vert->reset();
    }
}

template<class Type>
void LinkedGraph<Type>::addVertex(int degree, int* links, Type* weights){
	LinkedVertex<Type> *vertexToAdd = new LinkedVertex<Type>(numOfVertex);
	for(int i=0;i<degree;i++){
		vertexToAdd->addEdge(links[i],weights[i]);
		std::cout<<links[i]<<": "<<weights[i]<<std::endl;
	}
	vertexList.append(vertexToAdd);
	numOfVertex++;
	created = true;
}
template<class Type>
void LinkedGraph<Type>::addVertex(int n){
	if(created)
		return;
	for(int i=0;i<n;i++){
		LinkedVertex<Type> *vertexToAdd = new LinkedVertex<Type>(numOfVertex);
		vertexList.append(vertexToAdd);
		numOfVertex++;
	}
	created=true;
}
template<class Type>
void LinkedGraph<Type>::addFromMatrix(Type* data, int numOfVertices){
	if(created)
		return;
	for(int i=0;i<numOfVertices;i++){
		LinkedVertex<Type> *vertexToAdd = new LinkedVertex<Type>(numOfVertex);
		for(int j=0;j<numOfVertices;j++){
			if(data[i*numOfVertices+j]!=0)
				vertexToAdd->addEdge(j,data[i*numOfVertices+j]);
		}
		numOfVertex++;
		vertexList.append(vertexToAdd);
	}
	created=true;
}
template<class Type>
linkedVertexData<Type>* LinkedGraph<Type>::getNextVertexFrom(int vertex){
	LinkedVertex<Type>* vertexPointer = vertexList.read(vertex);
	return vertexPointer->getNextVertex();
}
template<class Type>
int LinkedGraph<Type>::getDegreeOf(int vertex){
	LinkedVertex<Type>* vertexPointer = vertexList.read(vertex);
	return vertexPointer->getDegree();
}
template<class Type>
void LinkedGraph<Type>::addEdge(int from, int to, Type weight){
	LinkedVertex<Type>* fromVertex = vertexList.read(from);
	LinkedVertex<Type>* toVertex = vertexList.read(to);
	fromVertex->addEdge(toVertex->getId(),weight);
}
template<class Type>
void LinkedGraph<Type>::addSymmetricEdge(int from, int to, Type weight){
	LinkedVertex<Type>* fromVertex = vertexList.read(from);
	LinkedVertex<Type>* toVertex = vertexList.read(to);
	fromVertex->addEdge(toVertex->getId(),weight);
	toVertex->addEdge(fromVertex->getId(),weight);
}
template<class Type>
void LinkedGraph<Type>::deleteEdge(int from, int to){
	LinkedVertex<Type>* fromVertex = vertexList.read(from);
	LinkedVertex<Type>* toVertex = vertexList.read(to);
	fromVertex->deleteEdgeTo(toVertex->getId());
}
template<class Type>
void LinkedGraph<Type>::deleteVertex(int a){

}
template<class Type>
Type* LinkedGraph<Type>::getAsMatrix(){
	Type *data = new Type[numOfVertex*numOfVertex];
 	for(int i=0;i<numOfVertex*numOfVertex;i++){
		data[i]=0;
	}
	for(int i=0;i<numOfVertex;i++){
		LinkedVertex<Type>* currentVertex = vertexList.read(i);
		int degree = currentVertex->getDegree();
		currentVertex->reset();
		for(int j=0; j< degree; j++){
			linkedVertexData<Type>* nextVertex = currentVertex->getNextVertex();
			LinkedVertex<Type>* vertexPointer = vertexList.read(nextVertex->vertex);
			data[i*numOfVertex+nextVertex->vertex] = nextVertex->weight;
		}
	}
	return data;
}
