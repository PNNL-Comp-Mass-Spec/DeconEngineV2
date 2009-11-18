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
#include "RawData.h"
#include <vector>

//set preprocessor declaration MASSLYNX_4_INSTALLED in project properties if 
// MassLynx 4 is installed on computer (Masslynx 3 not supported currently).
// Also make sure that DacServer.dll exists in c:\Masslynx or change the path
// below accordingly. 
#ifdef MASSLYNX_4_INSTALLED
#import "..\Library\DacServer.dll"
#endif

namespace Engine {
 namespace Readers
{
	enum enmMassLynxVersion { NONE= 0, V_3=3, V_4=4 } ; 

	class   MicromassRawData : public RawData
	{
		class ScanInfo
		{
		public:
			short mshort_function_num ;
			int mint_scan_num ; 
		};

		bool mbln_get_ms_data_only ; 
		static const int MAX_FUNCTIONS = 16 ; 
		char marr_filename[512] ; 
		int marr_num_scans_per_function[MAX_FUNCTIONS] ; 
		std::vector<ScanInfo> mvectScanInfo ; 

		enmMassLynxVersion menm_masslynx_version ; 
		short mshort_num_functions ; 
		int mint_num_points ; 

		void InitializeForV4() ; 
		void InitializeForV3() ; 

		bool LoadHeaderV4() ; 
		bool LoadHeaderV3() ; 
		bool LoadHeader() ; 

#ifdef MASSLYNX_4_INSTALLED
		DACSERVERLib::IDACHeader *mptr_dac_header ; 
		DACSERVERLib::IDACSpectrum *mptr_dac_spectrum ; 
		DACSERVERLib::IDACScanStats *mptr_dac_scan_stat ; 
		DACSERVERLib::IDACFunctionInfo *mptr_dac_func_info ; 
#endif 

		bool GetRawDataV3(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num) ; 
		bool GetRawDataV3(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 

		bool GetRawDataV4(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num) ; 
		bool GetRawDataV4(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 

		double GetScanTimeV4(int scan_num) ; 
		int GetScanSizeV4() ; 
		int GetNumScansV4() ;
		void GetTicFromFileV4(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ;  

		double GetScanTimeV3(int scan_num) ; 
		int GetScanSizeV3() ; 
		int GetNumScansV3() ;
		void GetTicFromFileV3(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ;  

	public:
		MicromassRawData(void)  ; 
		~MicromassRawData(void) ;  
		virtual void GetScanDescription(int scan, char *description) ;
		const char *GetFileName() ;
		FileType GetFileType() { return MICROMASSRAWDATA ; } ; 

		void SetMassLynxVersion(enmMassLynxVersion version)
		{
			menm_masslynx_version = version ; 
		}

		enmMassLynxVersion GetLatestInstalledVersion() ; 
		virtual void Load(char *file) ; 
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num) ; 
		bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 
		double GetScanTime(int scan_num) ; 
		int GetScanSize() ; 
		int GetFirstScanNum() { return 1 ; } 
		int GetLastScanNum() { return GetNumScans()-1 ; } ;
		int GetNumScans() ;
		int GetNumScansLoaded() { return GetNumScans() ; } 
		double GetSignalRange(int scan_num) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ;  
		
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
			return 0; 
		}
		int GetMSLevel(int scan_num)
		{
			//future work 
			return 1;
		}
	};
}
}