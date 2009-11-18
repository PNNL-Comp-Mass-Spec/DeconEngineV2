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
#include "../Utilities/Helpers.h"
#include <vector>
#include <float.h>
#include <iostream>
#include <iomanip>


const double MAX_MZ = 100000 ; 
const double MIN_MZ = 392.2944 ; 
const double MAX_MASS = 10000000 ; 

namespace Engine
{
	namespace Calibrations
	{
		enum  CalibrationType	{ 
			A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C_OVERFCUBE = 0, 
			A_OVER_F_PLUS_B_OVER_FSQ, 
			A_OVER_F_PLUS_B_OVER_FSQ_PLUS_CI_OVERFSQ, 
			A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C,
			AF_PLUS_B, 
			F, 
			A_OVER_F_PLUS_B,  
			A_OVER_F_PLUS_B_PLUS_CI, 
			TIME_A_TSQ_PLUS_B_T_PLUS_C, 
			BRUKER_CALMET, 
			UNDEFINED
		} ; 
			// Type 0: m/z = A/f + B/f^2 + C/f^3
			// Type 1: m/z = A/f + |Vt|B/f^2
			// Type 2: m/z = A/f + |Vt|B/f^2 + I|Vt|C/f^2
			// Type 3: m/z = A/f + |Vt|B/f^2 + C
			// Type 4: m/z = f 
			// Type 5: m/z = A/(f+B)
			// Type 6: m/z = A/(f+B+CI)
			// Type 7: t = A*t^2 + B*t + C
			// Type 9: This is to support bruker calmet 1
			//         m/z = (-A - SQRT(A^2 - 4(B-f)C))/2(B-f)
			//         f   = A/mz + C/mz^2 +B

		class   CCalibratorImp
		{
			CalibrationType menm_calibration_type ; 

			// Normalizer for calibrator type 2.
			double mdbl_intensity_normalizer ; 

		protected:
			int mint_num_points_in_scan ; 
			double mdbl_sample_rate ; 

			double mdbl_low_mass_frequency ;
			double mdbl_frequency_shift ; // don't know what it is yet exatcly. initialize to 0.

			double mdbl_calib_const_a ; 
			double mdbl_calib_const_b ;
			double mdbl_calib_const_c ; 

			int mint_byte_order ; 


			inline void GetMassFromFreq0(double freq, double &mass)
			{
				if (freq == 0)
				{
					mass = MAX_MASS ; 
					return ; 
				}
				mass =  mdbl_calib_const_a / freq + mdbl_calib_const_b / (freq * freq) + mdbl_calib_const_c / (freq * freq * freq) ; 
				return ; 
			}

			inline void GetMassFromFreq1(double freq, double &mass)
			{
				if (freq == 0)
				{
					mass = MAX_MASS ; 
					return ; 
				}
				mass = mdbl_calib_const_a / freq + mdbl_calib_const_b / (freq * freq) ; 
				return ; 
			}

			inline void GetMassFromFreq2(double freq, double &mass)
			{
				if(freq == 0)
				{
					mass = MAX_MASS ; 
					return ; 
				}
				mass = mdbl_calib_const_a / freq + mdbl_calib_const_b / (freq * freq) + mdbl_calib_const_c * mdbl_intensity_normalizer / (freq * freq) ; 

			}
			inline void GetMassFromFreq3(double freq, double &mass)
			{
				if(freq == 0)
				{
					mass = MAX_MASS ; 
					return ;
				}
				mass = mdbl_calib_const_a / freq + mdbl_calib_const_b / (freq * freq) + mdbl_calib_const_c ; 
			}
			inline void GetMassFromFreq4(double freq, double &mass)
			{
				mass = mdbl_calib_const_a * freq + mdbl_calib_const_b ; 
			}
			inline void GetMassFromFreq5(double freq, double &mass)
			{
				mass = freq ; 
			}
			inline void GetMassFromFreq6(double freq, double &mass)
			{
				if(freq + mdbl_calib_const_b != 0)
					mass = mdbl_calib_const_a / (freq + mdbl_calib_const_b) ; 
				else if(freq - mdbl_calib_const_b <= 0)
					mass = mdbl_calib_const_a ; 
				else
					mass = 0 ;
				return ; 
			}

			inline void GetMassFromFreq7(double freq, double &mass)
			{
				if(freq == 0)
				{	
					mass = MAX_MASS ;
					return ; 
				}
				if ( freq - mdbl_calib_const_b + mdbl_calib_const_c * mdbl_intensity_normalizer < 0 ) 
				{
					mass = mdbl_calib_const_a / freq ; 
					return ; 
				}
				
				mass = mdbl_calib_const_a / (freq + mdbl_calib_const_b + mdbl_calib_const_c * mdbl_intensity_normalizer) ; 
			}

			inline void GetMassFromFreq9(double freq, double &mass)
			{
								
				if(freq != 0)
					mass = (-mdbl_calib_const_a - sqrt((mdbl_calib_const_a * mdbl_calib_const_a ) - 4 * (mdbl_calib_const_b - freq) * mdbl_calib_const_c)) / (2 * (mdbl_calib_const_b - freq)) ; 
				else
					mass = MAX_MASS ; 

				

				if (mass > 810 && mass < 813)
				{
					
					std::cout<<mdbl_calib_const_a<<"\t"<<mdbl_calib_const_b<<"\t"<<mdbl_calib_const_c<<"\t"<<std::setprecision(15)
						<<freq<<"\t"<<std::setprecision(15)<<mass<<std::endl ; }
				
			}


			double Mass2Freq0(double mass)
			{
				// Reverse the Freq2Mass equation and solve for frequency.
				mass = Helpers::absolute(mass) ; 
				double p, q, r; 
				p = -mdbl_calib_const_a / mass ; 
				q = -mdbl_calib_const_b / mass ; 
				r = -mdbl_calib_const_c / mass ; 
				return GetFrequencyFromCubicEquation0(p, q, r, mass) ;
			}
			double Mass2Freq1(double mass)
			{
				// Reverse the Freq2Mass equation and solve for frequency.
				mass = Helpers::absolute(mass) ; 
				double b, c ; 
				b = - mdbl_calib_const_a/mass ; 
				c = - mdbl_calib_const_b/mass ; 
				return GetFrequencyFromQuadraticEquation1(b, c, mass) ; 	
			}
			double Mass2Freq2(double mass)
			{
				// Reverse the Freq2Mass equation and solve for frequency.
				mass = Helpers::absolute(mass) ; 
				return GetFrequencyFromQuadraticEquation2(-1*mdbl_calib_const_a/mass, -1*(mdbl_calib_const_b + mdbl_calib_const_c * mdbl_intensity_normalizer)/mass, mass) ; 					
			}			
			double Mass2Freq3(double mass)
			{
				// Reverse the Freq2Mass equation and solve for frequency.
				double a ; 
				mass = Helpers::absolute(mass) ; 
				a = (mass - mdbl_calib_const_c) ; 
				if (a == 0)
					a = 1 ; 
				return GetFrequencyFromQuadraticEquation3(-1*mdbl_calib_const_a/a, -1*mdbl_calib_const_b/a, mass) ; 					
			}
			double Mass2Freq4(double mass)
			{
				// Reverse the Freq2Mass equation and solve for frequency.
				mass = Helpers::absolute(mass) ; 
				return (mass - mdbl_calib_const_b) / mdbl_calib_const_a ;
			}

			double Mass2Freq5(double mass)
			{
				return mass ; 
			}
			double Mass2Freq6(double mass) 
			{
				// Reverse the Freq2Mass equation and solve for frequency.
				mass = Helpers::absolute(mass) ; 
				return mdbl_calib_const_a/mass - mdbl_calib_const_b  ; 
			}
			double Mass2Freq7(double mass) 
			{
				if (mass == 0)
					mass = 1 ; 
				return mdbl_calib_const_a/mass - mdbl_calib_const_b - mdbl_calib_const_c * this->mdbl_intensity_normalizer ; 
			}	
			double Mass2Freq9(double mass) 
			{
				// Reverse the Freq2Mass equation and solve for frequency.
				mass = Helpers::absolute(mass) ; 
				if (mass != 0)
					mass = mdbl_calib_const_a / mass + mdbl_calib_const_c / (mass * mass) + mdbl_calib_const_b ; 
				return mass ; 
			}


			double GetFrequencyFromCubicEquation0(double p, double q, double r, double mass)
			{
				// Solves the equation F^3 + p F^2 + q F + r = 0  and chooses root that 
				// is closes to mass. 
				if (mass == 0)
					mass = 1 ; 

				double roots[3] ; 
				int num_roots = Helpers::CubeRoots(p, q, r, roots) ; 

				int best = 0 ; 
				double best_delta = DBL_MAX ; 
				for (int i = 0 ; i < num_roots ; i++)
				{
					double this_mass ; 
					GetMassFromFreq(roots[i], this_mass); 
					double delta = Helpers::absolute(this_mass - mass) ; 
					if (delta  < best_delta)
					{
						best_delta = delta ;
						best = i ; 
					}
				}
				return roots[best] ; 
			}

			double GetFrequencyFromQuadraticEquation1(double b, double c, double mass)
			{
				double roots[2] ; 
				Helpers::SolveQuadraticEquation(b,c, roots) ; 					
				if (roots[0] < 0 && roots[1] < 0)
					// both are negative frequencies. return MAX_MASS 
					return 0 ; 
				// otherwise return the positive one, if one of them is negative.
				if (roots[0] < 0)
					return roots[1] ; 
				if (roots[1] < 0)
					return roots[0] ; 

				// otherwise, return the one that gives a mass value closer to the one return.
				double mass1 ; 
				GetMassFromFreq1(roots[0], mass1) ; 
				double mass_delta1 = mass1 - mass ; 
				double mass2 ; 
				GetMassFromFreq1(roots[1], mass2) ; 
				double mass_delta2 = mass2 - mass ; 
				if (Helpers::absolute(mass_delta1) < Helpers::absolute(mass_delta2))
					return roots[0] ; 
				return roots[1] ; 					
			}

			double GetFrequencyFromQuadraticEquation2(double b, double c, double mass)
			{
				double roots[2] ; 
				Helpers::SolveQuadraticEquation(-1*mdbl_calib_const_a/mass, -1*(mdbl_calib_const_b + mdbl_calib_const_c * mdbl_intensity_normalizer)/mass, roots) ; 					
				if (roots[0] < 0 && roots[1] < 0)
					// both are negative frequencies. return MAX_MASS 
					return 0 ; 
				// otherwise return the positive one, if one of them is negative.
				if (roots[0] < 0)
					return roots[1] ; 
				if (roots[1] < 0)
					return roots[0] ; 

				// otherwise, return the one that gives a mass value closer to the one return.
				double mass1 ; 
				GetMassFromFreq2(roots[0], mass1) ; 
				double mass_delta1 = mass1 - mass ; 
				double mass2 ; 
				GetMassFromFreq2(roots[1], mass2) ; 
				double mass_delta2 = mass2 - mass ; 

				if (Helpers::absolute(mass_delta1) < Helpers::absolute(mass_delta2))
					return roots[0] ; 
				return roots[1] ; 					
			}

			double GetFrequencyFromQuadraticEquation3(double b, double c, double mass)
			{
				double roots[2] ; 
				Helpers::SolveQuadraticEquation(b, c, roots) ; 					
				if (roots[0] < 0 && roots[1] < 0)
					// both are negative frequencies. return MAX_MASS 
					return 0 ; 
				// otherwise return the positive one, if one of them is negative.
				if (roots[0] < 0)
					return roots[1] ; 
				if (roots[1] < 0)
					return roots[0] ; 

				// otherwise, return the one that gives a mass value closer to the one return.
				double mass1 ; 
				GetMassFromFreq(roots[0], mass1) ; 
				double mass_delta1 = mass1 - mass ; 
				double mass2 ; 
				GetMassFromFreq(roots[1], mass2) ; 
				double mass_delta2 = mass2 - mass ; 
				if (Helpers::absolute(mass_delta1) < Helpers::absolute(mass_delta2))
					return roots[0] ; 
				return roots[1] ; 					
			}


			int GetRawPointsApplyFFT5(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities, int num_pts_used)
			{
				// no fft required here.
				if (num_pts_used > mint_num_points_in_scan)
					num_pts_used = mint_num_points_in_scan ; 

				int capacity = (int)mzs->capacity() ; 
				if (capacity < num_pts_used)
				{
					mzs->reserve(num_pts_used) ; 
					intensities->reserve(num_pts_used) ; 
				}
				double ms, it ; 
				float int_float ; 
				for (int i = 0 ; i < num_pts_used ; i++)
				{
					GetMassFromFreq(i*mdbl_sample_rate, ms) ; 
					int_float = data_ptr[i] ; 
					it = (double) int_float ; 
					mzs->push_back(ms) ; 
					intensities->push_back(it) ; 
				}
				return num_pts_used ; 
			}
		public:
			CCalibratorImp(CalibrationType type) ;
			~CCalibratorImp() ;

			void SetByteOrder(int order);
			int GetByteOrder() ; 
			void SetLowMassFrequency(double low_f) ; 
			void SetSize(int sz);
			void SetSampleRate(double sw_w);
			void SetCalibrationEquationParams(double a ,double b, double c) ; 

			double GetLowMassFrequency() ; 
			int GetSize() ; 
			double GetSampleRate() ; 


			virtual CalibrationType GetCalibrationType() ;

			double TimeVal(int index) ; 
			void SetIntensityNormalizer(double i) { mdbl_intensity_normalizer = i ; } 
			double GetIntensityNormalizer() { return mdbl_intensity_normalizer ; } 

			double Mass2Freq(double mass)
			{
				if (menm_calibration_type == 0)
					return Mass2Freq0(mass) ; 
				if (menm_calibration_type == 1)
					return Mass2Freq1(mass) ; 
				if (menm_calibration_type == 2)
					return Mass2Freq2(mass) ; 
				if (menm_calibration_type == 3)
					return Mass2Freq3(mass) ; 
				if (menm_calibration_type == 4)
					return Mass2Freq4(mass) ; 
				if (menm_calibration_type == 5)
					return Mass2Freq5(mass) ; 
				if (menm_calibration_type == 6)
					return Mass2Freq6(mass) ; 
				if (menm_calibration_type == 7)
					return Mass2Freq7(mass) ; 
				if (menm_calibration_type == 9)
					return Mass2Freq9(mass) ; 
				throw "Calibration Type not Supported" ; 
				return 0 ; 
			}
			inline void GetMassFromFreq(double freq, double &mass)
			{
				if (menm_calibration_type == 1)
				{
					GetMassFromFreq1(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 0)
				{
					GetMassFromFreq0(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 2)
				{
					GetMassFromFreq2(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 3)
				{
					GetMassFromFreq3(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 4)
				{
					GetMassFromFreq4(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 5)
				{
					GetMassFromFreq5(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 6)
				{
					GetMassFromFreq6(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 7)
				{
					GetMassFromFreq7(freq, mass) ; 
					return ; 
				}
				if (menm_calibration_type == 9)
				{
					GetMassFromFreq9(freq, mass) ; 
					return ; 
				}
				throw "Calibration Type not Supported" ; 
				return ; 
			}

			inline double MZVal(int index)
			{
				// should never be called here with CalibrationType = 5.
				if (menm_calibration_type == F)
					return 0 ; 

				double mz ; 
				if (index >= this->mint_num_points_in_scan/2)
					index = mint_num_points_in_scan/2 - 1 ;
				if (this->mdbl_low_mass_frequency == 0)
				{
					GetMassFromFreq(this->mdbl_frequency_shift + index * this->mdbl_sample_rate / this->mint_num_points_in_scan, mz) ; 
					if (mz > 0)
						return mz ; 
					GetMassFromFreq((index * this->mdbl_sample_rate)/this->mint_num_points_in_scan, mz) ; 
					if (mz > 0)
						return mz ; 
					return 0 ; 
				}
				else
				{
					GetMassFromFreq(this->mdbl_low_mass_frequency - this->mdbl_frequency_shift - (index * this->mdbl_sample_rate) / this->mint_num_points_in_scan, mz) ; 
					if (mz > 0)
						return mz ; 
					return 0 ; 
				}
				return 0 ; 
			}

			int FindIndexByMass(double  val) ; 
			int GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities) ;
			int GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities, int num_pts_used) ;
			int GetRawPointsFromTransformedData(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities) ; 

		};
	}
}