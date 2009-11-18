// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\ChiSqFit.h"
#include <time.h>

namespace Engine
{
	namespace HornTransform
	{
		ChiSqFit::ChiSqFit(void)
		{
		}

		ChiSqFit::~ChiSqFit(void)
		{
		}

		double ChiSqFit::FitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, double mz_delta, 
			double min_intensity_for_score, bool debug)
		{
			clock_t start_t = clock() ; 
			int num_points = (int)mvect_distribution_mzs.size() ; 
			if (num_points < 3)
				return 1 ; 

			double fit = 0 ; 
			double sum = 0 ; 
			for (int point_num = 0 ; point_num < num_points ; point_num++)
			{
				double mz = mvect_distribution_mzs[point_num] + mz_delta ; 
				double theoretical_intensity = mvect_distribution_intensities[point_num] ; 

				if (theoretical_intensity >= min_intensity_for_score)
				{
					// observed intensities have to be normalized so that the maximum intensity is 100, 
					// like that for theoretical intensities. 
					double observed_intensity = 100 * GetPointIntensity(mz, pk_data.mptr_vect_mzs, pk_data.mptr_vect_intensities) / pk.mdbl_intensity ; 
					double intensity_diff = (observed_intensity - theoretical_intensity) ;
					double intensity_sum = (observed_intensity + theoretical_intensity) ;
					fit += (intensity_diff * intensity_diff) / intensity_sum ; 
					sum += theoretical_intensity * observed_intensity ; 
				}
			}

			clock_t stop_t = clock() ; 
			mint_fit_processing_time += (stop_t - start_t) ; 
			return fit/(sum + 0.001) ; 
		}

		double ChiSqFit::FitScore(PeakProcessing::PeakData &pk_data, short cs, double intensity_normalizer, double mz_delta, 
			double min_intensity_for_score, bool debug)
		{
			clock_t start_t = clock() ; 

			int num_points = (int)mvect_distribution_mzs.size() ; 
			if (num_points < 3)
				return 1 ; 

			double fit = 0 ; 
			double sum = 0 ; 
			for (int point_num = 0 ; point_num < num_points ; point_num++)
			{
				double mz = mvect_distribution_mzs[point_num] + mz_delta ; 
				double theoretical_intensity = mvect_distribution_intensities[point_num] ; 

				if (theoretical_intensity >= min_intensity_for_score)
				{
					// observed intensities have to be normalized so that the maximum intensity is 100, 
					// like that for theoretical intensities. 
					double observed_intensity = 100 * GetPointIntensity(mz, pk_data.mptr_vect_mzs, pk_data.mptr_vect_intensities) / intensity_normalizer ; 
					double intensity_diff = (observed_intensity - theoretical_intensity) ;
					double intensity_sum = (observed_intensity + theoretical_intensity) ;
					fit += (intensity_diff * intensity_diff) / intensity_sum ; 
					sum += theoretical_intensity * observed_intensity ; 
				}
			}
			clock_t stop_t = clock() ; 
			mint_fit_processing_time += (stop_t - start_t) ; 

			return fit/(sum + 0.001) ; 
		}
	}
}