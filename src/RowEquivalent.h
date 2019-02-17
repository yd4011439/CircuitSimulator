#pragma once
#include<iostream>
class MatrixSolver{
private:
    MatrixSolver(){}
    int *rowMap;
    int numOfEq;
    double *Matrix;
    double *solutionRow;
    void printMat(){
        for(int i=0;i<numOfEq;i++){
            for(int j=0;j<numOfEq+1;j++)
            std::cout<<Matrix[rowMap[i]*(numOfEq+1)+j]<<"\t";
            std::cout<<std::endl;
        }
    }
    void copySolution(){
        for(int j=0;j<numOfEq;j++){
            solutionRow[j] = Matrix[rowMap[j]*(numOfEq+1)+numOfEq];
        }
    }

   void normalize(int row){
        double temp = Matrix[rowMap[row]*(numOfEq+1)+row];
        for(int j=row;j<=numOfEq;j++){
            Matrix[rowMap[row]*(numOfEq+1)+j] = Matrix[rowMap[row]*(numOfEq+1)+j]/temp;
        }
    }
    void spreadChangeBelow(int row){
        for(int j=row+1;j<numOfEq;j++){
            double temp = Matrix[rowMap[j]*(numOfEq+1)+row];
            for(int k=row;k<=numOfEq;k++)
                Matrix[rowMap[j]*(numOfEq+1)+k] = Matrix[rowMap[j]*(numOfEq+1)+k] - temp*Matrix[rowMap[row]*(numOfEq+1)+k];
        }
    }
    void spreadChangeAbove(int row){
        for(int j=0;j<=row-1;j++){
            double temp = Matrix[rowMap[j]*(numOfEq+1)+row];
            for(int k=row;k<=numOfEq;k++)
                Matrix[rowMap[j]*(numOfEq+1)+k] = Matrix[rowMap[j]*(numOfEq+1)+k] - temp*Matrix[rowMap[row]*(numOfEq+1)+k];

        }
    }

public:
    bool setMatrix(double* matrix,int numOfEquations, double *solnRow){
        Matrix = matrix;
        numOfEq = numOfEquations;
        rowMap = new int[numOfEquations];
        solutionRow = solnRow;
        for(int i=0;i<numOfEq;i++)
            rowMap[i] = -1;
        for(int i=0;i<numOfEq;){
            for(int j=0;j<numOfEq;j++){
                if(rowMap[j]==-1&&Matrix[i*(numOfEq+1)+j]!=0){
                    rowMap[j] = i;
                    goto next;
                }
            }
            next:
                i++;
        }
        printMat();
        return true;
    }
	 void getSolution(){
        /*For Debug*///
        //printMat();
        int i = 0;
        for(;i<numOfEq-1;i++){
            normalize(i);
            spreadChangeBelow(i);
        }
        for(;i>=0;i--){
            normalize(i);
            spreadChangeAbove(i);
        }
        copySolution();
        /*For Debug*///
        //printMat();
    }
    static MatrixSolver mSolver;
};
