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
#include "PeakProcessor/Peak.h"

namespace DeconToolsV2
{
	namespace Peaks
	{
		public ref class clsPeak
		{
			public:
			//! mz of the peak.
			double mdbl_mz ; 
			//!  intensity of peak.
			double mdbl_intensity ;
			//! Signal to noise ratio
			double mdbl_SN ; 
			//! index in PeakData::mvect_peak_tops std::vector. 
			int mint_peak_index ;
			//! index in mzs, intensity vectors that were used to create the peaks in PeakProcessor::DiscoverPeaks.
			int mint_data_index ;
			//! Full width at half maximum for peak.
			double mdbl_FWHM ;
			clsPeak(void);
			~clsPeak(void);
			void Set(Engine::PeakProcessing::Peak &pk) ;
		};
	}
}
