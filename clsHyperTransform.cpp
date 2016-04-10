#include ".\clshypertransform.h"
#include "Utilities/Interpolation.h"
#include <vector>
using System::Math ; 
namespace DeconToolsV2
{
	clsHyperTransform::clsHyperTransform(void)
	{
	}

	clsHyperTransform::~clsHyperTransform(void)
	{
	}

	void clsHyperTransform::GetHyperTransformSpectrum(array<DeconToolsV2::HornTransform::clsHornTransformResults^>^ (&marr_transformResults), double mostAbundantMW,
        short charge, array<float> ^ (&sumMZs), array<float> ^ (&sumIntensities), array<float> ^ (&mzs),
        array<float> ^ (&intensities))
	{
		std::vector<int> vectIndicesToConsider ; 
		// go through all the transforms and find which ones have most abundant mass between current value and 
		// x Daltons
		int numResults = marr_transformResults->Length ; 
		short maxCharge = 0 ;
		double massRange = 0 ; 

		for (int transformNum = 0 ; transformNum < numResults ; transformNum++)
		{
			DeconToolsV2::HornTransform::clsHornTransformResults^ result = marr_transformResults[transformNum] ; 
			double massDiff = System::Math::Abs((result->mdbl_most_intense_mw - mostAbundantMW)/1.003) ; 
			double massDiffRound = (double) ((int)(massDiff+0.5)) ; 
			if (massDiffRound > 3)
				continue ;
			double toleranceDiff = System::Math::Abs(massDiff - massDiffRound * 1.003) ; 
			if (toleranceDiff < System::Math::Max(0.2,result->mdbl_fwhm * 5))
			{
				// consider this peak for addition. 
				vectIndicesToConsider.push_back(transformNum) ; 
				if (result->mshort_cs > maxCharge)
					maxCharge = result->mshort_cs ; 
				if (result->mdbl_most_intense_mw - result->mdbl_mono_mw > 2*massRange)
				{
					massRange = 2*(result->mdbl_most_intense_mw - result->mdbl_mono_mw) ; 
				}
			}
		}

		if (massRange < 8)
		{
			massRange = 8 ; 
		}
		if (massRange >16)
		{
			massRange = 16 ;
		}

		double minMZForOut = (mostAbundantMW-massRange/2)/charge + 1.00727638 ;  
		double maxMZForOut = (mostAbundantMW+massRange/2)/charge + 1.00727638 ;
		const int numPointsForOut = 4*1024 ; 
		double currentMZ = minMZForOut ; 
		double mzInterval = (maxMZForOut - minMZForOut) / numPointsForOut ; 
        sumMZs = gcnew array<float>(numPointsForOut);
        sumIntensities = gcnew array<float>(numPointsForOut);
		for (int ptNum = 0 ; ptNum < numPointsForOut ; ptNum++)
		{
			sumMZs[ptNum] = (float)currentMZ ; 
			sumIntensities[ptNum] = 0 ; 
			currentMZ += mzInterval ; 
		}
		
		Engine::Utilities::Interpolation interp ; 
		std::vector<double> vectMz ; 
		std::vector<double> vectIntensity ; 
		int numPts = mzs->Length ; 
		for (int ptNum = 0 ; ptNum < numPts ; ptNum++)
		{
			double mz = mzs[ptNum]; 
			vectMz.push_back(mz) ; 
			double intense = intensities[ptNum] ; 
			vectIntensity.push_back(intense) ; 
		}

		interp.Spline(vectMz, vectIntensity, 0,0) ; 
		for (int index = 0 ; index < (int) vectIndicesToConsider.size() ; index++)
		{
			DeconToolsV2::HornTransform::clsHornTransformResults^ result = marr_transformResults[vectIndicesToConsider[index]] ; 
			double currentMZ = ((minMZForOut-1.00727638) * charge)/result->mshort_cs + 1.00727638 ; 
			for (int ptNum = 0 ; ptNum < numPointsForOut ; ptNum++)
			{
				sumIntensities[ptNum] +=  (float) interp.Splint(vectMz, vectIntensity, currentMZ) ; 
				currentMZ += (mzInterval * charge)/result->mshort_cs ; 
			}
		}
	}
}
