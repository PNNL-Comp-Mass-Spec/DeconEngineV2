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
#include "HornTransform/IsotopeFitRecord.h" 
namespace DeconToolsV2
{
	namespace HornTransform
	{
		public __gc class clsHornTransformResults
		{
		public:
			//! peak index of the peak.
			int mint_peak_index ; 
			//! scan number of peak
			int mint_scan_num ; 
			//! charge state 
			short mshort_cs ;
			//! intensity of feature; maximum value that can be represented is 2147483648
			int mint_abundance;
			//! intensity of feature (as a double)
			double mdbl_abundance;
			//! m/z value of most abundant peak in the feature.
			double mdbl_mz ;
			//! fit value .
			double mdbl_fit ;
			//! Number of data points used to compute the fit value
			int mint_fit_count_basis;
			//! average mw for the feature. 
			double mdbl_average_mw ;
			//! monoisotopic mw of feature.
			double mdbl_mono_mw ;
			//! mw at the most intense isotope.
			double mdbl_most_intense_mw ; 
			//! full width at half maximum of the peak.
			double mdbl_fwhm ; 
			//! signal to noise for the most intense isotopic peak.
			double mdbl_sn ; 
			//! intensity of monoisotopic peak observed.
			int mint_mono_intensity  ;
			//! intensity of the third isotopic peak observed. Used by other software for processing of O16/O18  data.
			int mint_iplus2_intensity  ;
			//! difference between obsered m/z and m/z from theoretical distribution of composition from Averagine
			double mdbl_delta_mz ; 
			//! need multiple isotopes to determine charge
			bool mbln_need_multiple_isotopes ; 
			//! number of isotope peaks
			int mint_num_isotopes_observed ; 
			//! array of indices of peak tops
			int marr_isotope_peak_indices __gc [] ; 

		public:
			clsHornTransformResults(void);
			~clsHornTransformResults(void);
			virtual Object* Clone()
			{
				clsHornTransformResults *result = new clsHornTransformResults() ; 
				result->mint_peak_index = this->mint_peak_index ; 
				result->mint_scan_num = this->mint_scan_num ; 
				result->mshort_cs = this->mshort_cs ;
				result->mint_abundance = this->mint_abundance ;
				result->mdbl_abundance = this->mdbl_abundance;
				result->mdbl_mz = this->mdbl_mz ;
				result->mdbl_fit = this->mdbl_fit ;
				result->mint_fit_count_basis = this->mint_fit_count_basis ;
				result->mdbl_average_mw = this->mdbl_average_mw ;
				result->mdbl_mono_mw = this->mdbl_mono_mw ;
				result->mdbl_most_intense_mw = this->mdbl_most_intense_mw ; 
				result->mdbl_fwhm = this->mdbl_fwhm ; 
				result->mdbl_sn = this->mdbl_sn ; 
				result->mint_mono_intensity = this->mint_mono_intensity ;
				result->mint_iplus2_intensity = this->mint_iplus2_intensity ;
				result->mdbl_delta_mz = this->mdbl_delta_mz ; 
				result->mint_num_isotopes_observed = this->mint_num_isotopes_observed ; 
				result->marr_isotope_peak_indices = new int __gc [mint_num_isotopes_observed] ; 
				for (int i = 0 ; i < mint_num_isotopes_observed ; i++)
				{
					result->marr_isotope_peak_indices[i] = this->marr_isotope_peak_indices[i] ; 
				}
				return result ; 
			}

			void Set(Engine::HornTransform::IsotopeFitRecord &fitRecord)
			{
				mint_peak_index = fitRecord.mint_peak_index ; 
				mint_scan_num = fitRecord.mint_scan_num ; 
				mshort_cs = fitRecord.mshort_cs ;
				mint_abundance = fitRecord.mint_abundance ;
				mdbl_abundance = fitRecord.mdbl_abundance;
				mdbl_mz = fitRecord.mdbl_mz ;
				mdbl_fit = fitRecord.mdbl_fit ;
				mint_fit_count_basis = fitRecord.mint_fit_count_basis ;
				mdbl_average_mw = fitRecord.mdbl_average_mw ;
				mdbl_mono_mw = fitRecord.mdbl_mono_mw ;
				mdbl_most_intense_mw = fitRecord.mdbl_most_intense_mw ; 
				mdbl_fwhm = fitRecord.mdbl_fwhm ; 
				mdbl_sn = fitRecord.mdbl_sn ; 
				mint_mono_intensity = fitRecord.mint_mono_intensity ;
				mint_iplus2_intensity = fitRecord.mint_iplus2_intensity ;
				mdbl_delta_mz = fitRecord.mdbl_delta_mz ; 
				mint_num_isotopes_observed = fitRecord.mint_num_isotopes_observed ; 
				marr_isotope_peak_indices = new int __gc [mint_num_isotopes_observed] ; 
				for (int i = 0 ; i < mint_num_isotopes_observed ; i++)
				{
					marr_isotope_peak_indices[i] = fitRecord.marr_isotope_peak_indices[i] ; 
				}
			}
		};
	}
}
