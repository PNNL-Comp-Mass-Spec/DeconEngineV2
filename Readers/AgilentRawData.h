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

//set preprocessor declaration ANALYST_INSTALLED in project properties if 
// analyst is installed on computer.
// Also copy the following files into the bin folder in DeconEngine folder :

//ExploreDataObjects.dll
//ExploreDataObjectsManaged.dll
//ExploreDataObjectsps.dll
#ifdef ANALYST_INSTALLED

#import "..\bin\exploredataobjects.dll"

#include <map> 

namespace Engine {
 namespace Readers
{
	class  AgilentRawData : public RawData
	{

		ExploreDataObjects::IFMANSpecData *mobj_spec_data ; 
		ExploreDataObjects::IFMANWiffFile *mobj_wiff_file ; 
		ExploreDataObjects::IFMANChromData *mobj_chrom_data ; 
		IID mriid_wiff_file ; 

		char marr_filename[MAX_FNAME_LEN] ; 
		int	GetSpecData(void) ; 

		long mlng_num_samples ;
		long mlng_num_periods ; 
		long mlng_num_experiments ; 

		long mlng_start_cycle ; 
		long mlng_stop_cycle ; 
		long mlng_stop_period ;
		long mlng_start_period ; 

		long mlng_sample_num ;
		long mlng_period_num ;
		long mlng_experiment_num ;
		int mlng_num_points ; 
		double mdbl_signal_level ; 

		double mdbl_last_scan_time ; 
		int mint_last_scan_num ; 

		void GetMzIntFromGolay(std::vector<double> *mzs, std::vector<double> *intensities) ; 
	public:
		virtual void Close() ; 
		virtual void Load(char *file) ; 
		FileType GetFileType() { return AGILENT_TOF ; } ;  

		const char* GetFileName() ;
		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) ; 
		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 
		double GetScanTime(int scan_num) ; 
		int GetScanSize() ; 
		AgilentRawData(void) ;
		~AgilentRawData(void) ;
		int GetNumScansLoaded() { return GetNumScans() ; } 
		int GetNumScans() ; 
		int GetFirstScanNum() { return 1 ; } ; 
		double GetSignalRange(int scan_num) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ; 
		int GetParentScan(int scan_num);
		bool IsMSScan(int scan_num);
		int GetMSLevel(int scan_num) ; 
		double GetParentMz(int scan_num);
		bool IsProfileScan(int scan_num) ;
		virtual void GetScanDescription(int scan, char *description) ;
		int GetLastScanNum() { return GetNumScans() ; } ; 
	};
}
}
#endif 