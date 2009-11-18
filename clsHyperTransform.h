#pragma once
#include "clsHornTRansformResults.h" 
#using <mscorlib.dll>

namespace DeconToolsV2
{
	public __gc class clsHyperTransform
	{
	public:
		clsHyperTransform(void);
		~clsHyperTransform(void);
		void GetHyperTransformSpectrum(DeconToolsV2::HornTransform::clsHornTransformResults* (&marr_transformResults) __gc [], double mostAbundantMW, 
			short charge, float (&sumMZs) __gc [], float (&sumIntensities) __gc[], float (&mzs) __gc [], 
			float (&intensities) __gc[]) ;
	};
}