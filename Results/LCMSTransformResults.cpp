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
			mbln_save_UIMF = false ; 
			Reset() ; 
			mbln_save_structs_in_memory = false; 
			PEAK_BLOCK_SIZE = 64 * 1024 ; 
			mdeque_peaks.SetBlockSize(PEAK_BLOCK_SIZE) ; 
			mint_num_peaks_stored = 0 ; 
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
				int res = remove(marr_temp_iso_file) ; 
				res = remove(marr_temp_peak_file) ; 
			}
			ClearPeaksAndIso() ; 

		}
		
		void LCMSTransformResults::ClearPeaksAndIso()
		{
			mdeque_peaks.Clear() ; 
			mdeque_transforms.Clear() ; 
			// mint_num_peaks_stored and mint_num_isos_stored are not set to 0, unless reset is called,
			// because these files might yet be in the temp files. 
		}

		void LCMSTransformResults::Reset()
		{
			mbln_sorted = true ;
			mint_last_scan_added = -1 ; 
			mbln_deisotoped = false ; 
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

			mint_num_peaks_stored = 0 ; 
			mint_num_iso_stored = 0 ; 
			ClearPeaksAndIso() ; 
			mbln_data_in_memory_is_incomplete = false ; 

		}
		
		void LCMSTransformResults::AddInfoForUIMFScan(int scan, double bp_mz, double bp_intensity, 
			double tic, double signal_range, int num_peaks, int num_deisotoped, double time, 
			short ms_level, double drift_time)
		{
			mbln_save_UIMF = true ; 
			mmap_scan_bp_mz.insert(std::pair<int, double>(scan, bp_mz)) ; 
			mmap_scan_bp_intensity.insert(std::pair<int, double>(scan, bp_intensity)) ; 
			mmap_scan_tic.insert(std::pair<int, double>(scan, tic)) ; 
			mmap_scan_num_peaks.insert(std::pair<int, int>(scan, num_peaks)) ; 
			mmap_scan_num_deisotoped.insert(std::pair<int, int>(scan, num_deisotoped)) ; 
			mmap_scan_num_mslevel.insert(std::pair<int, short>(scan, ms_level)) ; 
			mmap_scan_time.insert(std::pair<int, double>(scan, time)) ; 
			mmap_scan_num_drift_time.insert(std::pair<int, double> (scan, drift_time)) ; 
			mmap_scan_signal_range.insert(std::pair<int, double>(scan, signal_range)) ; 
		}


		void LCMSTransformResults::AddInfoForScan(int scan, double bp_mz, double bp_intensity, 
			double tic, double signal_range, int num_peaks, int num_deisotoped, double time, 
			short ms_level)
		{
			mmap_scan_bp_mz.insert(std::pair<int, double>(scan, bp_mz)) ; 
			mmap_scan_bp_intensity.insert(std::pair<int, double>(scan, bp_intensity)) ; 
			mmap_scan_tic.insert(std::pair<int, double>(scan, tic)) ; 
			mmap_scan_num_peaks.insert(std::pair<int, int>(scan, num_peaks)) ; 
			mmap_scan_num_deisotoped.insert(std::pair<int, int>(scan, num_deisotoped)) ; 
			mmap_scan_num_mslevel.insert(std::pair<int, short>(scan, ms_level)) ; 
			mmap_scan_time.insert(std::pair<int, double>(scan, time)) ; 		
			mmap_scan_signal_range.insert(std::pair<int, double>(scan, signal_range)) ; 
		}


		void LCMSTransformResults::AddPeaksForScan(int scan_num,
			std::vector<Engine::PeakProcessing::Peak> &vect_peaks)
		{
			try
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
				mint_num_peaks_stored += num_peaks ; 
				// Create LCMSPeaks.
				std::vector<LCMSPeak<PeakMinInfo> > vect_lc_peaks ; 
				vect_lc_peaks.reserve(vect_peaks.size()) ;
				LCMSPeak<PeakMinInfo> lc_peak ;
				Engine::PeakProcessing::Peak pk ; 
				for (int pk_num = 0 ; pk_num < num_peaks ; pk_num++)
				{
					pk = vect_peaks[pk_num] ; 
					lc_peak.mint_scan_num = scan_num ; 
					lc_peak.mobj_peak.mflt_intensity = (float) pk.mdbl_intensity ; 
					lc_peak.mobj_peak.mflt_mz = (float) pk.mdbl_mz ; 
					vect_lc_peaks.push_back(lc_peak) ; 
				}
				
				// First thing we do is write the peaks to temporary peaks file. 
				// We take a pointer to the first element in vector because we know 
				// that it stores data in contiguous memory blocks.
				if (num_peaks != 0)
					mptr_peak_file->write((char *)&vect_lc_peaks[0], num_peaks * SIZE_OF_LC_PEAK) ; 

				if (!mbln_save_structs_in_memory)
					return ; 
				// Now lets add the new data to our data structures.
				mdeque_peaks.Append(vect_lc_peaks) ; 
			}
			catch (std::bad_alloc &bad)
			{
				mdeque_peaks.Clear() ; 
				mdeque_transforms.Clear() ; 
				mbln_data_in_memory_is_incomplete = true ; 
			}
		}

		void LCMSTransformResults::AddTransforms(std::vector<Engine::HornTransform::IsotopeFitRecord> &vect_fit_results)
		{
			try
			{
				if (vect_fit_results.size() == 0)
					return ; 

				mint_num_iso_stored += vect_fit_results.size() ; 
				const int SIZE_OF_ISO = sizeof(Engine::HornTransform::IsotopeFitRecord) ; 
				// First thing we do is write the isotope fit records to temporary isotope records file. 
				// We take a pointer to the first element in vector because we know 
				// that it stores data in contiguous memory blocks.
				if (vect_fit_results.size() != 0)
					mptr_iso_file->write((char *)&vect_fit_results[0], vect_fit_results.size() * SIZE_OF_ISO) ; 
				if (!mbln_save_structs_in_memory)
					return ; 
				mdeque_transforms.Append(vect_fit_results) ;
			}
			catch (std::bad_alloc &bad)
			{
				if (mbln_save_structs_in_memory)
				{
					mdeque_peaks.Clear() ; 
					mdeque_transforms.Clear() ; 
				}
				mbln_data_in_memory_is_incomplete = true ; 
			}
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
			//Preserve coloumn headers for VIPER to load it in
			fout<<"scan_num"<<","<<"charge"<<","<<"abundance"<<","<<"mz";
			fout<<","<<"fit"<<","<<"average_mw"<<","<<"monoisotopic_mw"<<","<<"mostabundant_mw";
			fout<<","<<"fwhm"<<","<<"signal_noise"<<","<<"mono_abundance"<<","<<"mono_plus2_abundance";
			fout <<"\n" ;

			fout.precision(4) ; 
			fout.setf(std::ios::fixed, std::ios::floatfield);

			if (mbln_save_structs_in_memory && !mbln_data_in_memory_is_incomplete)
			{
				// write from stored data in mdeque_isotopes
				Engine::Utilities::BlockDeque<Engine::HornTransform::IsotopeFitRecord >::iterator iter ; 
				Engine::Utilities::BlockDeque<Engine::HornTransform::IsotopeFitRecord >::iterator iter_first = mdeque_transforms.begin() ; 
				Engine::Utilities::BlockDeque<Engine::HornTransform::IsotopeFitRecord >::iterator iter_last = mdeque_transforms.end() ; 

				for (iter = iter_first ; iter != iter_last ; iter++)
				{
					Engine::HornTransform::IsotopeFitRecord record = *iter ; 
					fout<<record.mint_scan_num<<","<<record.mshort_cs<<","<<record.mint_abundance<<","<<record.mdbl_mz ; 
					fout<<","<<record.mdbl_fit<<","<<record.mdbl_average_mw<<","<<record.mdbl_mono_mw<<","<<record.mdbl_most_intense_mw ; 
					fout<<","<<record.mdbl_fwhm<<"," ; 
					fout.precision(2) ; 
					fout<<record.mdbl_sn<<"," ; 
					fout.precision(4) ; 
					fout<<record.mint_mono_intensity<<"," ; 
					fout<<record.mint_iplus2_intensity<<"\n" ; 
				}
			}
			else
			{
				// Read in from stored file, little at a time and write it out;
				mptr_iso_file->seekg(0, std::ios::beg) ; 
				mptr_iso_file->flush() ; 
				mptr_iso_file->clear() ;

				bool fail = mptr_iso_file->fail() ; 
				if (fail || mptr_iso_file->tellg() != (std::ios::pos_type) 0)
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
					int num_read = mptr_iso_file->gcount()/SIZE_OF_ISO ; 
					if (num_read != 0)
					{
						for (int iso_num = 0 ; iso_num != num_read ; iso_num++)
						{
							Engine::HornTransform::IsotopeFitRecord record = temp[iso_num] ; 
							fout<<record.mint_scan_num<<","<<record.mshort_cs<<","<<record.mint_abundance<<","<<record.mdbl_mz ; 
							fout<<","<<record.mdbl_fit<<","<<record.mdbl_average_mw<<","<<record.mdbl_mono_mw<<","<<record.mdbl_most_intense_mw ; 
							fout<<","<<record.mdbl_fwhm<<"," ; 
							fout.precision(2) ; 
							fout<<record.mdbl_sn<<"," ; 
							fout.precision(4) ; 
							fout<<record.mint_mono_intensity<<"," ; 
							fout<<record.mint_iplus2_intensity<<"\n" ; 
						}
					}
					else
					{
						mptr_iso_file->close();						
						fout.close() ; 
						return ; 
					}
				}
				mptr_iso_file->clear() ; 

			}
			fout.close();
		}

		void LCMSTransformResults::SaveResultsV1Scan(char *scan_file_name, bool save_signal_range)
		{
			std::ofstream fout(scan_file_name);				 						
			//Preserve coloumn headers for VIPER to load it in
			fout<<"scan_num,scan_time,type,bpi,bpi_mz,tic,num_peaks,num_deisotoped" ; 

			if (save_signal_range)
				fout<<",time_domain_signal\n" ; 
			else
				fout<<"\n";

			fout.precision(4) ; 
			fout.setf(std::ios::fixed, std::ios::floatfield);

			for (std::map<int,double>::iterator iter = mmap_scan_bp_intensity.begin() ; iter != mmap_scan_bp_intensity.end() ; 
				iter++)
			{
				int scan_num = (*iter).first ; 
				fout<<scan_num<<","<<mmap_scan_time[scan_num]<<","<<mmap_scan_num_mslevel[scan_num]<<","<<mmap_scan_bp_intensity[scan_num]<<","<<mmap_scan_bp_mz[scan_num]<<"," ; 					
				fout<<mmap_scan_tic[scan_num]<<","<<mmap_scan_num_peaks[scan_num]<<","<<mmap_scan_num_deisotoped[scan_num] ; 
				if(save_signal_range)
				{
					fout<<","<<mmap_scan_signal_range[scan_num]<<"\n" ; 
				}
				else
				{
					fout<<"\n" ; 
				}
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

			// write out number of isotopic signatures detected. 
			fout.write((char *)&mint_num_iso_stored, sizeof(int)) ; 
			WriteTempIsosToFile(&fout) ; 
			fout.close() ; 		

			mptr_iso_file->close() ; 
			delete mptr_iso_file ; 
			mptr_iso_file = NULL ; 

			mptr_peak_file->close() ; 
			delete mptr_peak_file ; 
			mptr_peak_file = NULL ; 

			int res = remove(marr_temp_iso_file) ; 
			res = remove(marr_temp_peak_file) ; 

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
			mbln_data_in_memory_is_incomplete = false ; 

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
			fin.read((char *)&mint_num_peaks_stored, INT_SIZE) ; 
			int num_read = (int) fin.gcount() ; 

			const int num_peaks_read_per_batch = 64 * 1024 ; 
			
			std::vector<LCMSPeak<PeakMinInfo> > vect_lc_peaks ; 
			vect_lc_peaks.resize(num_peaks_read_per_batch) ; 

			int total_read = 0; 
			while (total_read < mint_num_peaks_stored)
			{
				if (num_peaks_read_per_batch + total_read > mint_num_peaks_stored)
				{
					fin.read((char *)&vect_lc_peaks[0], (mint_num_peaks_stored - total_read) * SIZE_OF_LC_PEAK) ; 
				}
				else
				{
					fin.read((char *)&vect_lc_peaks[0], num_peaks_read_per_batch * SIZE_OF_LC_PEAK) ; 
				}
				num_read = (int) fin.gcount() / SIZE_OF_LC_PEAK ;
				mdeque_peaks.Append(vect_lc_peaks, 0, num_read) ; 
				total_read += num_read ; 
			}
			vect_lc_peaks.clear() ; 

			bool read_transforms = true ; 
			if (read_transforms)
			{
				fin.read((char *)&mint_num_iso_stored, INT_SIZE) ; 
				const int SIZE_OF_ISO = sizeof(Engine::HornTransform::IsotopeFitRecord) ; 
				const int num_iso_read_per_batch = 1024 ; 

				std::vector<Engine::HornTransform::IsotopeFitRecord > vect_isos ; 
				vect_isos.resize(num_iso_read_per_batch) ; 

				while (mdeque_transforms.size() < mint_num_iso_stored)
				{
					fin.read((char *)&vect_isos[0], num_iso_read_per_batch * SIZE_OF_ISO) ; 
					num_read = (int) fin.gcount() / SIZE_OF_ISO ; 
					mdeque_transforms.Append(vect_isos, 0, num_read) ; 
				}
			}

			if (mint_num_peaks_stored != 0)
			{
				mint_min_scan = mdeque_peaks.GetPoint(0).mint_scan_num ; 
				mint_max_scan = mdeque_peaks.GetPoint(mdeque_peaks.size()-1).mint_scan_num ; 
				mint_last_scan_added = mint_max_scan ; 
			}
			else
			{
				mint_min_scan = mint_max_scan = mint_last_scan_added = -1 ; 
			}

			fin.close() ; 

			CreateIndexesOnData() ; 
		}

		void LCMSTransformResults::SaveResultsV1(char *file_name, bool save_signal_range)
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
			SaveResultsV1Scan(scan_file_name, save_signal_range) ; 

			char data_file_name[512] ; 
			strcpy(data_file_name, file_name) ; 
			strcat(data_file_name, "_peaks.dat") ; 
			SaveResultsV1Data(data_file_name) ; 

		}


		void LCMSTransformResults::SaveResults(char *file_name, bool save_signal_range)
		{
			SaveResultsV1(file_name, save_signal_range) ; 
		}
		void LCMSTransformResults::LoadResults(char *file_name)
		{
			LoadResultsV1Data(file_name) ; 
		}

		void LCMSTransformResults::CreateIndexesOnData()
		{
			if (mdeque_peaks.size() == 0)
				return ; 

			mmap_scan_peak_indices.clear() ; 
			int num_peaks = mdeque_peaks.size() ; 
			int last_scan = -1 ; 

			Engine::Results::LCMSPeak<PeakMinInfo> current_peak ; 

			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter ; 
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter_first = mdeque_peaks.begin() ; 
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter_last = mdeque_peaks.end() ; 

			current_peak = *iter_first ; 
			mint_min_scan = mint_max_scan = current_peak.mint_scan_num ; 
			int pk_num = 0 ; 
			for (iter = iter_first ; iter != iter_last ; iter++)
			{
				current_peak = *iter ; 
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
				pk_num++ ;
			}
		}		
		
		void LCMSTransformResults::GetSIC(int min_scan, int max_scan, float min_mz, float max_mz, std::vector<float> &vect_intensities)
		{
			// go through each scan and find peaks. 
			if ((min_scan < mint_min_scan && max_scan < mint_min_scan)
					|| (min_scan > mint_max_scan && max_scan > mint_max_scan))
			{
				for (int scan_num = min_scan ; scan_num < max_scan ; scan_num++)
					vect_intensities.push_back(0) ; 
				return ; 
			}

			int num_peaks = mdeque_peaks.size() ; 
			LCMSPeak<PeakMinInfo> lc_peak ;
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter ; 
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter_first  ; 
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter_last ; 

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
						iter_first = mdeque_peaks.get_iter(scan_start_index) ; 
						iter_last = mdeque_peaks.get_iter(scan_stop_index) ; 
						for (iter = iter_first ; iter != iter_last ; iter++)
						{
							lc_peak = *iter ; 
							double current_intensity = (double) lc_peak.mobj_peak.mflt_intensity ; 
							double current_mz = (double) lc_peak.mobj_peak.mflt_mz ; 
							if (current_mz > max_mz)
								break ;

							if (current_mz > min_mz && current_intensity > max_intensity)
							{
								max_intensity = current_intensity ; 
							}
						}
						vect_intensities.push_back((float)max_intensity) ; 
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
			int scan_stop_index = mdeque_peaks.size() ;  
			if (scan_num < mint_max_scan - 1)
				scan_stop_index = mmap_scan_peak_indices[scan_num+1] ; 

			if (scan_start_index == scan_stop_index)
				return ; 
			LCMSPeak<PeakMinInfo> lc_peak ;  
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter ; 
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter_first = mdeque_peaks.get_iter(scan_start_index) ; 
			Engine::Utilities::BlockDeque<LCMSPeak<PeakMinInfo> >::iterator iter_last = mdeque_peaks.get_iter(scan_stop_index); 

			for (iter = iter_first ; iter != iter_last ; iter++)
			{
				lc_peak = *iter ; 
				float current_intensity = lc_peak.mobj_peak.mflt_intensity ; 
				float current_mz = lc_peak.mobj_peak.mflt_mz ; 
				vect_mzs.push_back(current_mz) ; 
				vect_intensities.push_back(current_intensity) ; 
			}

		}

	}
}