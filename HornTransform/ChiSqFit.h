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
#include "IsotopeFit.h" 

namespace Engine
{
	namespace HornTransform
	{	
		class  ChiSqFit : public IsotopeFit
		{
		public:
			ChiSqFit(void);
			~ChiSqFit(void);
			double FitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, double mz_delta, 
				double min_intensity_for_score, bool debug = false); 
			double FitScore(PeakProcessing::PeakData &pk_data, short cs, double normalizer, double mz_delta, 
				double min_intensity_for_score, bool debug = false); 
		};
	}
}