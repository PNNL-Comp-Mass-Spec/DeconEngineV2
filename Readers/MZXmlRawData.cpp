// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\mzxmlrawdata.h"
#include "float.h"
#include <iostream> 
namespace Engine
{
	namespace Readers
	{
		MZXmlRawData::MZXmlRawData(void)
		{
			mint_num_scans = 0 ; 
			mptr_FILE = NULL ; 
			mptr_ScanIndex = NULL ; 
			mint_current_scan = -1 ; 
		}

		MZXmlRawData::~MZXmlRawData(void)
		{
			if (mptr_FILE != NULL)
			{
				Close() ;
				mptr_FILE = NULL ;
			}
			if (mptr_ScanIndex != NULL)
				delete [] mptr_ScanIndex ;
		}

		const char* MZXmlRawData::GetFileName()
		{
			return mstr_file_name ; 
		}

		FileType MZXmlRawData::GetFileType()
		{
			return MZXMLRAWDATA ; 
		}
		
		void MZXmlRawData::Close()
		{
			mint_num_scans = 0 ; 
			if (mptr_FILE == NULL)
				return ; 
			fclose(mptr_FILE) ; 
			mptr_FILE = NULL ; 
		}

		int MZXmlRawData::GetNumScans()
		{
			return mint_num_scans ; 
		}

		double MZXmlRawData::GetScanTime(int scan_num)
		{
			if (mint_current_scan == scan_num)
			{
				return mobj_CurrentScanHeader.retentionTime ; 
			}
			else
			{
				struct ScanHeaderStruct scanHeader ;

				// its time to read in that scan.
				mint_current_scan = scan_num ; 
				readHeader(mptr_FILE, mptr_ScanIndex[scan_num], &scanHeader) ;
				return scanHeader.retentionTime ; 
			}
		}

		int MZXmlRawData::GetScanSize()
		{
			return 0 ; 
		}

		void MZXmlRawData::Load (char *file_name)
		{
			strcpy(mstr_file_name, file_name) ; 
			if (mptr_FILE != NULL)
			{
				delete mptr_FILE ; 
				mptr_FILE = NULL ; 
			}
			mptr_FILE = fopen(mstr_file_name, "r") ; 
			if (mptr_FILE == NULL)
			{
				char err_msg[MAX_ERR_LEN] ; 
				strcpy(err_msg, "Unable to open MZXML file: ") ; 
				strcat(err_msg, mstr_file_name) ; 
				throw err_msg ; 
			}

			if (mptr_ScanIndex != NULL)
			{
				delete [] mptr_ScanIndex ;
			}

			mpos_IndexOffset = getIndexOffset(mptr_FILE) ;
			readIndex(mptr_FILE , mpos_IndexOffset, &mint_num_scans, mptr_ScanIndex);
		}

		int MZXmlRawData::GetParentScan(int scan_num)
		{
			int scan = scan_num;
			struct ScanHeaderStruct scanHeader ; 
			readHeader(mptr_FILE, mptr_ScanIndex[scan], &scanHeader) ;	

			for (scan = scan_num - 1;; scan--)
			{
				readHeader(mptr_FILE, mptr_ScanIndex[scan], &scanHeader);	
				if (scanHeader.msLevel == 1)
					break;
			}

			return scan ; 
		}

		bool MZXmlRawData::IsFTScan(int scan_num)
		{
			int scan = scan_num ; 			
			int error = -1 ; 
			struct ScanHeaderStruct scanHeader ; 
			readHeader(mptr_FILE, mptr_ScanIndex[scan], &scanHeader) ;	

			if (strstr(scanHeader.msTypeString, "FTMS"))
				return true ; 
			else if (strstr(scanHeader.msTypeString, "ITMS"))
				return false ; 
		 
			//default
			return false ; 			
		}

		
		bool MZXmlRawData::IsProfileScan(int scan_num)
		{
			int error = -1 ; 
			struct ScanHeaderStruct scanHeader ; 
			readHeader(mptr_FILE, mptr_ScanIndex[scan_num], &scanHeader) ;			
			
			if (scanHeader.profileType == 'p')
				return true ; 
			else if (scanHeader.profileType == 'c') 
				return false ; 
			
			//default
			return true ; 
		}

		
		int MZXmlRawData::GetMSLevel(int scan_num) 
		{
			 int ms_level = 0 ;
			 struct ScanHeaderStruct scanHeader ; 
			 readHeader(mptr_FILE, mptr_ScanIndex[scan_num], &scanHeader) ;	
			 return scanHeader.msLevel ; 			 
		}

		double MZXmlRawData::GetParentMz(int scan_num)
		{
			double parent_mz = 0 ; 
			int ms_level = 0 ;
			struct ScanHeaderStruct scanHeader ; 
			readHeader(mptr_FILE, mptr_ScanIndex[scan_num], &scanHeader) ;	
			return scanHeader.precursorMZ ; 		
		}
		
		bool MZXmlRawData::IsMSScan(int scan_num)
		{
			int ms_level = GetMSLevel(scan_num) ; 
			if (ms_level == 1)
				return true ; 
			else
				return false ; 
		}

		bool MZXmlRawData::GetRawData(std::vector <double>	*mzs, std::vector<double> *intensities, int scan_num) 
		{
			return GetRawData(mzs, intensities,	scan_num, -1) ;	
		}

		bool MZXmlRawData::GetRawData(std::vector <double>	*mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{

			if (scan_num > mint_num_scans)
			{
				char mesg[MAX_ERR_LEN] ; 
				strcpy(mesg, "File only has ") ; 
				_itoa(mint_num_scans, &mesg[strlen(mesg)], 10) ; 
				strcat(mesg, " scans. Cannot read to scan number: ") ; 
				_itoa(scan_num, &mesg[strlen(mesg)], 10) ; 
				throw mesg ; 
			}

			mint_current_scan = scan_num ; 
			readHeader(mptr_FILE, mptr_ScanIndex[scan_num], &mobj_CurrentScanHeader);

			mzs->clear() ; 
			intensities->clear() ; 

			int num_points = mobj_CurrentScanHeader.peaksCount ; 
			if (num_pts > 0 && num_pts < num_points)
			{
				num_points = num_pts ; 
			}

			mzs->reserve(num_points) ; 
			intensities->reserve(num_points) ; 

			float *peaks = readPeaks(mptr_FILE, mptr_ScanIndex[mint_current_scan]);
			float fMass;
			float fInten;
			double max_intensity = -1*DBL_MAX, min_intensity = DBL_MAX; 
			for (int pt_num = 0 ; pt_num < 2*num_points-1 ; pt_num+=2)
			{	   
				fMass=peaks[pt_num];
				fInten=peaks[pt_num+1];
				if (fInten > max_intensity)
					max_intensity = fInten ; 
				if (fInten < min_intensity) //Anoop Jan 08: was > before
					min_intensity = fInten ; 
				mzs->push_back(fMass) ; 
				intensities->push_back(fInten) ; 
			}

			delete [] peaks ; 
			mdbl_signal_level = max_intensity - min_intensity ; 

			return true	; 
		}

		double MZXmlRawData::GetSignalRange(int scan_num)
		{
			return 0 ; 
		}

		void MZXmlRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) 
		{
			if (mptr_FILE == NULL)
				return ; 

			intensities->clear() ; 
			scan_times->clear() ; 

			struct ScanHeaderStruct scanHeader ;
			bool nonzero_time_reported = false ; 
			for (int scan_num = 0 ; scan_num <= mint_num_scans ; scan_num++)
			{
				// its time to read in that scan.
				mint_current_scan = scan_num ; 
				readHeader(mptr_FILE, mptr_ScanIndex[scan_num], &scanHeader) ;
				scan_times->push_back(scanHeader.retentionTime) ; 
				if (scanHeader.retentionTime != 0.0)
					nonzero_time_reported = true ; 
				if (base_peak_tic)
				{
					intensities->push_back(scanHeader.basePeakIntensity) ; 
				}
				else
				{
					intensities->push_back(scanHeader.totIonCurrent) ; 
				}
			}

			// for mzxml files in which no retention times were recorded. 
			if (!nonzero_time_reported)
			{
				for (int scan_num = 0 ; scan_num <= mint_num_scans ; scan_num++)
				{
					(*scan_times)[scan_num] = scan_num ; 
				}
			}


		}

	}
}