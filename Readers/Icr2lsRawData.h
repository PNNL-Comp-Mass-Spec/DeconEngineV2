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
#include "../Utilities/Apodization.h" 

namespace Engine {
 namespace Readers
{

	enum Icr2lsFileType { ICR2LS_TIME = 1, ICR2LS_FREQ, ICR2LS_MASS } ; 
	enum Icr2lsDataType { ICR2LS_INT_NO_HEADER = 1, ICR2LS_FLT_SUN_EXTREL, ICR2LS_HEADER } ; 

	class   Icr2lsRawData : public RawData
	{
		Icr2lsFileType menm_file_type ; 
		Icr2lsDataType menm_data_type ; 
		double mdbl_sample_rate ; 
		double mdbl_low_mass_freq ; 
		double mdbl_freq_shift ; 
		int mint_num_segments ; 
		double mdbl_max_point ; 
		Engine::Utilities::ApodizationType menmApodizationType ; 
		int mint_apodization_apex_percent ; 
		double mdbl_apodization_min_x ; 
		double mdbl_apodization_max_x ; 
		short mshort_num_zeros ; 
		short mshort_num_zeros_in_read_data ; 

		CalibrationType menm_calibration_type ; 
		//		' FTMS cal types:
		//     '   1  m/z = A/f + |Vt|B/f^2
		//     '   3  m/z = A/f + |Vt|B/f^2 + C
		//     '   2  m/z = A/f + |Vt|B/f^2 + I|Vt|C/f^2
		double mdbl_calib_const_a ; 
		double mdbl_calib_const_b ; 
		double mdbl_calib_const_c ; 
		double mdbl_intensity_calibration ; 


		char *marr_file_name ; 
		float *mptr_data ; 
		float *mptr_data_copy ; 
		int mint_last_scan_num ; 
		int mint_num_points_in_scan ; 

		int mint_allocated_size ; 
		double mdbl_signal_range ; 
		bool mbln_tic_file ; 

		bool mbln_use_specified_calibration ; 

#pragma warning(disable:4251)
		std::map<int, std::string> mmap_files_names ;
#pragma warning(default:4251)
		long ReadHeader(char *f_name) ; 
		void GetFileName(int scan_num, char *file_name) ; 

		void AddFilesInDir(char *path, char *folder_name) ; 
		void AddScanFile(char *path, char *file_name) ; 
		void AddScanFile(char *file_name) ; 

		bool ReadHeader(std::ifstream &fin) ; 
		bool LoadFile(char *f_name, int scan_num) ; 
		bool LoadFile(int scan_num) ; 
	public:
		virtual void Load(char *file) ; 
		void AddScanFile(char *file_name, int scan_num) ; 
		double GetSignalRange(int scan_num) ; 

		const char *GetFileName() ;
		FileType GetFileType() { return ICR2LSRAWDATA ; } ;  
		
		double GetSampleRate() { return mdbl_sample_rate ; } 
		double GetScanTime(int scan_num) ; 
		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) ; 
		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 
		int GetScanSize() ; 
		int GetNumScansLoaded() { return GetNumScans() ; } 
		Icr2lsRawData(void) ;
		~Icr2lsRawData(void) ;
		int GetNumScans() ; 
		void SetIsTic(bool is_tic) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ;
		int GetNextScanNum(int current_scan_num) ; 
		int GetFirstScanNum() ; 		
		virtual int GetLastScanNum(); 

		void GetScanDescription(int scan, char *description) ; 

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
		int GetMSLevel(int scan_num)
		{
			return 1 ;
		}
		virtual bool IsFTScan(int scanNum)
		{
			return true ; 
		}

		// once this calibration function is called, the default calibration is 
		// overridden. 
		virtual void OverrideDefaultCalibrator(CalibrationType calibType, double A, double B, double C) ;

		void SetApodizationZeroFillOptions(Engine::Utilities::ApodizationType type, double min_x, double max_x, int apodization_percent,
			short num_zeros)
		{
			menmApodizationType = type ; 
			mdbl_apodization_min_x = min_x ; 
			mdbl_apodization_max_x = max_x ; 
			mint_apodization_apex_percent = apodization_percent ; 
			mshort_num_zeros = num_zeros ; 
		}
		void GetFTICRTransient(std::vector<float> &vect_intensities)
		{
			// first reload current data. 
			int i = 0 ; 
			char file_name[256] ; 
			const int flt_size = sizeof(float) ; 
			GetFileName(mint_last_scan_num, file_name) ; 
			if (file_name[0] == '\0')
				return ; 
			bool loaded = LoadFile(file_name, mint_last_scan_num) ; 
			if (!loaded)
				return ; 
			memcpy(mptr_data_copy, mptr_data, mint_allocated_size*flt_size) ; 			
			for (int pt_num = 0 ; pt_num < mint_allocated_size ; pt_num++)
			{
				vect_intensities.push_back(mptr_data_copy[pt_num]) ; 
			}
		}
	};
}
}