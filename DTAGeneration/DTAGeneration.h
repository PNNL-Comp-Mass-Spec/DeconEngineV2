// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "MSnInformationRecord.h"
#include "ProfileRecord.h"
#include "../PeakProcessor/PeakProcessor.h" 
#include "../HornTransform/MassTransform.h" 
#include "../ChargeDetermination/SVMChargeDetermination.h"
#include "../Readers/RawData.h"
#include "../DeconEngineUtils.h"
#include <map>

namespace Engine
{
	namespace DTAProcessing
	{

		class DTAProcessor
		{		
			int mint_minIonCount;
			int mint_minScan;
			int mint_maxScan;			
			int mint_window_size ; 
			int mint_consider_charge ; 
			int mint_isolation_window_size ; 

			double mdbl_minMass;
			double mdbl_maxMass;
			double mdbl_pkBkgRatio;
			double mdbl_peptideMinBkgRatio;
			double mdbl_min_peptide_intensity;
			double mdbl_parent_Intensity;
			double mdbl_parent_Mz;				
			double mdbl_cc_mass ;
			double mdbl_min_fit_for_single_spectra  ; 

			double mdbl_mono_mz_from_header ; 

			bool mbln_create_log_file_only ; 
			bool mbln_create_composite_dta ; 
			bool mbln_consider_multiple_precursors ; 
			bool mbln_is_profile_data_for_mzXML ; 

			std::vector<double> mvect_mzs_parent;
			std::vector<double> mvect_intensities_parent;
			std::vector<double> mvect_mzs_msN;
			std::vector<double> mvect_intensities_msN;
			std::vector<int> mvect_chargeStateList;

			Engine::PeakProcessing::Peak mobj_parentPeak ; 			
			Engine::HornTransform::IsotopeFitRecord mobj_transformRecord ; 	
			Engine::DTAProcessing::MSnInformationRecord mobj_msn_record ;
			Engine::DTAProcessing::ProfileRecord mobj_profile_record ; 
			Engine::PeakProcessing::PeakProcessor *mobj_parent_peak_processor;			
			Engine::PeakProcessing::PeakProcessor *mobj_msN_peak_processor;
			Engine::PeakProcessing::PeakProcessor *mobj_summed_peak_processor ; 
			Engine::HornTransform::MassTransform *mobj_mass_transform;
			Engine::ChargeDetermination::SVMChargeDetermine *mobj_svm_charge_determination;
			Engine::ChargeDetermination::AutoCorrelationChargeDetermine *mobj_autocorrelation_charge_determination ; 
			std::vector <Engine::HornTransform::IsotopeFitRecord> mvect_transformRecords ; 
			std::vector <Engine::DTAProcessing::MSnInformationRecord> mvect_msn_records ; 
			std::vector <Engine::DTAProcessing::ProfileRecord> mvect_profile_records ; 
		
						

			public:					
			
				int mint_msNscanIndex ; 
				int mint_NumMSnScansProcessed ; 				
				int mint_NumDTARecords ; 

				Engine::Readers::RawData *mobj_raw_data_dta;	
				Engine::Readers::FileType menm_dataset_type ; 

				std::map <int, int> mmap_msN_parentIndex;			

				char mch_log_filename[256]; 
				char mch_comb_dta_filename[256] ; 
				char mch_profile_filename[256] ; 
				char mch_mgf_filename[256] ; 
				char mch_output_file[256];	
				char mch_dataset_name[256] ; 

				std::ofstream mfile_log ; 
				std::ofstream mfile_comb_dta ; 
				std::ofstream mfile_mgf ; 
								
				//! default constructor.
				DTAProcessor(void);
				//! destructor.
				~DTAProcessor(void);

				//! Sets the Peak Processor Options for peak data
				/*!
					\param s2n Signal-To-Noise Ratio
					\param thresh Peak-Intensity Threshold
					\param fit_type Peak-Fit Type					
				*/

				void SetDTAOptions(int minIonCount, int minScan, int maxScan, double minMass, double maxMass, bool createLogFileOnly, bool createCompositeDTA, int considerCharge, bool considerMultiplePrecursors, 
					int isolationWindowSize, bool isProfileDataForMzXML) ; 
				void SetPeakProcessorOptions(double s2n, double thresh, bool thresholded, Engine::PeakProcessing::PEAK_FIT_TYPE fit_type);
				void SetMassTransformOptions(short max_charge, double max_mw, double max_fit, double min_s2n, double cc_mass, 
					double delete_threshold_intensity, double min_theoretical_intensity_for_score, short num_peaks_for_shoulder, 
					bool use_mercury_caching, bool o16_o18_media, std::string averagine_mf, std::string tag_mf, 
					bool thrash_or_not, bool complete_fit, bool chk_again_ch1,   
					Engine::HornTransform::IsotopicFittingType fit_type, Engine::TheoreticalProfile::AtomicInformation atomic_info)	; 
	
				void GetParentScanSpectra(int parent_scan_num,  double peakBkgRatio, double peptideMinBkgRatio);
				void GetMsNSpectra(int msN_scan_number, double peakBkgRatio, double peptideMinBkgRatio);									
				bool GenerateDTA(int msN_scan_number, int parent_scan_number) ; 
				void WriteDTAFile(int msN_scan_num, int parent_scan_num);	
				void WriteLogFile() ; 		
				void WriteProfileFile() ; 
				void CreateMSnRecord(int msn_scan_num, int msn_scan_level, int parent_scan, int parent_scan_level ) ; 												
				void CreateProfileRecord(int msn_scan_num, int parent_scan_num) ; 
				void WriteToMGF(int msN_scan_num, int parent_scan_num) ; 
				void WriteLowResolutionDTAFile();
				bool ContainsProfileData(int parent_scan);
				bool GenerateDTALowRes(int msN_scan_number, int parent_scan_number, int parent_scan_index);
				void InitializeSVM(char *file_name);
				void DetermineChargeForEachScan();
				void SetPeakParametersLowResolution(double pkBkgRatio, double peptideMinBkgRatio);
				bool FindPrecursorForChargeStates() ;	
				void WriteLowResolutionMGFFile() ;
				bool DeisotopeSummedSpectra(int parent_scan_number, double parent_mz) ;
				bool IsFTData(int parent_scan) ; 


  		};
	}
}