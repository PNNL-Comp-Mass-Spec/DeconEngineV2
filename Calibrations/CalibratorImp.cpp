// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\calibratorimp.h"
#include <fstream> 
#include "../utilities/fft32.h" 
#include <float.h> 
#include <math.h>

namespace Engine
{
	namespace Calibrations
	{

		CCalibratorImp::CCalibratorImp(CalibrationType type) 
		{
			mdbl_frequency_shift = 0 ; 
			mdbl_low_mass_frequency = 0 ; 
			mdbl_calib_const_a = mdbl_calib_const_b = mdbl_calib_const_c = 0 ;
			mdbl_intensity_normalizer = 1 ; 
			menm_calibration_type = type ; 
		} ;
		CCalibratorImp::~CCalibratorImp() 
		{
		};

		CalibrationType CCalibratorImp::GetCalibrationType() 
		{ 
			return menm_calibration_type ; 
		} 

		void CCalibratorImp::SetByteOrder(int order)
		{
			mint_byte_order = order ; 
		}
		int CCalibratorImp::GetByteOrder()
		{
			return mint_byte_order ; 
		}
		double CCalibratorImp::TimeVal(int index)
		{
			if (this->mint_num_points_in_scan-1 < index)
				return -1 ; 

			return index / this->mdbl_sample_rate ;
		}


		int CCalibratorImp::GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities) 
		{
			int num_pts = GetRawPointsApplyFFT(data_ptr, mzs, intensities, this->mint_num_points_in_scan) ; 
			return num_pts ; 
		}

		int CCalibratorImp::GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities, int num_pts_used) 
		{
			if (menm_calibration_type == 5)
				return GetRawPointsApplyFFT5(data_ptr, mzs, intensities, num_pts_used) ; 

			mzs->clear() ; 
			intensities->clear() ; 


			for (int i = 0 ; i < num_pts_used/2 ; i++)
			{
				double point1 = data_ptr[2*i];
				double point2 = data_ptr[2*i+1];
			}

			if ((int)mzs->capacity() < num_pts_used/2)
				mzs->reserve(num_pts_used/2) ; 

			if ((int)intensities->capacity() < num_pts_used/2)
				intensities->reserve(num_pts_used/2) ; 

			int sign = 1 ; 
			int result = FFT::realft(num_pts_used, data_ptr, sign) ; 

			// intensities need to be put in increasing order of mz. But here they are in 
			// descending order. So insert and then reverse.  
			//UPDATE:  [gord]   Bruker Solarix mz data is not in descending order!  so must check this.


			int numIncreasingValuesOutOfFirst100=0; 
			int numDecreasingValuesOutOfFirst100=0; 
			
			double previousMZValue = 0;

			for (int i = 0 ; i < num_pts_used/2 ; i++)
			{

				double mz_val = MZVal(i) ; 
				if (i >1 && i < 100)    // checking the first 100 points of the loop will give an idea if mz's are increasing or decreasing
				{
					if (mz_val >previousMZValue)
					{
						numIncreasingValuesOutOfFirst100++;
					}
					else if (mz_val < previousMZValue)
					{
						numDecreasingValuesOutOfFirst100++;
					}
				}

				double point1 = data_ptr[2*i];
				double point2 = data_ptr[2*i+1];



				//double intensity_val = sqrt(data_ptr[2*i+1] * data_ptr[2*i+1] + data_ptr[2*i] * data_ptr[2*i]) ; 
				double intensity_val = sqrt(point2 * point2 + point1 * point1) ; 

				if (mz_val == 0 )
					continue ;
				mzs->push_back(mz_val) ; 

				intensities->push_back(intensity_val) ; 
				previousMZValue = mz_val;
			}

			int num_pts = (int)intensities->size() ; 
			double temp_intensity ; 
			double temp_mz ; 

			bool mzValuesAreDecreasing = (numDecreasingValuesOutOfFirst100 > 0);
			if (mzValuesAreDecreasing)   
			{
				//reverses the array
				for (int i = 0 ; i < num_pts/2 ; i++)
				{
					temp_intensity = (*intensities)[i] ; 
					(*intensities)[i] = (*intensities)[num_pts-1-i] ; 
					(*intensities)[num_pts-1-i] = temp_intensity ; 

					temp_mz = (*mzs)[i] ;
					(*mzs)[i] = (*mzs)[num_pts-1-i]; 
					(*mzs)[num_pts-1-i] = temp_mz ;
				}
			}
			
			int orig_num_pts = num_pts ; 
			while(num_pts > 1 && (*mzs)[num_pts-1] < (*mzs)[num_pts-2])
			{
				num_pts-- ; 
			}
			double diff = (*mzs)[num_pts-1] - (*mzs)[num_pts-2] ; 
			while(num_pts < orig_num_pts)
			{
				(*mzs)[num_pts-1] = (*mzs)[num_pts-2] + diff ; 
				num_pts++ ; 
			}
			return num_pts ; 
		}

		void CCalibratorImp::SetLowMassFrequency(double low_f)
		{
			this->mdbl_low_mass_frequency = low_f ; 
		}

		void CCalibratorImp::SetSize(int sz)
		{
			this->mint_num_points_in_scan = sz ; 
		}

		void CCalibratorImp::SetSampleRate(double srate)
		{
			this->mdbl_sample_rate = srate ; 
		}

		double CCalibratorImp::GetLowMassFrequency()
		{
			return this->mdbl_low_mass_frequency ; 
		}

		int CCalibratorImp::GetSize()
		{
			return this->mint_num_points_in_scan ; 
		}

		double CCalibratorImp::GetSampleRate()
		{
			return this->mdbl_sample_rate ; 
		}


		void CCalibratorImp::SetCalibrationEquationParams(double a ,double b, double c)
		{
			mdbl_calib_const_a = a ;
			mdbl_calib_const_b = b ;
			mdbl_calib_const_c = c ;
		}

		int CCalibratorImp::GetRawPointsFromTransformedData(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities)
		{
			mzs->clear() ; 
			intensities->clear() ; 

			if ( (int)mzs->capacity() < this->mint_num_points_in_scan/2)
				mzs->reserve(this->mint_num_points_in_scan/2) ; 

			if ((int)intensities->capacity() < this->mint_num_points_in_scan)
				intensities->reserve(this->mint_num_points_in_scan/2) ; 

			double last_mz = DBL_MIN ; 
			for (int i = 0 ; i < this->mint_num_points_in_scan/2 ; i++)
			{
				double mz_val = MZVal(i) ;
				if (mz_val < last_mz)
				{
					// probably went over the edge of the frequency domain. Stop Here.
					return i; 
				}
				double intensity_val = sqrt(data_ptr[2*i] * data_ptr[2*i] + data_ptr[2*i+1] * data_ptr[2*i+1]) ; 
				mzs->push_back(mz_val) ; 
				intensities->push_back(intensity_val) ; 
				last_mz = mz_val ; 
			}
			return this->mint_num_points_in_scan/2 ; 
		}

		int CCalibratorImp::FindIndexByMass(double  val)
		{
			int index ; 
			if (this->mdbl_low_mass_frequency == 0)
			{
				double freq = Mass2Freq(val) ; 
				index = (int) (((freq - this->mdbl_frequency_shift) / this->mdbl_sample_rate) * this->mint_num_points_in_scan) ; 
			}
			else
			{
				double freq = Mass2Freq(val) ; 
				if (freq > this->mdbl_low_mass_frequency)
					freq = this->mdbl_low_mass_frequency ;
				if (freq < this->mdbl_low_mass_frequency - this->mdbl_sample_rate / 2)
					freq = this->mdbl_low_mass_frequency - this->mdbl_sample_rate / 2 ; 
				index = (int) (((this->mdbl_low_mass_frequency - freq - this->mdbl_frequency_shift) / this->mdbl_sample_rate) * this->mint_num_points_in_scan) ;
			}
			if (index > this->mint_num_points_in_scan / 2) 
				index = this->mint_num_points_in_scan / 2 ; 
			if (index < 0)
				index = 0 ;
			// since the order of  things is reversed.
			index = this->mint_num_points_in_scan/2 - index  ; 
			return index ; 
		}




	}
}