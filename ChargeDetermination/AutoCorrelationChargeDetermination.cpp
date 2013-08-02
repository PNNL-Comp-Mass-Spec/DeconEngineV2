// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "../Utilities/Helpers.h" 
#include "AutoCorrelationChargeDetermination.h"
#include <iostream>
#include <fstream>
#include <time.h> 

namespace Engine
{
	namespace ChargeDetermination
	{

		AutoCorrelationChargeDetermine::AutoCorrelationChargeDetermine(void)
		{
			//might be too high a value. 
			mshort_max_charge = 25 ; 
			mint_autocorrelation_time = 0 ; 
			mint_spline_time = 0 ; 
			mint_total_cs_time = 0 ;
		}

		AutoCorrelationChargeDetermine::~AutoCorrelationChargeDetermine(void)
		{
		}


		void AutoCorrelationChargeDetermine::ACss(std::vector<double> &Iv, std::vector<double> &Ov)
		{
			int     i,j,topIndex;
			double  sum,ave;

			int IvN = (int)Iv.size() ; 

			ave=0.0;
			for(j=0;j<IvN;j++) 
				ave += Iv[j];
			ave = ave/IvN;
			//	for(i=0;i<IvN/2;i++)  GAA 09/27/03
			for(i=0;i<IvN;i++)
			{
				sum = 0.0;
				topIndex = IvN-i-1;
				for(j=0;j<topIndex;j++)
					sum += ((Iv[j]-ave) * (Iv[j+i]-ave));

				if(j > 0)
				{
					// too much weight given to high charges this way. DJ Jan 07 2007
					Ov.push_back((float)(sum/IvN)) ;
					//Ov.push_back(sum/j) ;
				}
				else
					Ov.push_back(0) ;
			}
		}

		short AutoCorrelationChargeDetermine::GetChargeState(PeakProcessing::Peak &pk, PeakProcessing::PeakData &peak_data, bool debug)
		{
			// Disable timing (MEM 2013)
			// clock_t auto_start_t = clock() ;
			debug = false ; 

			double minMZ, maxMZ ; 
			double minus = 0.1 ; 
			double plus = 1.1 ; // right direction to look
			std::vector<double> *mzs = peak_data.mptr_vect_mzs ; 
			std::vector<double> *intensities = peak_data.mptr_vect_intensities ; 

			double FWHM = pk.mdbl_FWHM ; 

			int start_index = mobj_peak_index.GetNearest(*mzs, pk.mdbl_mz - FWHM - minus, pk.mint_data_index) ; 
			int stop_index = mobj_peak_index.GetNearest(*mzs, pk.mdbl_mz + FWHM + plus, pk.mint_data_index) ; 

			int num_pts = stop_index - start_index ; 
			int numL = num_pts ; 

			if (num_pts < 5)
				return -1 ; 

			if (num_pts < 256)
				numL = 10 * num_pts ; 

			mvect_X.clear() ; 
			mvect_Y.clear() ; 
			mvect_charge_list.clear() ; 
			mvect_Iv.clear() ; 
			mvect_autocorrelation_scores.clear() ; 

			// odd behaviour / bug in vb code here .. should have started at start_index.
			for (int i = start_index + 1 ; i <= stop_index ; i++)
			{
				double mz = (*mzs)[i] ; 
				double intensity = (*intensities)[i] ; 
				mvect_X.push_back(mz) ; 
				mvect_Y.push_back(intensity) ; 
			}

			// Disable timing (MEM 2013)
			// clock_t start_t = clock() ;
			mobj_interpolation.Spline(mvect_X, mvect_Y, 0, 0) ; 

			minMZ = mvect_X[0];
			maxMZ = mvect_X[num_pts-1];
			mvect_Iv.reserve(numL) ; 
			mvect_autocorrelation_scores.reserve(numL) ; 

			double xVal ; 
			double fVal ; 
			for(int i=0; i< numL; i++)
			{
				xVal = (minMZ + ((maxMZ-minMZ)*i) / numL) ;
				fVal = mobj_interpolation.Splint(mvect_X, mvect_Y, xVal);
				mvect_Iv.push_back(fVal) ;
			}
			// Disable timing (MEM 2013)
			// clock_t stop_t = clock() ; 
			// mint_spline_time += (stop_t - start_t) ; 

			if (debug)
			{
				std::cerr<<"mz,intensity\n" ; 
				for(int i=0; i< numL; i++)
				{
					xVal = (minMZ + ((maxMZ-minMZ)*i) / numL) ;
					fVal = mobj_interpolation.Splint(mvect_X, mvect_Y, xVal);
					std::cerr<<xVal<<","<<mvect_Iv[i]<<std::endl ; 
				}
			}

			// Disable timing (MEM 2013)
			// start_t = clock() ;

			ACss(mvect_Iv, mvect_autocorrelation_scores);
			if (debug)
			{
				int num_pts = mvect_autocorrelation_scores.size() ; 
				std::cerr<<"AutoCorrelation values\n" ; 
				for (int i = 0 ; i < num_pts ; i++)
				{
					double score = mvect_autocorrelation_scores[i] ; 
					std::cerr<<((maxMZ-minMZ)*i)/numL<<","<<score<<std::endl ; 
				}
			}

			// Disable timing (MEM 2013)
			// stop_t = clock() ; 
			// mint_autocorrelation_time += (stop_t - start_t) ; 

			int j=0;
			int minN ; 
			while(j < numL-1 && mvect_autocorrelation_scores[j] > mvect_autocorrelation_scores[j+1])
				j++;
			minN = j;

			// Determine the highest CS peak
			double best_ac_score = -1.0;
			short bestCS = -1 ;
			HighestCSPeak(minMZ, maxMZ, minN, mvect_autocorrelation_scores, mshort_max_charge, best_ac_score, bestCS);


			if(best_ac_score == -1.0)
				return -1;  // Didn't find anything

			// Now go back through the CS peaks and make a list of all CS that are
			// at least 10% of the highest
		
			GenerateCSList(minMZ, maxMZ, minN, mvect_autocorrelation_scores, mshort_max_charge, best_ac_score, mvect_charge_list);
			
			// Get the final CS value to be returned
			short tmp;
			double peakA=0.0;
			
			short returnCSVal = -1;
			if(FWHM > 0.1)
				FWHM = 0.1;

			int num_cs = (int) mvect_charge_list.size() ; 
			PeakProcessing::Peak iso_peak ; 

			for(int i = 0 ; i < num_cs ; i++)
			{
				// no point retesting previous charge. 
				tmp = mvect_charge_list[i] ;
				bool skip = false ; 
				for (int j = 0 ; j < i ; j++)
				{
					if (mvect_charge_list[j] == tmp)
					{
						skip = true ; 
						break ; 
					}
				}
				if (skip)
					continue ; 
				if(tmp > 0)
				{
					peakA = pk.mdbl_mz + (1.0 / (double) tmp);
					bool found = true ; 
					found = peak_data.GetPeakFromAllOriginalIntensity(peakA - FWHM, peakA + FWHM, iso_peak) ; 
					if (found)
					{
						returnCSVal = tmp ; 
						if (iso_peak.mdbl_mz * tmp < 3000)
							break ; 
						// if the mass is greater than 3000, lets make sure that multiple isotopes exist.
						peakA = pk.mdbl_mz - (1.03 / (double) tmp);
						found = peak_data.GetPeakFromAllOriginalIntensity(peakA - FWHM, peakA + FWHM, iso_peak) ;
						if (found)
						{
							// Disable timing (MEM 2013)
							// mint_total_cs_time += (clock() - auto_start_t) ; 
							return tmp ; 
						}
					}
					else
					{
						peakA = pk.mdbl_mz - (1.0 / (double) tmp);
						found = peak_data.GetPeakFromAllOriginalIntensity(peakA - FWHM, peakA + FWHM, iso_peak) ; 
						if (found && iso_peak.mdbl_mz * tmp < 3000)
						{
							// Disable timing (MEM 2013)
							// mint_total_cs_time += (clock() - auto_start_t) ; 
							return tmp ; 
						}
					}
				}
			}

			// Disable timing (MEM 2013)
			// mint_total_cs_time += (clock() - auto_start_t) ; 
			return returnCSVal;
		}


		void AutoCorrelationChargeDetermine::GenerateCSList(double minMZ, double maxMZ, int minN, 
				std::vector<double> &vect_autocorrelation_scores, double maxCS, double best_ac_score, 
				std::vector<short> &CSList)
		{
			bool goingUp, wasGoingUp;
			short CS = -1 ; 
			// Preparation...
			wasGoingUp = false;
			goingUp = false;

			int num_pts = (int) vect_autocorrelation_scores.size() ; 
			// First determine the highest CS peak...
			for(int i=minN; i<num_pts; i++)
			{
				if (i < 2)
					continue ; 
				if (vect_autocorrelation_scores[i] > vect_autocorrelation_scores[i-1])
					goingUp = true ; 
				else
					goingUp = false ;

				if(wasGoingUp && !goingUp)
				{
					double dbl_cs = num_pts / ((maxMZ-minMZ)*(i-1)) ;
					double current_auto_cor_score = vect_autocorrelation_scores[i-1] ; 
					if((current_auto_cor_score > best_ac_score*0.1) && (dbl_cs <= 1.0 * maxCS))
					{
						// remvect_autocorrelation_scoresed condition of absolution. 
						CS = (short) (0.5 + dbl_cs) ; 
						CSList.push_back(CS);
					}
				}
				wasGoingUp = goingUp;
			}	
			return;
		}

		void AutoCorrelationChargeDetermine::HighestCSPeak(double minMZ, double maxMZ, int minN, 
				std::vector<double> &vect_autocorrelation_scores, short maxCS, double &best_ac_score, 
				short &bestCS)
		{
			bool goingUp, wasGoingUp;
			int num_pts = (int) vect_autocorrelation_scores.size() ; 

			// Preparation...
			wasGoingUp = false;
			goingUp = false;

			short CS ; 
			// First determine the highest CS peak...
			for(int i=minN; i<num_pts; i++)
			{
				if (i < 2)
					continue ; 
				(vect_autocorrelation_scores[i] - vect_autocorrelation_scores[i-1] > 0) ? goingUp = true : goingUp = false;
				if(wasGoingUp && !goingUp)
				{
					CS = (short) (0.5 + num_pts / ((maxMZ-minMZ)*(i-1)));
					double current_auto_cor_score = vect_autocorrelation_scores[i-1] ; 
					if((Helpers::absolute(current_auto_cor_score/vect_autocorrelation_scores[0]) > 0.05) && CS <= maxCS)
					{
						if(Helpers::absolute(current_auto_cor_score) > best_ac_score)
						{
							best_ac_score = Helpers::absolute(current_auto_cor_score);
							bestCS = CS;
						}
					}
				}
				wasGoingUp = goingUp;
			}	
			return;
		}
	}
}