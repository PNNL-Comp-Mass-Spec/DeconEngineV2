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
#using <mscorlib.dll>
#include "clsPeakProcessor.h" 
#include "clsTransformResults.h" 
#include "clsHornTRansformParameters.h" 
#include "clsRawData.h" 
#include "clsDTAGenerationParameters.h"
namespace DeconToolsV2
{
	public enum enmProcessState {IDLE = 0, RUNNING, COMPLETE, ERROR}; 
	
	public ref class clsProcRunner
	{
		int mint_percent_done ; 
		int mint_current_scan ; 
		enmProcessState menm_state ; 
		System::String ^mstr_file_name ; 
		System::String ^mstr_output_path_for_dta_creation ; 
		DeconToolsV2::Readers::FileType menm_file_type ;
		DeconToolsV2::Peaks::clsPeakProcessorParameters ^mobj_peak_parameters ; 
		DeconToolsV2::HornTransform::clsHornTransformParameters ^mobj_transform_parameters ; 
		DeconToolsV2::Readers::clsRawDataPreprocessOptions ^mobj_fticr_preprocess_parameters ; 
		DeconToolsV2::Results::clsTransformResults ^mobj_results ; 
		DeconToolsV2::DTAGeneration::clsDTAGenerationParameters ^mobj_dta_generation_parameters ;		

		DeconToolsV2::Results::clsTransformResults^ CreateTransformResults(System::String ^file_name, 
			DeconToolsV2::Readers::FileType file_type, DeconToolsV2::Peaks::clsPeakProcessorParameters ^peak_parameters, 
			DeconToolsV2::HornTransform::clsHornTransformParameters ^transform_parameters, 
			DeconToolsV2::Readers::clsRawDataPreprocessOptions ^fticr_preprocess_parameters, 
			bool save_peaks, bool transform) ;
	public:
		clsProcRunner(void);
		~clsProcRunner(void);
		void Reset()
		{
			mint_percent_done = 0 ; 
			mint_current_scan = 0 ; 
			menm_state = IDLE ; 
		}
		
        //void OutfileNames(char* fileName, char* iso_file_name, char* scan_file_name, char* data_file_name);
		void CreateTransformResultWithPeaksOnly() ; 
		void CreateTransformResultWithNoPeaks() ; 
		void CreateTransformResults() ; 
		void MemoryTest() ; 
		void CreateDTAFile() ; 

		int CurrentScanNum()
		{
			if (menm_state == IDLE)
				return  0 ;
			return  mint_current_scan ;
		}

		int PercentDone()
		{
			if (menm_state == IDLE)
				return  0 ;
			else if (menm_state == RUNNING)
				return  mint_percent_done ;
			else
				return 100 ; 
		}
		int ProcessState()
		{
			return menm_state ; 
		}
		System::String^ FileName()
		{
			return mstr_file_name ; 
		}
		void FileName(System::String^ file_name)
		{
			mstr_file_name = file_name ; 
		}		
		DeconToolsV2::Readers::FileType FileType()
		{
			return menm_file_type ; 
		}
		void FileType(DeconToolsV2::Readers::FileType file_type)
		{
			menm_file_type = file_type ; 
		}
		System::String^ OutputPathForDTACreation()
		{
			return mstr_output_path_for_dta_creation ; 
		}
		void OutputPathForDTACreation(System::String^ path)
		{
			mstr_output_path_for_dta_creation = path ; 
		}
		int trial()
		{
			return 1 ;
		}
		DeconToolsV2::Peaks::clsPeakProcessorParameters^ PeakProcessorParameters()
		{
			return mobj_peak_parameters ; 
		}
		void PeakProcessorParameters(DeconToolsV2::Peaks::clsPeakProcessorParameters ^peak_parameters)
		{
			mobj_peak_parameters = peak_parameters ; 
		}
		DeconToolsV2::HornTransform::clsHornTransformParameters^ HornTransformParameters()
		{
			return mobj_transform_parameters ; 
		}
		void HornTransformParameters(DeconToolsV2::HornTransform::clsHornTransformParameters ^transform_parameters)
		{
			mobj_transform_parameters = transform_parameters ; 
		}
		DeconToolsV2::Results::clsTransformResults^ HornTransformResults()
		{
			return mobj_results ; 
		}
		DeconToolsV2::DTAGeneration::clsDTAGenerationParameters^ DTAGenerationParameters()
		{
			return mobj_dta_generation_parameters ;
		}
		void  DTAGenerationParameters(DeconToolsV2::DTAGeneration::clsDTAGenerationParameters ^dta_generation_parameters)
		{
			mobj_dta_generation_parameters  = dta_generation_parameters;
		}
		DeconToolsV2::Readers::clsRawDataPreprocessOptions^ FTICRPreprocessOptions()
		{
			return mobj_fticr_preprocess_parameters ;
		}	
		void  FTICRPreprocessOptions(DeconToolsV2::Readers::clsRawDataPreprocessOptions ^parameters)
		{
			mobj_fticr_preprocess_parameters = parameters ;
		}
		
	};
}