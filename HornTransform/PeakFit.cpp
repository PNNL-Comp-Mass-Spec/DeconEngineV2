// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\PeakFit.h"
#include <time.h>

namespace Engine
{
	namespace HornTransform
	{
		PeakFit::PeakFit(void)
		{
		}

		PeakFit::~PeakFit(void)
		{
			int i = 0 ;
		}

		double PeakFit::FitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, double mz_delta, 
			double min_intensity_for_score, bool debug)
		{
			// Disable timing (MEM 2013)
			// clock_t start_t = clock() ; 
			int num_points = (int)mvect_distribution_mzs.size() ; 
			if (num_points < 3)
				return 1 ; 


			double fit = 0 ; 
			double sum = 0 ; 
			double last_yval = 0 ; 
			double diff = 0 ; 

			PeakProcessing::Peak peak_pk ; 
		
			for (int point_num = 0 ; point_num < num_points ; point_num++)
			{
				double mz = mvect_distribution_mzs[point_num] + mz_delta ; 
				double theoretical_intensity = mvect_distribution_intensities[point_num] ; 

				// observed intensities have to be normalized so that the maximum intensity is 100, 
				if (theoretical_intensity >= min_intensity_for_score && diff >= 0 && theoretical_intensity < last_yval)
				{
					bool found = false ; 
					// remember you migth be searching for the current peak (which has already been 
					// taken out of the list of peaks. So first check it.
					if (abs(pk.mdbl_mz - mz) < 2*pk.mdbl_FWHM)
					{
						found = true ; 
						peak_pk = pk ; 
					}
					else
					{
						pk_data.FindPeak(mz - pk.mdbl_FWHM, mz + pk.mdbl_FWHM, peak_pk) ; 
						if (peak_pk.mdbl_mz > 0)
							found = true ; 
					}
					if (found)
					{
						double observed_intensity = 100 * peak_pk.mdbl_intensity / pk.mdbl_intensity ; 
						double intensity_diff = (observed_intensity - theoretical_intensity) ;
						double intensity_avg = (observed_intensity + theoretical_intensity)/2 ;
						fit += intensity_diff * intensity_diff ; 
						sum += intensity_avg * intensity_avg ; 
					}
					else
					{
						fit += theoretical_intensity * theoretical_intensity ;
						sum += theoretical_intensity * theoretical_intensity ; 
					}
				}
				diff = theoretical_intensity - last_yval ; 
				last_yval = theoretical_intensity ; 
			}
			
			// Disable timing (MEM 2013)
			// clock_t stop_t = clock() ; 
			// mint_fit_processing_time += (stop_t - start_t) ; 

			return fit/(sum + 0.001) ; 
		}

		double PeakFit::FitScore(PeakProcessing::PeakData &pk_data, short cs, double intensity_normalizer, double mz_delta, 
			double min_intensity_for_score, bool debug)
		{

			throw "Don't Ever come into this FitScore overload in PeakFit" ; 
			
			// Disable timing (MEM 2013)
			// clock_t start_t = clock() ; 
			int num_points = (int)mvect_distribution_mzs.size() ; 
			if (num_points < 3)
				return 1 ; 

			double fit = 0 ; 
			double sum = 0 ; 
			double last_yval = 0 ; 
			double diff = 0 ; 

			PeakProcessing::Peak peak_pk ; 
		
			for (int point_num = 0 ; point_num < num_points ; point_num++)
			{
				double mz = mvect_distribution_mzs[point_num] + mz_delta ; 
				double theoretical_intensity = mvect_distribution_intensities[point_num] ; 

				// observed intensities have to be normalized so that the maximum intensity is 100, 
				if (theoretical_intensity >= min_intensity_for_score && diff >= 0 && theoretical_intensity < last_yval)
				{
					bool found = pk_data.GetPeak(mz - 0.1, mz + 0.1, peak_pk) ; 
					if (found)
					{
						double observed_intensity = 100 * peak_pk.mdbl_intensity / intensity_normalizer ; 
						double intensity_diff = (observed_intensity - theoretical_intensity) ;
						double intensity_sum = (observed_intensity + theoretical_intensity) ;
						fit += intensity_diff * intensity_diff ; 
						sum += theoretical_intensity * theoretical_intensity ; 
					}
					else
					{
						fit += theoretical_intensity * theoretical_intensity ;
						sum += theoretical_intensity * theoretical_intensity ; 
					}
				}
				diff = theoretical_intensity - last_yval ; 
				last_yval = theoretical_intensity ; 
			}

			// Disable timing (MEM 2013)
			// clock_t stop_t = clock() ; 
			// mint_fit_processing_time += (stop_t - start_t) ; 

			return fit/(sum + 0.001) ; 
		}

	}
}