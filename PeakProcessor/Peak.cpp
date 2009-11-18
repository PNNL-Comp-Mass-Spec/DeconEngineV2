// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "peak.h"

namespace Engine
{
	namespace PeakProcessing
	{
			Peak::Peak(void) 
			{ 
				mdbl_mz = 0 ; 
				mdbl_intensity = 0 ; 
				mdbl_SN = 0 ; 
				mint_peak_index = -1 ; 
				mint_data_index = -1 ; 
				mdbl_FWHM = 0 ; 
			}

			Peak::Peak(const Peak &a)
			{
				mdbl_mz = a.mdbl_mz ; 
				mdbl_intensity = a.mdbl_intensity ; 
				mdbl_SN = a.mdbl_SN ; 
				mint_peak_index = a.mint_peak_index ; 
				mint_data_index = a.mint_data_index ; 
				mdbl_FWHM = a.mdbl_FWHM ; 
			}

			Peak& Peak::operator=(const Peak &a)
			{
				mdbl_mz = a.mdbl_mz ; 
				mdbl_intensity = a.mdbl_intensity ; 
				mdbl_SN = a.mdbl_SN ; 
				mint_peak_index = a.mint_peak_index ; 
				mint_data_index = a.mint_data_index ;
				mdbl_FWHM = a.mdbl_FWHM ; 
				return *this ; 
			}

			void Peak::Set(double mz, double intensity, double signal2noise, int peak_idx, int data_idx, double fwhm)
			{
				mdbl_mz = mz ; 
				mdbl_intensity = intensity ; 
				mdbl_SN = signal2noise ; 
				mint_peak_index = peak_idx ; 
				mint_data_index = data_idx ; 
				mdbl_FWHM = fwhm ; 
			}
			Peak::~Peak(void) {} 

			// Used by the stl algorithm sort to sort std::vector of peaks in descending order of mdbl_intensity.
			bool PeakIntensityComparison(Peak &pk1, Peak &pk2)
			{
				if (pk1.mdbl_intensity > pk2.mdbl_intensity)
					return true ; 
				if (pk1.mdbl_intensity < pk2.mdbl_intensity)
					return false ; 
				return pk1.mdbl_mz > pk2.mdbl_mz ; 
			}
			std::ostream& operator<<(std::ostream &out, const Peak &a)
			{
				out<<a.mdbl_mz<<" "<<a.mdbl_intensity<<" "<<a.mdbl_FWHM<<" "<<a.mdbl_SN<<" "<<a.mint_data_index<<" "<<a.mint_peak_index<<std::endl ; 
				return out ; 
			}
	}
}