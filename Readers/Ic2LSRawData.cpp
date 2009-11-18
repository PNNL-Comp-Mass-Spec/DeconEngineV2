// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#define _WIN32_WINNT 0x0400

#include "ICR2LSRawData.h"
#include "../Utilities/helpers.h" 
#include "float.h" 
#include <fstream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include "../Utilities/Apodization.h" 


namespace Engine 
{
	namespace Readers
	{
		bool Icr2lsRawData::LoadFile(char *f_name, int scan_num)
		{
			const int flt_size = sizeof(float) ; 
			std::ifstream fin(f_name, std::ios::binary) ; 
			bool found_data = ReadHeader(fin) ;
			if (!found_data)
			{
				if (fin.is_open())
					fin.close() ; 
				return false ; 
			}
			int pos = fin.tellg() ; 
			if (mint_allocated_size < mint_num_points_in_scan)
			{
				mint_allocated_size = mint_num_points_in_scan ; 
				if (mptr_data != NULL)
				{
					delete [] mptr_data ; 
					delete [] mptr_data_copy ; 
				}
				mptr_data = new float [mint_num_points_in_scan] ; 
				mptr_data_copy = new float [mint_num_points_in_scan] ; 
			}
			fin.get() ; 
			pos = fin.tellg() ; 
			fin.close() ; 
			
			fin.open(f_name, std::ios::binary) ; 
			fin.seekg(pos) ; 
			fin.read((char *)mptr_data, mint_num_points_in_scan * flt_size) ; 
			int num_read = fin.gcount() ; 
			fin.close() ; 
			double max_intensity = -1*DBL_MAX ; 
			double min_intensity = DBL_MAX ; 
			const int interval_size = 2000 ; 
			int skip = (mint_num_points_in_scan - 1) / interval_size + 1 ; 
			bool max_side = true ; // to take reading from max size.
			for (int i = 0 ; i < mint_num_points_in_scan ; i+= skip)
			{
				double current_max_intensity = -1*DBL_MAX ; 
				double current_min_intensity = DBL_MAX ; 

				for (int j = i ; j < mint_num_points_in_scan && j < i + skip ; j++)
				{
					if (current_max_intensity < mptr_data[j])
						current_max_intensity = mptr_data[j] ; 
					if (current_min_intensity > mptr_data[j])
						current_min_intensity = mptr_data[j] ; 
				}
				double current_intensity ; 

				if (max_side)
				{
					current_intensity = current_max_intensity ; 
					max_side = false ; 
				}
				else
				{
					current_intensity = current_min_intensity ; 
					max_side = true ; 
				}

				if (max_intensity < current_intensity)
					max_intensity = current_intensity ; 
				if (min_intensity > current_intensity)
					min_intensity = current_intensity ; 
			}
			mdbl_signal_range = (max_intensity - min_intensity) ; 

			if (mbln_tic_file)
				menm_calibration_type = (CalibrationType) 5 ;

			if (menmApodizationType != Engine::Utilities::NOAPODIZATION)
				Engine::Utilities::Apodization::Apodize(mdbl_apodization_min_x, mdbl_apodization_max_x, 
				mdbl_sample_rate, false, menmApodizationType, mptr_data, mint_num_points_in_scan, 
				mint_apodization_apex_percent) ; 

			if (mshort_num_zeros != 0)
			{
				mint_allocated_size = mint_num_points_in_scan * (1<<mshort_num_zeros) ; 
				float *temp = new float [mint_allocated_size] ; 
				memcpy(temp, mptr_data, mint_num_points_in_scan*sizeof(float)) ; 
				for (int zeroIndex = mint_num_points_in_scan ; zeroIndex < mint_allocated_size ; 
					zeroIndex++)
				{
					temp[zeroIndex] = 0 ; 
				}
				if (mptr_data != NULL)
				{
					delete [] mptr_data ; 
					delete [] mptr_data_copy ; 
				}
				mptr_data = temp ; 
				mptr_data_copy = new float [mint_allocated_size] ; 
			}


			if (!mbln_use_specified_calibration || mobj_calibrator == NULL)
			{
				Calibrations::CCalibrator *calib = new Calibrations::CCalibrator(menm_calibration_type) ; 
				calib->SetSize(mint_num_points_in_scan) ; 
				calib->SetLowMassFrequency(mdbl_low_mass_freq) ; 
				calib->SetSampleRate(mdbl_sample_rate);
				
				//[gord] this hack is meant to reverse the sign of calibrationConstantB, resulting in the correct
				//m/z calculation for CalibrationType 9
				if (menm_calibration_type == (CalibrationType)9)
				{
					mdbl_calib_const_b = -1 * mdbl_calib_const_b;
				}


				calib->SetCalibrationEquationParams(mdbl_calib_const_a, mdbl_calib_const_b, mdbl_calib_const_c);
				if (mobj_calibrator != NULL)
				{
					delete mobj_calibrator ; 
					mobj_calibrator = 0 ; 
				}
				SetCalibrator(calib) ; 
			}
			else
			{
				mobj_calibrator->SetSize(mint_num_points_in_scan) ; 
				mobj_calibrator->SetLowMassFrequency(mdbl_low_mass_freq) ; 
				mobj_calibrator->SetSampleRate(mdbl_sample_rate);
			}

			return true ; 
		}

		
		void Icr2lsRawData::OverrideDefaultCalibrator(CalibrationType calibType, double A, double B, double C)
		{
			mbln_use_specified_calibration = true ; 
			menm_calibration_type = calibType ; 
			Calibrations::CCalibrator *calib = new Calibrations::CCalibrator(menm_calibration_type) ; 
			calib->SetSize(mint_num_points_in_scan) ; 
			calib->SetLowMassFrequency(mdbl_low_mass_freq) ; 
			calib->SetSampleRate(mdbl_sample_rate);
			mdbl_calib_const_a = A ; 
			mdbl_calib_const_b = B ; 
			mdbl_calib_const_c = C ; 
			calib->SetCalibrationEquationParams(mdbl_calib_const_a, mdbl_calib_const_b, mdbl_calib_const_c);
			if (mobj_calibrator != NULL)
			{
				delete mobj_calibrator ; 
				mobj_calibrator = 0 ; 
			}
			SetCalibrator(calib) ; 
		}

		void Icr2lsRawData::SetIsTic(bool is_tic) 
		{
			mbln_tic_file = is_tic ; 
		}


		inline void GetLine(std::ifstream &fin, char *buffer, const int max_size)
		{
			int copy_pt = 0 ; 
			char copy_char ;
			bool start_copy = false ; 
			while(copy_pt < max_size-1 && (copy_char = fin.get()) != '\n')
			{
				if (isalpha(copy_char))
					start_copy = true ; 
				if (start_copy)
					buffer[copy_pt++] = copy_char ; 
			}
			buffer[copy_pt] = '\0'; 
			return ; 
		}
		bool Icr2lsRawData::ReadHeader(std::ifstream &fin)
		{
			const int BUFFER_SIZE = 512 ; 
			char buffer[BUFFER_SIZE] ; 

			const char *comment_tag = "Comment:" ; 
			const char *commentend_tag = "CommentEnd" ; 
			const char *end_tag = "End" ; 
			const char *file_type_tag = "FileType:" ; 
			const char *data_type_tag = "DataType:" ; 
			const char *num_samples_tag = "NumberOfSamples:" ; 
			const char *sample_rate_tag = "SampleRate:" ;
			const char *low_mass_freq_tag = "LowMassFreq:" ; 
			const char *freq_shift_tag = "FreqShift:" ; 
			const char *number_of_segments_tag = "NumberSegments:" ; 
			const char *max_point_tag = "MaxPoint:" ; 
			const char *cal_type_tag = "CalType:" ; 
			const char *cal_a_tag = "CalA:" ;
			const char *cal_b_tag = "CalB:" ;
			const char *cal_c_tag = "CalC:" ;
			const char *intensity_tag = "Intensity:" ;
			const char *zero_fills_tag = "ZeroFills:" ;
			const char *xmin_tag = "CurrentXmin:" ; 
			const char *xmax_tag = "CurrentXmax:" ; 

			while (!fin.eof())
			{
				GetLine(fin, buffer, BUFFER_SIZE) ; 
				if (_strnicmp(buffer, end_tag, strlen(end_tag)) == 0)
					break ; 
				if (_strnicmp(buffer, comment_tag, strlen(comment_tag)) == 0)
				{
					while(!fin.eof())
					{
						GetLine(fin, buffer, BUFFER_SIZE) ; 
						if (_strnicmp(buffer, commentend_tag, strlen(commentend_tag)) == 0)
						{
							break ; 
						}
					}
				}
				char *ptr = NULL ; 
				if ((ptr = strstr(buffer,file_type_tag))!= NULL)
				{
					ptr += strlen(file_type_tag) ; 
					menm_file_type = (Icr2lsFileType) atoi(ptr) ; 
				}
				if ((ptr = strstr(buffer,data_type_tag))!= NULL)
				{
					ptr += strlen(data_type_tag) ; 
					menm_data_type = (Icr2lsDataType) atoi(ptr) ; 
				}
				if ((ptr = strstr(buffer,num_samples_tag)) != NULL)
				{
					ptr += strlen(num_samples_tag) ; 
					mint_num_points_in_scan = atoi(ptr) ; 
				}
				if ((ptr = strstr(buffer,sample_rate_tag)) != NULL)
				{
					ptr += strlen(sample_rate_tag) ; 
					mdbl_sample_rate = atof(ptr) ; 
				}
				if ((ptr = strstr(buffer,low_mass_freq_tag)) != NULL)
				{
					ptr += strlen(low_mass_freq_tag) ; 
					mdbl_low_mass_freq = atof(ptr) ; 
				}
				if ((ptr = strstr(buffer,freq_shift_tag)) != NULL)
				{
					ptr += strlen(freq_shift_tag) ; 
					mdbl_freq_shift = atof(ptr) ; 
				}
				if ((ptr = strstr(buffer,number_of_segments_tag)) != NULL)
				{
					//forgeddd abouddd idd!!
				}
				if ((ptr = strstr(buffer,max_point_tag)) != NULL)
				{
					//forgeddd abouddd idd!!
				}
				if ((ptr = strstr(buffer,cal_type_tag)) != NULL)
				{
					ptr += strlen(cal_type_tag) ; 
					int cal_num = atoi(ptr) ;
					menm_calibration_type = (CalibrationType) cal_num ; 
				}
				if ((ptr = strstr(buffer,cal_a_tag )) != NULL)
				{
					ptr += strlen(cal_a_tag) ; 
					mdbl_calib_const_a = atof(ptr) ; 
				}
				if ((ptr = strstr(buffer,cal_b_tag)) != NULL)
				{
					ptr += strlen(cal_b_tag) ; 
					mdbl_calib_const_b = atof(ptr) ; 
					// somehow, because of a nice quirk in the s/w, this is stored negatively.

					
					mdbl_calib_const_b = -1 * mdbl_calib_const_b ;     //Gord says:  this might be the source of the discrepancy 
																	// for CalibrationType 9; between ICR2LS and DeconTools!
																	// See the LoadFile method and the adjustment made downstream of this method call
					
					
				}
				if ((ptr = strstr(buffer,cal_c_tag)) != NULL)
				{
					ptr += strlen(cal_c_tag) ; 
					mdbl_calib_const_c = atof(ptr) ; 
				}
				if ((ptr = strstr(buffer,intensity_tag)) != NULL)
				{
					ptr += strlen(intensity_tag) ; 
					mdbl_intensity_calibration = atof(ptr) ; 
				}
				if ((ptr = strstr(buffer,zero_fills_tag)) != NULL)
				{
					ptr += strlen(zero_fills_tag) ; 
					mshort_num_zeros_in_read_data = (short) atoi(ptr) ; 
				}
				if ((ptr = strstr(buffer,xmin_tag)) != NULL)
				{
				}
				if ((ptr = strstr(buffer,xmax_tag)) != NULL)
				{
				}
			}
			return true ; 
		}

		bool Icr2lsRawData::LoadFile(int scan_num)
		{
			char file_name[256] ;
			GetFileName(scan_num, file_name) ; 
			LoadFile(file_name, scan_num) ; 
			return true ; 
		}

		void Icr2lsRawData::GetFileName(int scan_num, char *temp_path)
		{
			std::map<int, std::string>::iterator iter = mmap_files_names.find(scan_num) ; 
			if (iter == mmap_files_names.end())
			{
				temp_path[0] = '\0' ; 
				return ; 
			}
			strcpy(temp_path, (*iter).second.c_str()) ; 
		}

		bool Icr2lsRawData::GetRawData(std::vector <double>*mzs, std::vector<double>*intensities, int scan_num) 
		{
			int num_pts = mint_num_points_in_scan * (1<<mshort_num_zeros); 
			return GetRawData(mzs, intensities, scan_num, num_pts) ; 
		}

		double Icr2lsRawData::GetScanTime(int scan_num)
		{
			return scan_num ; 
		}

		double Icr2lsRawData::GetSignalRange(int scan_num) 
		{
			// only returns a value if the current scan is the one we are asking for. 
			if (mint_last_scan_num == scan_num)
				return mdbl_signal_range ; 
			return 0 ; 
		}

		void Icr2lsRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{			
			SetIsTic(true) ; // don't ask why, in fact..forgedd abouddd idddd!!
			int i = 0 ; 
			char file_name[256] ; 
			const int flt_size = sizeof(float) ; 
			int scan_num = 0 ; //as the tic is atored as scan 0 

			GetFileName(scan_num, file_name) ;  
			if (file_name[0] == '\0')
				return  ; 
			mint_last_scan_num = scan_num ; 
			bool loaded = LoadFile(file_name, scan_num) ;
			int	num_pts = mint_num_points_in_scan*(1<<mshort_num_zeros) ; 
			memcpy(mptr_data_copy, mptr_data, mint_num_points_in_scan*(1<<mshort_num_zeros)*flt_size) ; 
			scan_times->clear() ; 
			intensities->clear() ; 
			mobj_calibrator->GetRawPointsApplyFFT(mptr_data, scan_times, intensities, num_pts) ; 
			
		}


		bool Icr2lsRawData::GetRawData(std::vector <double>*mzs, std::vector<double>*intensities, int scan_num, int num_pts) 
		{
			int i = 0 ; 
			char file_name[256] ; 
			const int flt_size = sizeof(float) ; 

			GetFileName(scan_num, file_name) ; 
			if (file_name[0] == '\0')
				return false ; 


			if (scan_num != mint_last_scan_num)
			{
				mint_last_scan_num = scan_num ; 
				bool loaded = LoadFile(file_name, scan_num) ; 
				if (num_pts <= 0 || num_pts > mint_num_points_in_scan*(1<<mshort_num_zeros))
					num_pts = mint_num_points_in_scan*(1<<mshort_num_zeros) ; 
				if (!loaded)
					return false ; 
				memcpy(mptr_data_copy, mptr_data, mint_num_points_in_scan*(1<<mshort_num_zeros)*flt_size) ; 
			}
			else
			{
				if (num_pts <= 0 || num_pts > mint_num_points_in_scan * pow((float)2,(float)mshort_num_zeros))
					num_pts = mint_num_points_in_scan*(1<<mshort_num_zeros) ; 
				memcpy((char *)mptr_data, (char *)mptr_data_copy, num_pts) ; 
			}

			mzs->clear() ; 
			intensities->clear() ; 
			mobj_calibrator->GetRawPointsApplyFFT(mptr_data, mzs, intensities, num_pts) ; 
			return true ; 
		}

		void Icr2lsRawData::AddFilesInDir(char *directory_path, char *folder_name)
		{
			char file_name[512] ; 
			char directory_name[512] ; 
			// found a directory. If its of the form sxxx then get the files from it.
			if ((folder_name[0] == 'S' || folder_name[0] == 's') && 
				(folder_name[1] >= '0' && folder_name[1] <= '9') && 
				(folder_name[2] >= '0' && folder_name[2] <= '9') && 
				(folder_name[3] >= '0' && folder_name[3] <= '9')) 
			{
				WIN32_FIND_DATA FindFileData;
				HANDLE hFind;

				strcpy(file_name, directory_path) ; 
				int len = (int) strlen(directory_path) ; 
				if (directory_path[len-1] != '\\' && directory_path[len-1] != '/')
				{
					strcat(file_name, "\\") ; 
				}
				strcat(file_name, folder_name) ; 
				len = (int) strlen(folder_name) ; 
				if (folder_name[len-1] != '\\' && folder_name[len-1] != '/')
					strcat(file_name, "\\*") ;
				else
					strcat(file_name, "*") ; 

				hFind = FindFirstFile(file_name, &FindFileData);
				BOOL found = true ; 
				while(hFind != INVALID_HANDLE_VALUE && found) 
				{
					DWORD is_dir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ;
					if (is_dir)
					{
						//skip it.
					}
					else
					{
						strcpy(directory_name, directory_path) ; 
						int len = (int) strlen(directory_name) ; 
						if (directory_name[len-1] != '\\' && directory_name[len-1] != '/')
						{
							strcat(directory_name, "\\") ; 
						}
						strcat(directory_name, folder_name) ; 
						len = (int) strlen(directory_name) ; 
						if (directory_name[len-1] != '\\' && directory_name[len-1] != '/')
						{
							strcat(directory_name, "\\") ; 
						}
						strcpy(file_name, FindFileData.cFileName) ; 
						AddScanFile(directory_name, file_name) ;
					}
					found = FindNextFile(hFind, &FindFileData);
				}
				FindClose(hFind) ; 
			}
			return ;
		}

		void Icr2lsRawData::AddScanFile(char *path, char *file_name)
		{
			// add file to list of files. Extract scan number by starting from last letter
			// of file_name. 
			int len_filen = (int) strlen(file_name) ; 
			int len_pathn = (int) strlen(path) ; 
			char full_path[512] ; 
			int start_index = len_filen ; 
			for (int i = len_filen - 1 ; i >= 0 && file_name[i] >= '0' && file_name [i] <= '9' ; i--)
			{
				start_index = i ; 
			}
			if (start_index != len_filen && file_name[start_index-1] == '.')
			{
				// have the right format. 
				int scan_num = atoi(&file_name[start_index]) ;
				strcpy(full_path, path) ; 

				if (full_path[len_pathn-1] != '\\' && full_path[len_pathn] != '/')
					strcat(full_path, "\\") ; 
				strcat(full_path, file_name) ; 
				AddScanFile(full_path, scan_num) ; 
			}
			else
				AddScanFile(file_name, 0) ; //for the TIC
		}

		void Icr2lsRawData::AddScanFile(char *file_name)
		{
			// add file to list of files. Extract scan number by starting from last letter
			// of file_name. 
			int len_filen = (int) strlen(file_name) ; 
			int start_index = len_filen ; 
			for (int i = len_filen - 1 ; i >= 0 && file_name[i] >= '0' && file_name [i] <= '9' ; i--)
			{
				start_index = i ; 
			}
			if (start_index != len_filen && file_name[start_index-1] == '.')
			{
				// have the right format. 
				int scan_num = atoi(&file_name[start_index]) ;
				AddScanFile(file_name, scan_num) ; 
			}
			else
				AddScanFile(file_name, 0) ; 
		}

		void Icr2lsRawData::AddScanFile(char *file_name, int scan_num)
		{
			mmap_files_names.insert(std::pair<int, std::string>(scan_num, file_name)) ; 
		}

		void Icr2lsRawData::Load(char *directory_path)
		{
			char sfile_name[512]; 
			char file_name[512] ; 
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;
			BOOL found = true ; 

			strcpy(marr_file_name, directory_path) ; 
			if (!IsDir(directory_path))
			{
				AddScanFile(directory_path) ;
				return ; 
			}

			strcpy(sfile_name, directory_path) ; 
			int len = (int) strlen(directory_path) ; 
			
			if (directory_path[len-1] != '\\' && directory_path[len-1] != '/')
				strcat(sfile_name, "\\*") ; 
			else 
				strcat(sfile_name, "*") ; 
			
			hFind = FindFirstFile(sfile_name, &FindFileData);
			while(hFind != INVALID_HANDLE_VALUE && found) 
			{
				strcpy(file_name, FindFileData.cFileName) ;
				DWORD is_dir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ; 
				if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
				{
					// current or above directory. Do nothing.
				}
				else if (is_dir)
				{
					// Add files in directory to the list of files to process.
					AddFilesInDir(directory_path, file_name) ; 
				}
				else
				{
					AddScanFile(directory_path, file_name) ;
				}
				found = FindNextFile(hFind, &FindFileData);
			}
			FindClose(hFind);
			// get first file and call load on it. 
			return ; 
	}

		Icr2lsRawData::Icr2lsRawData(void) : mptr_data(0), mptr_data_copy(0), mint_allocated_size (0)
		{
			marr_file_name = new char [MAX_FNAME_LEN] ; 
			mint_last_scan_num = -1 ; 
			mint_num_points_in_scan = 0 ; 
			mint_allocated_size = 0 ;
			mbln_tic_file = false ; 
			menmApodizationType = Engine::Utilities::NOAPODIZATION ;
			mshort_num_zeros = 0 ;
			mbln_use_specified_calibration = false ; 
		} ;

		void Icr2lsRawData::GetScanDescription(int scan, char *description)
		{
			strcpy(description, "Scan #") ; 
			_itoa(scan, &description[strlen(description)], 10) ; 
		}

		Icr2lsRawData::~Icr2lsRawData(void)
		{
			delete [] mptr_data ; 
			delete [] mptr_data_copy ; 
			delete [] marr_file_name ; 
		} ;

		const char* Icr2lsRawData::GetFileName() 
		{
			return (char *)marr_file_name ; 
		}
		
		int Icr2lsRawData::GetScanSize() 
		{ 
			return this->mint_num_points_in_scan ; 
		} ; 

		int Icr2lsRawData::GetNumScans()
		{
			if (mmap_files_names.size() == 0)
				return 0 ; 
			std::map<int, std::string>::iterator iter = mmap_files_names.end() ; 
			iter-- ; 
			int max_scan = (*iter).first ; 
			return max_scan ; 
		}

		int Icr2lsRawData::GetNextScanNum(int current_scan_num)
		{
			std::map<int, std::string>::iterator iter = mmap_files_names.lower_bound(current_scan_num+1) ; 
			if (iter == mmap_files_names.end())
				return -1 ; 
			int scan_num = (*iter).first ; 
			return scan_num; 
		}

		int Icr2lsRawData::GetFirstScanNum()
		{
			if (mmap_files_names.size() == 0)
			{
				throw "No Scans loaded." ; 
			}

			std::map<int, std::string>::iterator iter = mmap_files_names.begin() ; 
			int scan_num = (*iter).first ; 
			return scan_num; 
		}

		int Icr2lsRawData::GetLastScanNum()
		{
			if (mmap_files_names.size() == 0)
			{
				throw "No Scans loaded." ; 
			}
			std::map<int, std::string>::iterator iter = mmap_files_names.end() ; 
			iter-- ; 
			int scan_num = (*iter).first ; 
			return scan_num; 
		}

	}
}