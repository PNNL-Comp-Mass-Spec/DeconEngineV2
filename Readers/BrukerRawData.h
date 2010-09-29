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

#include "rawdata.h"

namespace Engine {
 namespace Readers
{
	class   BrukerRawData : public RawData
	{
	private : 
		char *marr_headerName;
		char *marr_serName;

		int FindHeaderParams() ; 
		int mint_num_spectra ; 
		int mint_num_points_in_scan ; 
		int *marr_data_block ; 
		int *marr_data_block_copy ; 
		float *marr_temp_data_block ; 

		void SetDataSize(int sz) ; 
		int	ReadSpectraFloats(int spectra_num) ; 
		int mint_last_scan_num ; 
		double mdbl_signal_range ; 

		double GetBasePeakIntensity(std::vector<double> &mzs, std::vector<double> &intensities) ; 
		double GetTotalIonCount(std::vector<double> &mzs, std::vector<double> &intensities) ; 


		int GetNumSpectraFromFileSizeInfo(void) ; 


	public:
		~BrukerRawData(void);
		BrukerRawData(void);

		const char *GetFileName();
		FileType GetFileType() { return BRUKER ; } ;  

		void Open(char *header_n, char *ser_file_name);

		virtual void Load(char *file_n) ; 
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num) ;  
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ;  
		double GetScanTime(int scan_num) ; 
		int GetNumScansLoaded() { return mint_last_scan_num ; } ; 
		int GetNumScans() ; 
		int GetScanSize() ; 
		double GetSignalRange(int scan_num) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ; 
		int GetParentScan(int scan_num);
		bool IsMSScan(int scan_num);
		double GetParentMz(int scan_num);		
		bool IsProfileScan(int scan_num) ;
		int GetMSLevel(int scan_num) ; 
		int GetLastScanNum() { return mint_num_spectra ; } 
		int GetFirstScanNum() { return 1 ; } 


		

	};
}	
}
