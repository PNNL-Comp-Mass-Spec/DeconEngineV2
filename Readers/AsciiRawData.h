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
	class   AsciiRawData : public RawData
	{
	private : 
		char marr_file_name[512] ; 
		int mint_num_spectra ; 
		int mint_num_points_in_scan ; 
		std::vector<__int64> mvect_scan_start_position ; 

		std::vector<double> mvect_scan_time ; 
		std::vector<double> mvect_scan_tic ; 
		std::vector<double> mvect_scan_bpi ; 
		std::vector<double> mvect_scan_bpi_mz ; 

		int mint_file_handle ; 
		double mdbl_signal_range ; 
		char mchar_delimiter ; 
		double GetBasePeakIntensity(std::vector<double> &mzs, std::vector<double> &intensities, double &bpi_mz) ; 
		double GetTotalIonCount(std::vector<double> &mzs, std::vector<double> &intensities) ; 
		void Clear() ; 
		int mint_percent_done ;
	public:
		~AsciiRawData(void);
		AsciiRawData(void);

		virtual void Close() ; 
		int GetPercentDone() { return mint_percent_done ; } ; 
		const char *GetFileName();
		FileType GetFileType() { return ASCII ; } ;  

		virtual void Load(char *file_n) ; 
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num) ;  
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ;  
		double GetScanTime(int scan_num) ; 
		int GetNumScans() ; 
		int GetFirstScanNum() ; 
		int GetLastScanNum() { return GetNumScans()-1 ; } 
		int GetScanSize() ; 
		double GetSignalRange(int scan_num) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ; 

		virtual int GetNumScansLoaded() { return mint_num_spectra ; } ;
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
			return 1 ; 
		}

	};
}	
}