#include "YafmsRawData.h"
#include <map> 
#include <iostream>
#include <float.h>
#include <fstream> 
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <ctime>

using namespace System::Runtime::InteropServices;
using namespace	std	; 
using namespace YafmsLibrary;

namespace Engine 
{
	namespace Readers
	{
		YafmsRawData::YafmsRawData(void)
		{
			marr_data_block = NULL ; 
			marr_temp_data_block = NULL ; 
			mint_last_scan_size = 0 ; 
		}
		YafmsRawData::~YafmsRawData(void) 
		{
			if (marr_data_block != 0)
			{
				delete [] marr_data_block ;
				delete [] marr_temp_data_block ;
			} 
		}			

		void YafmsRawData::GetScanDescription(int scan, char *description)
		{
			int SpectraID = 1;
			ScanDescription = ReadYafms->GetScanDescription(SpectraID, scan);			
			description = (char*)(void*)Marshal::StringToHGlobalAnsi(ScanDescription);
		}
		const char* YafmsRawData::GetFileName()
		{
			return marr_filename ; 
		}

		int YafmsRawData::GetScanSize()
		{
			return mint_last_scan_size ; 
		}

		int YafmsRawData::GetNumScans()	
		{ 
			return (int)mlong_num_spectra ; 
		}
		int YafmsRawData::GetFirstScanNum() 
		{ 
			return (int)mlong_spectra_num_first ; 
		}
		int YafmsRawData::GetLastScanNum() 
		{ 
			return (int)mlong_spectra_num_last ; 
		}

		void YafmsRawData::Open(char *raw_file_name)
		{
			strcpy(marr_filename, raw_file_name) ; 
			try
			{
				//Open yafms file
				ReadYafms = new YafmsReader();
				ReadYafms->OpenYafms(marr_filename);	
				
				//Get number of scans, start scan, and last scan
				mlong_spectra_num_first = ReadYafms->GetFirstScanNum();
				mlong_spectra_num_last = ReadYafms->GetLastScanNum();
				mlong_num_spectra  = ReadYafms->GetTotalNonZeroScans();				
			}
			catch(char * str )
			{
				throw new Exception(str);
			}
		}

		void YafmsRawData::Close()
		{
			try
			{
				ReadYafms->CloseYafms(marr_filename);
			}
			catch(char * str )
			{
				throw new Exception(str);
			}
		}
		void YafmsRawData::Load(char *file)
		{
			YafmsRawData::Open(file);
		}
		double YafmsRawData::GetSignalRange(int scan_num)
		{
			if (scan_num == mint_last_scan_num)
				return mdbl_signal_range ; 
			
			double signal_range = 0 ; 
			int SpectraID = 1;

			//Get high_mass and PeakCount
			int peakcount = 0;
			double high_mass = ReadYafms->GetHighMass(SpectraID, scan_num);
			peakcount=ReadYafms->GetPeaksCount(SpectraID, scan_num);
			if (peakcount > 0)
			{
				//Get mz and intensity
				double mz __gc[]  = new double __gc[peakcount];
				float intensities __gc[] = new float __gc[peakcount];
				bool status = ReadYafms->GetSpectrum(SpectraID,scan_num,mz,intensities);	
				double min_intensity = DBL_MAX ; 
				double max_intensity = DBL_MIN ; 
				for (int j = 0; j < peakcount; j++ )
					{
						double intensity = intensities[j] ; 
						if (intensity > max_intensity) 
						max_intensity = intensity ; 
						if (intensity < min_intensity) 
						min_intensity = intensity ; 
					}
				signal_range = (max_intensity - min_intensity) ; 
			}

			return signal_range ; 
		}
		double YafmsRawData::GetScanTime(int scan_num)
		{
			if (scan_num == mint_last_scan_num)
				return mdbl_last_scan_time ; 

			double start_time  = 0; 
			int SpectraID = 1; //temporary, need to work on this. YS
			
			//Get strat_time
			start_time = ReadYafms->GetRetentionTime(SpectraID, scan_num);
	
			return start_time ; 
		}

		bool YafmsRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
			return GetRawData(mzs, intensities,	scan_num, -1)  ; 
		}
		bool YafmsRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int PeakCount)
		{
			mint_last_scan_num = scan_num ; 
			int lastWholeNumber = 0;
			double peak_width ;
			long scanN = scan_num;
			
			int SpectraID = 1; //temporary; YS

			//Get high_mass and PeakCount
			int peakcount = 0;
			double high_mass = ReadYafms->GetHighMass(SpectraID, scan_num);
			peakcount = ReadYafms->GetPeaksCount(SpectraID, scan_num);
			if (peakcount == 0)
				return false;

			//Get mz and intensity
			double mz __gc[]  = new double __gc[peakcount];
			float yafintensity __gc[] = new float __gc[peakcount];
			bool status = ReadYafms->GetSpectrum(SpectraID,scan_num,mz,yafintensity);	
			
			intensities->clear();
			mzs->clear();
			if ( peakcount < (int)intensities->capacity())
				{
					intensities->reserve(peakcount) ;
					mzs->reserve(peakcount);
				}

			double min_intensity = DBL_MAX ; 
			double max_intensity = DBL_MIN ; 

			for( int j = 0; j < peakcount; j++ )
			{
				if (mz[j] > high_mass)
					{
						break ; 
					}
				double intensity = (double)yafintensity[j] ; 
				if (intensity > max_intensity) 
					max_intensity = intensity ; 
				if (intensity < min_intensity) 
					min_intensity = intensity ; 
				
				double temp = mz[j];
				mzs->push_back(temp) ;
				intensities->push_back(intensity) ;
			}
			
			mdbl_signal_range = (max_intensity - min_intensity) ; 
			mint_last_scan_size = (int) mzs->size() ; 
			return true ; 
		}

		int YafmsRawData::GetParentScan(int scan_num)
		{			
			int msN_level = GetMSLevel(scan_num) ;
			int level = msN_level;

			int i = 0;

			while (level >= msN_level)
			{
				level = GetMSLevel(scan_num - i);
				i++;
			}
			i--;
			return(scan_num - i);
		}
		
		void YafmsRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{
			for (long scan_num = mlong_spectra_num_first ; scan_num <= mlong_spectra_num_last ; scan_num++)
			{
				int SpectraID = 1;
				double tic = (double)ReadYafms->GetTIC(SpectraID, scan_num);
				double base_intensity = (double)ReadYafms->GetBPI(SpectraID, scan_num);
				double start_time = (double)ReadYafms->GetRetentionTime(SpectraID, scan_num);
				
				if (base_peak_tic)
					intensities->push_back(base_intensity) ; 
				else
					intensities->push_back(tic) ; 
				scan_times->push_back(start_time) ; 
			}			
		}
		
		bool YafmsRawData::IsProfileScan(int scan_num)
		{
			int ms_level = GetMSLevel(scan_num) ;
			if (ms_level == 1)
				return true;
			else
				return false ;
		}

		bool YafmsRawData::IsFTScan(int scan_num)
		{	
			int SpectraID = 1;
			ScanDescription = ReadYafms->GetScanDescription(SpectraID, scan_num);			
			if (_strnicmp((char*)(void*)Marshal::StringToHGlobalAnsi(ScanDescription), "ft", 2) == 0)
			{
				return true ; 
			}
			return false ;			
		}

		double YafmsRawData::GetTICForScan(int scan_num)
		{
			int SpectraID = 1;
			double tic = (double)ReadYafms->GetTIC(SpectraID, scan_num);
			return tic ; 
		}

		int YafmsRawData::GetMSLevel(int scan_num)
		{
			int SpectraID = 1;
			int ms_level = 1;
			bool has_precursor = ReadYafms->HasPrecursor(SpectraID, scan_num);
			if (has_precursor)
			{
				ms_level = 2;
			}			
			return ms_level ; 			
		
		}
		bool YafmsRawData::IsMSScan (int scan_num)
		{
			//Returns true if the scan is a MS-level scan

			int ms_level = GetMSLevel(scan_num);
			if (ms_level == 1)
				return true;
			else
				return false;
		}	
		
		double YafmsRawData::GetMonoMZFromHeader(int scan_num)
		{
			int SpectraID = 1;
			return ReadYafms->GetPrecursorMZ(SpectraID, scan_num);
		}
		
		double YafmsRawData::GetParentMz(int scan_num)
		{
			//Return the parent m/z of the particular msN scan
			int SpectraID = 1;
			double parent_mz = 0;
			double PrecursorMZ = ReadYafms->GetPrecursorMZ(SpectraID, scan_num);		
			if (PrecursorMZ != NULL)
				parent_mz = PrecursorMZ;
			return parent_mz;

		}
	}
}
