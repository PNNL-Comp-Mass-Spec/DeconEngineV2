#pragma once
#include <vector> 

#define   SWAP(a,b)  tempr = (a);(a)=(b);(b)=tempr
namespace Engine
{
	namespace FFT
	{
		void Four1(unsigned long nn, std::vector<double> &Data, int isign) ; 
		int four1(int nn, float  *lpdata, int isign);
		int realft(int n, float *lpdata,  int isign);
		void four(int nn, float *data, int isign);
	}
}