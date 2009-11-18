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
#include "Results/LCMSTransformResults.h"
#include "clsLCMSPeak.h"
#include "DeconEngineUtils.h" 
#include "clsRawData.h"

namespace DeconToolsV2 
{
	namespace Results
	{
		public __gc class clsTransformResults
		{
			int mint_percent_done ; 
			DeconToolsV2::Readers::FileType menmFileType ; 
		public:
			Engine::Results::LCMSTransformResults __nogc *mobj_lcms_results ;
			int GetMinScan() ; 
			int GetMaxScan() ; 
			int GetNumPeaks() ; 
			void SetLCMSTransformResults(Engine::Results::LCMSTransformResults *results) ; 
			bool IsDeisotoped() ; 
			void GetRawData(DeconToolsV2::Results::clsLCMSPeak* (&lcms_peaks) __gc []) ;
			void GetRawDataSortedInIntensity(DeconToolsV2::Results::clsLCMSPeak* (&lcms_peaks) __gc []) ;
			void GetSIC(int min_scan, int max_scan, float mz, float mz_tolerance, float (&peak_intensities) __gc []) ; 			
			void GetScanPeaks(int scan_num, float (&peak_mzs) __gc [], float (&peak_intensities) __gc []) ; 
			void WriteResults(System::String *fileName, bool save_signal_range) ; 
			void WriteScanResults(System::String *fileName) ; 
			void ReadResults(System::String *fileName) ; 
			clsTransformResults(void);
			~clsTransformResults(void);

			__property System::String* get_FileName()
			{
				System::String *fileName = S"" ; 
				DeconEngine::Utils::GetStr(mobj_lcms_results->GetFileName(), &fileName) ; 
				return fileName ; 
			}

			__property DeconToolsV2::Readers::FileType get_FileType()
			{
				return menmFileType ; 
			}

			__property void set_FileType(DeconToolsV2::Readers::FileType fileType)
			{
				menmFileType = fileType ; 
			}

			__property float get_MonoMasses() __gc[] 
			{
				int numDeisotoped = mobj_lcms_results->GetNumTransforms() ; 
				float mono_masses __gc [] = new float __gc [numDeisotoped] ;   
				for (int index = 0 ; index < numDeisotoped ; index++)
				{
					Engine::HornTransform::IsotopeFitRecord fitRecord = mobj_lcms_results->GetIsoPattern(index) ; 
					mono_masses[index] = (float) fitRecord.mdbl_mono_mw ; 
				}
				return mono_masses ; 
			}

			__property short get_Charges() __gc[] 
			{
				int numDeisotoped = mobj_lcms_results->GetNumTransforms() ; 
				short charges __gc [] = new short __gc [numDeisotoped] ;   
				for (int index = 0 ; index < numDeisotoped ; index++)
				{
					Engine::HornTransform::IsotopeFitRecord fitRecord = mobj_lcms_results->GetIsoPattern(index) ; 
					charges[index] = fitRecord.mshort_cs ; 
				}
				return charges ; 
			}

			__property float get_Scans() __gc[] 
			{
				int numDeisotoped = mobj_lcms_results->GetNumTransforms() ; 
				float scans __gc [] = new float __gc [numDeisotoped] ;   
				for (int index = 0 ; index < numDeisotoped ; index++)
				{
					Engine::HornTransform::IsotopeFitRecord fitRecord = mobj_lcms_results->GetIsoPattern(index) ; 
					scans[index] = (float) fitRecord.mint_scan_num ; 
				}
				return scans ; 
			}

		};
	}
}