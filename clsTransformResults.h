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
		public ref class clsTransformResults
		{
			int mint_percent_done ; 
			DeconToolsV2::Readers::FileType menmFileType ; 
		public:
			Engine::Results::LCMSTransformResults *mobj_lcms_results ;
			int GetMinScan() ; 
			int GetMaxScan() ; 
			int GetNumPeaks() ; 
			void SetLCMSTransformResults(Engine::Results::LCMSTransformResults *results) ; 
			bool IsDeisotoped() ; 
			void GetRawData(array<DeconToolsV2::Results::clsLCMSPeak^>^ (&lcms_peaks)) ;
			void GetRawDataSortedInIntensity(array<DeconToolsV2::Results::clsLCMSPeak^>^ (&lcms_peaks)) ;
            void GetSIC(int min_scan, int max_scan, float mz, float mz_tolerance, array<float> ^ (&peak_intensities));
            void GetScanPeaks(int scan_num, array<float> ^ (&peak_mzs), array<float> ^ (&peak_intensities));
			void WriteResults(System::String ^fileName, bool save_signal_range) ; 
			void WriteScanResults(System::String ^fileName) ; 
			void ReadResults(System::String ^fileName) ; 
			clsTransformResults(void);
			~clsTransformResults(void);

			System::String^ FileName()
			{
				System::String ^fileName = "" ; 
				DeconEngine::Utils::GetStr(mobj_lcms_results->GetFileName(), &fileName) ; 
				return fileName ; 
			}

			DeconToolsV2::Readers::FileType FileType()
			{
				return menmFileType ; 
			}

			void FileType(DeconToolsV2::Readers::FileType fileType)
			{
				menmFileType = fileType ; 
			}

			array<float>^ MonoMasses() 
			{
				int numDeisotoped = mobj_lcms_results->GetNumTransforms() ; 
                array<float>^ mono_masses = gcnew array<float>(numDeisotoped);
				for (int index = 0 ; index < numDeisotoped ; index++)
				{
					Engine::HornTransform::IsotopeFitRecord fitRecord = mobj_lcms_results->GetIsoPattern(index) ; 
					mono_masses[index] = (float) fitRecord.mdbl_mono_mw ; 
				}
				return mono_masses ; 
			}

			array<short>^ Charges()
			{
				int numDeisotoped = mobj_lcms_results->GetNumTransforms() ; 
				array<short>^ charges = gcnew array<short>(numDeisotoped);   
				for (int index = 0 ; index < numDeisotoped ; index++)
				{
					Engine::HornTransform::IsotopeFitRecord fitRecord = mobj_lcms_results->GetIsoPattern(index) ; 
					charges[index] = fitRecord.mshort_cs ; 
				}
				return charges ; 
			}

            array<float>^ Scans()
			{
				int numDeisotoped = mobj_lcms_results->GetNumTransforms() ; 
                array<float>^ scans = gcnew array<float>(numDeisotoped);
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