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
#include "Peak.h" 
#include "PeakIndex.h"
#include <vector>
#include <map>

namespace Engine
{
	namespace  PeakProcessing 
	{
		//!  Used to store and retreive information about peaks that were found in the raw data.
		/*!
			This class stores all the information about peaks in the data. Additionally it is used in processing extensively. Because this information 
			needs to be accessed with high degree of efficiency we keep different maps.
			The order of processing is as follows. 
			-# PeakProcessor discovers all the peaks in the data using PeakProcessor::DiscoverPeaks and sets its member variable PeakProcessor::mobj_peak_data.
			-# PeakData keeps a std::vector of these peaks in PeakData::mvect_peak_tops. 
			-# PeakData keeps a map of unprocessed peaks in PeakData::mmap_pk_mz_index sorted by intensity. This allows us to pick up the index of the next most intense unprocessed
				peak in PeakData::mvect_peak_tops.
			-# PeakData keeps a map of all peaks in PeakData::mmap_pk_mz_index_all. This map keeps a sorted tree in terms of intensity of peaks for all the peaks in the std::vector
				PeakData::mvect_peak_tops.
		*/
		class PeakData
		{
			//! pointer to the std::vector of temporary peaks that are used during the processing. 
			std::vector<Peak> mvect_temp_peak_tops ;
			//! multimap of indices of unprocessed peaks in PeakData::mvect_temp_peak_tops sorted in descending intensity. This helps fast retrieval of the next most intense unprocessed peak.
			/*!
				\remarks While the intensity of the peaks might actually be double, for the map, we only used the integral values.
			*/
			std::multimap<int, int, std::greater<int> > mmap_pk_intensity_index ; 
			//! multimap of indices of unprocessed peaks in PeakData::mvect_temp_peak_tops sorted in ascending m/z. This helps fast retrieval when looking for an unprocessed peak around an approximate m/z.
			std::map<double, int> mmap_pk_mz_index ; 
			//! multimap of indices of all peaks in PeakData::mvect_temp_peak_tops sorted in ascending m/z. This helps fast retrieval when looking for a peak(not only unprocessed ones) around an approximate m/z.
			std::map<double, int> mmap_pk_mz_index_all ; 
			//! The TIC for current scan. 
			double mdbl_tic ; 
			//! helps searching for specific values in any sorted std::vector. 
			PeakIndex <double> mobj_pk_index ; 

		public:
			//! std::vector of peaks found in the data. It is recommended that this object not be touched
			// by calling functions. 
			std::vector<Peak> mvect_peak_tops ;
			//! pointer to std::vector of mz's in the raw data
			std::vector <double> *mptr_vect_mzs ; 
			//! pointer to std::vector of intensities in the raw data.  
			std::vector<double> *mptr_vect_intensities ; 

			//! Default constructor
			PeakData(void);
			//! copy constructor.
			PeakData(const PeakData &a);
			//! destructor.
			~PeakData(void);
			
			//! Clears auxillary data structures and initializes them at the start of processing.
			/*! 
				\remarks
				We track which peaks are unprocessed through the following variables:
				\arg \c PeakData::mmap_pk_intensity_index is the map of unprocessed peaks sorted in decreasing intensity, allowing for a quick binary search for the next most intense peak.
				\arg \c PeakData::mmap_pk_mz_index is the map of the same unprocessed peaks, but sorted by m/z for m/z searches.
				At the start of the deconvolution, these variable are filled in with pairs of (peak intensity, peak index) and (peak mz, peak index) for peaks which will be processed.
				As deconvolution processes, these elements are taken off the unprocessed maps. 
				The current function is called to set these variables.
			*/
			void InitializeUnprocessedPeakData() ; 

			//! Sort peak std::vector PeakData::mvect_peak_tops in order of decreasing intensity.
			void SortPeaksByIntensity() ; 

			//! Sets the option for PeakData processing.
			/*!
				\param num_iso_sum defines number of isotopes summed together to calculate the intensity of a feature. 
			*/
			void SetOptions(short num_iso_sum) ; 

			//! Gets the index^th peak in the std::vector PeakData::mvect_peak_tops.
			/*!
				\param index is the index of the peak in the std::vector PeakData::mvect_peak_tops. 
				\return pk is assigned the peak which is at the index^th position in std::vector PeakData::mvect_peak_tops.
			*/
			void GetPeak(int index, Peak &pk) ; 
			//! Adds a peak to the std::vector of peaks PeakData::mvect_peak_tops
			/*!
				\param pk is the peak that we want to add to our std::vector of peaks. 
			*/
			void AddPeak(Peak &pk) ; 
			//! Adds a peak to the processing list. 
			/*! 
				\param pk is the peak that we want to add to our processing list. 
				\remarks The processing list is really the set of 
				peaks that are unprocessed and the way these are tracked, are by puttting these indices in the processing maps
				PeakData::mmap_pk_intensity_index and PeakData::mmap_pk_mz_index
			*/
			void AddPeakToProcessingList(Peak &pk) ; 
			//! Returns number of peaks.
			/*!
				\return number of peaks found.
			*/
			int GetNumPeaks() ; 

			//! Get the most intense unprocessed peak in the given m/z range.
			/*!
				\param start_mz minimum m/z of the peak.
				\param stop_mz maximum m/z of the peak. 
				\param pk is assigned the most intense Peak with m/z between the start_mz and stop_mz.
				\return returns true if a peak was found and false if none was found.
			*/
			bool GetPeak(double start_mz, double stop_mz, Peak &pk) ;
			//! Gets the unprocessed peak whose m/z is closest to supplied mz in the m/z range (mz - width to mz + width).
			/*!
				\param mz the center m\z around which we want to look for a Peak.
				\param width the width of the m\z window in which we want to look for the peak.
				\param pk is the peak closest to m/z. 
				\return returns true if a peak was found in the window (mz - width to mz + width) and false if not found.
			*/
			bool GetClosestPeak(double mz, double width, Peak &pk) ; 
			//! Gets the peak (whether or not it is processed) whose m/z is closest to supplied mz in the m/z range (mz - width to mz + width).
			/*!
				\param mz the center m\z around which we want to look for a Peak.
				\param width the width of the m\z window in which we want to look for the peak.
				\param pk is the peak closest to m/z. 
				\return returns true if a peak was found in the window (mz - width to mz + width) and false if not found.
				\note The returned peak can have an intensity of 0 because it was already processed and removed.
			*/
			bool GetClosestPeakFromAll(double mz, double width, Peak &pk) ; 
			//! Gets the most intense peak(whether or not it is processed) in the m/z range (mz - width to mz + width).
			/*!
				\param start_mz minimum m/z of the Peak.
				\param stop_mz mimum m/z of the Peak.
				\param pk is set to the peak that was found. 
				\return returns true if a peak was found in the window (mz - width to mz + width) and false if not found.
				\note The returned peak can have an intensity of 0 because it was already processed and removed.
			*/
			bool GetPeakFromAll(double start_mz, double stop_mz, Peak &pk) ;
			//! Gets the most intense peak(whether or not it is processed) in the m/z range (mz - width to mz + width).
			/*!
				\param start_mz minimum m/z of the Peak.
				\param stop_mz mimum m/z of the Peak.
				\param pk is set to the peak that was found. 
				\param exclude_mass is the mass we need to exclude in this search.
				\return returns true if a peak was found in the window (mz - width to mz + width) and false if not found.
				\note The returned peak can have an intensity of 0 because it was already processed and removed.
			*/
			bool GetPeakFromAll(double start_mz, double stop_mz, Peak &pk, double exclude_mass) ;
			//! Gets the most intense peak(whether or not it is processed) in the m/z range (mz - width to mz + width). The intensity returned is the intensity in the original raw data std::vector PeakData::mptr_vect_intensities
			/*!
				\param start_mz minimum m/z of the Peak.
				\param stop_mz mimum m/z of the Peak.
				\param pk is set to the peak that was found. 
				\param exclude_mass is the mass we need to exclude in this search.
				\return returns true if a peak was found in the window (mz - width to mz + width) and false if not found.
				\note The returned peak has the intensity in the original raw data std::vector PeakData::mptr_vect_intensities.
			*/
			bool GetPeakFromAllOriginalIntensity(double start_mz, double stop_mz, Peak &pk, double exclude_mass) ;
			//! Gets the most intense peak(whether or not it is processed) in the m/z range (mz - width to mz + width). The intensity returned is the intensity in the original raw data std::vector PeakData::mptr_vect_intensities
			/*!
				\param start_mz minimum m/z of the Peak.
				\param stop_mz mimum m/z of the Peak.
				\param pk is set to the peak that was found. 
				\return returns true if a peak was found in the window (mz - width to mz + width) and false if not found.
				\note The returned peak has the intensity in the original raw data std::vector PeakData::mptr_vect_intensities.
			*/
			bool GetPeakFromAllOriginalIntensity(double start_mz, double stop_mz, Peak &pk) ;
			//! Gets the peak in the std::vector PeakData::mvect_peak_tops whoe m/z is exactly equal to mz.
			/*!
				\param mz m/z of the peak we are looking for. 
				\return pk is assigned the peak whose m/z equals input parameter.
			*/
			bool GetPeak(double mz, Peak &pk) ; 
			//! Get the most intense unprocessed peak in the given m/z range and remove it from the processing list.
			/*!
				\param start_mz minimum m/z of the peak.
				\param stop_mz maximum m/z of the peak. 
				\param pk is assigned the most intense Peak with m/z between the start_mz and stop_mz.
				\return returns true if a peak was found and false if none was found.
				\note The peak that is returned by this function is removed from the processing list. This is essentially the function that is called repeatedly in the deconvolution process which deisotopes peaks in order of decreasing intensity. 
			*/
			bool GetNextPeak(double start_mz, double stop_mz, Peak &pk) ;
			//! Removes the peak from the unprocessed list.
			/*!
				\param pk is the peak we want to remove from the unprocessed peaks.
				\note In order to remove the peak from the processing "list", we clear the indices of the peak from the unprocessed maps PeakData::mmap_pk_mz_index and PeakData::mmap_pk_intensity_index
			*/
			void RemovePeak(Peak &pk) ; 
			//! Removes all the peaks in the supplied m/z range from the unprocessed list.
			/*!
				\param start_peak is the minimum m/z.
				\param stop_peak is the maximum m/z.
				\param debug set to true if you want to print debug information from this function.
				\note In order to remove the peaks from the processing "list", we clear the indices of the peaks from the unprocessed maps PeakData::mmap_pk_mz_index and PeakData::mmap_pk_intensity_index
			*/
			void RemovePeaks(double start_peak, double stop_peak, bool debug = false) ; 
			//! Removes all the peaks whose m/zs are almost equal to the m/zs supplied in the std::vector peak_mzs with a supplied m/z tolerance.
			/*!
				\param peak_mzs is the std::vector of m/z values we want to remove.
				\param mz_tolerance used to search for the above peaks in PeakData::mptr_vect_
				\param debug set to true if you want to print debug information from this function.
				\note In order to remove the peaks from the processing "list", we clear the indices of the peaks from the unprocessed maps PeakData::mmap_pk_mz_index and PeakData::mmap_pk_intensity_index
				This function can be used to remove calibration peaks from the list. 
			*/
			void RemovePeaks(std::vector<double> &peak_mzs, double mz_tolerance, bool debug = false) ; 

			//! Sets the tic value for the peaks in current scan.
			/*!
				\param tic is the tic value we want to set.
			*/
			void SetTIC(double tic) ; 
			//! Gets the tic values for the peaks in current scan.
			/*!
				\return returns the tic value for the current scan.
			*/
			double GetTIC() ; 
			//! Filters the peak list and removes peaks that do not have any neighbouring peaks
			//! in the specified window.
			/*!
				\param tolerance is the window around a peak in which we look for neighbours.
			*/
			void FilterList(double tolerance) ; 
			//! Clears all the data structures in the PeakData instance.
			/*!
				\remarks
				It clears the following member variables of this instance: 
				- std::vector of peak tops PeakData::mvect_peak_tops
				- maps of unprocessed peaks:
					-# map of mz to indices: PeakData::mmap_pk_mz_index
					-# map of intensity to indices: PeakData::mmap_pk_intensity_index
				- map of all peaks PeakData::mmap_pk_mz_index_all
				- It also sets to NULL 
					-# arg pointer to std::vector of raw intensities PeakData::mptr_vect_intensities
					-# arg pointer to std::vector of raw intensities PeakData::mptr_vect_mzs
				\note This function does not clear or delete the vectors of raw mzs and intensities. That is left to the caller.
			*/
			void Clear() ; 
			//! Prints the Peaks to stderr.
			void PrintPeaks() ;
			//! Prints unprocessed peaks to stderr. 
			void PrintUnprocessedPeaks() ; 
			//! Prints unprocessed peaks to a given file after deleting its contents.
			/*!
				\param file_name file that we want to print peaks to.
			*/
			void PrintUnprocessedPeaks(char *file_name) ; 
			//! Gets a copy of the pointers to vectors of mzs, intensities from the raw data.
			/*!
				\param mzs pointer to pointer to mzs. *mzs = PeakData::mptr_vect_mzs
				\param rts pointer to pointer to rts. *rts = PeakData::mptr_vect_rts
			*/
			void GetMzIntVectors(void **mzs, void **rts) ; 
			//! Gets number of unprocessed peaks remaining. 
			/*!
				\return returns the number of unprocessed peaks. This is the same as mmap_pk_mz_index.size()
			*/
			int GetNumUnprocessedPeaks() ; 
			//! Finds the highest peak from the raw data vectors withing the specified m/z range.
			/*!
				\param start_mz minimum m\z at which to look for the peak
				\param stop_mz maximum m\z at which to look for the peak.
				\param peak  instance whose mz and intensity are set to the peak that is found.
				\note The function only sets the mz, intensity of the peak, not the other members (SN, FWHM etc).
			*/
			void FindPeak(double start_mz, double stop_mz, PeakProcessing::Peak &peak)  ; 
			//! Finds the highest peak from the raw data vectors withing the specified m/z range (reduced from original FindPeak
			/*!
				\param start_mz minimum m\z at which to look for the peak
				\param stop_mz maximum m\z at which to look for the peak.
				\param peak  instance whose mz and intensity are set to the peak that is found.
				\note The function only sets the mz, intensity of the peak, not the other members (SN, FWHM etc).
			*/
			void FindPeakAbsolute(double start_mz, double stop_mz, PeakProcessing::Peak &peak) ;

		};
	}
}