// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include "MercuryCache.h"
#include <float.h>

using namespace std ; 


namespace Engine
{
	namespace HornTransform
	{

		const double GAUSSIAN_PROFILE_FACTOR_VAL = 1.86 ; 
		
		inline void GetPeakAndIntensity(double mz1, double mz2, double I1, double I2, double halfFWHM, double &mz, double &I)
			{	
				double log_inv = 1/log(GAUSSIAN_PROFILE_FACTOR_VAL) ;
				mz = 0.5 * (mz1*mz1 - mz2*mz2 - halfFWHM*halfFWHM*log(I2/I1)*log_inv)/(mz1-mz2) ; 
				I = I1 * pow(GAUSSIAN_PROFILE_FACTOR_VAL, (mz1-mz) * (mz1-mz)/ (halfFWHM*halfFWHM)) ; 
			//	I = I1 * pow(2, (mz1-mz) * (mz1-mz)/ (halfFWHM*halfFWHM)) ; 
			}

		inline void GetPeakTops(std::vector<double> &mzs_dll, std::vector<double> &intensities_dll, std::vector<double> &pk_mz_vect, std::vector <double> &pk_int_vect, std::vector<int> &isotope_indices, double halfFWHM, double mz_spacing)
		{
			// Now go through each isotope, figure out its peaktop and the intensity at the peak top.
			// remember that this is nothing but a sampling issue. The peak tops might not be sampled. 

			int num_pts = (int) isotope_indices.size() ; 

			double mz ; 
			double I ; 

			for (int i = 0 ; i < num_pts ; i++)
			{
				int index = isotope_indices[i] ; 
				// for really high masses, some of the first few isotopes will not be present at all.
				if (index == -1)
					continue ; 
				double mz1 = mzs_dll[index] ; 
				double I1 = intensities_dll[index] ; 
				double mz2, I2 ; 
				
				// it might be the case that our peak has only one point and there is nothing before and after it, 
				// so we cannot get a good peak top estimate for it. If this is the case use, its mz and intensity,
				// without trying to adjust. 

				if (index < num_pts-1)
				{
					mz2 = mzs_dll[index+1] ; 
					I2 = intensities_dll[index+1] ; 
				}
				else
				{
					mz2 = mzs_dll[index-1] ; 
					I2 = intensities_dll[index-1] ; 
				}

				if (I2 > 0 && ((mz1-mz2) < 2*mz_spacing || (mz2-mz1 < 2*mz_spacing)))
				{
					GetPeakAndIntensity(mz1, mz2, I1, I2, halfFWHM, mz, I) ; 
					// also check for overflows etc by making sure the value didn't explode by a factor greater than 1.5.
					if (I > I1 * 1.5)
					{
						mz = mz1 ; 
						I = I1 ; 
					}
				}
				else
				{
					mz = mz1 ; 
					I = I1 ; 
				}

				pk_mz_vect.push_back(mz) ; 
				pk_int_vect.push_back(I) ; 
			}
		}

		inline void ExtractIsotopeIndices(std::vector<double>&mzs_dll, std::vector<double> &intensities_dll, std::vector<int> &peak_index, double mz_spacing, double threshold) 
		{
			peak_index.clear() ; 
			int num_input_points = (int) mzs_dll.size() ; 
			if (num_input_points == 0)
			{
				return ;
			}
			if (num_input_points == 1)
			{
				peak_index.push_back(0) ; 
				return ; 
			}

			// Find peaks and return their indices. In doing so, however, recognize that the input might be a bunch of 
			// separated peaks. So if there is a peak with just point point in it, there will be a discontinuous stretch on the xaxis.
			// estimate that by using two times the expected mz spacing.
			if (mzs_dll[1] - mzs_dll[0] > 2 * mz_spacing && intensities_dll[0] > threshold)
				peak_index.push_back(0) ; 


			double previous_mz_diff = 0 ; 
			double next_mz_diff = 0 ; 

			double current_mz = 0 ; 
			double previous_mz = 0 ; 
			double next_mz = 0 ; 

			double previous_intensity = 0 ;
			double current_intensity = 0 ;
			double next_intensity = 0 ; 

			next_mz = mzs_dll[1] ; 
			next_intensity = intensities_dll[1] ; 

			current_mz = mzs_dll[0] ; 
			current_intensity = intensities_dll[0] ; 

			next_mz_diff = next_mz - current_mz ; 

			for (int index = 1 ; index < num_input_points-1 ; index++)
			{
				previous_intensity = current_intensity ; 
				current_intensity = next_intensity ;
				next_intensity = intensities_dll[index+1]  ; 

				previous_mz = current_mz ; 
				current_mz = next_mz ;
				next_mz = mzs_dll[index+1]  ; 

				previous_mz_diff = next_mz_diff ;
				next_mz_diff = next_mz - current_mz ; 
				if (current_intensity >= previous_intensity && current_intensity > next_intensity)
				{
					if (current_intensity > threshold)
						peak_index.push_back(index);
				}
				else if (previous_mz_diff > 2*mz_spacing && next_mz_diff > 2*mz_spacing)
				{
					if (current_intensity > threshold)
						peak_index.push_back(index);
				}
			}	

			// now check if the last guy is a peak. 
			if (next_mz_diff > 2 * mz_spacing && next_intensity > threshold)
			{
				peak_index.push_back(num_input_points-1) ; 
			}
		}


		MercuryCache::MercuryCache(void)
		{
			mdbl_max_peak_mz = 0 ;
			mdbl_average_mw = 0 ; 
			mint_num_distributions = 0 ; 
			mint_distribution_processing_time = 0 ; 
			mint_mercury_size = 8192 ; 
			mdbl_cc_mass = 1.00727638 ; 
			mbln_use_cs_in_caching = true ; 
		}

		MercuryCache::~MercuryCache(void)
		{
			int i  = 0 ; 
		}

		void MercuryCache::FindPeak(double start_mz, double end_mz, double &mz, double &intensity)
		{
			mz = -1 ; 
			int num_isotopes = (int) mvect_isotope_dist_vals[mint_last_used_cache_position+7] ; 
			for (int i = 0 ; i < num_isotopes ; i++)
			{
				double mz_val = mvect_isotope_dist_vals[mint_last_used_cache_position+8+2*i] ; 
				if (mz_val >= start_mz && mz_val <= end_mz)
				{
					mz = mz_val ; 
					return ; 
				}
			}	
		}


		void MercuryCache::SetOptions(double cc_mass, int merc_size)
		{
			mdbl_cc_mass = cc_mass ; 
			mint_mercury_size = merc_size ; 
		}

		void MercuryCache::CacheIsotopeDistribution(double observed_most_abundant_mass, double most_abundant_mass, 
			double mono_mass, double average_mass, double most_abundant_mz, short charge, 
			double FWHM, double mass_variance, int num_pts_per_amu,  
			double min_intensity, std::vector<double> &vect_isotope_mzs, std::vector<double> &vect_isotope_intensities)
		{
			// we're going to extract the peak tops for this distribution and add them to the array of distributions. 
			int position = (int) mvect_isotope_dist_vals.size() ; 
			mmap_isotope_dist_vals_cached.insert(pair<int,int>((int)(observed_most_abundant_mass+0.5), position)) ; 
			// in the memory location we will save in the following order: 
			// most abundant mw, monoisotopic mw, average mw, mass variance, num_pts, mz1, int1, mz2, int2 .. 
			mvect_isotope_dist_vals.push_back(charge) ; 
			mvect_isotope_dist_vals.push_back(most_abundant_mass) ; 
			mvect_isotope_dist_vals.push_back(mono_mass) ; 
			mvect_isotope_dist_vals.push_back(average_mass) ; 
			mvect_isotope_dist_vals.push_back(mass_variance) ; 
			mvect_isotope_dist_vals.push_back(most_abundant_mz) ; 

			double amuperval = 1.0/num_pts_per_amu ; 

			int num_non_zero_isotopes = vect_isotope_mzs.size() ; 
			mvect_isotope_dist_vals.push_back(num_non_zero_isotopes) ; 

			for (int i = 0 ; i < num_non_zero_isotopes ; i++)
			{
				double peak_mz = vect_isotope_mzs[i] ; 
				double peak_intensity = vect_isotope_intensities[i] ; 
				mvect_isotope_dist_vals.push_back(peak_mz) ; 
				mvect_isotope_dist_vals.push_back(peak_intensity) ; 
			}
			
		}

		inline int MercuryCache::GetIsotopeDistributionCachedAtPosition(int position, short charge, 
			double FWHM, double min_theoretical_intensity, std::vector<double> &mzs, std::vector<double> &intensities)
		{
			
				mzs.clear() ; 
				intensities.clear() ; 
				// in the memory location we will save in the following order: 
				// charge, most abundant mw, monoisotopic mw, average mw, num_pts, mz1, int1, mz2, int2 .. 
				// use the FWHM that was passed. 
				double halfFWHM = FWHM/2 ; 
				mdbl_most_intense_mw = mvect_isotope_dist_vals[position+1] ; 
				mdbl_mono_mw = mvect_isotope_dist_vals[position+2]; 
				mdbl_average_mw = mvect_isotope_dist_vals[position+3] ; 
				double mass_variance = mvect_isotope_dist_vals[position+4] ; 
				mdbl_max_peak_mz = mvect_isotope_dist_vals[position+5] ;  
				int num_isotopes = (int) mvect_isotope_dist_vals[position+6] ; 


				int mass_range = (int) (sqrt(1 + mass_variance)*10.0/(float)charge);  /* +/- 5 sd's : Multiply charged */
				/* Set to nearest (upper) power of 2 */
				for (int i=1024; i>0; i/=2)
				{
					if (i < mass_range)
					{
						mass_range = i * 2 ;
						i = 0 ;   // break out of loop
					}
				}
				if (mass_range <= 0) 
					mass_range = 1 ;
				int points_per_amu = mint_mercury_size / mass_range;  /* Use maximum of 2048 real, 2048 imag points */
				double amuperpoint = 1.0/points_per_amu ; 

				int num_pts = 0 ; 
				for (int i = 0 ; i < mint_mercury_size ; i++)
				{
					double mz = amuperpoint * i  ;
					double ratio = pow(GAUSSIAN_PROFILE_FACTOR_VAL,-1*mz*mz/(halfFWHM*halfFWHM)) ; 
					if (ratio < min_theoretical_intensity /100)
						break ; 
					num_pts++ ; 
				}

				mzs.reserve(num_isotopes*2*num_pts) ; 
				intensities.reserve(num_isotopes*2*num_pts) ; 

				mint_last_used_cache_position = position ; 
				position = position + 7 ; 
				bool previous_intersected = false ; 
				double previous_intersect_mz = DBL_MAX ; 

				for (int isotope_num = 0 ; isotope_num < num_isotopes ; isotope_num++)
				{
					double mz_iso = mvect_isotope_dist_vals[position+2*isotope_num] ;
					double intensity_iso = mvect_isotope_dist_vals[position+2*isotope_num+1] ;

					double intersect_mz = DBL_MAX ; 
					if (isotope_num < num_isotopes-1)
					{
						double next_mz = mvect_isotope_dist_vals[position+2*(isotope_num+1)]; 
						double next_intensity = mvect_isotope_dist_vals[position+2*(isotope_num+1)+1] ;
						if (next_mz - num_pts * amuperpoint < mz_iso + num_pts * amuperpoint)
						{
							//intersecting, possibly. See what the m/z is till which current guy should 
							// go.
							intersect_mz = (mz_iso + next_mz)/2.0 - (halfFWHM * halfFWHM * log(next_intensity/intensity_iso))/(2*(next_mz-mz_iso)*log(GAUSSIAN_PROFILE_FACTOR_VAL)) ;
						}
					}
					int i = -num_pts ; 
					if (previous_intersected)
					{
						double mz_pt = mz_iso + i * amuperpoint ; 
						while(mz_pt < previous_intersect_mz && i <= num_pts)
						{
							i++ ; 
							mz_pt += amuperpoint ; 
						}
						previous_intersected = false ; 
					}

					for (; i <= num_pts ; i++)
					{
						double mz_diff = i * amuperpoint ; 
						double mz_pt = mz_iso + mz_diff ; 

						if (mz_pt > intersect_mz)
						{
							previous_intersected = true ; 
							previous_intersect_mz = intersect_mz ; 
							break ; 
						}
						double ratio = pow(GAUSSIAN_PROFILE_FACTOR_VAL,-1*(mz_diff/halfFWHM)*(mz_diff/halfFWHM)) ; 
						double intensity_calc = intensity_iso * ratio ; 
						if (intensity_calc > min_theoretical_intensity)
						{
							mzs.push_back(mz_pt) ; 
							intensities.push_back(intensity_calc) ; 
						}
					}
		
				}
				return 0;
		}
		bool MercuryCache::GetIsotopeDistributionCached(double observed_most_abundant_mass, short charge, double FWHM, 
			double min_theoretical_intensity, std::vector<double>&mzs, std::vector<double>&intensities)
		{
			mzs.clear() ; 
			intensities.clear() ; 

			bool found = false ; 
			int mass_val = (int)(observed_most_abundant_mass+0.5) ;   //[gord] masses are rounded off to an Int; so '0.5' is added to make sure rounding happens properly. 
			int position = -1 ; 
			
			//[gord]i think (!) the following advances to the mass_val and then if the chargeState
			//is used in caching then will find the correct chargeState and get its cached data
			for (std::multimap<int, int>::iterator iter = mmap_isotope_dist_vals_cached.find(mass_val) ;
				iter != mmap_isotope_dist_vals_cached.end() && (*iter).first == mass_val ; iter++)
			{
				position = (*iter).second ; 
				if (!mbln_use_cs_in_caching)
				{
					found = true ; 
					break ; 
				}
				else
				{
					short current_charge = (short) mvect_isotope_dist_vals[position] ; 
					if (current_charge == charge)
					{
						found = true ; 
						break ; 
					}
				}
			}
			if (found)    //found the mass value in question; now get the data
			{
				GetIsotopeDistributionCachedAtPosition(position, charge, FWHM, min_theoretical_intensity, 
					mzs, intensities) ; 
				return true ; 
			}
			return false ; 
		}
	}
}