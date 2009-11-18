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
#include <vector>
#include "CalibratorImp.h" 

namespace Engine
{
	namespace Calibrations
	{

		enum CalibrationType ; 
		class CCalibratorImp ; 
		class   CCalibrator
		{
		protected:
			CCalibratorImp *mobj_calibrator_imp ; 
			CalibrationType menm_calibration_type ; 

		public:
			CCalibrator() ; 
			CCalibrator(CalibrationType cal_type) ;
			CCalibrator(int cal_type) ;
			~CCalibrator() ;

			void Initialize(CalibrationType cal_type) ; 
			void SetLowMassFrequency(double low_f) ; 
			double GetLowMassFrequency() ; 
			void SetSize(int sz);
			int GetSize() ; 
			void SetSampleRate(double sw_w);
			double GetSampleRate() ; 
			void SetCalibrationEquationParams(double a ,double b, double c) ; 
			void SetIntensityNormalizer(double i) ; 
			double GetIntensityNormalizer() ; 
			void SetByteOrder(int order) ; 
			int GetByteOrder() ; 

			CalibrationType GetCalibrationType() ; 
			double TimeVal(int index) ;
			double Freq2Mass(double freq) ;
			double Mass2Freq(double mass) ;
			inline double MZVal(int index) 
			{
				return mobj_calibrator_imp->MZVal(index) ; 
			}
			int FindIndexByMass(double  val) ;
			int GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities) ;
			int GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector <double> *intensities, int num_pts_used) ;
			int GetRawPointsFromTransformedData(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities) ; 
			
		};
	}
}