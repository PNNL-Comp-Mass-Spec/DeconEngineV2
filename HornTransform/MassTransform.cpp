// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\masstransform.h"
#include "PeakFit.h" 
#include "AreaFit.h" 
#include "ChiSqFit.h" 
#include "IsotopeFit.h" 
#include <iostream>
#include <algorithm>
#include <time.h>
namespace Engine
{
	namespace HornTransform
	{
		MassTransform::MassTransform(void)
		{
			menm_isotope_fit_type = LEASTSQAREA ; 
			mobj_isotope_fitter = new AreaFit() ; 

			mshort_max_charge = 10 ; 
			mdbl_max_mw = 10000 ; 
			mdbl_max_fit = 0.15 ; 
			mdbl_min_s2n = 5 ;
			mdbl_delete_intensity_threshold = 1 ; 
			mdbl_min_theoretical_intensity_for_score = 1 ; 
			mbln_chk_o18_pairs = false ;
			mdbl_cc_mass = 1.00727638 ;   //Charge carrier mass = [atomic mass of hydrogen (1.007825) - atomic mass of an electron (0.00054858)]
			mbln_I_report = false ; 
			mbln_debug = false ; 
			mshort_num_peaks_for_shoulder = 1 ;
			mint_get_fit_score_time = 0 ; 
			mint_remaining_time = 0 ; 
			mbln_check_against_charge1 = false ; 

			mbln_isActualMonoMZUsed = false;
			mdbl_leftFitStringencyFactor = 1;
			mdbl_rightFitStringencyFactor = 1;

		}

		MassTransform::~MassTransform(void)
		{
			delete mobj_isotope_fitter ; 
		}

		void MassTransform::GetOptions(short &max_charge, double &max_mw, double &max_fit, double &min_s2n, 
			double &cc_mass, double &delete_threshold_intensity, double &min_theoretical_intensity_for_score, 
			short &num_peaks_for_shoulder, bool &check_fit_against_charge1, bool &use_mercury_caching, 
			bool &is_labelled_media)
		{
			max_charge = mshort_max_charge ; 
			max_mw = mdbl_max_mw ; 
			max_fit = mdbl_max_fit ; 
			min_s2n = mdbl_min_s2n ; 
			cc_mass = mdbl_cc_mass ; 
			delete_threshold_intensity = mdbl_delete_intensity_threshold ; 
			min_theoretical_intensity_for_score = mdbl_min_theoretical_intensity_for_score ; 
			num_peaks_for_shoulder = mshort_num_peaks_for_shoulder ; 			
			check_fit_against_charge1 = mbln_check_against_charge1 ; 
			use_mercury_caching = mobj_isotope_fitter->GetUseCaching() ; 
			is_labelled_media = mbln_chk_o18_pairs ; 
			
		}


		void MassTransform::GetIsotopeFitType(IsotopicFittingType &enm_isotope_fit_type)
		{
			enm_isotope_fit_type = menm_isotope_fit_type ; 
		}


		void MassTransform::GetIsotopeFitOptions(std::string &averagine_mf, std::string &tag_mf, bool &thrash_or_not, bool &complete_fit)
		{
			mobj_isotope_fitter->GetOptions(averagine_mf, tag_mf, thrash_or_not, complete_fit) ; 
		}


		void MassTransform::SetIsotopeFitType(IsotopicFittingType enm_isotope_fit_type)
		{
			if (enm_isotope_fit_type != menm_isotope_fit_type)
			{
				menm_isotope_fit_type = enm_isotope_fit_type ; 
				// Untill new options are provided save the current one.
				IsotopeFit *old_fit = mobj_isotope_fitter ; 
				switch (menm_isotope_fit_type)
				{
					case LEASTSQPEAK:
						mobj_isotope_fitter = new PeakFit() ; 
						// copy options
						(*mobj_isotope_fitter) = (*old_fit) ; 
						delete old_fit ; 
						break ; 
					case LEASTSQAREA:
						mobj_isotope_fitter = new AreaFit() ; 
						// copy options
						(*mobj_isotope_fitter) = (*old_fit) ; 
						delete old_fit ; 
						break ; 
					case CHISQ:
						mobj_isotope_fitter = new ChiSqFit() ; 
						// copy options
						(*mobj_isotope_fitter) = (*old_fit) ; 
						delete old_fit ; 
						break ; 
					default:
						break ; 
				}
			}
		}


		void MassTransform::SetOptions(short max_charge, double max_mw, double max_fit, double min_s2n, double cc_mass, 
			double delete_threshold_intensity, double min_theoretical_intensity_for_score, short num_peaks_for_shoulder,
			bool check_fit_against_charge1, bool use_mercury_caching, bool o16_o18_media) 			
		{
			mshort_max_charge = max_charge ; 
			mdbl_max_mw = max_mw ; 
			mdbl_max_fit = max_fit ; 
			mdbl_min_s2n = min_s2n ; 
			mdbl_cc_mass = cc_mass ;    
			mobj_isotope_fitter->SetCCMass(cc_mass) ; 
			mdbl_delete_intensity_threshold = delete_threshold_intensity ; 
			mdbl_min_theoretical_intensity_for_score = min_theoretical_intensity_for_score ; 
			mshort_num_peaks_for_shoulder = num_peaks_for_shoulder ; 
			mbln_chk_o18_pairs = o16_o18_media ; 
			mobj_isotope_fitter->SetUseCaching(use_mercury_caching) ; 
			mbln_check_against_charge1 = check_fit_against_charge1 ; 
			
		}

		//gord added
		void MassTransform::SetOptions(short max_charge, double max_mw, double max_fit, double min_s2n, double cc_mass, 
			double delete_threshold_intensity, double min_theoretical_intensity_for_score, short num_peaks_for_shoulder,
			bool check_fit_against_charge1, bool use_mercury_caching, bool o16_o18_media, 
			double leftFitStringencyFactor, double rightFitStringencyFactor, bool isActualMonoMZUsed)
		{
			mbln_isActualMonoMZUsed = isActualMonoMZUsed;
			mdbl_leftFitStringencyFactor = leftFitStringencyFactor;
			mdbl_rightFitStringencyFactor = rightFitStringencyFactor;
			
			this->SetOptions(max_charge, max_mw, max_fit, min_s2n, cc_mass, delete_threshold_intensity, 
				min_theoretical_intensity_for_score, num_peaks_for_shoulder, check_fit_against_charge1, 
				use_mercury_caching, o16_o18_media);
		}

		void MassTransform::SetIsotopeFitOptions(std::string averagine_mf, std::string tag_mf, bool thrash_or_not, bool complete_fit)
		{
			mobj_isotope_fitter->SetOptions(averagine_mf, tag_mf, mdbl_cc_mass, thrash_or_not, complete_fit) ; 
		}


		bool MassTransform::FindTransform(PeakProcessing::PeakData &pk_data, PeakProcessing::Peak &pk, IsotopeFitRecord &record, 
			double background_intensity)
		{
			IsotopeFitRecord temp_isotope_record ; 
			if (pk.mdbl_SN < mdbl_min_s2n || pk.mdbl_FWHM == 0)
			{
				return false ; 
			}
			
			double resolution = pk.mdbl_mz / pk.mdbl_FWHM ; 

			short chargeState = mobj_autocorrelation_dermine_cs.GetChargeState(pk, pk_data, mbln_debug) ; 			
			
			if (chargeState == -1 && mbln_check_against_charge1) 
			{
				chargeState = 1 ; 
			}

			if (mbln_debug)
			{
				std::cerr<<"Deisotoping :"<<pk.mdbl_mz ; 
				std::cerr<<"Charge = "<<chargeState<<std::endl ; 
			}

			if (chargeState == -1)
			{
				return false ; 
			}

			if ((pk.mdbl_mz + mdbl_cc_mass) * chargeState > mdbl_max_mw)
			{
				return false ; 
			}

			if (mbln_chk_o18_pairs)
			{
				PeakProcessing::Peak o16_peak ;
				if (pk.mdbl_FWHM < 1/chargeState)
				{
					// move back by 4 Da and see if there is a peak.
					double min_mz = pk.mdbl_mz - 4.0/chargeState - pk.mdbl_FWHM ; 
					double max_mz = pk.mdbl_mz - 4.0/chargeState + pk.mdbl_FWHM ; 
					bool found = pk_data.GetPeak(min_mz, max_mz, o16_peak) ; 
					if (found && o16_peak.mdbl_mz != pk.mdbl_mz)
					{
						// put back the current into the to be processed list of peaks. 
						pk_data.AddPeakToProcessingList(pk) ; 
						// reset pk to the right peak so that the calling function may 
						// know that the peak might have changed in the O16/O18 business
						pk = o16_peak ; 
						pk_data.RemovePeak(pk) ; 
						bool found_peak = FindTransform(pk_data, pk, record, background_intensity) ; 
						return found_peak; 
					}
				}
			}
			double best_fit = 0 ; 
			
			// Disable timing (MEM 2013)
			// clock_t start_t = clock() ; 

			PeakProcessing::Peak pkCharge1 = pk ;

			// Until now, we have been using constant theoretical delete intensity threshold..
			// instead, from now, we should use one that is proportional to intensity, for more intense peaks. 
			// However this will not solve all problems. If thrashing occurs, then the peak intensity will 
			// change when the function returns and we may not delete far enough. 
			//double delete_threshold = background_intensity / pk.mdbl_intensity * 100 ; 
			//if (background_intensity ==0 || delete_threshold > mdbl_delete_intensity_threshold)
			//	delete_threshold = mdbl_delete_intensity_threshold ; 
			double delete_threshold = mdbl_delete_intensity_threshold ; 
			best_fit = mobj_isotope_fitter->GetFitScore(pk_data, chargeState, pk, record, delete_threshold, mdbl_min_theoretical_intensity_for_score,
				mdbl_leftFitStringencyFactor,mdbl_rightFitStringencyFactor, mbln_debug) ; 

			mobj_isotope_fitter->GetZeroingMassRange(mdbl_zeroing_start_mz, mdbl_zeroing_stop_mz, record.mdbl_delta_mz, delete_threshold, mbln_debug) ;
			//best_fit = mobj_isotope_fitter->GetFitScore(pk_data, CS, pk, record, mdbl_delete_intensity_threshold, mdbl_min_theoretical_intensity_for_score, mbln_debug) ; 
			//mobj_isotope_fitter->GetZeroingMassRange(mdbl_zeroing_start_mz, mdbl_zeroing_stop_mz, record.mdbl_delta_mz, mdbl_delete_intensity_threshold, mbln_debug) ;

			if (mbln_check_against_charge1 && chargeState != 1)
			{
				IsotopeFitRecord recordCharge1 ; 
				double best_fit_charge1 = mobj_isotope_fitter->GetFitScore(pk_data, 1, pkCharge1, recordCharge1, delete_threshold, 
					mdbl_min_theoretical_intensity_for_score, mdbl_leftFitStringencyFactor,mdbl_rightFitStringencyFactor, mbln_debug) ; 
				//double best_fit_charge1 = mobj_isotope_fitter->GetFitScore(pk_data, 1, pkCharge1, recordCharge1, mdbl_delete_intensity_threshold, mdbl_min_theoretical_intensity_for_score, mbln_debug) ; 
				//mobj_isotope_fitter->GetZeroingMassRange(mdbl_zeroing_start_mz, mdbl_zeroing_stop_mz, record.mdbl_delta_mz, mdbl_delete_intensity_threshold, mbln_debug) ;
				double start_mz1 = 0 ; 
				double stop_mz1 = 0 ; 
				mobj_isotope_fitter->GetZeroingMassRange(start_mz1, stop_mz1, record.mdbl_delta_mz, delete_threshold, mbln_debug) ;
				if (best_fit > mdbl_max_fit && best_fit_charge1 < mdbl_max_fit)
				{
					best_fit = best_fit_charge1 ; 
					pk = pkCharge1 ; 
					record = recordCharge1 ; 
					mdbl_zeroing_start_mz = start_mz1 ; 
					mdbl_zeroing_stop_mz = stop_mz1 ; 
					chargeState = 1 ; 
				}
			}

			// Disable timing (MEM 2013)
			// mint_get_fit_score_time += (clock() - start_t) ; 

			if (best_fit > mdbl_max_fit)       // check if fit is good enough
				return false ; 

			if (mbln_debug)
				std::cerr<<"\tBack with fit = "<<record.mdbl_fit<<std::endl ; 

			record.mint_abundance = (int) pk.mdbl_intensity ; 
			record.mshort_cs = chargeState ; 

			PeakProcessing::Peak mono_peak ; 
			double mono_mz = record.mdbl_mono_mw/record.mshort_cs + mdbl_cc_mass ; 

			// used when mbln_I_report is true.
			PeakProcessing::Peak m3_peak ; 
			double mono_pls_two_mz = record.mdbl_mono_mw / record.mshort_cs + 2.0/record.mshort_cs  + mdbl_cc_mass ; 

			// Disable timing (MEM 2013)
			// start_t = clock() ; 
			pk_data.FindPeak(mono_mz - pk.mdbl_FWHM, mono_mz + pk.mdbl_FWHM, mono_peak) ; 
			
			pk_data.FindPeak(mono_pls_two_mz - pk.mdbl_FWHM, mono_pls_two_mz + pk.mdbl_FWHM, m3_peak) ; 
			
			// Disable timing (MEM 2013)
			// mint_remaining_time += (clock() - start_t) ; 

			record.mint_mono_intensity = (int) mono_peak.mdbl_intensity ; 
			record.mint_iplus2_intensity = (int) m3_peak.mdbl_intensity ; 

			record.mdbl_sn = pk.mdbl_SN ; 
			record.mdbl_fwhm = pk.mdbl_FWHM ; 
			record.mint_peak_index = pk.mint_peak_index ;

	
			SetIsotopeDistributionToZero(pk_data, pk, record.mdbl_mono_mw, chargeState, true, record, mbln_debug) ; 
			if (mbln_debug)
			{
				std::cerr<<"Performed deisotoping of "<<pk.mdbl_mz<<std::endl ; 
			}

		

			return true ; 
		}


		void MassTransform::SetPeakTopToZero(PeakProcessing::PeakData &pk_data, PeakProcessing::Peak &pk)
		{
			pk_data.RemovePeak(pk) ; 
		}

		void MassTransform::SetIsotopeDistributionToZero(PeakProcessing::PeakData &pk_data, PeakProcessing::Peak &pk, 
			double monomw, short CS, bool clear_spectrum, IsotopeFitRecord &record, bool debug)
		{
			std::vector<int>peak_indices ; 
			peak_indices.push_back(pk.mint_peak_index) ; 
			double mz_delta = record.mdbl_delta_mz ; 

			if (debug)
			{
				std::cerr<<"Clearing peak data for "<<pk.mdbl_mz<<" Delta = "<<mz_delta<<std::endl ;
				std::cerr<<"Zeroing range = "<<mdbl_zeroing_start_mz<<" to "<<mdbl_zeroing_stop_mz<<std::endl ;
			}

			double MaxMZ = 0 ; 
			if (mbln_chk_o18_pairs)
				MaxMZ = (monomw + 3.5) / CS + mdbl_cc_mass ; 


			int num_unprocessed_peak = pk_data.GetNumUnprocessedPeaks() ; 
			if ( num_unprocessed_peak == 0)
			{
				record.mint_num_isotopes_observed = 1 ;
				record.marr_isotope_peak_indices[0] = pk.mint_peak_index ; 
				return ; 
			}

			if (clear_spectrum)
			{
				if (debug)
					std::cerr<<"Deleting main peak :"<<pk.mdbl_mz<<std::endl ; 
				SetPeakToZero(pk.mint_data_index, pk_data.mptr_vect_intensities, pk_data.mptr_vect_mzs, debug) ; 
			}

			pk_data.RemovePeaks(pk.mdbl_mz-pk.mdbl_FWHM, pk.mdbl_mz + pk.mdbl_FWHM, debug) ; 

			if (1/(pk.mdbl_FWHM * CS) < 3)       // gord:  ??
			{
				record.mint_num_isotopes_observed = 1 ;
				record.marr_isotope_peak_indices[0] = pk.mint_peak_index ; 
				pk_data.RemovePeaks(mdbl_zeroing_start_mz, mdbl_zeroing_stop_mz, debug) ; 
				return ; 
			}

			PeakProcessing::Peak next_peak ; 

			// Delete isotopes of mzs higher than mz of starting isotope
			for (double peak = pk.mdbl_mz + 1.003/CS ; 
				!((mbln_chk_o18_pairs && peak > MaxMZ) || (peak > mdbl_zeroing_stop_mz + 2*pk.mdbl_FWHM)) ;
				peak += 1.003/CS)
			{
				if (debug)
				{
					std::cerr<<"\tFinding next peak top from "<<peak-2*pk.mdbl_FWHM<<" to "<<peak + 2*pk.mdbl_FWHM<<" pk = "<<peak<<" FWHM = "<<pk.mdbl_FWHM<<std::endl ; 

				}
				pk_data.GetPeakFromAll(peak-2*pk.mdbl_FWHM, peak+2*pk.mdbl_FWHM, next_peak) ; 

				if (next_peak.mdbl_mz == 0)
				{
					if (debug)
						std::cerr<<"\t\tNo peak found."<<std::endl ; 
					break ; 
				}
				if (debug)
				{
					std::cerr<<"\t\tFound peak to delete ="<<next_peak.mdbl_mz<<std::endl ; 
				}

				// Before assuming that the next peak is indeed an isotope, we must check for the height of this
				// isotope. If the height is greater than expected by a factor of 3, lets not delete it. 
				peak_indices.push_back(next_peak.mint_peak_index) ; 
				SetPeakToZero(next_peak.mint_data_index, pk_data.mptr_vect_intensities, pk_data.mptr_vect_mzs, debug) ; 

				pk_data.RemovePeaks(next_peak.mdbl_mz - pk.mdbl_FWHM, next_peak.mdbl_mz + pk.mdbl_FWHM, debug) ; 
				peak = next_peak.mdbl_mz ; 
			}


			// Delete isotopes of mzs lower than mz of starting isotope
			for (double peak = pk.mdbl_mz - 1.003/CS ; peak > mdbl_zeroing_start_mz - 2*pk.mdbl_FWHM ; peak -= 1.003/CS)
			{
				if (debug)
				{
					std::cerr<<"\tFinding previous peak top from "<<peak-2*pk.mdbl_FWHM<<" to "<<peak + 2*pk.mdbl_FWHM<<" pk = "<<peak<<" FWHM = "<<pk.mdbl_FWHM<<std::endl ; 
				}
				pk_data.GetPeakFromAll(peak-2*pk.mdbl_FWHM, peak+2*pk.mdbl_FWHM, next_peak) ; 
				if (next_peak.mdbl_mz == 0)
				{
					if (debug)
						std::cerr<<"\t\tNo peak found."<<std::endl ; 
					break ; 
				}
				if (debug)
				{
					std::cerr<<"\t\tFound peak to delete ="<<next_peak.mdbl_mz<<std::endl ; 
				}
				peak_indices.push_back(next_peak.mint_peak_index) ; 
				SetPeakToZero(next_peak.mint_data_index, pk_data.mptr_vect_intensities, pk_data.mptr_vect_mzs, debug) ;
				pk_data.RemovePeaks(next_peak.mdbl_mz - pk.mdbl_FWHM, next_peak.mdbl_mz + pk.mdbl_FWHM, debug) ; 
				peak = next_peak.mdbl_mz ; 
			}

			if (debug)
			{
				std::cerr<<"Done Clearing peak data for "<<pk.mdbl_mz<<std::endl ;
			}

			std::sort(peak_indices.begin(), peak_indices.end()) ; 
			// now insert into array.
			int num_peaks_observed = peak_indices.size() ;
			int num_isotopes_observed = 0 ; 
			int last_isotope_num_observed = -1 * INT_MAX ; 

			record.mint_num_isotopes_observed = peak_indices.size() ;

			for (int i = 0 ; i < num_peaks_observed ; i++)
			{
				int current_index = peak_indices[i] ; 
				PeakProcessing::Peak current_pk = pk_data.mvect_peak_tops[current_index] ; 
				int isotope_num = (int)( abs(((current_pk.mdbl_mz - pk.mdbl_mz)* CS)/1.003)+ 0.5) ; 
				if (current_pk.mdbl_mz < pk.mdbl_mz)
					isotope_num = -1 * isotope_num ; 
				if (isotope_num > last_isotope_num_observed)
				{
					last_isotope_num_observed = isotope_num ; 
					num_isotopes_observed++ ; 
					if (num_isotopes_observed > MAX_ISOTOPES)
						break ; 
					record.mint_num_isotopes_observed = num_isotopes_observed ; 
					record.marr_isotope_peak_indices[num_isotopes_observed-1] = peak_indices[i] ; 
				}
				else
				{
					record.marr_isotope_peak_indices[num_isotopes_observed-1] = peak_indices[i] ; 					
				}
			}
			if (debug)
			{
				std::cerr<<"Copied "<<record.mint_num_isotopes_observed<<" isotope peak indices into record "<<std::endl ;
			}
		}
		void MassTransform::SetPeakToZero(int index, std::vector<double> *intensities, std::vector<double> *mzs, bool debug)
		{
			double last_intensity = (*intensities)[index] ; 
			int count = -1 ; 
			double mz1, mz2 ; 

			if (debug)
				std::cerr<<"\t\tNum Peaks for Shoulder ="<<mshort_num_peaks_for_shoulder<<std::endl ; 

			for (int i = index-1 ; i >= 0 ; i--)
			{
				double this_intensity = (*intensities)[i] ;
				if (this_intensity <= last_intensity)
					count = 0 ; 
				else
				{
					count++ ; 
					mz1 = (*mzs)[i] ; 
					if (count >= mshort_num_peaks_for_shoulder)
						break ; 
				}
				(*intensities)[i] = 0 ; 

				last_intensity = this_intensity ; 
			}
			count = 0 ; 

			int num_pts = (int) intensities->size() ; 
			last_intensity = (*intensities)[index] ; 
			for (int i = index ; i < num_pts ; i++)
			{
				double this_intensity = (*intensities)[i] ;
				if (this_intensity <= last_intensity)
					count = 0 ; 
				else
				{
					count++ ; 
					mz2 = (*mzs)[i] ; 
					if (count >= mshort_num_peaks_for_shoulder)
						break ; 
				}
				(*intensities)[i] = 0 ; 
				last_intensity = this_intensity ; 
			}
			return ; 
		}

		void MassTransform::SetElementalIsotopeComposition(const TheoreticalProfile::AtomicInformation &iso_comp)
		{
			mobj_isotope_fitter->SetElementalIsotopeComposition(iso_comp) ; 
		}

		void MassTransform::GetElementalIsotopeComposition(TheoreticalProfile::AtomicInformation &iso_comp)
		{
			mobj_isotope_fitter->GetElementalIsotopeComposition(iso_comp) ; 
		}
		void MassTransform::Reset()
		{
			if (mobj_isotope_fitter != NULL)
			{
				mobj_isotope_fitter->Reset() ; 
			}
		}

	}
}