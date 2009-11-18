// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "peakfit.h"

namespace Engine
{
	namespace PeakProcessing
	{
		PeakFit::PeakFit(void): menm_peak_fit_type (QUADRATIC)
		{
		}

		PeakFit::~PeakFit(void)
		{
		}

		void PeakFit::SetOptions(PEAK_FIT_TYPE type)
		{
			menm_peak_fit_type = type ; 
		}

		double PeakFit::Fit(int index, std::vector <double> &mzs, std::vector<double>&intensities)
		{
			if (menm_peak_fit_type == APEX)
				return mzs[index] ; 
			else if (menm_peak_fit_type == QUADRATIC)
				return this->QuadraticFit(mzs, intensities,index) ; 
			else if (menm_peak_fit_type == LORENTZIAN)
			{
				double FWHM = this->mobj_peak_statistician.FindFWHM(mzs, intensities, index);
				if (FWHM != 0)
					return this->LorentzianFit(mzs, intensities, index, FWHM) ; 
				return mzs[index] ; 
			}
			return 0.0 ; 
		}

		double PeakFit::QuadraticFit(std::vector<double>&mzs, std::vector<double> &intensities, int index)
		{
			double x1,x2,x3;
			double y1,y2,y3;
			double d;

			if (index <1)
				return mzs[0] ; 
			if (index >= (int)mzs.size()-1)
				return mzs.back() ; 

			x1 = mzs[index-1];
			x2 = mzs[index];
			x3 = mzs[index+1];
			y1 = intensities[index-1];
			y2 = intensities[index];
			y3 = intensities[index+1];

			d = (y2-y1)*(x3-x2) - (y3-y2)*(x2-x1);
			if(d==0)
				return x2;  // no good.  Just return the known peak
			d = ((x1 + x2) - ((y2 - y1) * (x3 - x2) * (x1 - x3)) / d) / 2.0;
			return d;	// Calculated new peak.  Return it.
		}

		double PeakFit::LorentzianFit(std::vector<double>&mzs, std::vector<double>&intensities, int index, double FWHM)
		{
			double A = intensities[index] ; 
			double Vo = mzs[index] ; 
			int lstart, lstop ; 

			double E, CE, le ; 

			E = (Vo - mzs[index+1])/100 ; 
			E = Helpers::absolute(E) ; 

			if (index <1)
				return mzs[index] ; 
			if (index == mzs.size())
				return mzs[index] ; 

			lstart = mobj_peak_index.GetNearest(mzs, Vo + FWHM, index) + 1 ; 
			lstop = mobj_peak_index.GetNearest(mzs, Vo - FWHM, index) - 1 ; 

			CE = LorentzianLS(mzs, intensities, A, FWHM, Vo, lstart, lstop) ; 
			for (int i = 0 ; i < 50 ; i++)
			{
				le = CE ;
				Vo = Vo + E ; 
				CE = LorentzianLS(mzs, intensities, A, FWHM, Vo, lstart, lstop) ;
				if (CE > le) 
					break ; 
			}

			Vo = Vo - E ; 
			CE = LorentzianLS(mzs, intensities, A, FWHM, Vo, lstart, lstop) ; 
			for (int i = 0 ; i < 50 ; i++)
			{
				le = CE ;
				Vo = Vo - E ; 
				CE = LorentzianLS(mzs, intensities, A, FWHM, Vo, lstart, lstop) ; 
				if (CE > le) 
					break ; 
			}
			Vo = Vo + E ; 
			return Vo ; 
		}

		double PeakFit::LorentzianLS(std::vector<double> &mzs, std::vector<double> &intensities, double A, double FWHM, double Vo, int lstart, int lstop) 
		{
			double u ; 
			double Y1, Y2 ;
			double RMSerror = 0 ; 

			for (int index = lstart ; index <= lstop ; index++)
			{
				u = 2 / FWHM * (mzs[index]-Vo) ; 
				Y1 = (int) (A / (1 + u * u)) ;
				Y2 = intensities[index] ; 
				RMSerror = RMSerror + (Y1-Y2)*(Y1-Y2) ; 
			}
			return RMSerror ;
		}
	}
}