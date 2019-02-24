#include"tmp.hpp"
#include<cmath>
double BJTNPNf1(double x,double* value){
	return value[0]*1e-12*(pow(2.71828,x/(0.026*1/25))-1);
}
