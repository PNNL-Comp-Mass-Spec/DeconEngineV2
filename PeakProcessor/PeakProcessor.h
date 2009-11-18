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
#include <list> 
#include <map> 
#include <float.h>
#include "PeakData.h"
#include "PeakFit.h" 


namespace Engine
{
	namespace PeakProcessing
	{
		//! class that does the processing of peaks.  
		class PeakProcessor
		{
			//! helps searching for specific values in any sorted std::vector. 
			PeakIndex <double> mobj_pk_index ; 
			//! tolerance width used while searching in attention list in the peaks. 
			double mdbl_mz_tolerance_attention_list ; 
			//! flag to check attention list for peaks that we want to necessarily look for.
			bool mbln_chk_attention_list ; 
			//! map of attention list of m/z's. 
			/*!
				\remark This was implemented as a map because it would allow for direct sorted lookup for an m/z value.
			*/
			#pragma warning(disable: 4251)
			std::map<double, int> mmap_attention_list ; 
			#pragma warning(default: 4251)


			//! background intensity. When user sets min signal to noise, and min intensity, 
			// this value is set as min intensity / min signal to noise. 
			double mdbl_background_intensity ; 

			//! minimum intensity for a point to be considered a peak. 
			double mdbl_peak_intensity_threshold ;
			//! signal to noise threshold for a Peak to be considered as a peak.
			double mdbl_signal_2_noise_threshold ; 
			//! fit type used in the PeakFit class to detect peaks in the raw data object. 
			PEAK_FIT_TYPE menm_peak_fit_type  ; 
			PEAK_PROFILE_TYPE menm_profile_type;

			//! This variable helps find the m/z value of a peak using the specified fit function. 
			PeakFit mobj_peak_fit ; 

			//! this function loads in the attention list from a file.
			/*!
				\param file_name is the file from which to load the m/z's that are in the attention list. 
			*/
			void LoadAttentionList(char *file_name) ; 
			//! tests if a specific m/z value is in the attention list.
			/*!
				\param mz_val is the m/z value we are looking for. 
				\return returns true if mz_val is found in attention list, and false otherwise.
				\note The tolerance used in searching is specified in PeakProcessor::mdbl_mz_tolerance_attention_list
			*/
			bool IsInAttentionList(double mz_val);

			//! Is the data centroided ? 
			bool mbln_centroided_data ; 
			// if the data is thresholded, the ratio is taken as the ratio to background intensity.
			bool mbln_thresholded_data ; 
		public:
			//! pointer to PeakData instance that stores the peaks found by an instance of this PeakProcessor.
			PeakData *mobj_peak_data ; 
			//! pointer to PeakStatistician instance that is used to calculate signal to noise and full width at half maximum for the peaks in the raw data. 
			PeakStatistician<double, double> mobj_peak_statistician ; 
			//! sets the threshold for signal to noise for a peak to be considered as real.
			/*!
				\param s2n is the signal to noise threshold value.
				\remarks For a peak to be considered real it has to pass two criterias:
				- Its signal to noise must be greater than the threshold (PeakProcessor::mdbl_signal_2_noise_threshold)
				- Its intensity needs to be greater than the threshold (PeakProcessor::mdbl_peak_intensity_threshold)
			*/
			void SetSignal2NoiseThreshold(double s2n) ; 			
			//! sets the threshold intensity for a peak to be considered a peak. 
			/*!
				\param threshold is the threshold peak intensity.
				\note If threshold is less than zero, then the Helpers::absolute value of the threshold is used as the cutoff intensity.
				However, if threshold is greater than equal to zero, otherwise it is proportional to threshold * background intensity in scan. 
				\remarks For a peak to be considered real it has to pass two criterias:
				\arg \c Its signal to noise must be greater than the threshold (PeakProcessor::mdbl_signal_2_noise_threshold)
				\arg \c Its intensity needs to be greater than the threshold (PeakProcessor::mdbl_peak_intensity_threshold)
				\sa PeakProcessor::CalculatePeakIntensityThreshold
			*/
			void SetPeakIntensityThreshold(double threshold) ; 
			//! sets the m/z tolerance used in searching for attention list.
			/*!
				\param mz_tol specifies the m/z tolerance used.
			*/
			void SetAttentionListTolerance(double mz_tol) ; 
			//! sets the type of peak fitting used to find m/z values for peaks. 
			/*!
				\param type specifies the type of peak fitting. 
			*/
			void SetPeakFitType(PEAK_FIT_TYPE type) ; 
			//! sets the type of profile 
			/*! 
				\param profile is  a boolean, tru if profile data, false if centroided 
			*/
			void SetPeaksProfileType(bool profile);
			//! sets the flag for searching for m/z values in attention list in the raw data vectors.
			/*!
				\param set true/false value sets/unsets the flag to search for element in the m/z attention list. 
			*/
			void SetCheckAttentionList(bool set) ; 
			//! sets the options for this instance.
			/*!
				\param s2n sets the threshold signal to noise value.
				\param thresh sets the peak intensity threshold.				
				\param type sets the type of peak fitting algorithm used.
			*/
			void SetOptions(double s2n, double thresh, bool thresholded, PEAK_FIT_TYPE type) ; 
			//! default constructure.
			PeakProcessor();
			//! virtual destructor
			virtual ~PeakProcessor();
			//! clears the PeakData member variable PeakProcessor::mobj_peak_data and  PeakProcessor::mmap_attention_list
			void Clear() ; 
			//! Function discovers peaks in the m/z and intensity vectors supplied. 
			/*!
				\param vect_mz is the pointer to std::vector of m/z values 
				\param vect_intensity is the pointer to std::vector of intensity values 
				\return returns the number of peaks that were found in the vectors. 
				\remarks The function uses PeakStatistician::FindFWHM, and PeakStatistician::FindSignalToNoise functions 
				to discover the full width at half maximum and signal to noise values for a peak. The signal to noise of a 
				peak is tested against the threshold value before its accepted as a peak. All peaks are used during the process, 
				but once generated only those which are above mdbl_peak_intensity_threshold are tested for peptidicity by Deconvolution::HornMassTransform
			*/
			int DiscoverPeaks (std::vector<double> *vect_mz, std::vector<double> *vect_intensity) ;
			//! Function discovers peaks in the m/z and intensity vectors supplied within the supplied m/z window. 
			/*!
				\param vect_mz is the pointer to std::vector of m/z values 
				\param vect_intensity is the pointer to std::vector of intensity values 
				\param start_mz minimum m/z of the peak. 
				\param stop_mz maximum m/z of the peak.
				\return returns the number of peaks that were found in the vectors. 
				\remarks The function uses PeakStatistician::FindFWHM, and PeakStatistician::FindSignalToNoise functions 
				to discover the full width at half maximum and signal to noise values for a peak. The signal to noise of a 
				peak is tested against the threshold value before its accepted as a peak. All peaks are used during the process, 
				but once generated only those which are above mdbl_peak_intensity_threshold are tested for peptidicity by Deconvolution::HornMassTransform
			*/
			int DiscoverPeaks (std::vector<double> *vect_mz, std::vector<double> *vect_intensity, double start_mz, double stop_mz) ;
			//! Function discovers the most intense peak in the m/z and intensity vectors supplied within the supplied m/z window. 
			/*!
				\param vect_mz is the pointer to std::vector of m/z values 
				\param vect_intensity is the pointer to std::vector of intensity values 
				\param start_mz minimum m/z of the peak. 
				\param stop_mz maximum m/z of the peak.
				\param pk stores the most intense peak that is found.
				\param find_fwhm specifies whether or not to update the FWHM of the parameter pk
				\param find_sn specifies whether or not to update the signal to noise of the parameter pk
				\param fit_peak specifies whether we should just take the raw m/z value as the peak or use the member variable PeakProcessor::mobj_peak_fit to find the peak that fits.
				\return returns whether or not a peak was found. 
				\remarks The function uses PeakStatistician::FindFWHM, and PeakStatistician::FindSignalToNoise functions 
				to discover the full width at half maximum and signal to noise values for a peak. The signal to noise of a 
				peak is tested against the threshold value before its accepted as a peak. All peaks are used during the process, 
				but once generated only those which are above mdbl_peak_intensity_threshold are tested for peptidicity by Deconvolution::HornMassTransform
			*/
			bool DiscoverPeak(std::vector<double> &vect_mz, std::vector<double> &vect_intensity, double start_mz, double stop_mz, 
				Peak &pk, bool find_fwhm, bool find_sn, bool fit_peak) ; 
			//! Gets the FWHM for a point. 
			/*!
				\param vect_mz is std::vector of m/z values.
				\param vect_intensity is std::vector of intensity values.
				\param data_index is the index of the point at which we want to find FWHM and SN.
				\param SN is the threshold signal to noise.
				\return returns the m/z value of the peak. 
			*/
			double GetFWHM (std::vector<double> &vect_mz, std::vector<double> &vect_intensity, int data_index, double SN) ;
			//! Gets the FWHM for a point. 
			/*!
				\param vect_mz is std::vector of m/z values.
				\param vect_intensity is std::vector of intensity values.
				\param peak is the m/z value at which we want to find FWHM.
				\return returns the m/z value of the peak. 
			*/
			double GetFWHM (std::vector<double> &vect_mz, std::vector<double> &vect_intensity, double peak) ;
			//! Gets the signal to noise for a point. 
			/*!
				\param vect_intensity is std::vector of intensity values.
				\param data_index is the index of the point at which we want to find SN.
				\return returns the signal to noise value of the peak. 
			*/
			double GetSN (std::vector<double> &vect_intensity, int data_index) ;
			//! Gets the signal to noise for a point. 
			/*!
				\param vect_mz is std::vector of m/z values.
				\param vect_intensity is std::vector of intensity values.
				\param peak is the m/z value at which we want to find the signal to noise ratio..
				\return returns the signal to noise value of the peak. 
			*/
			double GetSN (std::vector<double> &vect_mz, std::vector<double> &vect_intensity, double peak) ;
			//! Removes peaks from unprocessed list that do not have any neighbour peaks within the specified tolerance window.
			/*!
				\param specifies the tolerance in looking for neighbouring peaks.
			*/
			void FilterPeakList(double tolerance) ; 
			double GetClosestPeakMz(double peakmz, Engine::PeakProcessing::Peak &Peak);
			//!Gets the closest to peakmz among the peak list vect_mz 
		};
	}
}
