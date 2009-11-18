// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "Apodization.h"
#include <math.h> 
namespace Engine
{
	namespace Utilities
	{
		 void Apodization::PerformInvertedTriangleApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			float              temp1,temp2;
			float               val1,val2,A;
			long                li;

			A = 1.0;
			temp1 = 0.5f * (float)(stopIndex - startIndex);
			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[2*li];
				val2 = vectIntensities[2*li+1];
				temp2 = (float)(li)/temp1;
				if(temp2 > 1.0) 
					temp2 = 2.0f - temp2;
				if(A==0.0) 
					temp2 = 1.0f - temp2;
				val1 *= temp2;
				val2 *= temp2;
				vectIntensities[2*li] = val1;
				vectIntensities[2*li+1] = val2;
			}
		}

		void Apodization::PerformInvertedWelchApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			double              temp1,temp2;
			float               val1,val2,A;
			long                li;

			A = 1.0;
			temp1 = 0.5 * (double)(stopIndex-startIndex - 1);
			temp2 = 0.5 * (double)(stopIndex-startIndex + 1);
			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[2*li];
				val2 = vectIntensities[2*li+1];
				val1 *= fabs(A - (float)((((double)(li)-temp1)/temp2)*(((double)(li)-temp1)/temp2)));
				val2 *= fabs(A - (float)((((double)(li)-temp1)/temp2)*(((double)(li)-temp1)/temp2)));
				vectIntensities[2*li] = val1;
				vectIntensities[2*li+1] = val2;
			}
		}

		 void Apodization::PerformInvertedHanningApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			double              temp1,temp2;
			float               val1,val2,A;
			long                li;

			A = 1.0;
			temp1 = 2.0 * 3.141592654;
			temp2 = (double)(stopIndex-startIndex - 1);
			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[2*li];
				val2 = vectIntensities[2*li+1];
				val1 *= fabs(A/2.0f - (float)cos(((double)(li)*temp1)/temp2)/2.0f);
				val2 *= fabs(A/2.0f - (float)cos(((double)(li)*temp1)/temp2)/2.0f);
				vectIntensities[2*li] = val1;
				vectIntensities[2*li+1] = val2;
			}
		}

		 void Apodization::PerformInvertedParzenApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			double              temp1,temp2;
			float               val1,val2,A;
			long                li;

			A = 1.0;
			temp1 = 0.5 * (double)(stopIndex-startIndex - 1);
			temp2 = 0.5 * (double)(stopIndex-startIndex + 1);
			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[2*li];
				val2 = vectIntensities[2*li+1];
				val1 *= fabs(A - (float) fabs(((double)(li)-temp1)/temp2));
				val2 *= fabs(A - (float) fabs(((double)(li)-temp1)/temp2));
				vectIntensities[2*li] = val1;
				vectIntensities[2*li+1] = val2;
			}
		}

		 void Apodization::PerformInvertedSquareApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			return ; 
		}

		 void Apodization::PerformTriangleApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			double              temp1,temp2;
			float               val1,A;
			long                li;

			A = 1.0;
			temp1 = 0.5 * (double)(stopIndex - startIndex);
			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[li];
				temp2 = (double)(li)/temp1;
				if(temp2 > 1.0) 
					temp2 = 2.0 - temp2;
				if(A==0.0) 
					temp2 = 1.0 - temp2;
				val1 = (float) (temp2 * val1) ;
				vectIntensities[li] = val1;
			}
		}

		 void Apodization::PerformWelchApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			double              temp1,temp2;
			float               val1, A;
			long                li;

			A = 1.0;
			temp1 = 0.5 * (double)(stopIndex-startIndex - 1);
			temp2 = 0.5 * (double)(stopIndex-startIndex + 1);

			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[li];
				val1 *= fabs(A - (float)((((double)(li)-temp1)/temp2)*(((double)(li)-temp1)/temp2)));
				vectIntensities[li] = val1;
			}
		}

		 void Apodization::PerformHanningApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			double              temp1,temp2;
			float               val1,A;
			long                li;

			A = 1.0;
			temp1 = 2.0 * 3.141592654;
			temp2 = (double)(stopIndex-startIndex - 1);

			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[li];
				val1 *= fabs(A/2.0f - (float)cos(((double)(li)*temp1)/temp2)/2.0f);
				vectIntensities[li] = val1;
			}
		}



		 void Apodization::PerformParzenApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			float val1 = 0 ,val2 = 0;
			long  li;
			float A = 1.0;
			double temp1 = 0.5 * (double)(stopIndex-startIndex - 1);
			double temp2 = 0.5 * (double)(stopIndex-startIndex + 1);

			for(li=startIndex;li<=stopIndex;li++)
			{
				val1 = vectIntensities[li];
				val1 *= fabs(A - (float)(fabs(((double)(li)-temp1)/temp2)));
				vectIntensities[li] = val1;
			}
		}

		 void Apodization::PerformSquareApodization(int startIndex, int stopIndex, float *vectIntensities)
		{
			return ; 
		}

		 void Apodization::Apodize(double minX, double maxX, double sampleRate, bool invert, 
			ApodizationType type, float *vectIntensities, int numPts, int apexPosition = 50)
		{
			int startIndex = (int) (minX * sampleRate) ; 
			int stopIndex = (int) (maxX * sampleRate); 
			if (startIndex > stopIndex)
			{
				int temp = startIndex ; 
				startIndex = stopIndex ; 
				stopIndex = startIndex ; 
			}
			if (stopIndex >= numPts)
				stopIndex = numPts-1 ; 

			if (apexPosition != 50)
			{
				int apexIndex = startIndex + ((stopIndex - startIndex) * apexPosition)/100 ; 
				for (int index = startIndex ; index < apexIndex ; index++)
				{
					vectIntensities[index] = vectIntensities[index] * (index-startIndex)/(apexIndex-startIndex) ; 
				}
				for (int index = apexIndex ; index < stopIndex ; index++)
				{
					vectIntensities[index] = vectIntensities[index] * (stopIndex-index)/(stopIndex-apexIndex) ; 
				}
			}
			if (!invert)
			{
				switch(type)
				{
				case SQUARE:
					PerformSquareApodization(startIndex, stopIndex, vectIntensities) ;
					break ; 
				case PARZEN:
					PerformParzenApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				case HANNING:
					PerformHanningApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				case WELCH:
					PerformWelchApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				case TRIANGLE:
					PerformTriangleApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				default:
					break ; 
				}
			}
			else
			{
				switch(type)
				{
				case SQUARE:
					PerformInvertedSquareApodization(startIndex, stopIndex, vectIntensities) ;
					break ; 
				case PARZEN:
					PerformInvertedParzenApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				case HANNING:
					PerformInvertedHanningApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				case WELCH:
					PerformInvertedWelchApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				case TRIANGLE:
					PerformInvertedTriangleApodization(startIndex, stopIndex, vectIntensities) ; 
					break ; 
				default:
					break ; 
				}
			}
		}
	}
}