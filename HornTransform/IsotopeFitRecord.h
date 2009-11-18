#pragma once

namespace Engine
{
	namespace HornTransform
	{

		const int MAX_ISOTOPES = 16 ; 
		const int MAX_ID_LEN = 256 ; 
		//! class to store results of isotope fitting.
		class  IsotopeFitRecord
		{
		public:
			//! index of the peak in this data. 
			int mint_peak_index ; 
			//! scan number of peak
			int mint_scan_num ; 
			//! charge state 
			short mshort_cs ;
			//! intensity of feature.
			int mint_abundance ;
			//! m/z value of most abundant peak in the feature.
			double mdbl_mz ;
			//! fit value .
			double mdbl_fit ;
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
			//! indices of the peak of the isotopes in the array of peaks discovered.
			int marr_isotope_peak_indices[MAX_ISOTOPES] ; 
			//! number of isotopes
			int mint_num_isotopes_observed ; 
			//! if it is possible that the record is linked to any other distribution
			//bool mbln_flag_isotope_link ; 


			//! default constructor
			IsotopeFitRecord() ; 
			//! destructor.
			~IsotopeFitRecord() ; 
		};
	}
}