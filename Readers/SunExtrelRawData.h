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
#include "rawdata.h"
#include <map> 

namespace Engine 
{
	namespace Readers
	{
		class  FinniganHeader
		{
			public:
			char ByteFormat ;
			char Processor ;
			char MagicNum ;
			char FileType ;
			short RevLevel ;
			short Descriptor ;
			int SeqSize ;
			int reserved ;
			int DataSize ;
			int TrailorRecordsSize ;
			int TrailorRecordsNamesSize ;
			int ExtendedDataSize ;
		} ; 


		class   SunExtrelRawData : public RawData
		{

			char *marr_file_name ; 
			float *mptr_data ; 
			float *mptr_data_copy ; 
			int mint_num_points_in_scan ; 

			double mdbl_calib_const_a ; 
			double mdbl_calib_const_b ; 
			double mdbl_calib_const_c ; 

			int mint_data_section_start ;
			int mint_allocated_size ; 

			short mshort_options_size ; 

			short mshort_ftype ; 

			bool mbln_zero_fill ; 
			short mshort_num_zero_fill ;
			int mint_num_scans ; 
			int mint_last_scan_num ; 
			double mdbl_signal_range ; 

	#pragma warning(disable: 4251)
			std::map<int, std::string> mmap_files_names ;
	#pragma warning(default: 4251)

			CalibrationType menm_calibration_type ; 
			//		' FTMS cal types:
			//     '   1  m/z = A/f + |Vt|B/f^2
			//     '   3  m/z = A/f + |Vt|B/f^2 + C
			//     '   2  m/z = A/f + |Vt|B/f^2 + I|Vt|C/f^2


			int ReadFinniganHeader(char *f_name, FinniganHeader &FH) ; 
			long ReadHeader(char *f_name) ; 
			long FindTrailorIndex(char *f_name, char *var_name) ; 
			char* FindDetectSlice(char *f_name) ; 
		
			void ExtractSettings(int start_p, char *option_str) ; 
			
			void ConvertDataByteOrder() ;
			void GetFileName(int i, char *file_name) ; 
			void AddFilesInDir(char *path, char *folder_name) ; 


			void AddScanFile(char *file_name, int scan_num) ; 
			void AddScanFile(char *file_name) ; 
			void AddScanFile(char *path, char *file_name) ; 
			bool LoadFile(char *f_name, int scan_num) ; 
			bool LoadFile(int scan_num) ; 

		public:
			virtual void Load(char *file) ; 
			double GetSignalRange(int scan_num) ; 

			const char *GetFileName() ;
			FileType GetFileType() { return SUNEXTREL ; } ;  
			
			double GetScanTime(int scan_num) ; 
			bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) ; 
			bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 
			int GetScanSize() ; 
			SunExtrelRawData(void) ;
			~SunExtrelRawData(void) ;
			int GetNumScansLoaded() { return mint_last_scan_num ; } 
			int GetNumScans() ; 
			void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) {}  ; 
			
			int GetParentScan(int scan_num)
			{
				//future work 
				return 0;
			}
			bool IsMSScan(int scan_num)
			{
				//future work
				return true;
			}		
			double GetParentMz(int scan_num)
			{
				//future work
				return 0;
			}
			bool IsProfileScan(int scan_num) 	
			{
				//future work
				return true;
			}
			virtual bool IsFTScan(int scanNum)
			{
				return true ; 
			}

			virtual int GetFirstScanNum() ; 
			virtual int GetLastScanNum() ; 
			virtual int GetNextScanNum(int current_scan_num) ; 
			int GetMSLevel(int scan_num)
			{
				//future work 
				return 1;
			}
		};
	}
}