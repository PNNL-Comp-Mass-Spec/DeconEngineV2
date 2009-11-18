// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\finniganrawdata.h"
#include <math.h>
#include <float.h>
#include <time.h> 
#include "..\Utilities\Interpolation.h"

#ifdef XCALIBUR_INSTALLED
namespace Engine 
{
	namespace Readers
	{
		FinniganRawData::~FinniganRawData(void)
		{			
			if (marr_data_block != 0)
			{
				delete [] marr_data_block ;
				delete [] marr_temp_data_block ;
			} 
			if (marr_rawfileName != NULL)
			{
				delete [] marr_rawfileName ; 
				marr_rawfileName = 0 ; 
			}
			// Let the COM object go
			if(m_xraw2_class != NULL)
				m_xraw2_class->Release();
		};

		FinniganRawData::FinniganRawData(void)
		{
			m_xraw2_class = NULL ;
			marr_data_block = NULL ; 
			marr_temp_data_block = NULL ; 
			mint_last_scan_size = 0 ; 
			marr_rawfileName = new char[512] ; 
			// Obtain an instance of the Finnigan file reader ActiveX control
			long nRet = GetXRawFileInstance();
			if(nRet)
				std::cerr << "Unable to get instance of XRawFile.ocx" << std::endl ;
		};

		void FinniganRawData::GetScanDescription(int scan, char *description)
		{
			_bstr_t bstr_filter ;
			m_xraw2_class->GetFilterForScanNum((long)scan, &bstr_filter.GetBSTR()) ;  
			strcpy(description,(char*)bstr_filter);		
		}


		const char* FinniganRawData::GetFileName()
		{
			return marr_rawfileName ; 
		}

		int FinniganRawData::GetScanSize()
		{
			return mint_last_scan_size ; 
		}

		int FinniganRawData::GetNumScans()	
		{ 
			return (int)mlong_num_spectra ; 
		}
		int FinniganRawData::GetFirstScanNum() 
		{ 
			return (int)mlong_spectra_num_first ; 
		}
		int FinniganRawData::GetLastScanNum() 
		{ 
			return (int)mlong_spectra_num_last ; 
		}
		int FinniganRawData::Open(char *raw_file_name)
		{
			strcpy(marr_rawfileName, raw_file_name) ; 
			
			_bstr_t bstr = marr_rawfileName ;
			BSTR bstr_type = bstr.GetBSTR() ;

			HRESULT res = m_xraw2_class->Open(bstr_type) ; 
			if(res != S_OK)
			{
				char message[512] ; 
				strcpy(message, "Unable to open XCalibur file: ") ; 
				strcat(message, marr_rawfileName) ; 
				throw message ;
			}

			// Get the number of spectra
			res = m_xraw2_class->SetCurrentController(0,1) ; 
			long nRet = m_xraw2_class->GetNumSpectra (&mlong_num_spectra );
			nRet = m_xraw2_class->GetFirstSpectrumNumber (&mlong_spectra_num_first) ;
			nRet = m_xraw2_class->GetLastSpectrumNumber (&mlong_spectra_num_last) ;
			
			
			
			if( nRet )
			{
				std::cerr << "Unable to get number of spectra from " << marr_rawfileName <<std::endl;
				return 1;
			}


			return 0;
		}


		void FinniganRawData::Close()
		{
			m_xraw2_class->Close();
		}

		void FinniganRawData::Load(char *file_n)
		{
			Open(file_n);
		}

		double FinniganRawData::GetSignalRange(int scan_num)
		{
			if (scan_num == mint_last_scan_num)
				return mdbl_signal_range ; 

			int lastWholeNumber = 0;
			double signal_range = 0 ; 
			VARIANT varMassList;
			VariantInit(&varMassList);
			VARIANT varPeakFlags;
			VariantInit(&varPeakFlags);
			long nArraySize = 0;

			_bstr_t bstr = "" ;
			BSTR bstr_type = bstr.GetBSTR() ; 
			double peak_width ;

			long scanN = scan_num;

			HRESULT res = m_xraw2_class->SetCurrentController(0,1) ;
			long nRet = m_xraw2_class->GetMassListFromScanNum (&scanN, 
						bstr_type,	// no filter
						0,			// no cutoff
						0,			// no cutoff
						0,			// all peaks returned
						FALSE,			// do not centroid
						&peak_width,
						&varMassList,		// mass list data
						&varPeakFlags,		// peak flags data
						&nArraySize );		// size of mass list array

			if( nArraySize )
			{
				// Get a pointer to the SafeArray
				SAFEARRAY FAR* psa = varMassList.parray;

				DataPeak* pDataPeaks = NULL;
				SafeArrayAccessData( psa, (void**)(&pDataPeaks) );

				double min_intensity = DBL_MAX ; 
				double max_intensity = DBL_MIN ; 

				for( long j=0; j<nArraySize; j++ )
				{
					double intensity = pDataPeaks[j].dIntensity ; 
					if (intensity > max_intensity) 
						max_intensity = intensity ; 
					if (intensity < min_intensity) 
						min_intensity = intensity ; 
				}
			
				signal_range = (max_intensity - min_intensity) ; 
			}

			if( varMassList.vt != VT_EMPTY )
			{
				SAFEARRAY FAR* psa = varMassList.parray;
				varMassList.parray = NULL;

				// Delete the SafeArray
				SafeArrayDestroy( psa );
			}

			if(varPeakFlags.vt != VT_EMPTY )
			{
				SAFEARRAY FAR* psa = varPeakFlags.parray;
				varPeakFlags.parray = NULL;

				// Delete the SafeArray
				SafeArrayDestroy( psa );
			}

			return signal_range ; 

		}

		double FinniganRawData::GetScanTime(int scan_num)
		{
			if (scan_num == mint_last_scan_num)
				return mdbl_last_scan_time ; 
			double start_time ; 

			m_xraw2_class->RTFromScanNum((long)scan_num, &start_time) ; 
			return start_time ; 
		}

	
		bool FinniganRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_points)
		{
			// Finnigan data is already truncated. Dont mess with it. 
			return GetRawData(mzs, intensities, scan_num) ; 
		}
	
		int FinniganRawData::GetParentScan(int scan_num)
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

		bool FinniganRawData::IsProfileScan(int scan_num)
		{

			long ms_level = 0;
			m_xraw2_class->IsProfileScanForScanNum(scan_num, &ms_level) ;
			if (ms_level == 1)
				return true ;
			else
				return false ;
		}

		bool FinniganRawData::IsFTScan(int scan_num)
		{	
			char ch_filter [512] ; 					
			_bstr_t bstr_filter ;
			m_xraw2_class->GetFilterForScanNum((long)scan_num, &bstr_filter.GetBSTR()) ;  
			strcpy(ch_filter,(char*)bstr_filter);		
			if (_strnicmp(ch_filter, "ft",2)== 0)
			{
				return true ; 
			}
			return false ;			
		}

		double FinniganRawData::GetAGCAccumulationTime(int scan_num)
		{			
			variant_t var_value ; 
			m_xraw2_class->GetTrailerExtraValueForScanNum((long)scan_num, "Ion Injection Time (ms):", &var_value.GetVARIANT());
			double time = 0.0 ; 
			time = (double) var_value.fltVal ; 
			return time ; 
		}

		double FinniganRawData::GetTICForScan(int scan_num)
		{
			long num_packets ; 
			double start_time ; 
			double low_mass ;
			double high_mass ; 
			double tic ; 
			double base_peak ; 
			double base_intensity ; 
			double frequency ; 
			long unif_time ; 
			long num_channels ; 

			m_xraw2_class->GetScanHeaderInfoForScanNum(scan_num, &num_packets, &start_time, &low_mass, &high_mass, &tic, &base_peak,
					&base_intensity, &num_channels, &unif_time, &frequency) ;  
			
			return tic ; 
		}

		

		int FinniganRawData::GetMSLevel(int scan_num)
		{
			int ms_level = 1;

			//gets the filter string
			char ch_filter [512] ; 					
			_bstr_t bstr_filter ;
			m_xraw2_class->GetFilterForScanNum((long)scan_num, &bstr_filter.GetBSTR()) ;  
			strcpy(ch_filter,(char*)bstr_filter);		
			
			//search for 'ms'
			for ( int chNum = 0; chNum < 512; chNum++)
			{				
				if (ch_filter[chNum] == 'm')
				{
					if(ch_filter[chNum+1] == 's')
					{
						chNum = chNum+2;
						char ch = ch_filter[chNum] ;
						char ch1 = ch_filter[chNum+1] ; 
						int ms = (int) ch ; 
						switch(ch)
						{
							case '2': ms_level = 2;
									  break;
							case '3': ms_level = 3;
									  break;
							case '4': ms_level = 4 ; 
									  break ;
							case '5': ms_level = 5 ; 
									  break ; 
							case '6': ms_level = 6 ; 
									  break ; 
							case '7': ms_level = 7 ; 
									  break ; 
							case '8': ms_level = 8 ; 
									  break ; 
							case '9': ms_level = 9 ; 
									  break ; 
							case '1': ms_level = 0 ; 
									break ;
						/*	case '10': ms_level  = 10 ; 
									   break ; 
							case '11': ms_level = 11 ; 
									   break ; 
							case '12': ms_level = 12 ; 
									   break ;
							case '13': ms_level  = 13 ; 
									   break ; 
							case '14': ms_level  = 14 ; 
									   break ; 
							case '15': ms_level  = 15 ; 
									   break ; */
							case ' ': ms_level = 1;
									  break;
							default : ms_level = 1;
									  break;
						}

						if (ms_level == 0)
						{
							switch(ch1)
							{
								case '0': ms_level  = 10 ; 
									   break ; 
								case '1': ms_level = 11 ; 
										break ; 
								case '2': ms_level = 12 ; 
										break ;
								case '3': ms_level  = 13 ; 
										break ; 
								case '4': ms_level  = 14 ; 
										break ; 
								case '5': ms_level  = 15 ; 
										break ; 
								case '6': ms_level  = 16 ; 
										break ;
								case '7': ms_level  = 17 ; 
										break ;
								case ' ':ms_level = 1 ; 
										break ; 
								default :ms_level = 1;
										break;
								
							}
						}
						return ms_level;
					}											
					
				}
			}
			return ms_level ; 
			
		
		}
		
		bool FinniganRawData::IsMSScan (int scan_num)
		{
			//Returns true if the scan is a MS-level scan

			int ms_level = GetMSLevel(scan_num);
			if (ms_level == 1)
				return true;
			else
				return false;
		}	
		
		double FinniganRawData::GetMonoMZFromHeader(int scan_num)
		{
			double mono_mz = 0 ; 
			variant_t var_value ; 
			m_xraw2_class->GetTrailerExtraValueForScanNum((long)scan_num, "Monoisotopic M/Z:", &var_value.GetVARIANT());
			mono_mz = var_value.dblVal ; 
			return mono_mz ; 				
		}

		short FinniganRawData::GetMonoChargeFromHeader(int scan_num) 
		{
			short cs = 0 ; 
			variant_t var_value ; 
			m_xraw2_class->GetTrailerExtraValueForScanNum((long)scan_num, "Charge State:", &var_value.GetVARIANT());
			cs = (short)var_value.iVal ; 
			return cs ;
		}


		
		double FinniganRawData::GetParentMz(int scan_num)
		{
			//Return the parent m/z of the particular msN scan
			double parent_mz = 0;
			
			//gets the filter string
			char ch_filter [512] ; 		
			char ch_mz[16];
			_bstr_t bstr_filter ;
			m_xraw2_class->GetFilterForScanNum((long)scan_num, &bstr_filter.GetBSTR()) ;  
			strcpy(ch_filter,(char*)bstr_filter);		

 			int ms_level = GetMSLevel(scan_num);
			
			int parent_count = 0;
			
			if (ms_level == 2)
			{
				for ( int chNum = 0; chNum < 512; chNum++)
				{					
					if (ch_filter[chNum] == '2')
					{
						chNum++;
						int mzIndex = 0;
						while(ch_filter[chNum] != '@')
						{
							ch_mz[mzIndex] = ch_filter[chNum];
							chNum++;
							mzIndex++;
						}
						break;
					}
				}
				
			}
			else
			{
				for (int chNum = 0; chNum < 512; chNum++)
				{
					if (ch_filter[chNum] == '@')
					{
						parent_count++;
						if (parent_count <= (ms_level - 1))
							break;
					}
				}
				
				while(ch_filter[chNum] != ' ')
						chNum ++;
				
				int mzIndex = 0;
				while(ch_filter[chNum] != '@')
				{
					ch_mz[mzIndex] = ch_filter[chNum];
					chNum++;
					mzIndex++;
				}
				
			}

			parent_mz = atof(ch_mz);		
			
			return parent_mz;

		}
		
		bool FinniganRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
			mint_last_scan_num = scan_num ; 
			int lastWholeNumber = 0;

			VARIANT varMassList;
			VariantInit(&varMassList);
			VARIANT varPeakFlags;
			VariantInit(&varPeakFlags);
			long nArraySize = 0;

			_bstr_t bstr = "" ;
			BSTR bstr_type = bstr.GetBSTR() ; 
			double peak_width ;

			long scanN = scan_num;

			HRESULT res = m_xraw2_class->SetCurrentController(0,1) ;
			long nRet = m_xraw2_class->GetMassListFromScanNum (&scanN, 
						bstr_type,	// no filter
						0,			// no cutoff
						0,			// no cutoff
						0,			// all peaks returned
						FALSE,			// do not centroid
						&peak_width,
						&varMassList,		// mass list data
						&varPeakFlags,		// peak flags data
						&nArraySize );		// size of mass list array

			long num_packets ; 
			double start_time ; 
			double low_mass ;
			double high_mass ; 
			double tic ; 
			double base_peak ; 
			double base_intensity ; 
			double frequency ; 
			long unif_time ; 
			long num_channels ; 

			m_xraw2_class->GetScanHeaderInfoForScanNum(scanN, &num_packets, &start_time, &low_mass, &high_mass, &tic, &base_peak,
				&base_intensity, &num_channels, &unif_time, &frequency) ;  
			mdbl_last_scan_time = start_time ; 
			if( nArraySize )
			{
				// Get a pointer to the SafeArray
				SAFEARRAY FAR* psa = varMassList.parray;

				DataPeak* pDataPeaks = NULL;
				SafeArrayAccessData( psa, (void**)(&pDataPeaks) );

				intensities->clear();
				mzs->clear();
				if ( nArraySize < (int)intensities->capacity())
				{
					intensities->reserve(nArraySize) ;
					mzs->reserve(nArraySize);
				}

				double min_intensity = DBL_MAX ; 
				double max_intensity = DBL_MIN ; 

				for( long j=0; j<nArraySize; j++ )
				{
					if (pDataPeaks[j].dMass > high_mass)
					{
						break ; 
					}
					double intensity = pDataPeaks[j].dIntensity ; 
					if (intensity > max_intensity) 
						max_intensity = intensity ; 
					if (intensity < min_intensity) 
						min_intensity = intensity ; 

					mzs->push_back(pDataPeaks[j].dMass) ;
					intensities->push_back(intensity) ;
				}
			
				mdbl_signal_range = (max_intensity - min_intensity) ; 

				// Release the data handle
				SafeArrayUnaccessData( psa );
			}

			if( varMassList.vt != VT_EMPTY )
			{
				SAFEARRAY FAR* psa = varMassList.parray;
				varMassList.parray = NULL;

				// Delete the SafeArray
				SafeArrayDestroy( psa );
			}

			if(varPeakFlags.vt != VT_EMPTY )
			{
				SAFEARRAY FAR* psa = varPeakFlags.parray;
				varPeakFlags.parray = NULL;

				// Delete the SafeArray
				SafeArrayDestroy( psa );
			}

			mint_last_scan_size = (int) mzs->size() ; 
			if (nArraySize == 0)
				return false ; 
			return true ; 
		}

		

		void FinniganRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{

			long num_packets ; 
			double start_time ; 
			double low_mass ;
			double high_mass ; 
			double tic ; 
			double base_peak ; 
			double base_intensity ; 
			double frequency ; 
			long unif_time ; 
			long num_channels ; 

			for (long scan_num = mlong_spectra_num_first ; scan_num <= mlong_spectra_num_last ; scan_num++)
			{
				m_xraw2_class->GetScanHeaderInfoForScanNum(scan_num, &num_packets, &start_time, &low_mass, &high_mass, &tic, &base_peak,
					&base_intensity, &num_channels, &unif_time, &frequency) ;  
				if (base_peak_tic)
					intensities->push_back(base_intensity) ; 
				else
					intensities->push_back(tic) ; 
				scan_times->push_back(start_time) ; 
			}

		}


		int FinniganRawData::GetXRawFileInstance(void)
		{
			CoInitialize( NULL );
			CLSID clsid ; 

			HRESULT res =  CLSIDFromString(L"{5FE970B2-29C3-11D3-811D-00104B304896}", &clsid ); 	

			if (res ==REGDB_E_WRITEREGDB)
			{
				throw "Unable to instantiate Finnigan objects: XRawFile from XRawfile.dll (version 2.0.0). Please check that the following dlls from XCalibur are avaialble on your system: CFRDBResources.dll, CFRUtil.dll, ExploreDataObjects.dll, ExploreDataObjectsManaged.dll, ExploreDataObjectsps.dll, FControl2.dll, Fglobal.dll, Fileio.dll, finDB.dll, finSSClientLib.dll, Fregistry.dll, XRawfile2.dll" ;
			}
			IID riid ;

			res = IIDFromString(L"{5FE970B1-29C3-11D3-811D-00104B304896}", &riid) ; 

			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate Finnigan objects: XRawFile from XRawfile.dll (version 2.0.0). Please check that the following dlls from XCalibur are avaialble on your system: CFRDBResources.dll, CFRUtil.dll, ExploreDataObjects.dll, ExploreDataObjectsManaged.dll, ExploreDataObjectsps.dll, FControl2.dll, Fglobal.dll, Fileio.dll, finDB.dll, finSSClientLib.dll, Fregistry.dll, XRawfile2.dll" ;
			}

			res = CoCreateInstance(clsid, NULL,CLSCTX_INPROC_SERVER, riid, (void **) &m_xraw2_class);
			if(res != S_OK)
			{
				throw "Unable to instantiate Finnigan objects: XRawFile from XRawfile.dll (version 2.0.0). Please check that the following dlls from XCalibur are avaialble on your system: CFRDBResources.dll, CFRUtil.dll, ExploreDataObjects.dll, ExploreDataObjectsManaged.dll, ExploreDataObjectsps.dll, FControl2.dll, Fglobal.dll, Fileio.dll, finDB.dll, finSSClientLib.dll, Fregistry.dll, XRawfile2.dll" ;
			}
			
			return 0;
		}
	}
}
#endif