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
			void SaveResultsV1Scan(char *scanFile) ; 
			void SaveResultsV1Data(char *dataFile) ; 
			void SaveResultsV1(char *file_name) ; 

			void LoadResultsV1Data(char *dataFile) ; 
			char marr_file_name[512] ; 
			void CreateIndexesOnData() ; 

			std::fstream *mptr_iso_file ; 
			std::fstream *mptr_peak_file ; 

			//! Keeps the index of the first peak for a scan in the vector mvect_peaks
			/*!
				The vector of peaks is to be kept sorted in scans followed by mzs. This map
				keeps track of the first peak seen for a scan. thus mmap_scan_peak_indices[123] 
				returns the index of the lowest mz peak for scan 123 in mvect_peaks.
				All the peaks in this vector from index mmap_scan_peak_indices[123] to index
				mmap_scan_peak_indices[124]-1 are peaks for scan 123. 
			*/
			std::map<int, int> mmap_scan_peak_indices ; 
			int mint_num_peaks_stored ; 
			int mint_num_peaks_left_in_current_block ; 
			static const int NUM_PEAKS_IN_BLOCK = 4 * 1024 ; 
			static const int PEAK_BLOCK_SIZE = sizeof(LCMSPeak<PeakMinInfo>) * NUM_PEAKS_IN_BLOCK ; 
			//! Stores all the peaks found in the lc ms experiment in memory blocks of 
			// size PEAK_BLOCK_SIZE. This allows large amount of data to be kept in 
			// memory without necessarily requiring them to be in contiguous memory spaces,
			// which can often result in memory allocation being unsuccessful.
			// In addition the peaks are written out to temporary files each time AddPeaksForScan
			// function is called. If memory allocation is unsuccessful, all the peaks 
			// stored thus far are cleared. And flag is set indicating not all data is stored in 
			// memory. 
			std::vector<LCMSPeak<PeakMinInfo> *> mvect_peak_blocks_ptr ; 

			int mint_num_iso_stored ; 
			int mint_num_iso_left_in_current_block ;
			static const int NUM_ISO_IN_BLOCK = 1024 ; 
			static const int ISO_BLOCK_SIZE = sizeof(Engine::HornTransform::IsotopeFitRecord) * NUM_ISO_IN_BLOCK ; 
			//! Stores all the deisotoped patterns found in the lc ms experiment in memory blocks 
			// of size mint_iso_block_size (see reasoning in explanation for mvect_peaks). In addition 
			// this data is written to temporary file each time AddTransformsForScan is called.  
			// If memory allocation is unsuccessful when new isotope patterns are added, all peaks stored 
			// are cleared and flag is set. 
			std::vector<Engine::HornTransform::IsotopeFitRecord *> mvect_transform_record_blocks_ptr ; 
			void ClearPeaksAndIso() ; 
			// copies specified number of peaks at specified index in vector into memory location. Allocation 
			// of memory at the pointer is responisibilty of the caller. 
			inline void CopyPeaks(int scan_num, std::vector<Engine::PeakProcessing::Peak> &vect_peaks, LCMSPeak<PeakMinInfo> *dest, 
				int start_index, int num_pks)
			{
				LCMSPeak<PeakMinInfo> lc_ms_peak ; 
				PeakMinInfo pk ; 
				Engine::PeakProcessing::Peak orig_pk ; 
				lc_ms_peak.mint_scan_num = scan_num ; 
				int to_index = start_index + num_pks ; 
				int num_copied = 0 ;
				for (int pk_num = start_index ; pk_num < to_index ; pk_num++)
				{
					orig_pk = vect_peaks[pk_num] ; 
					pk.mflt_intensity = (float) orig_pk.mdbl_intensity ; 
					pk.mflt_mz = (float) orig_pk.mdbl_mz ; 
					lc_ms_peak.mobj_peak = pk ; 
					dest[num_copied++] = lc_ms_peak ; 
				}
			}
			inline void WritePeaksToFile(std::fstream *ptr_fstream, int scan_num, std::vector<Engine::PeakProcessing::Peak> &vect_peaks)
			{
				int num_pks = vect_peaks.size() ; 
				const int SIZE_OF_LC_PEAK = sizeof(LCMSPeak<PeakMinInfo>) ; 
				PeakMinInfo pk ; 
				Engine::PeakProcessing::Peak orig_pk ;
				LCMSPeak<PeakMinInfo> lc_ms_peak ;
				lc_ms_peak.mint_scan_num = scan_num ; 
				LCMSPeak<PeakMinInfo> *tempMem = new LCMSPeak<PeakMinInfo> [num_pks] ; 

				if (tempMem == NULL)
				{
					// allocation failed. It might be time to clean up memory. For this function, write things 
					// point by point. 
					for (int pk_num = 0 ; pk_num < num_pks ; pk_num++)
					{
						orig_pk = vect_peaks[pk_num] ; 
						pk.mflt_intensity = (float) orig_pk.mdbl_intensity ; 
						pk.mflt_mz = (float) orig_pk.mdbl_mz ; 
						lc_ms_peak.mobj_peak = pk ; 
						ptr_fstream->write((char *)&lc_ms_peak, SIZE_OF_LC_PEAK) ; 
					}
				}
				else
				{
					for (int pk_num = 0 ; pk_num < num_pks ; pk_num++)
					{
						orig_pk = vect_peaks[pk_num] ; 
						pk.mflt_intensity = (float) orig_pk.mdbl_intensity ; 
						pk.mflt_mz = (float) orig_pk.mdbl_mz ; 
						lc_ms_peak.mobj_peak = pk ; 
						tempMem[pk_num] = lc_ms_peak ; 
					}
					ptr_fstream->write((char *)tempMem, num_pks * SIZE_OF_LC_PEAK) ; 
					delete [] tempMem ; 
				}
			}
			inline void WriteTempPeaksToFile(std::ostream *ptr_fstream)
			{
				mptr_peak_file->seekg(0, std::ios::beg) ; 
				bool success = mptr_peak_file->fail() ; 
				const int BLOCK_SIZE = 1024 ;  
				LCMSPeak<PeakMinInfo> temp[BLOCK_SIZE] ; 
				const int SIZE_OF_LC_PEAK = sizeof(LCMSPeak<PeakMinInfo>) ; 
				while (!mptr_peak_file->eof())
				{
					mptr_peak_file->read((char *)temp, SIZE_OF_LC_PEAK * BLOCK_SIZE) ; 
					int num_read = mptr_peak_file->gcount() ; 
					if (num_read != 0)
						ptr_fstream->write((char *)temp, num_read) ; 
				}
			}
			
		public:

			std::vector<Engine::HornTransform::IsotopeFitRecord> mvect_transform_records ; 
			void SaveScanResults(char *scanFile) { SaveResultsV1Scan(scanFile) ; } ; 

			std::map<int, double> mmap_scan_bp_mz ; 
			std::map<int, double> mmap_scan_bp_intensity ; 
			std::map<int, double> mmap_scan_tic ; 
			std::map<int, int> mmap_scan_num_peaks ; 
			std::map<int, int> mmap_scan_num_deisotoped ; 

			void AddInfoForScan(int scan, double bp_mz, double bp_intensity, double tic, int num_peaks, int num_deisotoped) ; 

	
			LCMSTransformResults() ; 
			~LCMSTransformResults() ; 
			void SaveResults(char *file_name)  ;
			void LoadResults(char *file_name) ; 
			// Peaks for scan are written out to file AND stored in the memory blocks of size 
			void AddPeaksForScan(int scan, std::vector<Engine::PeakProcessing::Peak> &peaks) ; 
			void AddTransformsForScan(int scan, std::vector<Engine::HornTransform::IsotopeFitRecord> &vect_fit_results) ; 
			void GetSIC(int min_scan, int max_scan, float min_mz, float max_mz, std::vector<float> &vect_intensities) ; 
			void GetScanPeaks(int scan_num, std::vector<float> &vect_mzs, std::vector<float> &vect_intensities) ; 
			int GetMinScan() ; 
 			int GetMaxScan() ; 
			bool IsDeisotoped() ; 
			const char *GetFileName() { return marr_file_name ; } 
			int GetNumPeaks() { return mint_num_peaks_stored ; }  ;
			inline LCMSPeak<PeakMinInfo> GetPeak(int pk_num) 
			{
				int block_num = pk_num/NUM_PEAKS_IN_BLOCK ; 
				int index = pk_num % NUM_PEAKS_IN_BLOCK ; 
				return mvect_peak_blocks_ptr.at(block_num)[index] ; 
			}  
			bool IsAllDataInMemory() { return !mbln_data_in_memory_is_incomplete ; } ; 
		};
	}
}