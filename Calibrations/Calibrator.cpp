// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\calibrator.h"
#include <fstream> 
#include "../utilities/fft32.h" 

namespace Engine
{
	namespace Calibrations
	{
		void CCalibrator::Initialize(CalibrationType cal_type)
		{
			menm_calibration_type = cal_type ; 
			mobj_calibrator_imp = new CCalibratorImp(cal_type) ; 
		}
		CCalibrator::CCalibrator()
		{
			mobj_calibrator_imp = NULL ; 
			menm_calibration_type = UNDEFINED ; 
		}

		CCalibrator::CCalibrator(int cal_type)
		{
			Initialize((CalibrationType)cal_type) ; 
		}

		CCalibrator::CCalibrator(CalibrationType cal_type)
		{
			Initialize(cal_type) ; 
		}

		CCalibrator::~CCalibrator() 
		{
			delete mobj_calibrator_imp ; 
		}

		void CCalibrator::SetLowMassFrequency(double low_f)
		{
			mobj_calibrator_imp->SetLowMassFrequency(low_f) ; 
		}

		double CCalibrator::GetLowMassFrequency() 
		{
			return mobj_calibrator_imp->GetLowMassFrequency() ; 
		}

		void CCalibrator::SetSize(int sz)
		{
			mobj_calibrator_imp->SetSize(sz) ; 
		}

		int CCalibrator::GetSize()
		{
			return mobj_calibrator_imp->GetSize() ; 
		}

		void CCalibrator::SetSampleRate(double sw_w)
		{
			mobj_calibrator_imp->SetSampleRate(sw_w) ; 
		}

		double CCalibrator::GetSampleRate()
		{
			return mobj_calibrator_imp->GetSampleRate() ; 
		}

		void CCalibrator::SetByteOrder(int order)
		{
			mobj_calibrator_imp->SetByteOrder(order) ; 
		}

		int CCalibrator::GetByteOrder()
		{
			return mobj_calibrator_imp->GetByteOrder() ; 
		}

		void CCalibrator::SetCalibrationEquationParams(double a ,double b, double c) 
		{
			mobj_calibrator_imp->SetCalibrationEquationParams(a,b,c) ; 
		}
		void CCalibrator::SetIntensityNormalizer(double norm)
		{
			mobj_calibrator_imp->SetIntensityNormalizer(norm) ; 
		}

		double CCalibrator::GetIntensityNormalizer()
		{
			return mobj_calibrator_imp->GetIntensityNormalizer() ; 
		}

		CalibrationType CCalibrator::GetCalibrationType() 
		{
			return menm_calibration_type ;
		}

		double CCalibrator::TimeVal(int index) 
		{
			return mobj_calibrator_imp->TimeVal(index) ; 
		}

		double CCalibrator::Freq2Mass(double freq) 
		{
			double mass ; 
			mobj_calibrator_imp->GetMassFromFreq(freq, mass) ; 
			return mass ; 
		}

		double CCalibrator::Mass2Freq(double mass) 
		{
			return mobj_calibrator_imp->Mass2Freq(mass) ; 
		}
		int CCalibrator::FindIndexByMass(double  val) 
		{
			return mobj_calibrator_imp->FindIndexByMass(val) ; 
		}

		int CCalibrator::GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities)
		{
			return mobj_calibrator_imp->GetRawPointsApplyFFT(data_ptr, mzs, intensities) ; 
		}

		int CCalibrator::GetRawPointsApplyFFT(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities, int num_pts_used)
		{
			int current_size = mobj_calibrator_imp->GetSize() ; 
			mobj_calibrator_imp->SetSize(num_pts_used) ; 
			int num_found =  mobj_calibrator_imp->GetRawPointsApplyFFT(data_ptr, mzs, intensities, num_pts_used) ; 
			mobj_calibrator_imp->SetSize(current_size) ; 
			return num_found ; 
		}


		int CCalibrator::GetRawPointsFromTransformedData(float *data_ptr, std::vector <double> *mzs, std::vector<double> *intensities) 
		{
			return mobj_calibrator_imp->GetRawPointsFromTransformedData(data_ptr, mzs, intensities) ; 
		}
	}
}