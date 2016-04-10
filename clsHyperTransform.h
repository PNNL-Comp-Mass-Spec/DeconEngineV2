#pragma once
#include "clsHornTRansformResults.h" 
#using <mscorlib.dll>

namespace DeconToolsV2
{
	public ref class clsHyperTransform
	{
	public:
		clsHyperTransform(void);
		~clsHyperTransform(void);
		void GetHyperTransformSpectrum(array<DeconToolsV2::HornTransform::clsHornTransformResults^>^ (&marr_transformResults), double mostAbundantMW, 
            short charge, array<float> ^ (&sumMZs), array<float> ^ (&sumIntensities), array<float> ^ (&mzs),
            array<float> ^ (&intensities));
	};
}