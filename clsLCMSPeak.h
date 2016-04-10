// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#using <mscorlib.dll>
#pragma once
namespace DeconToolsV2
{
	namespace Results
	{
		public ref class clsLCMSPeak
		{
		public: 
			int mint_scan ; 
			float mflt_mz ; 
			float mflt_intensity ; 
			clsLCMSPeak(int scan, float mz, float inten)
			{
				mint_scan = scan ; 
				mflt_mz = mz ; 
				mflt_intensity = inten ; 
			}

		} ; 
	}
}