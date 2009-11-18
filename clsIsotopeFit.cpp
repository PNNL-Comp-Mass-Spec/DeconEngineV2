// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\clsisotopefit.h"
#using <mscorlib.dll>
#include "PeakProcessor/PeakData.h"
#include "DeconEngineUtils.h"

#include <vector>

namespace DeconToolsV2
{
	clsIsotopeFit::clsIsotopeFit(void)
	{
		set_IsotopeFitType(AREA) ; 
	}

	clsIsotopeFit::~clsIsotopeFit(void)
	{
	}
	double clsIsotopeFit::GetFitScore(float (&mzs) __gc [], float (&intensities) __gc [], 
		DeconToolsV2::Peaks::clsPeak* (&peaks) __gc [], short charge, int peak_index, double delete_intensity_threshold,
		double min_intensity_for_score, System::Collections::Hashtable* elementCounts)
	{
		std::vector<double> vectMzs ;
		std::vector<double> vectIntensities ;
		int numPoints = mzs->Length ; 

		if (mzs->Length == 0)
		{
			throw new System::Exception(S"No data provided for the observed spectrum to match to") ; 
		}

		// mzs should be in sorted order
		double minMZ = mzs[0] ; 
		double maxMZ = mzs[numPoints-1] ; 
		DeconEngine::Utils::SetData(vectMzs, mzs) ; 
		DeconEngine::Utils::SetData(vectIntensities, intensities) ; 

		Engine::PeakProcessing::PeakData peakData ; 
		Engine::PeakProcessing::Peak currentPeak ; 

		DeconEngine::Utils::SetPeaks(peakData, peaks) ; 
		peakData.mptr_vect_mzs = &vectMzs ; 
		peakData.mptr_vect_intensities = &vectIntensities ; 
		peakData.GetPeak(peak_index, currentPeak) ; 

		Engine::TheoreticalProfile::MolecularFormula formula ; 
		DeconEngine::Utils::ConvertElementTableToFormula(mobj_fit->mobj_isotope_dist.mobj_elemental_isotope_composition, elementCounts, formula);

		return mobj_fit->GetFitScore(peakData, charge, currentPeak, formula, delete_intensity_threshold, min_intensity_for_score,false) ; 
	}

}