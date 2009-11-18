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
#include "../Calibrations/Calibrator.h"
#include <vector>

namespace Engine {
 namespace Readers
{

	const int MAX_FNAME_LEN = 512 ;
	const int MAX_ERR_LEN = 512 ;

	enum FileType { BRUKER = 0, FINNIGAN, MICROMASSRAWDATA, AGILENT_TOF, SUNEXTREL, ICR2LSRAWDATA, MZXMLRAWDATA, PNNL_IMS, BRUKER_ASCII, ASCII, PNNL_UIMF, YAFMS} ;

	enum CalibrationType	{ A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C_OVERFCUBE = 0, A_OVER_F_PLUS_B_OVER_FSQ,
						A_OVER_F_PLUS_B_OVER_FSQ_PLUS_CI_OVERFSQ, A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C,
						AF_PLUS_B, F, A_OVER_F_PLUS_B,  A_OVER_F_PLUS_B_PLUS_CI,
						TIME_A_TSQ_PLUS_B_T_PLUS_C, BRUKER_CALMET, UNDEFINED} ;

	static int BACKGROUND_RATIO_FOR_TIC = 3 ;
	static double MIN_MZ = 400 ;
	static double MAX_MZ = 2000 ;

	bool IsDir(char *path) ;

	class   RawData
	{
		FileType menm_file_type ;
	protected:
		Calibrations::CCalibrator *mobj_calibrator ;
		virtual void GetSummedSpectra(std::vector <double> *bins, std::vector <double> *intensities, int scan, int scan_range,
			double min_mz, double max_mz, double mz_bin) ;
		virtual void GetRawData(std::vector <double> &vectMZs, std::vector<double> &vectIntensities, int scan, double min_mz,
			double max_mz) ;

	public:
		static const int MAX_SCAN_SIZE = 4*1024*1024 ;
		RawData(void)  ;
		virtual ~RawData(void) ;
		virtual const char *GetFileName() = 0 ;
		virtual FileType GetFileType() = 0 ;
		virtual void SetCalibrator (Calibrations::CCalibrator *calib) ;
		virtual Calibrations::CCalibrator *GetCalibrator() { return mobj_calibrator ; } ;
		virtual bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)= 0 ;
		virtual bool GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)= 0 ;
		virtual int GetMassIndex(double  mz) ;
		virtual void Load(char *file_n) = 0 ;
		virtual void Close() {} ;
		virtual int GetNumScans() = 0 ;
		virtual double GetScanTime(int scan_num) = 0 ;
		virtual int GetScanSize() = 0 ;
		virtual int GetNumScansLoaded() { return GetNumScans() ; } ;
		virtual short GetSpectrumType(int scan_num)
		{
			return 0 ;
		}
		virtual void GetScanDescription(int scan, char *description)
		{
			strcpy(description, "Scan #") ;
			_itoa(scan, &description[strlen(description)], 10) ;
		}
		virtual double GetSignalRange(int scan_num) = 0 ;
		virtual void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) = 0  ;
		virtual int GetNextScanNum(int current_scan_num) { return current_scan_num + 1 ; } ;
		virtual int GetFirstScanNum() { return 1 ; } ;
		virtual int GetLastScanNum() { return 1 ; } ;
		virtual int GetParentScan(int scan_num) = 0;
		virtual bool IsMSScan(int scan_num) = 0;

		virtual int GetMSLevel(int scan_num) = 0 ;
		virtual bool IsProfileScan(int scan_num) = 0;
		virtual double GetParentMz(int scan_num)  = 0;

		virtual bool IsFTScan(int scanNum)
		{
			return false ;
		}
		virtual double  GetMonoMZFromHeader(int scan_num)
		{
			return 0 ;
		}
		virtual short GetMonoChargeFromHeader(int scan_num)
		{
			return 0 ;
		}
		virtual int GetFrameNumber()
		{
			return 0 ;
		};
		virtual void GetSummedSpectra(std::vector <double> *bins, std::vector <double> *intensities,
			int scan, int scan_range) ;
		virtual void GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities,
			int scan, int scan_range, double min_mz, double max_mz)  ;
		virtual double GetAGCAccumulationTime(int scan_num)
		{
			return 0 ;
		}
		virtual double GetTICForScan(int scan_num)
		{
			return 0 ;
		}
	};
}
}
