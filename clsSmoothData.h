#pragma once
#include "Smoothing\KernelSmoothing.h"
#using <mscorlib.dll>

namespace Decon2LS
{
 namespace Smoothing
 {
	public __gc class clsSmoothData
	{

		Engine::Smoothing::KernelSmoothing __nogc *mobj_filter_smooth;

		public:
		
			clsSmoothData(void);

			~clsSmoothData(void);

			void SmoothMSData(float (&mzs) __gc [], float (&intensities) __gc [], float(&smooth_intensities) __gc[]);
		
	};
 }
}

