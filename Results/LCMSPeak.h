// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include <vector>
#include <map> 

namespace Engine
{
	namespace Results
	{
		class PeakMinInfo
		{
		public:
			float mflt_mz ; 
			float mflt_intensity ; 
		} ; 

		template <class PeakClass> class LCMSPeak
		{
		public:
			PeakClass mobj_peak ; 
			int mint_scan_num ; 
			LCMSPeak <PeakClass>()
			{
				mint_scan_num = 0 ; 
			}
			LCMSPeak <PeakClass>(const LCMSPeak <PeakClass> &a)
			{
				mint_scan_num = a.mint_scan_num ; 
				mobj_peak = a.mobj_peak ; 
			}
			LCMSPeak<PeakClass>& operator=(const LCMSPeak<PeakClass> &a)
			{
				mint_scan_num = a.mint_scan_num ; 
				mobj_peak = a.mobj_peak ; 
				return *this ; 
			}

		} ; 
	}
}
