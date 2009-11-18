// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0


#include "AgilentRawData.h"
#include <map> 
#include <iostream>
#include <float.h>
#include <fstream> 

using namespace	std	; 
#ifdef ANALYST_INSTALLED
namespace Engine 
{
	namespace Readers
	{

		AgilentRawData::AgilentRawData(void)
		{
			GetSpecData() ;	
		}
		int	AgilentRawData::GetSpecData(void)
		{
			HRESULT	res	= IIDFromString(L"{26E42183-9803-11D1-A9F6-0060977F5C78}", &mriid_wiff_file) ; 
			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate Agilent object: IFMANWiffFile from ExploreDataObjects.dll.  You will need to install Analyst on this computer first or convert your file to MZXml format using mzStar on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			CoInitialize( NULL );
			CLSID clsid	; 

			res	=  CLSIDFromString(L"{C09D0EF8-926D-11D1-A9F2-0060977F5C78}", &clsid );		
			if (res ==REGDB_E_WRITEREGDB)
			{
				throw "Unable to instantiate Agilent object: FManSpecData from ExploreDataObjects.dll. You will need to install Analyst on this computer first or convert your file to MZXml format using mzStar on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			IID	riid ;
			res	= IIDFromString(L"{C09D0EF7-926D-11D1-A9F2-0060977F5C78}", &riid) ;	
			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate Agilent object: FMANSpecData from ExploreDataObjects.dll.  You will need to install Analyst on this computer first or convert your file to MZXml format using mzStar on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			res	= CoCreateInstance(clsid, NULL,CLSCTX_INPROC_SERVER, riid, (void **) &mobj_spec_data);
			if(res != S_OK)
			{
				throw "Unable to instantiate Agilent object: FMANSpecData from ExploreDataObjects.dll.  You will need to install Analyst on this computer first or convert your file to MZXml format using mzStar on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			// clsid for FMANChromData
			res	= IIDFromString(L"{C09D0EF5-926D-11D1-A9F2-0060977F5C78}", &riid) ;	
			if (res == E_INVALIDARG)
			{
				throw "Unable to instantiate Agilent object: IFMANChromData from ExploreDataObjects.dll.  You will need to install Analyst on this computer first or convert your file to MZXml format using mzStar on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			res	=  CLSIDFromString(L"{C09D0EF6-926D-11D1-A9F2-0060977F5C78}", &clsid );		
			if (res ==REGDB_E_WRITEREGDB)
			{
				throw "Unable to instantiate Agilent object: IFMANChromData from ExploreDataObjects.dll. You will need to install Analyst on this computer first or convert your file to MZXml format using mzStar on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}
			res	= CoCreateInstance(clsid, NULL,CLSCTX_INPROC_SERVER, riid, (void **) &mobj_chrom_data);
			if(res != S_OK)
			{
				throw "Unable to instantiate Agilent object: IFMANChromData from ExploreDataObjects.dll.  You will need to install Analyst on this computer first or convert your file to MZXml format using mzStar on http://sashimi.sourceforge.net/software_glossolalia.html." ;
			}

			if(res != S_OK)
				return 1;


			return 0;
		}


		AgilentRawData::~AgilentRawData(void) 
		{
			Close() ; 
		}

		void AgilentRawData::Load(char *file)
		{
			strcpy(marr_filename, file)	; 
			_bstr_t	bstr = marr_filename ;
			BSTR bstr_type = bstr.GetBSTR()	;
			HRESULT	res	= mobj_spec_data->put_WiffFileName(bstr_type) ;	
			IUnknownPtr	wiff_obj = mobj_spec_data->GetWiffFileObject() ; 
			res	= wiff_obj->QueryInterface(mriid_wiff_file,	(void **)&mobj_wiff_file) ;	  

			try
			{
				mlng_sample_num	= 1	; 
				mlng_period_num	= 0	; 
				mlng_experiment_num	= 0	; 
				mlng_num_samples = mobj_wiff_file->GetActualNumberOfSamples() ;	
				mlng_num_periods = mobj_wiff_file->GetActualNumberOfPeriods(mlng_sample_num) ; 
				mlng_num_experiments = mobj_wiff_file->GetNumberOfExperiments(mlng_sample_num,mlng_period_num) ; 
			} catch (_com_error& e)
			{
				std::cerr<<e.ErrorMessage()<<std::endl ; 
			}
			try
			{
				mobj_wiff_file->GetActualPeriodAndCycleFromTime(mlng_sample_num, mlng_experiment_num, 0, &mlng_start_period,	&mlng_start_cycle) ; 
				mobj_wiff_file->GetActualPeriodAndCycleFromTime(mlng_sample_num, mlng_experiment_num, 1000*60, &mlng_stop_period, &mlng_stop_cycle)	; 
			}catch (_com_error& e)
			{
				std::cerr<<e.ErrorMessage()<<std::endl ; 
			}
		}

		double AgilentRawData::GetScanTime(int scan_num)
		{
			if (scan_num ==	mint_last_scan_num)
				return mdbl_last_scan_time ; 
			float start_time = mobj_wiff_file->GetActualTimeFromPeriodAndCycle(mlng_sample_num,	mlng_period_num, mlng_experiment_num, (long) scan_num) ; 
			start_time = start_time/60.0f ; 
			return start_time ;	
		}

		const char* AgilentRawData::GetFileName()
		{
			return marr_filename ; 
		}

		int AgilentRawData::GetParentScan(int scan_num)
		{
			//future work 
			return 0;
		}

		bool AgilentRawData::IsMSScan(int scan_num)
		{
			//future work
			return true;
		}

		bool AgilentRawData::IsProfileScan(int scan_num)
		{
			//future work
			return true;
		}

		
		double AgilentRawData::GetParentMz(int scan_num)
		{
			//future work
			return 0;
		}
		int AgilentRawData::GetMSLevel(int scan_num)
		{			//future work
			return 1 ; 
		}

		void AgilentRawData::Close() 
		{
			try
			{
				if (mobj_wiff_file != NULL)
					mobj_wiff_file->CloseWiffFile()	;
				if (mobj_spec_data != NULL)
					mobj_spec_data->Release() ;  
				if (mobj_wiff_file != NULL) 
					mobj_wiff_file->Release() ; 
				if (mobj_chrom_data != NULL)
					mobj_chrom_data->Release() ; 
			}
			catch (System::Runtime::InteropServices::SEHException *err)
			{
				if (err != NULL)
					std::cerr<<"Close error with Agilent"<<std::endl ; 
			}
			catch (System::NullReferenceException *err)
			{
				std::cerr<<"Here"<<std::endl ; 
			}
			catch (_com_error *e)
			{
				if (e != NULL)
					std::cerr<<e->Description()<<std::endl ; 
			}
			catch (char *mesg)
			{
				std::cerr<<mesg<<std::endl ; 
			}
		}

		bool AgilentRawData::GetRawData(std::vector <double>	*mzs, std::vector<double> *intensities, int scan_num) 
		{
			return GetRawData(mzs, intensities,	scan_num, -1) ;	
		}

		void AgilentRawData::GetScanDescription(int scan, char *description)
		{
			_bstr_t bstr_filter ;
			bstr_filter = mobj_spec_data->GetDataTitle() ; 
			strcpy(description,(char*)bstr_filter);		
		}

		bool AgilentRawData::GetRawData(std::vector <double>	*mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
			float start_time = mobj_wiff_file->GetActualTimeFromPeriodAndCycle(mlng_sample_num,	mlng_period_num, mlng_experiment_num, (long) scan_num) ; 
			//start_time = start_time/60 ; 
			mdbl_last_scan_time	= start_time ; 
			mint_last_scan_num = scan_num ;	

			mobj_spec_data->SetSpectrum(mlng_sample_num, mlng_period_num, mlng_experiment_num, start_time, start_time) ;  
			mlng_num_points	= mobj_spec_data->GetNumberOfDataPoints() ;	
			if (num_pts	> 0)
				mlng_num_points	= num_pts ;	


			mzs->clear() ; 
			intensities->clear() ;
			if ((int)mzs->capacity() < mlng_num_points)
			{
				mzs->reserve(mlng_num_points) ;	
				intensities->reserve(mlng_num_points) ;	
			}

			double mz, intensity ; 
			double min_intensity = DBL_MAX ; 
			double max_intensity = DBL_MIN ; 
			for	(long i	= 0	; i	< mlng_num_points ;	i++)
			{
				mobj_spec_data->GetDataPoint(i,	&mz, &intensity) ; 
				if (mz == 0)
					continue ;
				if (intensity >	max_intensity) 
					max_intensity =	intensity ;	
				if (intensity <	min_intensity) 
					min_intensity =	intensity ;	

				mzs->push_back(mz) ; 
				intensities->push_back(intensity) ;	
			}

			mdbl_signal_level =	max_intensity -	min_intensity ;	

			return true	; 
		}

		int	AgilentRawData::GetScanSize() 
		{
			return mlng_num_points ; 
		}


		int	AgilentRawData::GetNumScans()
		{
			return (mlng_stop_cycle	- mlng_start_cycle + 1)	; 
		}


		double AgilentRawData::GetSignalRange(int scan_num)
		{
			if (scan_num ==	mint_last_scan_num)
				return mdbl_signal_level ; 


			float start_time = mobj_wiff_file->GetActualTimeFromPeriodAndCycle(mlng_sample_num,	mlng_period_num, mlng_experiment_num, (long) scan_num) ; 
			start_time = start_time/60 ;

			mobj_spec_data->SetSpectrum(mlng_sample_num, mlng_period_num, mlng_experiment_num, start_time, start_time) ;  
			mlng_num_points	= mobj_spec_data->GetNumberOfDataPoints() ;	

			double mz, intensity ; 
			double min_intensity = DBL_MAX ; 
			double max_intensity = DBL_MIN ; 
			for	(long i	= 0	; i	< mlng_num_points ;	i++)
			{
				mobj_spec_data->GetDataPoint(i,	&mz, &intensity) ; 
				if (mz == 0)
					continue ;
				if (intensity >	max_intensity) 
					max_intensity =	intensity ;	
				if (intensity <	min_intensity) 
					min_intensity =	intensity ;	
			}
			return (max_intensity -	min_intensity) ; 

		}

		void AgilentRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double>	*scan_times, bool base_peak_tic)
		{
			long all_data_available	; 
			double offset =	0 ;	
			mobj_wiff_file->GetTIC(mlng_sample_num,	mlng_period_num, mlng_experiment_num, 1, mlng_stop_cycle, offset, 
					mobj_chrom_data, &all_data_available) ;	
			int	num_pts	= mobj_chrom_data->GetNumberOfDataPoints() ; 
			for	(int i = 0 ; i < num_pts ; i++)
			{
				double time, intensity	;
				mobj_chrom_data->GetDataPoint(i, &time,	&intensity)	; 
				intensities->push_back(intensity) ;	
				scan_times->push_back(time)	; 
			}
			return ; 
		}
	}
}
#endif 
