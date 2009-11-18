using namespace System;
using namespace YafmsLibrary;
#pragma once

#include "rawdata.h"
#include "../Utilities/SavGolSmoother.h" 
#include <vcclr.h>
#include <map> 

namespace Engine 
{
	namespace Readers
	{
		class  YafmsRawData : public RawData
		{

		private : 
			typedef struct _datapeak
			{
				double dMass;
				double dIntensity;
			} DataPeak;
			
			char marr_filename[MAX_FNAME_LEN] ; 

			long mlong_num_spectra ; 
			long mlong_spectra_num_first ;
			long mlong_spectra_num_last ;
			int mint_last_scan_size ; 
			int mint_last_scan_num ; 
			double mdbl_last_scan_time ; 
			double mdbl_signal_range ; 
			gcroot<System::String*> ScanDescription;			

			int mint_num_points_in_scan ; 
			int *marr_data_block ; 
			float *marr_temp_data_block ; 
			bool mbln_is_file_open;
			
			void SetDataSize(int sz) ; 
			int	ReadSpectraFloats(int spectra_num) ; 

		public:
			~YafmsRawData(void) ;
			YafmsRawData(void) ;

			const char *GetFileName() ;
			FileType GetFileType() { return YAFMS ; } ;  

			gcroot<YafmsReader*> ReadYafms;
			

			int GetNumScansLoaded() { return GetNumScans() ; } 
			int GetNumScans();
			int GetScanSize(); 
			int GetFirstScanNum() ;
			int GetLastScanNum() ;

			void Open(char *raw_file_name) ;
			void Close();
			virtual void Load(char *file) ;
			double GetScanTime(int scan_num) ; 
			short GetSpectrumType(int scan_num)
			{
				return 1 ; 
			}

			bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num) ;  
			bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_points) ;  
			double GetSignalRange(int scan_num) ; 
			bool IsProfileScan(int scan_num) ;
			void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ; 
			int GetParentScan(int scan_num);
			bool IsMSScan(int scan_num);					
			double GetParentMz(int scan_num);
			int GetMSLevel(int scan_num) ;
			virtual void GetScanDescription(int scan, char *description) ;
			double GetMonoMZFromHeader(int scan_num) ;
			bool IsFTScan(int scanNum) ; 
			double GetTICForScan(int scan_num) ; 			
		};
	}
}

