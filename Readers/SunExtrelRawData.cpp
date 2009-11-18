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
#include <fstream>
#include "sunextrelrawdata.h"
#include "../Utilities/helpers.h" 
#include "float.h" 
#include <math.h>
#include <windows.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>


namespace Engine {
	namespace Readers
	{

		long SunExtrelRawData::ReadHeader(char *f_name)
		{
			// Gets the number of points in scan, the size of the header which gives the offset to the actual data.

				char *tag = FindDetectSlice(f_name) ; 

				if (tag != NULL)
				{
					int len = (int) strlen(tag) ; 
					char *temp_tag = new char [len+3] ; 
					strcpy(temp_tag, tag) ; 
					strcat(temp_tag, " {") ; 
					delete [] tag ; 
					tag = temp_tag ; 
				}
				else
				{
					char *temp_tag = "Type=detect" ; 
					tag = new char [strlen(temp_tag)+1] ;
					strcpy(tag, temp_tag) ; 
				}

  				//' Type of data
				//'      1 = Time
				//'      2 = Freq
				//'      3 = Mass
				//'      5 = X,Y floating point pairs...

				mshort_ftype = Helpers::ReadByte(f_name, 3) ; 
				mshort_options_size = Helpers::ReadInt16(f_name, 10) ; 
				mint_data_section_start = mshort_options_size + 64 ; 

				char *options_str = Helpers::ReadFileString(f_name, 64, mshort_options_size) ; 

				char *start_ptr = strstr(options_str, tag) ; 
				char *end_ptr = NULL ; 
				if (start_ptr != NULL)
					end_ptr = strstr(start_ptr, "}") ; 

				char *slice_options = NULL ; 
				if (end_ptr != NULL && start_ptr != NULL)
					slice_options = new char [end_ptr - start_ptr+2] ; 
				if (slice_options != NULL)
				{
					strncpy(slice_options, start_ptr, end_ptr - start_ptr+1) ; 
					slice_options[end_ptr-start_ptr+1] = '\0' ; 
				}

				ExtractSettings(0, slice_options) ; 
				
				if (this->mshort_ftype ==2 && this->mbln_zero_fill) 
				{
					this->mint_num_points_in_scan = this->mint_num_points_in_scan * (2 ^ this->mshort_num_zero_fill) ; 
				}

				//Determine the sequence total time...
				long pos = 0 ; 
				double total_time = 0 ; 
				while (true)
				{
					double this_time ; 
					if (!Helpers::GetDouble(options_str, "time:", pos, this_time))
						break ; 
					char *pos_ptr = strstr(&options_str[pos], "time:") ; 
					pos = long (pos_ptr - options_str + 5) ; 
					total_time = total_time + this_time ;
				}
		//     Call LogMess(ps, "Total Sequence Time = " & Format(TotalTime))      

				delete [] slice_options ; 
				delete [] options_str ; 
				delete [] tag ; 


				return this->mint_data_section_start ; 
		}


		void SunExtrelRawData::ExtractSettings(int start_p, char *option_str)
		{	
			short short_bluff ; 

			double sample_rate ; 
			double anal_trap_voltage ; 
			double conductance_lim_voltage, source_trap_voltage ;
			short calibration_type ; 
			double low_mass_frequency  ;

			int temp_int ; 
			Helpers::GetInt32(option_str, "dataPoints:", start_p, temp_int) ; 
			mint_num_points_in_scan = temp_int ; 
			
			Helpers::GetDouble(option_str, "dwell:", start_p, sample_rate) ;

			if (sample_rate != 0)
				sample_rate = 1/ sample_rate ; 

			if (Helpers::GetInt16(option_str, "Zerofill=true", start_p, short_bluff)) 
			{
				mbln_zero_fill = true ; 
			}

			short temp ; 
			Helpers::GetInt16(option_str, "ZerofillNumber=", start_p, temp) ; 
			mshort_num_zero_fill = temp ; 
			Helpers::GetDouble(option_str,"analyzerTrapVoltage:", start_p, anal_trap_voltage) ; 
			Helpers::GetDouble(option_str, "conductanceLimitVoltage:", start_p, conductance_lim_voltage) ; 
			Helpers::GetDouble(option_str, "sourceTrapVoltage:", start_p, source_trap_voltage) ; 
			Helpers::GetInt16(option_str, "calType:", start_p, calibration_type) ; 
			Helpers::GetDouble(option_str, "calReferenceFrequency:", start_p, low_mass_frequency) ; 

			double calib_a = 0 ;
			bool found_a ; 
			found_a = Helpers::GetDouble(option_str, "calC0:", start_p, calib_a) ; 
			double calib_b = 0 ;
			bool found_b ; 
			found_b = Helpers::GetDouble(option_str, "calC1:", start_p, calib_b) ; 
			double calib_c = 0 ;
			bool found_c ; 
			found_c = Helpers::GetDouble(option_str, "calC2:", start_p, calib_c) ; 

			Helpers::GetDouble(option_str, "chirpStartFrequency:", start_p, low_mass_frequency) ;

			if (Helpers::GetInt16(option_str, "detectType:analyzer", start_p, short_bluff))
			{			
				calib_b = calib_b * Helpers::absolute<double>(anal_trap_voltage) ; 
			}
			else
			{
				calib_b = calib_b * Helpers::absolute<double>(source_trap_voltage) ;
			}

			if (calibration_type == 2)
			{
				//  Call ErrorMess.LogError("Warning, type 2 calibration, make sure intensity option is defined in calibration generation dialog!", ps.FileName)
				calib_c = calib_c * Helpers::absolute<double>(anal_trap_voltage) ;
			}
			
			if (calib_a == 0 && calib_b == 0 && calib_c == 0)
			{
				//no header cal data found, use defaults!
				//Call ErrorMess.LogError("No calibration data in file, using defaults...", ps.FileName)
				calib_a = 108205311.2284 ; 
				calib_b = -1767155067.018 ; 
				calib_c = 29669467490280 ; 
			}
			menm_calibration_type = (CalibrationType) calibration_type ; 

			Calibrations::CCalibrator *calib = new Calibrations::CCalibrator(menm_calibration_type) ; 

			calib->SetSize(this->mint_num_points_in_scan) ; 
			calib->SetLowMassFrequency(low_mass_frequency) ; 
			calib->SetSampleRate(sample_rate);
			calib->SetCalibrationEquationParams(calib_a, calib_b, calib_c);
			if (mobj_calibrator != NULL)
			{
				delete mobj_calibrator ; 
				mobj_calibrator = 0 ; 
			}
			SetCalibrator(calib) ; 
		}
		
		int SunExtrelRawData::ReadFinniganHeader(char *f_name, FinniganHeader &FH)
		{
			FH.ByteFormat = Helpers::ReadByte(f_name, 0);
			FH.Processor = Helpers::ReadByte(f_name, 1);
			FH.MagicNum = Helpers::ReadByte(f_name, 2);
			FH.FileType = Helpers::ReadByte(f_name, 3);
			FH.RevLevel = Helpers::ReadInt16(f_name, 4);
			FH.Descriptor = Helpers::ReadInt16(f_name, 6);
			FH.SeqSize = Helpers::ReadInt32(f_name, 8);
			FH.DataSize = Helpers::ReadInt32(f_name, 16);
			FH.TrailorRecordsSize = Helpers::ReadInt32(f_name, 20);
			FH.TrailorRecordsNamesSize = Helpers::ReadInt32(f_name, 24);
			FH.ExtendedDataSize = Helpers::ReadInt32(f_name, 28);
			if(FH.MagicNum != 62) return -1 ; 
			if (FH.FileType != 1 && FH.FileType != 2 && FH.FileType != 3 && FH.FileType != 37)
				return -1 ; 
			return 0 ; 
		}

		long SunExtrelRawData::FindTrailorIndex(char *f_name, char *var_name)
		{
			// Need to stop reading everything one field at a time for 
			// these header files.
			FinniganHeader FH ;

			long Pos ; 
			long Length ;
			int len, start, count ; 
			char *Labels ; 

			if (ReadFinniganHeader(f_name, FH) != 0)
				return -1 ; 
			
			Pos = FH.SeqSize + FH.DataSize + FH.TrailorRecordsSize + 64 ; 
			Length = FH.TrailorRecordsNamesSize ;
			Labels = Helpers::ReadFileString(f_name, Pos,Length) ; 

			start = 0 ; 
			count = 0 ; 
			int index = -1 ; 

			while (start < Length)
			{
				len = (int) strlen(Labels + start) ; 
				if (strncmp(var_name, Labels + start, len) == 0)
					index = count ; 
				count = count + 1 ; 
				start += (len + 1) ; 
			}

			delete [] Labels ; 
			return index ; 
		}

		char* SunExtrelRawData::FindDetectSlice(char *f_name)
		{
			long pos ; 
			long trailor ; 
			char Rtype ;
			char Dtype ;
			int cnt ;
			char *tag ;
			int ln ;
			FinniganHeader FH ;

			if(ReadFinniganHeader(f_name, FH) == -1)
				return NULL ; 

			cnt = FindTrailorIndex(f_name, "Detect_Slice") ; 

			if (cnt == -1 )
				return NULL ; 

			trailor = 64 + FH.SeqSize + FH.DataSize + 16 * cnt ; 

			Rtype = Helpers::ReadByte(f_name, trailor) ; 
			Dtype = Helpers::ReadByte(f_name, trailor + 1) ;
			ln = Helpers::ReadInt16(f_name, trailor + 2) ;

			if (Rtype == 1 && Dtype == 8) 
			{
				// Now read the detect slice number!
				tag = Helpers::ReadFileString(f_name, trailor+8, ln-1) ; 
				return tag ; 
			}
			else if (Rtype == 17 && Dtype == 8)
			{
		// If here, then the slice name is saved in misc area
				pos = 64 + FH.SeqSize + FH.DataSize + FH.TrailorRecordsSize + FH.TrailorRecordsNamesSize ; 
				trailor = pos + Helpers::ReadInt32(f_name, trailor + 8) ; 
				tag = Helpers::ReadFileString(f_name, trailor, ln-1) ; 
				return tag ; 
			}
			return NULL ;
		}


		bool SunExtrelRawData::LoadFile(char *f_name, int scan_num)
		{
			// for SunExtrelRawData scan_num is always 0.
			scan_num = 0 ; 
			strcpy(marr_file_name, f_name) ; 
			long data_section_start = ReadHeader(marr_file_name) ; 
			long file_pointer = this->mint_data_section_start + scan_num * this->mint_num_points_in_scan * 4 ; 

			if (mint_allocated_size < mint_num_points_in_scan || mptr_data == 0)
			{
				delete [] mptr_data ; 
				delete [] mptr_data_copy ; 
				mptr_data = new float [mint_num_points_in_scan] ; 
				mptr_data_copy = new float [mint_num_points_in_scan] ; 
				mint_allocated_size = mint_num_points_in_scan ; 
			}

			std::ifstream sun_extrel_file(marr_file_name, std::ios::in | std::ios::binary);
			if(!sun_extrel_file.is_open())
				return false ; // File does not exist!

			// seek to location and read.
			sun_extrel_file.seekg(file_pointer);
			sun_extrel_file.read((char*) mptr_data, mint_num_points_in_scan*sizeof(float));
			bool found = sun_extrel_file.fail() ; 
			int count = sun_extrel_file.gcount() ; 
			sun_extrel_file.close();

			ConvertDataByteOrder() ; 
			return true ; 
		}

		void SunExtrelRawData::ConvertDataByteOrder()
		{
			unsigned char *ch, by ; 
			int i ;
			double min_intensity = DBL_MAX ; 
			double max_intensity = DBL_MIN ; 

			// Now convert to pc byte order
			for(i=0; i<mint_num_points_in_scan; i++)
			{
				ch = (unsigned char *) &mptr_data[i];
				by    = ch[0];
				ch[0] = ch[3];
				ch[3] = by;
				by    = ch[1];
				ch[1] = ch[2];
				ch[2] = by;
				mptr_data[i] *= 1.0;
				if (mptr_data[i] < min_intensity)
					min_intensity = mptr_data[i] ; 
				if (mptr_data[i] > max_intensity)
					max_intensity = mptr_data[i] ; 

				if((_status87() & (EM_INVALID|EM_DENORMAL|EM_ZERODIVIDE|EM_OVERFLOW|EM_UNDERFLOW)) != 0) 
				{
					mptr_data[i]=0.0;
					_clear87() ; 
				}
			}
			mdbl_signal_range = max_intensity - min_intensity ; 

		}

		void SunExtrelRawData::GetFileName(int scan_num, char *temp_path)
		{
			std::map<int, std::string>::iterator iter = mmap_files_names.find(scan_num) ; 
			if (iter == mmap_files_names.end())
			{
				temp_path[0] = '\0' ; 
				return ; 
			}
			strcpy(temp_path, (*iter).second.c_str()) ; 
		}

		bool SunExtrelRawData::GetRawData(std::vector <double>*mzs, std::vector<double>*intensities, int scan_num) 
		{
			int num_pts = mint_num_points_in_scan ; 
			return GetRawData(mzs, intensities, scan_num, num_pts) ; 
		}

		double SunExtrelRawData::GetScanTime(int scan_num)
		{
			return scan_num ; 
		}

		double SunExtrelRawData::GetSignalRange(int scan_num) 
		{
			// only returns a value if the current scan is the one we are asking for. 
			if (mint_last_scan_num == scan_num)
				return mdbl_signal_range ; 
			return 0 ; 
		}


		bool SunExtrelRawData::GetRawData(std::vector <double>*mzs, std::vector<double>*intensities, int scan_num, int num_pts) 
		{
			int i = 0 ; 
			char file_name[256] ; 
			const int flt_size = sizeof(float) ; 

			GetFileName(scan_num, file_name) ; 
			if (file_name[0] == '\0')
				return false ; 

			if (num_pts > mint_num_points_in_scan || scan_num != mint_last_scan_num)
			{
				mint_last_scan_num = scan_num ; 
				bool loaded = LoadFile(file_name, scan_num) ; 
				if (!loaded)
					return false ; 
				memcpy(mptr_data_copy, mptr_data, mint_num_points_in_scan*flt_size) ; 
			}
			else
			{
				memcpy((char *)mptr_data, (char *)mptr_data_copy, mint_num_points_in_scan*flt_size) ; 
			}
			if (num_pts <= 0)
				num_pts = mint_num_points_in_scan ; 

			mobj_calibrator->GetRawPointsApplyFFT(mptr_data, mzs, intensities, num_pts) ; 
			return true ; 
		}

		void SunExtrelRawData::AddFilesInDir(char *directory_path, char *folder_name)
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
						int len = (int)strlen(directory_name) ; 
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

		void SunExtrelRawData::AddScanFile(char *path, char *file_name)
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
				mmap_files_names.insert(std::pair<int, std::string>(scan_num, full_path)) ; 
			}
		}

		void SunExtrelRawData::AddScanFile(char *file_name)
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
		}

		void SunExtrelRawData::AddScanFile(char *file_name, int scan_num)
		{
			mmap_files_names.insert(std::pair<int, std::string>(scan_num, file_name)) ; 
		}



		void SunExtrelRawData::Load(char *directory_path)
		{
			char sfile_name[512]; 
			char file_name[512] ; 
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;
			BOOL found = true ; 

			if (!IsDir(directory_path))
			{
				AddScanFile(directory_path) ;
				return ; 
			}

			strcpy(sfile_name, directory_path) ; 
			int len = (int)strlen(directory_path) ; 
			
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

		SunExtrelRawData::SunExtrelRawData(void) : mptr_data(0), mptr_data_copy(0), mint_allocated_size (0)
		{
			marr_file_name = new char [MAX_FNAME_LEN] ; 
			mint_last_scan_num = -1 ; 
			mint_num_points_in_scan = 0 ; 
		} ;

		SunExtrelRawData::~SunExtrelRawData(void)
		{
			delete [] mptr_data ; 
			delete [] mptr_data_copy ; 
		} ;

		const char* SunExtrelRawData::GetFileName() 
		{
			return (char *)marr_file_name ; 
		}
		
		int SunExtrelRawData::GetScanSize() 
		{ 
			return this->mint_num_points_in_scan ; 
		} ; 


		int SunExtrelRawData::GetNumScans()
		{
			return mmap_files_names.size() ; 
		}

		int SunExtrelRawData::GetNextScanNum(int current_scan_num)
		{
			std::map<int, std::string>::iterator iter = mmap_files_names.lower_bound(current_scan_num+1) ; 
			if (iter == mmap_files_names.end())
				return -1 ; 
			int scan_num = (*iter).first ; 
			return scan_num; 
		}

		int SunExtrelRawData::GetFirstScanNum()
		{
			if (mmap_files_names.size() == 0)
			{
				throw "No Scans loaded." ; 
			}
			std::map<int, std::string>::iterator iter = mmap_files_names.begin() ; 
			int scan_num = (*iter).first ; 
			return scan_num; 
		}

		int SunExtrelRawData::GetLastScanNum()
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