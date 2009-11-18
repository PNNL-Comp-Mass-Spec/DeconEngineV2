#using <mscorlib.dll>
#include "clsSmoothData.h"

namespace Decon2LS
{
	namespace Smoothing
	{

		clsSmoothData::clsSmoothData(void)
		{		
			mobj_filter_smooth = new Engine::Smoothing::KernelSmoothing();
		}

		clsSmoothData::~clsSmoothData(void){};

		void clsSmoothData::SmoothMSData(float (&mzs) __gc [], float (&intensities) __gc [], float(&smooth_intensities) __gc[])
		{			
			std::vector<double> vectMzs ;
			std::vector<double> vectIntensities ;
			std::vector<double> vectSmoothedData ;
			int numPoints = mzs->Length ; 
			for (int ptNum = 0 ; ptNum < numPoints ; ptNum++)
			{
				vectMzs.push_back((double)mzs[ptNum]) ; 
				vectIntensities.push_back((double)intensities[ptNum]) ; 
				vectSmoothedData.push_back(0);
			}
			mobj_filter_smooth->SmoothData(&vectMzs, &vectIntensities, &vectSmoothedData);
		}
	}
}


