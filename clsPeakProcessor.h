// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#using <mscorlib.dll>
#include "clsPeak.h"
#include "PeakProcessor/PeakProcessor.h"
#include "clsPeakProcessorParameters.h" 

namespace DeconToolsV2
{
	public __value enum enmProfileType {CENTROIDED = 0, PROFILE}; 

	namespace Peaks
	{
		public __gc class clsPeakProcessor
		{
			enmProfileType menmProfileType ; 
			clsPeakProcessorParameters *mobj_parameters ; 
			Engine::PeakProcessing::PeakProcessor __nogc *mobj_peak_processor ;
		public:
			clsPeakProcessor(void);
			~clsPeakProcessor(void);
			double GetBackgroundIntensity(float (&intensities) __gc []) ; 
			void DiscoverPeaks(float (&mzs) __gc [], float (&intensities) __gc [], DeconToolsV2::Peaks::clsPeak* (&peaks) __gc [], 
				float start_mz, float stop_mz) ;
			void SetOptions(clsPeakProcessorParameters *parameters) ;
			__property enmProfileType get_ProfileType()
			{
				return  menmProfileType ; 
			}

			__property void set_ProfileType(enmProfileType type)
			{
				menmProfileType = type ; 
			}
		};
	}
}
