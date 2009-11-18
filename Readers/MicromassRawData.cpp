// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "MicromassRawData.h"
#include <stdlib.h>
#include "math.h"
#include <map>
#include <iostream>
namespace Engine 
{
	namespace Readers
	{

		bool IsMicroMassV4Installed()
		{
	#ifdef MASSLYNX_4_INSTALLED
			CoInitialize( NULL );
			CLSID clsid ; 
			HRESULT res =  CLSIDFromString(L"{111A3111-8A5C-11D5-809C-00508B5FFEC8}", &clsid ); 	

			if (res != S_OK)
				return false ; 

			return true ; 
	#endif
			return false ;
		}

		bool IsMicroMassV3Installed()
		{
			return false ; 
		}

		MicromassRawData::MicromassRawData(void)
		{
			mbln_get_ms_data_only = false ; 
			menm_masslynx_version = GetLatestInstalledVersion() ; 
			for (int funcNum = 0 ; funcNum < MAX_FUNCTIONS ; funcNum++)
				marr_num_scans_per_function[funcNum] = 0 ; 

			mint_num_points = 0 ; 

			if (menm_masslynx_version == V_4)
				InitializeForV4() ; 
			else if (menm_masslynx_version == V_3)
				InitializeForV3() ;

		}

		void MicromassRawData::InitializeForV4()
		{
	#ifdef MASSLYNX_4_INSTALLED
			CLSID clsid ; 
			IID riid ;
			HRESULT res ; 
			
			// DACHeader		
			res =  CLSIDFromString(L"{111A3111-8A5C-11D5-809C-00508B5FFEC8}", &clsid ); 	
			if (res ==REGDB_E_WRITEREGDB)
			{
				throw "Unable to instantiate MassLynx objects: IDACHeader from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = IIDFromString(L"{111A3110-8A5C-11D5-809C-00508B5FFEC8}", &riid) ; 
			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate MassLynx objects: IDACHeader from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = CoCreateInstance(clsid, NULL,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_header);
			if(res != S_OK)
			{
				throw "Unable to instantiate MassLynx objects: IDACHeader from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
		
			// DACSpectrum
			res =  CLSIDFromString(L"{42BAE6E4-3D52-11D5-8043-00508B5FFEC8}", &clsid ); 	
			if (res ==REGDB_E_WRITEREGDB)
			{
				throw "Unable to instantiate MassLynx objects: IDACSpectrum from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = IIDFromString(L"{42BAE6E3-3D52-11D5-8043-00508B5FFEC8}", &riid) ; 
			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate MassLynx objects: IDACSpectrum from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = CoCreateInstance(clsid, NULL,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_spectrum);
			if(res != S_OK)
			{
				throw "Unable to instantiate MassLynx objects: IDACSpectrum from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			// DACScanStats
			res =  CLSIDFromString(L"{0D0678C2-3A1B-11D5-8040-00508B5FFEC8}", &clsid ); 	
			if (res ==REGDB_E_WRITEREGDB)
			{
				throw "Unable to instantiate MassLynx objects: IDACScanStats from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = IIDFromString(L"{0D0678C1-3A1B-11D5-8040-00508B5FFEC8}", &riid) ; 
			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate MassLynx objects: IDACScanStats from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = CoCreateInstance(clsid, NULL,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_scan_stat);
			if(res != S_OK)
			{
				throw "Unable to instantiate MassLynx objects: IDACScanStats from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			// DACFunctionInfo
			res =  CLSIDFromString(L"{63E4A0C2-5684-11D5-8063-00508B5FFEC8}", &clsid ); 	
			if (res ==REGDB_E_WRITEREGDB)
			{
				throw "Unable to instantiate MassLynx objects: IDACFunctionInfo from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = IIDFromString(L"{63E4A0C1-5684-11D5-8063-00508B5FFEC8}", &riid) ; 
			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate MassLynx objects: IDACFunctionInfo from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res = CoCreateInstance(clsid, NULL,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_func_info);
			if(res != S_OK)
			{
				throw "Unable to instantiate MassLynx objects: IDACFunctionInfo from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
	#endif 
		}
		void MicromassRawData::InitializeForV3()
		{
		}

		MicromassRawData::~MicromassRawData(void)
		{
		}

		const char* MicromassRawData::GetFileName()
		{
			return NULL ; 
		}

		void MicromassRawData::Load(char *file)
		{

			if (strlen(file) < 4)
			{
				char message[512] ; 
				// should end in .dat or .raw
				strcpy(message, "Invalid Masslynx 4 file name: ") ; 
				strcat(message, file) ; 
				strcat(message, " Should have a .raw in the path name") ; 
				throw message ; 
			}

			if (_strcmpi(&file[strlen(file)-4], ".raw")== 0)
			{
				strcpy(marr_filename, file) ; 
			}
			else if (_strcmpi(&file[strlen(file)-4], ".dat")== 0)
			{
				//find index of .raw the file name or complain.
				char *ptr = strstr(file, ".raw") ; 
				if (ptr == NULL)
				{
					char message[512] ; 
					// should end in .dat or .raw
					strcpy(message, "Invalid Masslynx 4 file name: ") ; 
					strcat(message, file) ; 
					strcat(message, " Should have a .raw in the path name") ; 
					throw message ; 
				}
				strncpy(marr_filename, file, ptr - file+4) ; 
				marr_filename[ptr-file+4] = '\0' ; 
			}

			LoadHeader() ; 
		}

		bool MicromassRawData::LoadHeaderV4()
		{
	#ifdef MASSLYNX_4_INSTALLED
			_bstr_t bstr_file = marr_filename ;
			// initialize dDACScanStats object
			HRESULT res = mptr_dac_scan_stat->GetScanStats(bstr_file, 1, 0, 1); 
			if (res != S_OK)
			{
				char message[512] ; 
				strcpy(message, "Unable to open Masslynx 4 file: ") ; 
				strcat(message, marr_filename) ; 
				throw message ; 
			}
			mvectScanInfo.clear() ; 
			if (mbln_get_ms_data_only)
				mshort_num_functions = 1 ; 
			else
				mshort_num_functions = (short) mptr_dac_func_info->GetNumFunctions(bstr_file) ; 

			std::map<double, ScanInfo> mapScanStats ; 
			for (short func_num = 1 ; func_num <= mshort_num_functions ; func_num++)
			{
				ScanInfo info ; 
				mptr_dac_func_info->GetFunctionInfo(bstr_file, func_num) ; 
				marr_num_scans_per_function[func_num] = mptr_dac_func_info->NumScans ; 
				for (int scan_num = 1 ; scan_num <= marr_num_scans_per_function[func_num] ; 
					scan_num++)
				{
					mptr_dac_scan_stat->GetScanStats(bstr_file, func_num, 0, scan_num) ; 
					float rt = mptr_dac_scan_stat->RetnTime ; 
					info.mshort_function_num = func_num ; 
					info.mint_scan_num = scan_num ; 
					mapScanStats.insert(std::pair<double, ScanInfo>(rt, info)) ; 
				}
			}
			for (std::map<double, ScanInfo>::iterator iter = mapScanStats.begin() ; iter != mapScanStats.end() ; iter++)
			{
				mvectScanInfo.push_back((*iter).second) ; 
			}
	#endif 
			return false ; 
		}


		bool MicromassRawData::LoadHeaderV3()
		{
			return false ; 
		}

		bool MicromassRawData::LoadHeader()
		{
			if (menm_masslynx_version == V_4)
				return LoadHeaderV4() ; 
			else if (menm_masslynx_version == V_3)
				return LoadHeaderV3() ; 
			return false ; 
		}

		bool MicromassRawData::GetRawDataV3(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
			return false ; 
		}

		bool MicromassRawData::GetRawDataV3(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
			return false ; 
		}

		bool MicromassRawData::GetRawDataV4(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
	#ifdef MASSLYNX_4_INSTALLED
			if (scan_num < 1)
				throw "Scan Number needs to be at least 1" ; 
			_bstr_t bstr = marr_filename ;

			ScanInfo info = mvectScanInfo[scan_num-1] ; 
			std::cerr<<bstr<<" Scan = "<<info.mint_scan_num<<" Func # = "<<info.mshort_function_num<<std::endl ; 
			mptr_dac_spectrum->GetSpectrum(bstr, info.mshort_function_num, 0, info.mint_scan_num) ; 

			long num_pts_spectrum ;
			mptr_dac_spectrum->get_NumPeaks(&num_pts_spectrum) ; 
			mint_num_points = num_pts_spectrum ; 

			mzs->clear() ; 
			intensities->clear() ;
			if (num_pts_spectrum > (int) mzs->capacity())
			{
				mzs->reserve(num_pts_spectrum) ; 
				intensities->reserve(num_pts_spectrum) ; 
			}

			_variant_t var_mzs ; 
			_variant_t var_intensities ; 
			var_mzs = mptr_dac_spectrum->Masses ; 
			var_intensities = mptr_dac_spectrum->Intensities ; 

			float *mz_arr, *intensity_arr ; 

			SafeArrayAccessData(var_mzs.parray, (void **) &mz_arr) ; 
			SafeArrayAccessData(var_intensities.parray, (void **) &intensity_arr) ; 

			for (int i = 0 ; i < num_pts_spectrum ; i++)
			{
				mzs->push_back(mz_arr[i]) ; 
				intensities->push_back(intensity_arr[i]) ; 
			}

			SafeArrayUnaccessData(var_mzs.parray) ; 
			SafeArrayUnaccessData(var_intensities.parray) ; 
			return true ; 
	#endif
			return false ;
		}

		bool MicromassRawData::GetRawDataV4(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
	#ifdef MASSLYNX_4_INSTALLED
			_bstr_t bstr = marr_filename ;
			if (scan_num < 1)
				throw "Scan Number needs to be at least 1" ; 

	
			ScanInfo info = mvectScanInfo[scan_num-1] ; 
			mptr_dac_spectrum->GetSpectrum(bstr, info.mshort_function_num, 0, info.mint_scan_num) ; 

			long num_pts_spectrum ;
			mptr_dac_spectrum->get_NumPeaks(&num_pts_spectrum) ; 
			num_pts_spectrum = min(num_pts_spectrum, num_pts) ; 

			mzs->clear() ; 
			intensities->clear() ;
			if (num_pts_spectrum > (int) mzs->capacity())
			{
				mzs->reserve(num_pts_spectrum) ; 
				intensities->reserve(num_pts_spectrum) ; 
			}

			_variant_t var_mzs ; 
			_variant_t var_intensities ; 
			var_mzs = mptr_dac_spectrum->Masses ; 
			var_intensities = mptr_dac_spectrum->Intensities ; 

			float *mz_arr = new float[num_pts_spectrum] ; 
			float *intensity_arr = new float[num_pts_spectrum] ; 

			SafeArrayAccessData(var_mzs.parray, (void **) &mz_arr) ; 
			SafeArrayAccessData(var_intensities.parray, (void **) &intensity_arr) ; 

			for (int i = 0 ; i < num_pts_spectrum ; i++)
			{
				mzs->push_back(mz_arr[i]) ; 
				intensities->push_back(intensity_arr[i]) ; 
			}

			delete [] mz_arr ;
			delete [] intensity_arr ; 
			return true ; 
	#endif 
			return false ; 
		}

		bool MicromassRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
			switch (menm_masslynx_version)
			{
				case Readers::V_3:
					return GetRawDataV3(mzs, intensities, scan_num) ; 
					break ; 
				case Readers::V_4:
					return GetRawDataV4(mzs, intensities, scan_num) ; 
					break ; 
				default:
					break ; 
			}		
			return false ; 
		}

		bool MicromassRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
			switch (menm_masslynx_version)
			{
				case Readers::V_3:
					return GetRawDataV3(mzs, intensities, scan_num, num_pts) ; 
					break ; 
				case Readers::V_4:
					return GetRawDataV4(mzs, intensities, scan_num, num_pts) ; 
					break ; 
				default:
					break ; 
			}		
			return false ; 
		}

		double MicromassRawData::GetScanTimeV4(int scan_num)
		{
	#ifdef MASSLYNX_4_INSTALLED
			_bstr_t bstr_file_name = marr_filename ;
			if (scan_num < 1)
				throw "Scan Number needs to be at least 1" ; 

			ScanInfo info = mvectScanInfo[scan_num-1] ; 
			mptr_dac_scan_stat->GetScanStats(bstr_file_name, info.mshort_function_num, 0, info.mint_scan_num) ; 
			return mptr_dac_scan_stat->RetnTime ; 
	#endif
			return 0 ; 
		}

		double MicromassRawData::GetScanTimeV3(int scan_num)
		{
			return 0 ; 
		}

		double MicromassRawData::GetScanTime(int scan_num)
		{
			switch (menm_masslynx_version)
			{
				case Readers::V_3:
					return GetScanTimeV3(scan_num) ; 
					break ; 
				case Readers::V_4:
					return GetScanTimeV4(scan_num) ; 
					break ; 
				default:
					break ; 
			}		
			return 0 ; 
		}

		int MicromassRawData::GetScanSize()
		{
			return mint_num_points ; 
		}

		int MicromassRawData::GetNumScans()
		{
			return (int) mvectScanInfo.size() ; 
		}

		double MicromassRawData::GetSignalRange(int scan_num)
		{
			return 0; 
		}

		inline void GetStringRepresentation(char *buffer, double value)
		{
			int precision = 4 ; 
			int intLen = (int)(precision*1.0 + log10(value)) ; 
			_gcvt(value, intLen, buffer) ; 
			if (strlen(buffer) > 0 && buffer[strlen(buffer)-1] == '.')
			{
				buffer[strlen(buffer)-1] = '\0' ; 
			}
		}

		void MicromassRawData::GetScanDescription(int scan, char *description)
		{
#ifdef MASSLYNX_4_INSTALLED
			_bstr_t bstr_file_name = marr_filename ;

			if (scan < 1)
				throw "Scan Number needs to be at least 1" ; 
			ScanInfo info = mvectScanInfo[scan-1] ; 
			mptr_dac_scan_stat->GetScanStats(bstr_file_name, info.mshort_function_num, 0, info.mint_scan_num) ; 

			float rt = mptr_dac_scan_stat->RetnTime ; 
			float bpi = mptr_dac_scan_stat->BPI ; 
			float bpim = mptr_dac_scan_stat->BPM ;
			float lowMass = mptr_dac_scan_stat->LoMass ; 
			float hiMass = mptr_dac_scan_stat->HiMass ; 

			if (info.mshort_function_num > 1)
			{
				strcpy(description, "MSMS (fnc ") ; 
				_itoa((int)info.mshort_function_num, &description[strlen(description)], 10) ; 
				strcat(description, " scan: ") ; 
				_itoa((int)info.mint_scan_num, &description[strlen(description)], 10) ; 
				strcat(description, " )") ; 
			}
			else
			{
				strcpy(description, "MS") ; 
			}
			int a = 0 , b= 0 ; 
			strcat(description, " Scan ") ; 
			_itoa(scan, &description[strlen(description)], 10) ; 
			char buffer[64] ;

			strcat(description, "RT: ") ; 
			GetStringRepresentation(buffer, (double)rt) ; 
			strcat(description, buffer) ; 

			strcat(description, " BPI Mass: ") ; 
			GetStringRepresentation(buffer, (double)bpim) ; 
			strcat(description, buffer) ; 
			strcat(description, "[") ; 
			GetStringRepresentation(buffer, (double)lowMass) ; 
			strcat(description, buffer) ; 
			strcat(description, ",") ; 
			GetStringRepresentation(buffer, (double)hiMass) ; 
			strcat(description, buffer) ; 
			strcat(description, "]") ; 
#endif
		}

		void MicromassRawData::GetTicFromFileV4(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{
	#ifdef MASSLYNX_4_INSTALLED
			// Tics only come from MS data. 

			_bstr_t bstr_file_name = marr_filename ;

			int num_scans = GetNumScans() ; 

			for (int scan_num = 0 ; scan_num < num_scans ; scan_num++)
			{
				ScanInfo info = mvectScanInfo[scan_num] ; 
				if (info.mshort_function_num != 1)
					continue ; 
				mptr_dac_scan_stat->GetScanStats(bstr_file_name, info.mshort_function_num, 0, info.mint_scan_num) ; 
				if (base_peak_tic)
				{
					intensities->push_back(mptr_dac_scan_stat->BPI) ; 
				}
				else
				{
					intensities->push_back(mptr_dac_scan_stat->TIC) ; 
				}
				scan_times->push_back(mptr_dac_scan_stat->RetnTime) ; 
			}
	#endif 
			return ; 
		}

		void MicromassRawData::GetTicFromFileV3(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{
			return ; 
		}

		void MicromassRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{
			switch (menm_masslynx_version)
			{
				case Readers::V_3:
					GetTicFromFileV3(intensities, scan_times, base_peak_tic) ;
					return ; 
					break ; 
				case Readers::V_4:
					GetTicFromFileV4(intensities, scan_times, base_peak_tic) ;
					return ; 
					break ; 
				default:
					break ; 
			}		
		}

		enmMassLynxVersion MicromassRawData::GetLatestInstalledVersion()
		{
			if (IsMicroMassV4Installed())
			{
				return Readers::V_4 ;
			}
			else if (IsMicroMassV3Installed())
			{
				// check for version 3.
				return Readers::V_3 ;
			}

			return NONE ; 
		}
	}
}