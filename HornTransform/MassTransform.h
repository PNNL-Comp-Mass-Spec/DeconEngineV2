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
#include "IsotopeFit.h"
#include "../ChargeDetermination/AutoCorrelationChargeDetermination.h"
#include "../TheoreticalProfile/MercuryIsotopeDistribution.h"
#include <vector> 

namespace Engine
{
	namespace HornTransform
	{
		class   MassTransform
		{
			private:
			// maximum charge to check while deisotoping
			short mshort_max_charge ; 
			// maximium MW for deisotoping
			double mdbl_max_mw ; 
			// maximum fit value to report a deisotoped peak
			double mdbl_max_fit ; 
			// minimum signal to noise for a peak to consider it for deisotoping.
			double mdbl_min_s2n ;
			// Is the medium a mixture of O16 and O18 labelled peptides. 
			bool mbln_chk_o18_pairs ;
			// This is usually set for O16/O18 labelling to get the intensity of the singly isotopically 
			// labelled O16 peak. i.e. When a peptide is labelled with O18 media, usually the mass is 
			// expected to shift by 4 Da because of replacement of two O16 atoms with two O18 atoms. 
			// However, sometimes because of incomplete replacement, only one O16 ends up getting replaced.
			// This results in isotope pairs separated by 2 Da. When this flag is set, the intensity of 
			// this +2Da pair peak is reported to adjust intensity of the O18 pair subsequent to analysis.
			bool mbln_I_report ; 
			// mass of charge carrier
			double mdbl_cc_mass ; 
			// minimum intensity of a point in the theoretical profile of a peptide for it to be 
			// considered in scoring. See Also: IsotopeFit::GetScore 
			double mdbl_min_theoretical_intensity_for_score ; 
			// After deisotoping is done, we delete the isotopic profile.  This threshold sets the 
			// value of the minimum intensity of a peak to delete. Note that ths intensity 
			// is in the theoretical profile which is scaled to where the maximum peak has an intensity of 100.
			// See Also: IsotopeFit::GetZeroingMassRange
			double mdbl_delete_intensity_threshold ; 
			// After deisotoping is performed, we delete points corresponding to the isotopic profile,
			// To do so, we move to the left and right of each isotope peak and delete points till the shoulder of the peak
			// is reached. To decide if the current point is a shoulder, we check if the next
			// (mshort_num_peaks_for_shoulder) # of points are of continuously increasing intensity.
			// See also: SetPeakToZero
			short mshort_num_peaks_for_shoulder ; 
			// When deleting an isotopic profile, this value is set to the first m/z to perform deletion at.
			double mdbl_zeroing_start_mz ; 
			// When deleting an isotopic profile, this value is set to the last m/z to perform deletion at.
			double mdbl_zeroing_stop_mz ; 
			// Check feature against charge 1 . 
			bool mbln_check_against_charge1 ; 

			//gord added
			bool mbln_isActualMonoMZUsed;
			double mdbl_leftFitStringencyFactor;
			double mdbl_rightFitStringencyFactor;
			bool mbln_sum_spectra_across_frame_range;
			int mint_num_frames_to_sum_over;

			bool mbln_UseRAPIDDeconvolution;
			


			int mint_get_fit_score_time ; 
			int mint_remaining_time ; 

			IsotopicFittingType menm_isotope_fit_type ;
			IsotopeFit *mobj_isotope_fitter ; 
			ChargeDetermination::AutoCorrelationChargeDetermine mobj_autocorrelation_dermine_cs ; 

			void SetIsotopeDistributionToZero(PeakProcessing::PeakData &pk_data, PeakProcessing::Peak &pk, 
				double monomw, short CS, bool clear_spectrum, IsotopeFitRecord &record, bool debug=false) ; 
			void SetPeakToZero(int index, std::vector<double> *intensities, std::vector<double> *mzs, bool debug = false) ; 
			void SetPeakTopToZero(PeakProcessing::PeakData &pk_data, PeakProcessing::Peak &pk) ; 
		public:
			void GetProcessingTimes(int &cs_time, int &ac_time, int &spline_time, int &isotope_dist_time, int &fit_time,
				int &remaining_time, int &get_fit_score_time, int &find_peak_calc, int &find_peak_cached)
			{
				cs_time = mobj_autocorrelation_dermine_cs.mint_total_cs_time ; 
				ac_time = mobj_autocorrelation_dermine_cs.mint_autocorrelation_time ; 
				spline_time = mobj_autocorrelation_dermine_cs.mint_spline_time ; 
				mobj_isotope_fitter->GetProcessingTime(isotope_dist_time, fit_time, find_peak_calc, find_peak_cached) ; 
				remaining_time = mint_remaining_time ; 
				get_fit_score_time = mint_get_fit_score_time ; 
			}
			bool mbln_debug ; 
			MassTransform(void);
			~MassTransform(void);
			
			void GetOptions(short &max_charge, double &max_mw, double &max_fit, double &min_s2n, double &cc_mass, 
				double &delete_threshold_intensity, double &min_theoretical_intensity_for_score, 
				short &num_peaks_for_shoulder, bool &check_fit_against_charge1, bool &use_mercury_caching, 
				bool &is_labelled_media) ; 
			void GetIsotopeFitType(IsotopicFittingType &enm_isotope_fit_type) ; 
			void GetIsotopeFitOptions(std::string &averagine_mf, std::string &tag_mf, bool &thrash_or_not, bool &complete_fit) ; 


			void SetOptions(short max_charge, double max_mw, double max_fit, double min_s2n, double cc_mass, 
				double delete_threshold_intensity, double min_theoretical_intensity_for_score, short num_peaks_for_shoulder, 
				bool check_fit_against_charge1, bool use_mercury_caching, bool o16_o18_media); 

			void SetOptions(short max_charge, double max_mw, double max_fit, double min_s2n, double cc_mass, 
				double delete_threshold_intensity, double min_theoretical_intensity_for_score, short num_peaks_for_shoulder, 
				bool check_fit_against_charge1, bool use_mercury_caching, bool o16_o18_media, 
				double leftFitStringencyFactor, double rightFitStringencyFactor, bool isActualMonoMZUsed) ; 

			void SetIsotopeFitType(IsotopicFittingType enm_isotope_fit_type) ; 
			void SetIsotopeFitOptions(std::string averagine_mf, std::string tag_mf, bool thrash_or_not, bool complete_fit) ; 
			virtual bool FindTransform(PeakProcessing::PeakData &pk_data, PeakProcessing::Peak &pk, IsotopeFitRecord &record,
				double background_intensity=0);

			void SetElementalIsotopeComposition(const TheoreticalProfile::AtomicInformation &iso_comp) ; 
			void GetElementalIsotopeComposition(TheoreticalProfile::AtomicInformation &iso_comp) ; 
			void Reset() ; 
		};
	}
}