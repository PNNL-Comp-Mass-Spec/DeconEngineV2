#pragma once
#include <vector>
#include <map> 
#include "../PeakProcessor/Peak.h"
#include "../HornTransform/IsotopeFitRecord.h"
#include "../Utilities/BlockDeque.h"
#include <fstream> 
#include "LCMSPeak.h" 

namespace Engine
{
	namespace Results
	{
		//! This class stores information about the results from deisotoping an LC-MS dataset.
		/*! 
			Peaks found in the scans of the dataset are stored in a vector and sorted by scan, mz. 
			Peaks can be fetched from the entire dataset or for a given range of scans and mz.
			Deisotoped results are also stored and can be accessed for particular scans and mz ranges.
		*/
		class LCMSTransformResults
		{
			bool mbln_save_structs_in_memory ;
			bool mbln_save_UIMF; 
			char marr_temp_iso_file[128] ; 
			char marr_temp_peak_file[128] ; 
			bool mbln_deisotoped ; 
			bool mbln_sorted ; 
			bool mbln_data_in_memory_is_incomplete ; 
			int mint_last_scan_added ; 
			int mint_min_scan ; 
			int mint_max_scan ; 
			void Reset() ; 
			void SaveResultsV1Iso(char *isoFile) ; 
			void SaveResultsV1Scan(char *scanFile, bool save_signal_range) ; 
			void SaveResultsV1Data(char *dataFile) ; 
			void SaveResultsV1(char *file_name, bool save_signal_range) ; 

			void LoadResultsV1Data(char *dataFile) ; 
			char marr_file_name[512] ; 
			void CreateIndexesOnData() ; 

			std::fstream *mptr_iso_file ; 
			std::fstream *mptr_peak_file ; 

			//! Keeps the index of the first peak for a scan in mdeque_peaks
			/*!
				The BlockDeque of peaks is to be kept sorted in scans followed by mzs. This map
				keeps track of the first peak seen for a scan. thus mmap_scan_peak_indices[123] 
				returns the index of the lowest mz peak for scan 123 in mdeque_peaks.
				All the peaks in this BlockDeque from index mmap_scan_peak_indices[123] to index
				mmap_scan_peak_indices[124]-1 are peaks for scan 123. 
			*/
			std::map<int, int> mmap_scan_peak_indices ; 

			int PEAK_BLOCK_SIZE ; 
			//! Stores all the peaks found in the lc ms experiment in memory blocks of 
			// size PEAK_BLOCK_SIZE. This allows large amount of data to be kept in 
			// memory without necessarily requiring them to be in contiguous memory spaces,
			// which can often result in memory allocation being unsuccessful.
			// In addition the peaks are written out to temporary files each time AddPeaksForScan
			// function is called. If memory allocation is unsuccessful, all the peaks 
			// stored thus far are cleared. And flag is set indicating not all data is stored in 
			// memory. 
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> > mdeque_peaks ; 
			// Tracks how many peaks were seen thus far. Remember to use this variable 
			// for count of number of peaks, because mbln_save_structs_in_memory might be 
			// false. 
			int mint_num_peaks_stored ; 

			int ISO_BLOCK_SIZE ; 
			//! Stores all the deisotoped patterns found in the lc ms experiment in memory blocks 
			// of size ISO_BLOCK_SIZE (see reasoning in explanation for mdeque_peaks). In addition 
			// this data is written to temporary file each time AddTransformsForScan is called.  
			// If memory allocation is unsuccessful when new isotope patterns are added, all peaks stored 
			// are cleared and flag is set. 
			Engine::Utilities::BlockDeque<Engine::HornTransform::IsotopeFitRecord > mdeque_transforms ; 
			int mint_num_iso_stored ;

			void ClearPeaksAndIso() ; 

			inline void WriteTempPeaksToFile(std::ostream *ptr_fstream)
			{
				mptr_peak_file->flush() ; 
				mptr_peak_file->clear() ; 
				mptr_peak_file->seekg(0, std::ios::beg) ; 
				bool fail = mptr_peak_file->fail() ; 
				if (fail || mptr_peak_file->tellg() != (std::ios::pos_type) 0)
				{
					mptr_peak_file->close() ; 
					mptr_peak_file->open(marr_temp_peak_file, std::ios_base::binary | std::ios_base::in) ; 
				}
				const int BLOCK_SIZE = 1024 ;  
				LCMSPeak<PeakMinInfo> temp[BLOCK_SIZE] ; 
				const int SIZE_OF_LC_PEAK = sizeof(LCMSPeak<PeakMinInfo>) ; 
				while (!mptr_peak_file->eof())
				{
					mptr_peak_file->read((char *)temp, SIZE_OF_LC_PEAK * BLOCK_SIZE) ; 
					int num_read = mptr_peak_file->gcount() ; 
					if (num_read != 0)
						ptr_fstream->write((char *)temp, num_read) ; 
					else
					{
						mptr_peak_file->close() ;
						return ; 
					}
				}
			}
			inline void WriteTempIsosToFile(std::ostream *ptr_fstream)
			{
				if (mbln_save_structs_in_memory && !mbln_data_in_memory_is_incomplete)
				{
					// write from stored data in mdeque_isotopes
					Engine::Utilities::BlockDeque<Engine::HornTransform::IsotopeFitRecord >::iterator iter ; 
					Engine::Utilities::BlockDeque<Engine::HornTransform::IsotopeFitRecord >::iterator iter_first = mdeque_transforms.begin() ; 
					Engine::Utilities::BlockDeque<Engine::HornTransform::IsotopeFitRecord >::iterator iter_last = mdeque_transforms.end() ; 

					/*
					Needs work
					for (iter = iter_first ; iter != iter_last ; iter++)
					{
						Engine::HornTransform::IsotopeFitRecord record = *iter ; 						
						fout.precision(2) ; 
						ptr_fstream->write(record.mdbl_sn,<<"," ; 
						fout.precision(4) ; 
						fout<<record.mint_mono_intensity<<"," ; 
						fout<<record.mint_iplus2_intensity<<"\n" ; 
					}*/
				}
				else
				{
					mptr_iso_file->flush() ; 
					mptr_iso_file->clear() ; 
					mptr_iso_file->seekg(0, std::ios::beg) ; 
					bool fail = mptr_iso_file->fail() ; 
					if (fail || (mptr_iso_file->tellg() != (std::ios::pos_type)0))
					{
						mptr_iso_file->close() ; 
						mptr_iso_file->open(marr_temp_iso_file, std::ios_base::binary | std::ios_base::in) ; 
					}
					
					const int BLOCK_SIZE = 1024 ;  
					Engine::HornTransform::IsotopeFitRecord temp[BLOCK_SIZE] ; 
					const int SIZE_OF_ISO = sizeof(Engine::HornTransform::IsotopeFitRecord) ; 
					while (!mptr_iso_file->eof())
					{
						mptr_iso_file->read((char *)temp, SIZE_OF_ISO * BLOCK_SIZE) ; 
						int num_read = mptr_iso_file->gcount() ; 
						if (num_read != 0)
							ptr_fstream->write((char *)temp, num_read) ; 
						else
						{
							mptr_iso_file->close();
							return ; 
						}
							
					}
				}
			}
			
		public:
			void SaveScanResults(char *scanFile, bool save_signal_range) { SaveResultsV1Scan(scanFile, save_signal_range) ; } ; 

			std::map<int, double> mmap_scan_bp_mz ; 
			std::map<int, double> mmap_scan_time ; 
			std::map<int, double> mmap_scan_bp_intensity ; 
			std::map<int, double> mmap_scan_tic ; 
			std::map<int, double> mmap_scan_signal_range ; 
			std::map<int, int> mmap_scan_num_peaks ; 
			std::map<int, int> mmap_scan_num_deisotoped ; 
			std::map<int, short> mmap_scan_num_mslevel ; 
			std::map<int, double> mmap_scan_num_drift_time ; 

			void AddInfoForUIMFScan(int scan, double bp_mz, double bp_intensity, double tic, double signal_range, int num_peaks, int num_deisotoped,
				double time, short ms_level, double drift_time) ;
			void AddInfoForScan(int scan, double bp_mz, double bp_intensity, double tic, double signal_range, int num_peaks, int num_deisotoped,
				double time, short ms_level) ; 			
	
			LCMSTransformResults() ; 
			~LCMSTransformResults() ; 
			void SaveResults(char *file_name, bool save_signal_range)  ;
			void LoadResults(char *file_name) ; 
			// Peaks for scan are written out to file AND stored in the memory blocks of size 
			void AddPeaksForScan(int scan_num, std::vector<Engine::PeakProcessing::Peak> &peaks) ; 
			void AddTransforms(std::vector<Engine::HornTransform::IsotopeFitRecord> &vect_fit_results) ; 			
			void GetSIC(int min_scan, int max_scan, float min_mz, float max_mz, std::vector<float> &vect_intensities) ; 
			void GetScanPeaks(int scan_num, std::vector<float> &vect_mzs, std::vector<float> &vect_intensities) ; 
			int GetMinScan() ; 
 			int GetMaxScan() ; 
			bool IsDeisotoped() ; 
			const char *GetFileName() { return marr_file_name ; } 
			int GetNumPeaks() { return mint_num_peaks_stored ; }  ;
			int GetNumTransforms() { return mint_num_iso_stored ; } ; 
			inline void GetAllPeaks(std::vector<Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo> > &vectPeaks)
			{
				mdeque_peaks.Get(vectPeaks, 0, mint_num_peaks_stored) ; 
			}
			inline LCMSPeak<PeakMinInfo> GetPeak(int pk_num) 
			{
				return mdeque_peaks.GetPoint(pk_num) ; 
			}  
			inline Engine::HornTransform::IsotopeFitRecord GetIsoPattern(int iso_num) 
			{
				return mdeque_transforms.GetPoint(iso_num) ; 
			}
			bool IsAllDataInMemory() { return !mbln_data_in_memory_is_incomplete ; } ; 
			

		};
	}
}