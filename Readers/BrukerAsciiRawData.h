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
#include <string>
#include <fstream> 
#include "rawdata.h"

namespace Engine {
 namespace Readers
{
	class   BrukerAsciiRawData : public RawData
	{
	private : 
		char marr_file_name[512] ; 
		int mint_num_spectra ; 
		int mint_num_points_in_scan ; 
		int mint_last_scan_num ; 
		std::vector<__int64> mvect_scan_start_position ; 
		std::vector<double> mvect_scan_time ; 
		int mint_file_handle ; 
		double mdbl_signal_range ; 
		char marr_buffer[MAX_SCAN_SIZE] ; 

		double GetBasePeakIntensity(std::vector<double> &mzs, std::vector<double> &intensities) ; 
		double GetTotalIonCount(std::vector<double> &mzs, std::vector<double> &intensities) ; 
		void Clear() ; 
	public:
		~BrukerAsciiRawData(void);
		BrukerAsciiRawData(void);

		const char *GetFileName();
		FileType GetFileType() { return BRUKER_ASCII ; } ;  

		virtual void Close() ; 
		virtual void Load(char *file_n) ; 
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num) ;  
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ;  
		double GetScanTime(int scan_num) ; 
		int GetNumScans() ; 
		int GetScanSize() ; 
		double GetSignalRange(int scan_num) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ; 
		virtual int GetNumScansLoaded() { return mint_last_scan_num ; } ;
		int GetFirstScanNum() { return 1 ; }
		int GetLastScanNum() { return mint_num_spectra ; } ; 
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
			//future work 
			return 1;
		}


	};
}	
}