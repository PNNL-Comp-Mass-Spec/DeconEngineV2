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
#include <stdio.h> 
#include "RAMP/base64.h"
#include "RAMP/ramp.h"

namespace Engine
{
	namespace Readers
	{
		class MZXmlRawData : public RawData
		{
			std::FILE  *mptr_FILE ;
			char mstr_file_name[MAX_FNAME_LEN] ;
			off_t  mpos_IndexOffset;
			off_t  *mptr_ScanIndex;
			int mint_num_scans ; 
			int mint_current_scan ; 
			double mdbl_signal_level ; 
			struct ScanHeaderStruct mobj_CurrentScanHeader;
		public:
			virtual void Load (char *file_name) ; 
			MZXmlRawData(void);
			~MZXmlRawData(void);
			bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) ;
			bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 
			virtual const char *GetFileName() ;
			virtual FileType GetFileType() ; 
			virtual void Close() ; 
			virtual int GetNumScans() ;
			virtual double GetScanTime(int scan_num) ; 
			virtual int GetScanSize() ; 
			virtual short GetSpectrumType(int scan_num) 
			{
				return 0 ;
			}
			void GetScanDescription(int scan, char *description)
			{
				strcpy(description, "Scan #") ; 
				_itoa(scan, &description[strlen(description)], 10) ; 
			}

			virtual double GetSignalRange(int scan_num) ; 
			virtual void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ;  
			virtual int GetNextScanNum(int current_scan_num) { return current_scan_num + 1 ; } ; 
			virtual int GetFirstScanNum() { return 1 ; } ; 
			virtual int GetLastScanNum() { return mint_num_scans ; } ; 
			virtual int GetNumScansLoaded() { return mint_current_scan ; } ;
			int GetParentScan(int scan_num) ;			
			bool IsMSScan(int scan_num) ;			
			double GetParentMz(int scan_num) ;
			int GetMSLevel(int scan_num) ; 
			bool IsProfileScan(int scan_num) ;
			bool IsFTScan(int scan_num) ; 
						

		};
	}
}