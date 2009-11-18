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

#include "PeakIndex.h" 
#include "PeakStatistician.h"

namespace Engine
{
	namespace PeakProcessing
	{
		//! enumeration for type of fit. 
		enum PEAK_FIT_TYPE 
		{ 
			APEX = 0,  /*!< The peak is the m/z value higher than the points before and after it */ 
			QUADRATIC, /*!< The peak is the m/z value which is a quadratic fit of the three points around the apex */ 
			LORENTZIAN /*!< The peak is the m/z value which is a lorentzian fit of the three points around the apex */ 
		} ; 
		enum PEAK_PROFILE_TYPE
		{
			PROFILE = 0,
			CENTROIDED
		} ; 

		//! Used for detecting peaks in the data. 
		class PeakFit
		{
			//! Type of fit function used to find the peaks
			PEAK_FIT_TYPE menm_peak_fit_type ;
			//! Indexer to find points in the mz vectors. 
			PeakIndex<double> mobj_peak_index ; 
			//! member variable to find out information about peaks such as signal to noise and full width at half maximum.
			PeakStatistician<double, double> mobj_peak_statistician ; 

			//! Gets the peak that fits the point at a given index with a quadratic fit.
			/*!
				\param index index of the point in the m/z vectors which is the apex of the peak.
				\param mzs std::vector of raw data of m\zs. 
				\param intensities std::vector of raw data of intensities. 
				\return returns the m/z of the peak. 
			*/
			double QuadraticFit(std::vector<double>&mzs, std::vector<double> &intensities, int index);
			//! Gets the peak that fits the point at a given index with a Lorentzian fit.
			/*!
				\param index index of the point in the m/z vectors which is the apex of the peak.
				\param mzs std::vector of raw data of m\zs. 
				\param intensities std::vector of raw data of intensities. 
				\return returns the m/z of the peak. 
			*/
			double LorentzianFit(std::vector<double>&mzs, std::vector<double>&intensities, int index, double FWHM) ;
			//! Gets the peak that fits the point at a given index with a Lorentzian least square fit.
			/*!
				\param index index of the point in the m/z vectors which is the apex of the peak.
				\param mzs std::vector of raw data of m\zs. 
				\param intensities std::vector of raw data of intensities. 
				\return returns the m/z of the fit peak. 
			*/
			double LorentzianLS(std::vector<double> &mzs, std::vector<double> &intensities, double A, double FWHM, double Vo, int lstart, int lstop)  ; 

		public:
			//! Default constructor. 
			/*!
				\note By default uses Quadratic fit. 
			*/
			PeakFit(void);
			//! Destructor.
			~PeakFit(void);
			//! Sets the type of fit.
			/*!
				\param type sets the type of fit function that this instance uses. 
			*/
			void SetOptions(PEAK_FIT_TYPE type) ;
			//! Gets the peak that fits the point at a given index by the specified peak fit function.
			/*!
				\param index index of the point in the m/z vectors which is the apex of the peak.
				\param mzs std::vector of raw data of m\zs. 
				\param intensities std::vector of raw data of intensities. 
				\return returns the m/z of the peak. 
			*/
			double Fit(int index, std::vector <double> &mzs, std::vector<double>&intensities);
		};
	}
}