#include "LCMSTransformResults.h"
#include <fstream> 
#include "LCMSPeakIndex.h"
#include <iostream> 
#include <time.h> 

namespace Engine
{
	namespace Results
	{
		const int MAX_INFORMATION_BLOCK_SIZE_V1 = 512 ; 

		LCMSTransformResults::LCMSTransformResults()
		{
			mptr_iso_file = NULL ; 
			mptr_peak_file = NULL ; 
			Reset() ; 
			mbln_save_structs_in_memory = false; 
		}

		LCMSTransformResults::~LCMSTransformResults()
		{
			int i = 0 ; 
			if (mptr_iso_file != NULL)
			{
				if (mptr_iso_file->is_open())
				{
					mptr_iso_file->close() ; 
					mptr_peak_file->close() ; 
				}
				delete mptr_iso_file ; 
				delete mptr_peak_file ; 
			}
			ClearPeaksAndIso() ; 
			int res = remove(marr_temp_iso_file) ; 
			res = remove(marr_temp_peak_file) ; 

		}
		
		void LCMSTransformResults::ClearPeaksAndIso()
		{
			for (int pk_block = 0 ; pk_block < mvect_peak_blocks_ptr.size() ; pk_block++)
			{
				LCMSPeak<PeakMinInfo> *pk_block_ptr = mvect_peak_blocks_ptr[pk_block] ; 
				delete [] pk_block_ptr ; 
			}
			mvect_peak_blocks_ptr.clear() ; 

			for (int iso_block = 0 ; iso_block < mvect_transform_record_blocks_ptr.size() ; iso_block++)
			{
				Engine::HornTransform::IsotopeFitRecord *iso_block_ptr = mvect_transform_record_blocks_ptr[iso_block] ; 
				delete [] iso_block_ptr ; 
			}
			mvect_transform_record_blocks_ptr.clear() ; 
			mint_num_iso_stored = 0 ;
			mint_num_iso_left_in_current_block = 0 ; 
			mint_num_peaks_stored = 0 ;
			mint_num_peaks_left_in_current_block = 0 ; 
		}

		void LCMSTransformResults::Reset()
		{
			mbln_sorted = true ;
			mint_last_scan_added = -1 ; 
			mbln_deisotoped = false ; 
			mvect_transform_records.clear() ; 
			mint_min_scan = INT_MAX ;
			mint_max_scan = -1 ; 

			strcpy(marr_temp_iso_file, "tmp.iso.") ; 

			if (mptr_iso_file != NULL)
			{
				if (mptr_iso_file->is_open())
				{
					mptr_iso_file->close() ; 
					mptr_peak_file->close() ; 
				}
				delete mptr_iso_file ; 
				delete mptr_peak_file ; 
			}
			int time_t = (int) clock() ; 
			_itoa(time_t, marr_temp_iso_file + 8 ,10) ; 


			strcpy(marr_temp_peak_file, "tmp.peak.") ; 
			_itoa(time_t, marr_temp_peak_file+ 9 ,10) ; 

			mptr_iso_file = new std::fstream(marr_temp_iso_file, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary | std::ios_base::in) ; 
			mptr_peak_file = new std::fstream(marr_temp_peak_file, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary | std::ios_base::in) ;

			ClearPeaksAndIso() ; 
			mbln_data_in_memory_is_incomplete = false ; 

		}
		
		void LCMSTransformResults::AddInfoForScan(int scan, double bp_mz, double bp_intensity, double tic, int num_peaks, int num_deisotoped)
		{
			mmap_scan_bp_mz.insert(std::pair<int, double>(scan, bp_mz)) ; 
			mmap_scan_bp_intensity.insert(std::pair<int, double>(scan, bp_intensity)) ; 
			mmap_scan_tic.insert(std::pair<int, double>(scan, tic)) ; 
			mmap_scan_num_peaks.insert(std::pair<int, int>(scan, num_peaks)) ; 
			mmap_scan_num_deisotoped.insert(std::pair<int, int>(scan, num_deisotoped)) ; 
		}

		void LCMSTransformResults::AddPeaksForScan(int scan_num,
			std::vector<Engine::PeakProcessing::Peak> &vect_peaks)
		{
			const int SIZE_OF_LC_PEAK = sizeof(LCMSPeak<PeakMinInfo>) ; 
			if (scan_num > mint_max_scan)
			{
				mint_max_scan = scan_num ; 
			}
			if (scan_num < mint_min_scan)
			{
				mint_min_scan = scan_num ; 
			}

			int num_peaks = vect_peaks.size() ; 

			// so we have reached the next scan where peaks were seen.
			// It can be the case that no peaks were seen for intermediate scan.
			for (int scan = mint_last_scan_added + 1 ; scan <= scan_num ; scan++)
			{
				mmap_scan_peak_indices.insert(std::pair<int,int>(scan,mint_num_peaks_stored)) ; 
			}

			LCMSPeak<PeakMinInfo> *destPtr = NULL ;  
			if (mint_num_peaks_stored != 0 && mint_num_peaks_left_in_current_block != 0)
			{
				destPtr = mvect_peak_blocks_ptr.at(mvect_peak_blocks_ptr.size()-1) + (NUM_PEAKS_IN_BLOCK-mint_num_peaks_left_in_current_block) ; 
			}

			// First thing we do is write the peaks to temporary peaks file. 
			WritePeaksToFile(mptr_peak_file, scan_num, vect_peaks) ; 
			mint_num_peaks_stored += num_peaks ; 
			mint_last_scan_added = scan_num ; 

			if (!mbln_save_structs_in_memory)
				return ; 

			int start_index = 0 ; 
			while (start_index <  num_peaks)
			{
				if (num_peaks > start_index + mint_num_peaks_left_in_current_block)
				{
					if (mint_num_peaks_left_in_current_block != 0)
					{
						CopyPeaks(scan_num, vect_peaks, destPtr, start_index, mint_num_peaks_left_in_current_block) ; 
						start_index += mint_num_peaks_left_in_current_block ; 
						mint_num_peaks_left_in_current_block = 0 ; 
					}
					else
					{
						destPtr = new LCMSPeak<PeakMinInfo> [NUM_PEAKS_IN_BLOCK] ;  
						if (destPtr == NULL)
						{
							// Rollback memory and set flag. 
							mbln_data_in_memory_is_incomplete = true ; 
							ClearPeaksAndIso() ; 
							// Now try again. If this does not work, throw and exception. 
							destPtr = new LCMSPeak<PeakMinInfo> [NUM_PEAKS_IN_BLOCK] ; 
							if (destPtr == NULL)
							{
								throw "Memory allocation for peaks failed even on clearing memory" ; 
							}
						}
						mint_num_peaks_left_in_current_block = NUM_PEAKS_IN_BLOCK ; 
						mvect_peak_blocks_ptr.push_back(destPtr) ; 
					}
				}
				else
				{
					CopyPeaks(scan_num, vect_peaks, destPtr, start_index, num_peaks - start_index) ; 
					mint_num_peaks_left_in_current_block -= (num_peaks - start_index) ; 
					start_index = num_peaks ; 
				}
			}

		}

		void LCMSTransformResults::AddTransformsForScan(int scan, 
			std::vector<Engine::HornTransform::IsotopeFitRecord> &vect_fit_results)
		{
			if (mvect_transform_records.capacity() < mvect_transform_records.size() + vect_fit_results.size())
			{
				mvect_transform_records.reserve(2*mvect_transform_records.capacity()) ; 
			}
			mvect_transform_records.insert(mvect_transform_records.end(), vect_fit_results.begin(), vect_fit_results.end()) ; 
		}

		int LCMSTransformResults::GetMinScan()
		{
			return mint_min_scan ; 
		}
		int LCMSTransformResults::GetMaxScan()
		{
			return mint_max_scan ; 
		}
		
		bool LCMSTransformResults::IsDeisotoped()
		{
			return mbln_deisotoped ; 
		}

		void LCMSTransformResults::SaveResultsV1Iso(char *iso_file_name)
		{
			std::ofstream fout(iso_file_name);				 						
			if (!mbln_data_in_memory_is_incomplete)
			{
				//Preserve coloumn headers for VIPER to load it in
				fout<<"scan_num"<<","<<"charge"<<","<<"abundance"<<","<<"mz";
				fout<<","<<"fit"<<","<<"average_mw"<<","<<" monoisotopic_mw"<<","<<"mostabundant_mw";
				fout<<","<<"fwhm"<<","<<"signal_noise"<<","<<"mono_abundance"<<","<<"mono_plus2_abundance";
				fout <<"\n" ;		
				int num_pts = mvect_transform_records.size();
				for (int i = 0 ; i < num_pts ; i++)
				{
					Engine::HornTransform::IsotopeFitRecord record = mvect_transform_records[i] ; 
					fout<<record.mint_scan_num<<","<<record.mshort_cs<<","<<record.mint_abundance<<","<<record.mdbl_mz ; 
					fout<<","<<record.mdbl_fit<<","<<record.mdbl_average_mw<<","<<record.mdbl_mono_mw<<","<<record.mdbl_most_intense_mw ; 
					fout<<","<<record.mdbl_fwhm<<","<<record.mdbl_sn<<","<<record.mint_mono_intensity<<"," ; 
					fout<<record.mint_iplus2_intensity<<"\n" ; 
				}
			}
			else
			{
			}
			fout.close();
		}

		void LCMSTransformResults::SaveResultsV1Scan(char *scan_file_name)
		{
			std::ofstream fout(scan_file_name);				 						
			//Preserve coloumn headers for VIPER to load it in
			fout<<"scan_num,bpi,bpi_mz,tic,num_peaks,num_deisotoped\n";

			for (std::map<int,double>::iterator iter = mmap_scan_bp_intensity.begin() ; iter != mmap_scan_bp_intensity.end() ; 
				iter++)
			{
				int scan_num = (*iter).first ; 
				fout<<scan_num<<","<<mmap_scan_bp_intensity[scan_num]<<","<<mmap_scan_bp_mz[scan_num]<<"," ; 
				fout<<mmap_scan_tic[scan_num]<<","<<mmap_scan_num_peaks[scan_num]<<","<<mmap_scan_num_deisotoped[scan_num]<<"\n" ; 
			}
			fout.close();
		}

		void LCMSTransformResults::SaveResultsV1Data(char *data_file_name)
		{
			const int INT_SIZE = sizeof(int) ; 
			const int SIZE_OF_LC_PEAK = sizeof(LCMSPeak<PeakMinInfo>) ; 
			std::ofstream fout(data_file_name, std::ios::binary) ; 
			int filename_len = strlen(data_file_name) ; 
			fout.write((char *)&filename_len, INT_SIZE) ; 
			fout.write(data_file_name, filename_len) ; 

			char *version_str = "Version: 1.0" ;
			int version_str_len = strlen(version_str) ; 

			fout.write((char *)&version_str_len, INT_SIZE) ; 
			fout.write(version_str, version_str_len) ; 

			// now to write the peaks. Write out the number of peaks so that reading program knows how many to read
			fout.write((char *)&mint_num_peaks_stored, INT_SIZE) ; 

			WriteTempPeaksToFile(&fout) ; 

			int num_isotopic_peaks = mvect_transform_records.size() ; 
			// write out number of isotopic signatures detected. 
			fout.write((char *)&num_isotopic_peaks, sizeof(int)) ; 
			const int isotope_fit_record_size = sizeof(Engine::HornTransform::IsotopeFitRecord) ; 
			char *temp = new char [num_isotopic_peaks*isotope_fit_record_size] ; 

			for (int fit_record_num = 0 ; fit_record_num < num_isotopic_peaks ; fit_record_num++)
			{
				Engine::HornTransform::IsotopeFitRecord *record_ptr = 
					(Engine::HornTransform::IsotopeFitRecord *) &temp[fit_record_num*isotope_fit_record_size] ; 
				*record_ptr = mvect_transform_records[fit_record_num] ; 
			}
			fout.write(temp, num_isotopic_peaks*isotope_fit_record_size) ; 
			fout.close() ; 		
			delete [] temp ; 

			mptr_iso_file->close() ; 
			delete mptr_iso_file ; 
			mptr_iso_file = NULL ; 

			mptr_peak_file->close() ; 
			delete mptr_peak_file ; 
			mptr_peak_file = NULL ; 

		}


		//__declspec(align(4)) struct SavePeak
		//{
		//	int min_scan ; 
		//	double mdbl_mz ; 
		//	double mdbl_intensity ; 
		//};
		void LCMSTransformResults::LoadResultsV1Data(char *data_file_name)
		{
			mbln_save_structs_in_memory = true ;
			const int SIZE_OF_LC_PEAK = sizeof(LCMSPeak<PeakMinInfo>) ; 
			ClearPeaksAndIso() ; 
			const int INT_SIZE = sizeof(int) ; 

			std::ifstream fin(data_file_name, std::ios::binary) ; 
			int filename_len = 0 ; 
			fin.read((char *)&filename_len, INT_SIZE) ; 
			fin.read(marr_file_name, filename_len) ; 
			marr_file_name[filename_len] = '\0' ; 

			char version_str[32] ;
			int version_str_len = 0 ; 

			fin.read((char *)&version_str_len, INT_SIZE)  ; 
			fin.read(version_str, version_str_len) ; 

			// how many peaks do we need to read ? 
			int num_pks = 0 ; 
			fin.read((char *)&num_pks, INT_SIZE) ; 
			int num_read = (int) fin.gcount() ; 

			const int pk_size = sizeof(LCMSPeak<PeakMinInfo>) ; 
			mint_num_peaks_stored = 0 ; 
			while (mint_num_peaks_stored < num_pks)
			{
				LCMSPeak<PeakMinInfo> *pk_ptr = new LCMSPeak<PeakMinInfo> [NUM_PEAKS_IN_BLOCK] ; 
				mvect_peak_blocks_ptr.push_back(pk_ptr) ;
				int num_to_read = num_pks - mint_num_peaks_stored ; 
				if (NUM_PEAKS_IN_BLOCK  < num_to_read)
					num_to_read = NUM_PEAKS_IN_BLOCK ; 
				fin.read((char *)pk_ptr, num_to_read * SIZE_OF_LC_PEAK) ; 
				num_read = (int) fin.gcount() ; 
				mint_num_peaks_stored += num_to_read ; 
				mint_num_peaks_left_in_current_block = NUM_PEAKS_IN_BLOCK - num_to_read ;
			}

			int num_isotopic_peaks = 0 ; 
			fin.read((char *)&num_isotopic_peaks, INT_SIZE) ; 
			const int isotope_fit_record_size = sizeof(Engine::HornTransform::IsotopeFitRecord) ; 
			char *temp = new char [num_isotopic_peaks*isotope_fit_record_size] ; 
			fin.read(temp, num_isotopic_peaks*isotope_fit_record_size) ; 
			num_read = (int) fin.gcount() ; 

			mvect_transform_records.reserve(num_isotopic_peaks) ; 
			for (int fit_record_num = 0 ; fit_record_num < num_isotopic_peaks ; fit_record_num++)
			{
				Engine::HornTransform::IsotopeFitRecord *record_ptr = 
					(Engine::HornTransform::IsotopeFitRecord *) &temp[fit_record_num*isotope_fit_record_size] ; 
				mvect_transform_records.push_back(*record_ptr) ; 
			}

			if (num_pks != 0)
			{
				mint_min_scan = mvect_peak_blocks_ptr.at(0)->mint_scan_num ; 
				mint_max_scan = mvect_peak_blocks_ptr.at(mvect_peak_blocks_ptr.size()-1)[NUM_PEAKS_IN_BLOCK - mint_num_peaks_left_in_current_block-1].mint_scan_num ; 
				mint_last_scan_added = mint_max_scan ; 
			}
			else
			{
				mint_min_scan = mint_max_scan = mint_last_scan_added = -1 ; 
			}

			delete [] temp ; 
			fin.close() ; 

			CreateIndexesOnData() ; 
		}

		void LCMSTransformResults::SaveResultsV1(char *file_name)
		{
			//need to save three types of files: 
			// 1. CSV File
			// 2. Scan File
			// 3. Raw Data file (.dat file).
			char iso_file_name[512] ; 
			strcpy(iso_file_name, file_name) ; 
			strcat(iso_file_name, "_isos.csv") ; 
			SaveResultsV1Iso(iso_file_name) ; 

			char scan_file_name[512] ; 
			strcpy(scan_file_name, file_name) ; 
			strcat(scan_file_name, "_scans.csv") ; 
			SaveResultsV1Scan(scan_file_name) ; 

			char data_file_name[512] ; 
			strcpy(data_file_name, file_name) ; 
			strcat(data_file_name, "_peaks.dat") ; 
			SaveResultsV1Data(data_file_name) ; 

		}


		void LCMSTransformResults::SaveResults(char *file_name)
		{
			SaveResultsV1(file_name) ; 
		}
		void LCMSTransformResults::LoadResults(char *file_name)
		{
			LoadResultsV1Data(file_name) ; 
		}

		void LCMSTransformResults::CreateIndexesOnData()
		{
			if (mvect_peak_blocks_ptr.size() == 0)
				return ; 

			mmap_scan_peak_indices.clear() ; 
			int num_peaks = mint_num_peaks_stored ; 
			int last_scan = -1 ; 

			Engine::Results::LCMSPeak<PeakMinInfo> current_peak ; 

			Engine::Results::LCMSPeak<PeakMinInfo> *pkPtr = mvect_peak_blocks_ptr[0] ; 
			mint_min_scan = mint_max_scan = pkPtr[0].mint_scan_num ; 

			int current_block = 0 ; 
			int num_blocks = mvect_peak_blocks_ptr.size() ; 
			int pk_num_in_block = 0 ;
			for (int pk_num = 0 ; pk_num < num_peaks ; pk_num++)
			{
				current_peak = *pkPtr ; 
				if (current_peak.mint_scan_num != last_scan)
				{
					if (current_peak.mint_scan_num > mint_max_scan)
						mint_max_scan = current_peak.mint_scan_num ; 

					// so we have reached the next scan where peaks were seen.
					// It can be the case that no peaks were seen for intermediate peaks.
					for (int scan_num = last_scan + 1 ; scan_num <= current_peak.mint_scan_num ; scan_num++)
					{
						mmap_scan_peak_indices.insert(std::pair<int,int>(scan_num,pk_num)) ; 
					}
					last_scan = current_peak.mint_scan_num ; 
				}
				pk_num_in_block++ ; 
				if (pk_num_in_block == NUM_PEAKS_IN_BLOCK)
				{
					current_block++ ; 
					if (current_block != num_blocks)
					{
						pkPtr = mvect_peak_blocks_ptr[current_block] ; 
					}
					pk_num_in_block = 0 ; 
				}
				else
				{
					pkPtr++ ; 
				}
			}
		}

		void LCMSTransformResults::GetSIC(int min_scan, int max_scan, float min_mz, float max_mz, std::vector<float> &vect_intensities)
		{
			const int SIZE_OF_LC_PEAK = sizeof(LCMSPeak<PeakMinInfo>) ; 
			// go through each scan and find peaks. 

			if ((min_scan < mint_min_scan && max_scan < mint_min_scan)
					|| (min_scan > mint_max_scan && max_scan > mint_max_scan))
			{
				for (int scan_num = min_scan ; scan_num < max_scan ; scan_num++)
					vect_intensities.push_back(0) ; 
				return ; 
			}

			int num_peaks = mint_num_peaks_stored ; 
			LCMSPeak<PeakMinInfo> *peakPtr ;

			for (int current_scan = min_scan ; current_scan <= max_scan ; current_scan++)
			{
				if (current_scan < mint_min_scan || current_scan > mint_max_scan)
				{
					vect_intensities.push_back(0) ; 
				}
				else
				{
					int scan_start_index = mmap_scan_peak_indices[current_scan] ; 
					int scan_stop_index = num_peaks ;  
					if (current_scan < mint_max_scan - 1)
						scan_stop_index = mmap_scan_peak_indices[current_scan+1] ; 

					if (scan_start_index == scan_stop_index)
					{
						vect_intensities.push_back(0) ; 
					}
					else
					{
						double max_intensity = 0 ; 
						int current_block = scan_start_index/NUM_PEAKS_IN_BLOCK ; 
						peakPtr = mvect_peak_blocks_ptr[current_block] ; 
						int in_block_index = scan_start_index % NUM_PEAKS_IN_BLOCK ; 
						peakPtr += in_block_index  ; 
						for (int mz_index = scan_start_index ; mz_index < scan_stop_index ; mz_index++)
						{
							double current_intensity = (double) peakPtr->mobj_peak.mflt_intensity ; 
							double current_mz = (double) peakPtr->mobj_peak.mflt_mz ; 
							if (current_mz > max_mz)
								break ;

							if (current_mz > min_mz && current_intensity > max_intensity)
							{
								max_intensity = current_intensity ; 
							}
							in_block_index++ ; 
							if (in_block_index == NUM_PEAKS_IN_BLOCK)
							{
								current_block++ ; 
								peakPtr = mvect_peak_blocks_ptr[current_block] ; 
								in_block_index = 0 ; 
							}
							else
							{
								peakPtr++ ; 
							}
						}
						vect_intensities.push_back(max_intensity) ; 
					}
				}
			}
		}

		void LCMSTransformResults::GetScanPeaks(int scan_num, std::vector<float> &vect_mzs, std::vector<float> &vect_intensities)
		{
			if (scan_num < mint_min_scan || scan_num > mint_max_scan)
			{
				return ; 
			}
			int scan_start_index = mmap_scan_peak_indices[scan_num] ; 
			int scan_stop_index = mint_num_peaks_stored ;  
			if (scan_num < mint_max_scan - 1)
				scan_stop_index = mmap_scan_peak_indices[scan_num+1] ; 

			if (scan_start_index == scan_stop_index)
				return ; 
			int num_blocks = mvect_peak_blocks_ptr.size() ; 
			int current_block = scan_start_index/NUM_PEAKS_IN_BLOCK ; 
			int in_block_index = scan_start_index % NUM_PEAKS_IN_BLOCK ; 
			LCMSPeak<PeakMinInfo> *peakPtr = mvect_peak_blocks_ptr[current_block] ; 
			peakPtr += in_block_index ; 

			for (int pt_index = scan_start_index ; pt_index < scan_stop_index ; pt_index++)
			{
				float current_intensity = peakPtr->mobj_peak.mflt_intensity  ; 
				float current_mz = peakPtr->mobj_peak.mflt_mz ; 
				vect_mzs.push_back(current_mz) ; 
				vect_intensities.push_back(current_intensity) ; 
				in_block_index++ ; 
				if (in_block_index == NUM_PEAKS_IN_BLOCK)
				{
					current_block++ ; 
					if (current_block == num_blocks)
						break ; 
					peakPtr = mvect_peak_blocks_ptr[current_block] ; 
					in_block_index = 0 ; 
				}
				else
				{
					peakPtr++ ; 
				}
			}

		}


	}
}