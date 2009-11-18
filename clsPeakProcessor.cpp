// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "clspeakprocessor.h"
#include "DeconEngineUtils.h"
#using <mscorlib.dll>
#include <vector>

namespace DeconToolsV2
{
	namespace Peaks
	{
		clsPeakProcessor::clsPeakProcessor(void)
		{
			mobj_peak_processor = new Engine::PeakProcessing::PeakProcessor () ; 
			mobj_parameters = new clsPeakProcessorParameters() ; 
			mobj_peak_processor->SetOptions(mobj_parameters->get_SignalToNoiseThreshold(), 0, false, (Engine::PeakProcessing::PEAK_FIT_TYPE) mobj_parameters->get_PeakFitType()) ; 
			menmProfileType = PROFILE ; 
		}

		clsPeakProcessor::~clsPeakProcessor(void)
		{
			if (mobj_peak_processor != NULL)
			{
				delete mobj_peak_processor ; 
				mobj_peak_processor = NULL ; 
			}
		}

		double clsPeakProcessor::GetBackgroundIntensity(float (&intensities) __gc [])
		{
			double thres = DeconEngine::Utils::GetAverage(intensities, FLT_MAX) ; 
			thres = DeconEngine::Utils::GetAverage(intensities, (float)(5*thres)) ;
			return thres ; 
		}

		void clsPeakProcessor::DiscoverPeaks(float (&mzs) __gc [], float (&intensities) __gc [], 
			DeconToolsV2::Peaks::clsPeak* (&peaks) __gc [], float start_mz, float stop_mz) 
		{
			std::vector<double> vectMzs ;
			std::vector<double> vectIntensities ;
			int numPoints = mzs->Length ; 
			for (int ptNum = 0 ; ptNum < numPoints ; ptNum++)
			{
				vectMzs.push_back((double)mzs[ptNum]) ; 
				vectIntensities.push_back((double)intensities[ptNum]) ; 
			}

			double backgroundIntensity = GetBackgroundIntensity(intensities) ; 
			mobj_peak_processor->SetPeakIntensityThreshold(backgroundIntensity*mobj_parameters->get_PeakBackgroundRatio()) ; 

			int numPeaks = this->mobj_peak_processor->DiscoverPeaks(&vectMzs, &vectIntensities, start_mz, stop_mz) ; 

			peaks = new clsPeak* __gc [numPeaks] ; 
			for (int pkNum = 0 ; pkNum < numPeaks ; pkNum++)
			{
				peaks[pkNum] = new clsPeak() ; 
				Engine::PeakProcessing::Peak pk ; 
				this->mobj_peak_processor->mobj_peak_data->GetPeak(pkNum, pk) ; 
				peaks[pkNum]->Set(pk) ; 
			}
			
			vectMzs.clear();
			vectIntensities.clear();

		}

		void clsPeakProcessor::SetOptions(clsPeakProcessorParameters *parameters)
		{
			mobj_parameters = parameters ; 
			// the minimum intensity is not set till the actual data is available in DiscoverPeaks
			mobj_peak_processor->SetOptions(mobj_parameters->get_SignalToNoiseThreshold(), 0, mobj_parameters->get_ThresholdedData(), (Engine::PeakProcessing::PEAK_FIT_TYPE)mobj_parameters->get_PeakFitType()) ; 
		}
	}
}
