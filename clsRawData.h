// Written by Navdeep Jaitly and Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include "Readers\ReaderFactory.h"
#include "Readers\AsciiRawData.h"
#include "clsRawDataPreprocessOptions.h" 
#include "CalibrationSettings.h"
#include "DeconEngineUtils.h"
#include "Readers\Icr2LSRawData.h" 

#using <mscorlib.dll>
//using namespace UIMFLibrary;

namespace DeconToolsV2
{
 namespace Readers
	{
		public enum FileType {BRUKER = 0, FINNIGAN, MICROMASSRAWDATA, AGILENT_TOF, SUNEXTREL, ICR2LSRAWDATA, MZXMLRAWDATA, PNNL_IMS, BRUKER_ASCII, ASCII, PNNL_UIMF, YAFMS, UNDEFINED} ; 

		public ref class clsRawData
		{
			Engine::Readers::RawData *mobj_raw_data ; 
			clsRawDataPreprocessOptions ^mobj_preprocess_options ; 

		public:
			clsRawData();
			clsRawData(System::String ^file_name, FileType file_type);
			int GetFirstScanNum() ; 


			void SetFFTCalibrationValues(CalibrationSettings ^calSettings);

			~clsRawData(void);

			DeconToolsV2::Readers::FileType FileType()
			{
				if (mobj_raw_data == NULL)
					return DeconToolsV2::Readers::FileType::UNDEFINED ; 
				return (DeconToolsV2::Readers::FileType) mobj_raw_data->GetFileType() ; 
			}

			DeconToolsV2::Readers::clsRawDataPreprocessOptions^ FTICRRawPreprocessOptions()
			{
				return mobj_preprocess_options ; 
			}
			void FTICRRawPreprocessOptions(DeconToolsV2::Readers::clsRawDataPreprocessOptions ^value)
			{
				mobj_preprocess_options = value ; 
				if (mobj_raw_data != NULL && mobj_raw_data->GetFileType() == ICR2LSRAWDATA)
				{
					Engine::Readers::Icr2lsRawData *icr_raw_data = (Engine::Readers::Icr2lsRawData *)mobj_raw_data ; 
					icr_raw_data->SetApodizationZeroFillOptions((Engine::Utilities::ApodizationType)mobj_preprocess_options->ApodizationType,
						mobj_preprocess_options->ApodizationMinX, mobj_preprocess_options->ApodizationMaxX, 
						mobj_preprocess_options->ApodizationPercent, mobj_preprocess_options->NumZeroFills) ; 
					if (mobj_preprocess_options->ApplyCalibration())
					{
						icr_raw_data->OverrideDefaultCalibrator((Engine::Readers::CalibrationType)mobj_preprocess_options->CalibrationType,
							mobj_preprocess_options->A, mobj_preprocess_options->B, mobj_preprocess_options->C) ; 
					}
				}
			}

			System::String^ FileName()
			{
				System::String ^file_name = "" ; 
				if (mobj_raw_data == NULL)
					return nullptr ; 
				DeconEngine::Utils::GetStr(mobj_raw_data->GetFileName(), &file_name) ; 
				return file_name ; 
			}

			int PercentDone()
			{
				if (mobj_raw_data == NULL)
					return 0 ; 
				if (mobj_raw_data->GetFileType() == ASCII)
				{
					return ((Engine::Readers::AsciiRawData *)mobj_raw_data)->GetPercentDone() ; 
				}
				int num_scans = mobj_raw_data->GetNumScans() ; 
				if (num_scans != 0)
				{
					int percent_done = (100 * mobj_raw_data->GetNumScansLoaded())/ num_scans  ; 
					if (percent_done < 0)
						return 0 ; 
					if (percent_done > 100)
						return 100 ; 
					return percent_done ; 
				}
				return 0 ; 
			}

			System::String^ StatusMessage()
			{
				if (mobj_raw_data == NULL)
					return "" ; 
				int current_scan = mobj_raw_data->GetNumScansLoaded() ; 
				int num_scans = mobj_raw_data->GetNumScans() ; 
				return System::String::Concat("Processed :", System::Convert::ToString(current_scan), " of ", System::Convert::ToString(num_scans), " scans") ; 
			}

			bool IsFTScan(int scanNum)
			{
				return mobj_raw_data->IsFTScan(scanNum) ; 
			}

			void LoadFile(char *file_name, DeconToolsV2::Readers::FileType file_type) ; 
			void LoadFile(array<char>^ file_name, DeconToolsV2::Readers::FileType file_type) ; 
			void LoadFile(System::String ^file_name, DeconToolsV2::Readers::FileType file_type) ; 

			void Close() ; 
			int GetNumScans() ;
			double GetScanTime(int scan_num) ; 		
			int GetMSLevel(int scan_num) ; 
			int GetScanSize() ; 
			short GetSpectrumType(int scan_num) ;
            void GetTicFromFile(array<float> ^ (&intensities), array<float> ^ (&scan_times), bool base_peak_tic);
			int GetTIC(int scan_num);
			
            void GetSpectrum(int scan_num, array<double> ^ (&mzs), array<double> ^ (&intensities));
			int GetParentScan(int scan_num);
            void GetMzsInRange(array<float> ^ (&in_mzs), array<float> ^ (&in_intensities), array<float> ^ (&out_mzs), array<float> ^ (&out_intensities), float central_value, float range);
			System::String ^GetScanDescription(int scan_num) ; 
			double GetFTICRSamplingRate() ;
            void GetFTICRTransient(array<float> ^ (&in_intensities));
            void GetSummedSpectra(int current_scan, int scan_range, array<double> ^ (&mzs), array<double> ^ (&intensities));
            void GetSummedSpectra(int start_scan, int stop_scan, double min_mz, double max_mz, array<double> ^ (&mzs), array<double> ^ (&intensities));

			/*double GetDriftTime(int scanNum);
			double GetFramePressure(int frameNum);
			int GetNumOfFrames();*/
			/*void GetSummedFrameSpectra(double (&mzs) __gc[], double (&intensities) __gc[], 
				int startFrame, int endFrame, double min_mz, double max_mz, int imsScanNum);*/

			/*void GetSummedFrameAndScanSpectra(DataReader *uimfDataReader, double (&mzs) __gc[], double (&intensities) __gc[],
				int start_frame, int end_frame, int ims_start_scan, int ims_end_scan, double min_mz, double max_mz, int numBins );*/




			

		};
	}
}