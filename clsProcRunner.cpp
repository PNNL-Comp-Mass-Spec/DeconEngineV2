// Written by Navdeep Jaitly and Anoop Mayampurath
// for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include<fstream>
#include ".\clsprocrunner.h"
#include "Readers\ReaderFactory.h"
#include "DeconEngineUtils.h"
#include "PeakProcessor/PeakProcessor.h"
#include "clsTransformResults.h"
#include "HornTransform/MassTransform.h"
#include "DTAGeneration/DTAGeneration.h"
#include "Utilities/SavGolSmoother.h"
#include "Readers/UIMFRawData.h"
#include "Readers/IMSRawData.h"
#include <time.h>

namespace DeconToolsV2
{
	clsProcRunner::clsProcRunner(void)
	{
		mint_percent_done = 0;
		menm_state = IDLE ;
		mstr_file_name = NULL ;
		mstr_output_path_for_dta_creation = NULL ; 
		mobj_peak_parameters = new DeconToolsV2::Peaks::clsPeakProcessorParameters();
		mobj_transform_parameters = new DeconToolsV2::HornTransform::clsHornTransformParameters();
		mobj_dta_generation_parameters = new DeconToolsV2::DTAGeneration::clsDTAGenerationParameters() ; 
		mobj_results = NULL ;
	}

	clsProcRunner::~clsProcRunner(void)
	{
		if (mobj_results != NULL)
		{
			delete mobj_results ; 
			mobj_results = NULL ; 
		}			
	}

	DeconToolsV2::Results::clsTransformResults* clsProcRunner::CreateTransformResults(System::String *file_name,
		DeconToolsV2::Readers::FileType file_type, DeconToolsV2::Peaks::clsPeakProcessorParameters *peak_parameters,
		DeconToolsV2::HornTransform::clsHornTransformParameters *transform_parameters, 
		DeconToolsV2::Readers::clsRawDataPreprocessOptions *fticr_preprocess_parameters, 
		bool save_peaks, bool transform)
	{
		if (menm_state == RUNNING)
		{
			throw new System::Exception(S"Process already running in clsProcRunner. Cannot run two processes with same object");
		}

		if (file_name == NULL || file_name == S"")
		{
			throw new System::Exception(S"Please enter a file name to process");
		}

		if (peak_parameters == NULL)
		{
			throw new System::Exception(S"Please specify peak processing parameters.");
		}
		if (transform_parameters == NULL)
		{
			throw new System::Exception(S"Please specify mass transform parameters.");
		}
		Engine::Readers::RawData __nogc *raw_data = NULL ; 
		Engine::Results::LCMSTransformResults __nogc *lcms_results = NULL ; 
		Engine::PeakProcessing::PeakProcessor __nogc *peak_processor = NULL ;
		Engine::PeakProcessing::PeakProcessor __nogc *original_peak_processor = NULL ; 
		Engine::HornTransform::MassTransform __nogc *mass_transform = NULL ;
		Engine::Utilities::SavGolSmoother __nogc *sgSmoother = NULL ;
		Engine::Utilities::Interpolation __nogc *interpolator = NULL ;
//		Engine::ResultChecker::LCMSCheckResults __nogc *lcms_checker = NULL ; 
		DeconToolsV2::Results::clsTransformResults *transform_results = __gc new DeconToolsV2::Results::clsTransformResults() ;


		try
		{

			std::vector<Engine::HornTransform::IsotopeFitRecord> vect_transform_records ; 
			// while the thresholded parameter is already set in the clsPeakProcessParameters, we would
			// like to override that here if the data type is Finnigan because that data is threshold.
			bool thresholded = false ;
			if (file_type == DeconToolsV2::Readers::FINNIGAN || file_type == DeconToolsV2::Readers::MZXMLRAWDATA)
				thresholded = true ;
			else
				thresholded = peak_parameters->get_ThresholdedData() ;


			mint_percent_done = 0 ;
			menm_state = RUNNING ;

			// Create a RawData object and read through each scan and discover peaks.

			char file_name_ch[256] ;
			DeconEngine::Utils::GetStr(file_name, file_name_ch) ;
			// enumerations of file type are the same in Readers namespace and
			// DeconWrapperManaged namespace.
			raw_data = Engine::Readers::ReaderFactory::GetRawData((Engine::Readers::FileType)file_type, file_name_ch) ;
			if (file_type == DeconToolsV2::Readers::ICR2LSRAWDATA && fticr_preprocess_parameters != NULL)
			{
				Engine::Readers::Icr2lsRawData *icr_raw_data = (Engine::Readers::Icr2lsRawData *)raw_data ; 
				icr_raw_data->SetApodizationZeroFillOptions((Engine::Utilities::ApodizationType)fticr_preprocess_parameters->get_ApodizationType(),
					fticr_preprocess_parameters->get_ApodizationMinX(), fticr_preprocess_parameters->get_ApodizationMaxX(), 
					fticr_preprocess_parameters->get_ApodizationPercent(), fticr_preprocess_parameters->get_NumZeroFills()) ; 
				if (fticr_preprocess_parameters->get_ApplyCalibration())
				{
					icr_raw_data->OverrideDefaultCalibrator((Engine::Readers::CalibrationType) fticr_preprocess_parameters->get_CalibrationType(),
							fticr_preprocess_parameters->get_A(), fticr_preprocess_parameters->get_B(), 
							fticr_preprocess_parameters->get_C()) ; 
				}
			}

			if (raw_data == NULL)
			{
				throw new System::Exception(System::String::Concat(S"Could not open raw file: ", file_name));
			}

			lcms_results = new Engine::Results::LCMSTransformResults() ;

			peak_processor = new Engine::PeakProcessing::PeakProcessor() ;
			original_peak_processor = new Engine::PeakProcessing::PeakProcessor() ;
			
			//lcms_checker = new Engine::ResultChecker::LCMSCheckResults()  ; 
			// Set parameters for discovering peaks. intensity threshold is set below.
			peak_processor->SetOptions(peak_parameters->get_SignalToNoiseThreshold(), 0, thresholded, (Engine::PeakProcessing::PEAK_FIT_TYPE)peak_parameters->get_PeakFitType()) ;
			original_peak_processor->SetOptions(peak_parameters->get_SignalToNoiseThreshold(), 0, thresholded, (Engine::PeakProcessing::PEAK_FIT_TYPE)peak_parameters->get_PeakFitType()) ;
			
			if (transform)
			{
				mass_transform = new Engine::HornTransform::MassTransform() ;
				mass_transform->SetElementalIsotopeComposition(*transform_parameters->get_ElementIsotopeComposition()->mobjAtomicInfo) ;
				if (transform_parameters != NULL)
				{
					char averagine_formula[512] ;
					char tag_formula[512] ;
					//mass_transform->SetOptions(transform_parameters->get_MaxCharge(), transform_parameters->get_MaxMW(), transform_parameters->get_MaxFit(),
					//	transform_parameters->get_MinS2N(), transform_parameters->get_CCMass(),transform_parameters->get_DeleteIntensityThreshold(),
					//	transform_parameters->get_MinIntensityForScore(), transform_parameters->get_NumPeaksForShoulder(),
					//	transform_parameters->get_CheckAllPatternsAgainstCharge1(), transform_parameters->get_UseMercuryCaching(), transform_parameters->get_O16O18Media()) ;


					mass_transform->SetOptions(transform_parameters->get_MaxCharge(), transform_parameters->get_MaxMW(), transform_parameters->get_MaxFit(),
						transform_parameters->get_MinS2N(), transform_parameters->get_CCMass(),transform_parameters->get_DeleteIntensityThreshold(),
						transform_parameters->get_MinIntensityForScore(), transform_parameters->get_NumPeaksForShoulder(),
						transform_parameters->get_CheckAllPatternsAgainstCharge1(), transform_parameters->get_UseMercuryCaching(),
						transform_parameters->get_O16O18Media(), transform_parameters->get_LeftFitStringencyFactor(), 
						transform_parameters->get_RightFitStringencyFactor(), transform_parameters->get_IsActualMonoMZUsed());

					averagine_formula[0] = '\0' ;
					tag_formula[0] = '\0' ;

					DeconEngine::Utils::GetStr(transform_parameters->get_AveragineFormula(), averagine_formula) ;
					if (transform_parameters->get_TagFormula() != NULL)
					{
						DeconEngine::Utils::GetStr(transform_parameters->get_TagFormula(), tag_formula) ;
					}
					mass_transform->SetIsotopeFitOptions(averagine_formula, tag_formula, transform_parameters->get_ThrashOrNot(), transform_parameters->get_CompleteFit()) ;
					mass_transform->SetIsotopeFitType((Engine::HornTransform::IsotopicFittingType) transform_parameters->get_IsotopeFitType()) ; 
				}

			}

			std::vector<double> vect_mzs ;
			std::vector<double> vect_intensities;
			std::vector<double> temp_vect_mzs ;
			std::vector<double> temp_vect_intensities;

			clock_t start_t = clock() ;

			int min_scan = 1 ; 
			if (transform_parameters->get_UseScanRange() && transform_parameters->get_MinScan() > 1)
				min_scan = transform_parameters->get_MinScan() ;
			if (min_scan < raw_data->GetFirstScanNum())
				min_scan = raw_data->GetFirstScanNum() ; 
			int max_scan = raw_data->GetLastScanNum() ; 
			if (transform_parameters->get_UseScanRange() && transform_parameters->get_MaxScan() < max_scan)
				max_scan = transform_parameters->get_MaxScan() ;
			

			if (transform_parameters->get_ZeroFill())
			{
				interpolator = __nogc new Engine::Utilities::Interpolation() ;
			}

			if (transform_parameters->get_UseSavitzkyGolaySmooth())
			{
				sgSmoother = __nogc new  Engine::Utilities::SavGolSmoother(transform_parameters->get_SGNumLeft(),
					transform_parameters->get_SGNumRight(),transform_parameters->get_SGOrder()) ;
			}

			int raw_data_read_time = 0 ;
			int preprocessing_time = 0 ;
			int transform_time = 0 ;
			double scan_time = 0 ;
			double drift_time = 0 ;
			short scan_ms_level = 1 ;

			int average_time = 0 ;
			int current_time = 0 ;
			int tic_time = 0 ;
			int peak_discover_time = 0 ;
			int peak_save_time = 0 ;
			
			//2009-04-03 [gord] will no longer use the SlidingWindow. It has litte speed benefit and there might be a bug
			/*if (transform_parameters->get_SumSpectraAcrossScanRange())
			{
				if (file_type == DeconToolsV2::Readers::PNNL_UIMF)
					((Engine::Readers::UIMFRawData *)raw_data)->InitializeSlidingWindow(transform_parameters->get_NumScansToSumOver());
				else if (file_type == DeconToolsV2::Readers::PNNL_IMS)
					((Engine::Readers::IMSRawData *)raw_data)->InitializeSlidingWindow(transform_parameters->get_NumScansToSumOver());
			}*/
			
			for(int scan_num = min_scan ; scan_num <= max_scan && scan_num != -1 ; scan_num = raw_data->GetNextScanNum(scan_num))
			{					
				clock_t start_time = clock() ; 
				peak_processor->Clear() ;
				original_peak_processor->Clear() ;
				mint_current_scan = scan_num ;
				if (min_scan != max_scan)
					mint_percent_done = ((scan_num-min_scan)*100)/(max_scan - min_scan)  ;
				vect_mzs.clear() ; 
				vect_intensities.clear() ; 
				temp_vect_mzs.clear() ; 
				temp_vect_intensities.clear() ; 

				clock_t current_time = clock() ; 

				//Check if it needs to be processed
				if (raw_data->IsMSScan(scan_num))
				{
					scan_ms_level = 1 ; 
				}
				else 
				{
					scan_ms_level = 2 ; 
				}
				if (scan_ms_level != 1 && !transform_parameters->get_ProcessMSMS())
					continue ; 
				
				if (scan_ms_level != 1 && transform_parameters->get_ProcessMSMS() && 
					!raw_data->IsFTScan(scan_num)) 
					continue ; 

				//Get this scan first
				raw_data->GetRawData(&vect_mzs, &vect_intensities, scan_num) ;	
				raw_data->GetRawData(&temp_vect_mzs, &temp_vect_intensities, scan_num) ;	

				

				// ------------------------------ Spectra summing ----------------------------------
				if (transform_parameters->get_SumSpectra())                    // sum all spectra
				{
					double min_mz ; 
					double max_mz ; 
					if (transform_parameters->get_UseMZRange())
					{					
						min_mz = transform_parameters->get_MinMZ() ; 
						max_mz = transform_parameters->get_MaxMZ() ; 
					}
					else
					{
						int num_mzs_this_scan = vect_mzs.size() ; 
						min_mz = vect_mzs[0] ; 
						max_mz = vect_mzs[num_mzs_this_scan - 1] ; 
					}
					vect_mzs.clear() ; 
					vect_intensities.clear() ; 
					raw_data->GetSummedSpectra(&vect_mzs, &vect_intensities, min_scan, max_scan, min_mz, max_mz ) ; 
					scan_num = max_scan ;
					mint_percent_done = 50 ; 						
				}
				else if (transform_parameters->get_SumSpectraAcrossScanRange())    // sum across range
				{
					//// AM: Save original intensity of peaks prior to peaks
					if (transform_parameters->get_ZeroFill())
					{
						interpolator->ZeroFillMissing(vect_mzs, vect_intensities, transform_parameters->get_NumZerosToFill()) ;
					}
					if (transform_parameters->get_UseSavitzkyGolaySmooth())
					{
						sgSmoother->Smooth(&vect_mzs, &vect_intensities) ;
					}
					double orig_thres = DeconEngine::Utils::GetAverage(vect_intensities, FLT_MAX) ;
					double orig_background_intensity = DeconEngine::Utils::GetAverage(vect_intensities, (float)(5*orig_thres)) ;
					original_peak_processor->SetPeakIntensityThreshold(orig_background_intensity * peak_parameters->get_PeakBackgroundRatio()) ;
					int orig_numPeaks = 0 ; 
					if (mobj_transform_parameters->get_UseMZRange())
						orig_numPeaks = original_peak_processor->DiscoverPeaks(&temp_vect_mzs, &temp_vect_intensities, mobj_transform_parameters->get_MinMZ(), mobj_transform_parameters->get_MaxMZ()) ;
					else
						orig_numPeaks = original_peak_processor->DiscoverPeaks(&temp_vect_mzs, &temp_vect_intensities) ; 

					// now sum
					vect_mzs.clear() ; 
					vect_intensities.clear() ; 
					int scan_range = transform_parameters->get_NumScansToSumOver() ; 	
					
					//2009-04-03 [gord] will no longer use the SlidingWindow. It has litte speed benefit and there might be a bug
					//if (file_type == DeconToolsV2::Readers::PNNL_UIMF)
					//	((Engine::Readers::UIMFRawData *)raw_data)->GetSummedSpectraSlidingWindow(&vect_mzs, &vect_intensities, scan_num, scan_range) ; 
					//	//((Engine::Readers::UIMFRawData *)raw_data)->GetSummedSpectra(&vect_mzs, &vect_intensities, scan_num, scan_range) ;  //Gord added this 					

					//else if (file_type == DeconToolsV2::Readers::PNNL_IMS)
					//	((Engine::Readers::IMSRawData *)raw_data)->GetSummedSpectraSlidingWindow(&vect_mzs, &vect_intensities, scan_num, scan_range) ; 
					//else
					
					raw_data->GetSummedSpectra(&vect_mzs, &vect_intensities, scan_num, scan_range) ; 					
				}
				
				raw_data_read_time += (clock() - current_time) ; 

				scan_time = raw_data->GetScanTime(scan_num) ;
				if (file_type == DeconToolsV2::Readers::PNNL_UIMF)
				{
					drift_time =  ((Engine::Readers::UIMFRawData *)raw_data)->GetDriftTime(scan_num) ; 
				}
				else
					drift_time = - 1; 
			
				if (vect_mzs.size() == 0)
				{
					if (save_peaks)
					{
						if (file_type == DeconToolsV2::Readers::PNNL_UIMF)
							lcms_results->AddInfoForUIMFScan(scan_num, 0, 0, 0, 0, 0, 0, scan_time, scan_ms_level, drift_time) ; 
						else
							lcms_results->AddInfoForScan(scan_num, 0, 0, 0, 0, 0, 0, scan_time, scan_ms_level) ;
					}
					continue ;
				}

				
				//------------------------------------- Zero fill --------------------------------------
				if (transform_parameters->get_ZeroFill())
				{
					interpolator->ZeroFillMissing(vect_mzs, vect_intensities, transform_parameters->get_NumZerosToFill()) ;
				}
				
				// ------------------------------------ Smooth -----------------------------------------
				if (transform_parameters->get_UseSavitzkyGolaySmooth())
				{
					sgSmoother->Smooth(&vect_mzs, &vect_intensities) ;
				}

				double minMZ = vect_mzs[0] ;
				double maxMZ = vect_mzs[(int)vect_mzs.size()-1] ;

				current_time = clock() ; 
				double thres = DeconEngine::Utils::GetAverage(vect_intensities, FLT_MAX) ;
				double background_intensity = DeconEngine::Utils::GetAverage(vect_intensities, (float)(5*thres)) ;
				average_time += clock() - current_time ; 
				double bpi = 0, bp_mz = 0 ;
				current_time = clock() ; 
				double tic_intensity = 0 ; 
				if (mobj_transform_parameters->get_UseMZRange())
				{
					tic_intensity = DeconEngine::Utils::GetTIC(mobj_transform_parameters->get_MinMZ(),
						mobj_transform_parameters->get_MaxMZ(), vect_mzs, vect_intensities, 
						(float)(background_intensity * peak_parameters->get_PeakBackgroundRatio()),
						bpi, bp_mz) ;
				}
				else
				{
					tic_intensity = DeconEngine::Utils::GetTIC(400.0, 2000.0, vect_mzs, vect_intensities, 
						(float)(background_intensity * peak_parameters->get_PeakBackgroundRatio()), bpi, bp_mz) ;
				}
				tic_time += clock() - current_time ; 

				peak_processor->SetPeakIntensityThreshold(background_intensity * peak_parameters->get_PeakBackgroundRatio()) ;
				int numPeaks = 0 ; 
				if (mobj_transform_parameters->get_UseMZRange())
				{
					current_time = clock() ; 
					numPeaks = peak_processor->DiscoverPeaks(&vect_mzs, &vect_intensities, mobj_transform_parameters->get_MinMZ(), mobj_transform_parameters->get_MaxMZ()) ;
					peak_discover_time += clock() - current_time ; 
				}
				else
				{
					current_time = clock() ; 
					numPeaks = peak_processor->DiscoverPeaks(&vect_mzs, &vect_intensities) ;
					peak_discover_time += clock() - current_time ; 
				}

				if (save_peaks)
				{
					current_time = clock() ; 
					lcms_results->AddPeaksForScan(scan_num, peak_processor->mobj_peak_data->mvect_peak_tops) ;
					peak_save_time += clock() - current_time ; 
				}
				preprocessing_time += (clock() - current_time) ; 

				int numDeisotoped = 0 ;
				preprocessing_time += clock() - start_time ; 

				if (file_type != DeconToolsV2::Readers::PNNL_UIMF && scan_num % 20 == 0)
				{
					clock_t current_t = clock() ;
					int all = current_t - start_t ;
					Console::WriteLine(System::String::Concat(
						S" Raw Reading Time = ", Convert::ToString(raw_data_read_time), 
						S" Average Time = ", Convert::ToString(average_time), 
						S" TIC Time = ", Convert::ToString(tic_time), 
						S" Peak Discover Time = ", Convert::ToString(peak_discover_time), 
						S" Peak Save Time = ", Convert::ToString(peak_save_time), 
						S" PreProcessing Time = ", Convert::ToString(preprocessing_time), 
						S" preprocess-read= ", Convert::ToString(preprocessing_time-raw_data_read_time)
						)) ;
				}

				
				// ------------------------ Mass Transform -----------------------------------------
				if(transform)
				{
					double min_peptide_intensity = background_intensity * transform_parameters->get_PeptideMinBackgroundRatio() ;
					if (transform_parameters->get_UseAbsolutePeptideIntensity())
					{
						if (min_peptide_intensity < transform_parameters->get_AbsolutePeptideIntensity())
							min_peptide_intensity = transform_parameters->get_AbsolutePeptideIntensity() ;
					}

					Engine::PeakProcessing::Peak currentPeak ;
					Engine::PeakProcessing::Peak originalPeak ; 
					Engine::HornTransform::IsotopeFitRecord transformRecord ;

					peak_processor->mobj_peak_data->InitializeUnprocessedPeakData() ;
					
					bool found = peak_processor->mobj_peak_data->GetNextPeak(minMZ, maxMZ, currentPeak) ;
					double fwhm_SN = currentPeak.mdbl_FWHM ;

					mass_transform->Reset() ; 
					vect_transform_records.clear() ; 
					while(found)
					{
						if (currentPeak.mdbl_intensity < min_peptide_intensity)
							break ;

						bool found_transform = false ;
						current_time = clock() ;
						try
						{
							found_transform = mass_transform->FindTransform(*peak_processor->mobj_peak_data, 
								currentPeak, transformRecord, background_intensity) ;
							transform_time += (clock() - current_time) ; 

							// AM: if summing over a window, reinsert the original intensity     // [gord]  why?
							if(found_transform && transformRecord.mshort_cs <= transform_parameters->get_MaxCharge()
								 && transform_parameters->get_SumSpectraAcrossScanRange())
							{
								original_peak_processor->mobj_peak_data->InitializeUnprocessedPeakData() ; 
								originalPeak.mdbl_intensity = -1.0 ; 
								originalPeak.mdbl_mz = -1.0 ; 								
								original_peak_processor->mobj_peak_data->FindPeak(transformRecord.mdbl_mz - 2*transformRecord.mdbl_fwhm, transformRecord.mdbl_mz + 2*transformRecord.mdbl_fwhm, originalPeak) ; 
 								if (originalPeak.mdbl_intensity > 0) 
								{	
									transformRecord.mint_abundance =  (int)originalPeak.mdbl_intensity ; 
									transformRecord.mdbl_fwhm = originalPeak.mdbl_FWHM   ;       // [gord] this might be the source of why FWHM is sometimes 0
								}
								else
								{
									found_transform = false ; //AM : do not add to results									
								}
							}


							if (found_transform && transformRecord.mshort_cs <= transform_parameters->get_MaxCharge())
							{
								numDeisotoped++ ;
								transformRecord.mint_scan_num = scan_num ;


								if (transform_parameters->get_IsActualMonoMZUsed())
								{
									//retrieve experimental monoisotopic peak
									int monoPeakIndex = transformRecord.marr_isotope_peak_indices[0];
									Engine::PeakProcessing::Peak monoPeak ;
									peak_processor->mobj_peak_data->GetPeak(monoPeakIndex, monoPeak);

									//set threshold at 20% less than the expected 'distance' to the next peak
									double errorThreshold = 1.003 / transformRecord.mshort_cs;
									errorThreshold = errorThreshold - errorThreshold * 0.2;

									double calc_monoMZ = transformRecord.mdbl_mono_mw / transformRecord.mshort_cs + 1.00727638;


									if (abs(calc_monoMZ - monoPeak.mdbl_mz) < errorThreshold)
									{
										transformRecord.mdbl_mono_mw = monoPeak.mdbl_mz * transformRecord.mshort_cs - 1.00727638 * transformRecord.mshort_cs;
									}
									else
									{

									}




								}



								vect_transform_records.push_back(transformRecord) ;
							}
							found = peak_processor->mobj_peak_data->GetNextPeak(minMZ, maxMZ, currentPeak) ;
						}
						catch (char *mesg1)
						{
							Console::WriteLine(mesg1) ; 
							Console::WriteLine(Convert::ToString(currentPeak.mint_peak_index)) ; 
						}
					}
					lcms_results->AddTransforms(vect_transform_records) ; 
				//	lcms_checker->AddTransformsToCheck(vect_transform_records)  ; 
					if (file_type != DeconToolsV2::Readers::PNNL_UIMF )
					//if (file_type != DeconToolsV2::Readers::PNNL_UIMF && scan_num % 20 == 0)
					{
						int iso_time=0, spline_time=0, ac_time=0, fit_time = 0, cs_time =0, get_fit_score_time = 0, 
							remainder_time = 0, find_peak_calc = 0, find_peak_cached = 0   ;
						clock_t current_t = clock() ;
						int all = current_t - start_t ;
						mass_transform->GetProcessingTimes(cs_time, ac_time, spline_time, iso_time, fit_time,
							remainder_time, get_fit_score_time, find_peak_calc, find_peak_cached) ;
						Console::WriteLine(System::String::Concat(S"Scan # =", Convert::ToString(scan_num), 
							S" CS= ", Convert::ToString(cs_time),
							S" Isotope= ", Convert::ToString(iso_time),
							S" FitScore= ", Convert::ToString(fit_time), 
							S" GetFitScore= ", Convert::ToString(get_fit_score_time), 
							S" GetFitScore-Isotope-FitScore-FindPeak= ", Convert::ToString(get_fit_score_time-fit_time-find_peak_cached-find_peak_calc-iso_time), 
//							S" Raw Reading Time = ", Convert::ToString(raw_data_read_time), 
//							S" PreProcessing Time = ", Convert::ToString(preprocessing_time), 
							S" Transform= ", Convert::ToString(transform_time), 
							S" Remaining= ", Convert::ToString(remainder_time), 
							S" transform-cs-get_fit= ", Convert::ToString(transform_time-cs_time-get_fit_score_time), 
							S" All= ", Convert::ToString(all)
//							S" all-transform-preprocess-read= ", Convert::ToString(all-transform_time-preprocessing_time-raw_data_read_time)
							)) ;
					}
				}
				if (save_peaks)
				{
					double signal_range = raw_data->GetSignalRange(scan_num) ; 
					if (file_type == DeconToolsV2::Readers::PNNL_UIMF)
						lcms_results->AddInfoForUIMFScan(scan_num, bp_mz, bpi, tic_intensity, signal_range, numPeaks, numDeisotoped, scan_time, scan_ms_level, drift_time) ;							
					else
						lcms_results->AddInfoForScan(scan_num, bp_mz, bpi, tic_intensity, signal_range, numPeaks, numDeisotoped, scan_time, scan_ms_level) ; 
				}

			}			

		
			transform_results->SetLCMSTransformResults(lcms_results) ; 

			// Anoop: write out those one s which are probab;y wrong
			/*char outfile[256] ; 
			int dotIndex = file_name->IndexOf(".") ; 
			System::String __gc *output_file_str = file_name->Remove(dotIndex, mstr_file_name->Length - dotIndex) ; 
			DeconEngine::Utils::GetStr(output_file_str, outfile) ; 			
			lcms_checker->WriteOutTransformsToCheck(outfile) ; */


			mint_percent_done = 100 ;

		}
		catch (char *mesg)
		{
			if (lcms_results != NULL)
			{
				delete lcms_results ; 
				lcms_results = NULL ;
			}
			if (sgSmoother != NULL)
			{
				delete sgSmoother ;
				sgSmoother = NULL ; 
			}
			if (interpolator != NULL)
			{
				delete interpolator ; 
				interpolator = NULL ; 
			}
			if (peak_processor != NULL) 
			{
				delete peak_processor ;
				peak_processor = NULL ; 
			}
			if( raw_data != NULL)
			{
				raw_data->Close() ; 
				delete raw_data ; 
			}

			if (mass_transform != NULL)
			{
				delete mass_transform ;
				mass_transform = NULL ; 
			}
			System::String *exception_msg = new System::String(mesg) ; 
			menm_state = enmProcessState::ERROR ;
			throw new System::Exception(exception_msg) ; 
		}

		if (sgSmoother != NULL)
		{
			delete sgSmoother ;
			sgSmoother = NULL ; 
		}
		if (interpolator != NULL)
		{
			delete interpolator ; 
			interpolator = NULL ; 
		}
		if (peak_processor != NULL) 
		{
			delete peak_processor ;
			peak_processor = NULL ; 
		}
		if( raw_data != NULL)
		{
			raw_data->Close() ; 
			delete raw_data ; 
		}

		if (mass_transform != NULL)
		{
			delete mass_transform ;
			mass_transform = NULL ; 
		}
		menm_state = COMPLETE ;
		return transform_results ;
	}


	void clsProcRunner::CreateDTAFile()	 
	{
		//Main function to create DTA files 

		//check if we have everything
		if (mobj_peak_parameters == NULL)
		{
			throw new System::Exception(S"Peak parameters not set.") ;
		}
		if (mobj_transform_parameters == NULL)
		{
			throw new System::Exception(S"Horn Transform parameters not set.") ;
		}
		if (mobj_dta_generation_parameters == NULL)
		{
			throw new System::Exception(S"DTA Generation parameters not set.") ; 
		}

		if (menm_state == RUNNING)
		{
			throw new System::Exception(S"Process already running in clsProcRunner. Cannot run two processes with same object");
		}
	
		Engine::DTAProcessing::DTAProcessor __nogc *dta_processor = NULL;


		try
		{
			mint_percent_done = 0 ;
			menm_state = RUNNING ;
			
			dta_processor = new Engine::DTAProcessing::DTAProcessor();

			//Read the rawfile in		
			char file_name_ch[256] ;
			DeconEngine::Utils::GetStr(mstr_file_name, file_name_ch) ;		
			std::ifstream fin(file_name_ch, std::ios::in | std::ios::binary);
			if (!fin)
			{
				std::cout << "Unable to open input file" ;
				return;
			}

			// Check input format
			int dotIndex = mstr_file_name->IndexOf(".");
			char input_file_format[256] ; 
			System::String __gc *inpFileFormat = mstr_file_name->Remove(0, dotIndex+1) ;
			DeconEngine::Utils::GetStr(inpFileFormat, input_file_format) ;
			if ((strcmp(input_file_format, "RAW")==0) || (strcmp(input_file_format, "raw")==0))
				menm_file_type = DeconToolsV2::Readers::FileType::FINNIGAN;
			else if (strcmp(input_file_format, "mzXML")==0)
				menm_file_type = DeconToolsV2::Readers::FileType::MZXMLRAWDATA ; 				
			else
			{
				throw new System::Exception(S"Invalid input file format.") ; 
			}			

			// Set output path and filename
			char  *output_file_ch  = NULL ; 
			if (mstr_output_path_for_dta_creation != NULL)
			{				
				char output_path_ch[256] ; 
				char raw_file_ch[256] ; 
				int slashIndex = mstr_file_name->LastIndexOf("\\") ; 					
				System::String __gc *raw_name_plus_extension = mstr_file_name->Remove(dotIndex, mstr_file_name->Length - dotIndex);
				System::String __gc *raw_name = raw_name_plus_extension->Remove(0, slashIndex) ; 
				DeconEngine::Utils::GetStr(raw_name, raw_file_ch) ;
				DeconEngine::Utils::GetStr(mstr_output_path_for_dta_creation, output_path_ch) ; 
				output_file_ch = strcat(output_path_ch, raw_file_ch) ; 
			}
			else
			{
				char outfile[256] ; 
				System::String __gc *output_file_str = mstr_file_name->Remove(dotIndex, mstr_file_name->Length - dotIndex) ; 
				DeconEngine::Utils::GetStr(output_file_str, outfile) ; 
				output_file_ch = outfile ; 
			}

			bool thresholded ; 
			if (menm_file_type == DeconToolsV2::Readers::FileType::FINNIGAN || menm_file_type == DeconToolsV2::Readers::FileType::MZXMLRAWDATA)
				thresholded = true ;
			else
				thresholded = mobj_peak_parameters->get_ThresholdedData() ;
			
			//Raw Object
			dta_processor->mobj_raw_data_dta = Engine::Readers::ReaderFactory::GetRawData((Engine::Readers::FileType)menm_file_type, file_name_ch) ;
			dta_processor->menm_dataset_type = (Engine::Readers::FileType)menm_file_type ; 
			
			//File name base for all dtas
			strcpy(dta_processor->mch_output_file, output_file_ch) ; 

			//Datasetname
			int lastSlashIndex = mstr_file_name->LastIndexOf("\\") ; 					
			System::String __gc *data_name_plus_extension = mstr_file_name->Remove(dotIndex, mstr_file_name->Length - dotIndex);
			System::String __gc *data_name = data_name_plus_extension->Remove(0, lastSlashIndex+1) ; 
			DeconEngine::Utils::GetStr(data_name, dta_processor->mch_dataset_name) ;

			// File name for log file
			bool create_log_file_only = false ; 
			strcpy(dta_processor->mch_log_filename, output_file_ch);
			strcat(dta_processor->mch_log_filename, "_DeconMSn_log.txt");

			//File name for profile data
			strcpy(dta_processor->mch_profile_filename, output_file_ch) ; 
			strcat(dta_processor->mch_profile_filename, "_profile.txt") ; 
			
			if (mobj_dta_generation_parameters->get_OutputType() == DeconToolsV2::DTAGeneration::OUTPUT_TYPE::LOG)		
			{
				create_log_file_only = true ; 
			}
			//file name for composite dta file
			bool create_composite_dta = false ; 
			if (mobj_dta_generation_parameters->get_OutputType() == DeconToolsV2::DTAGeneration::OUTPUT_TYPE::CDTA)
			{
					strcpy(dta_processor->mch_comb_dta_filename, output_file_ch);
					strcat(dta_processor->mch_comb_dta_filename, "_dta.txt");
					dta_processor->mfile_comb_dta.open(dta_processor->mch_comb_dta_filename, std::ios::out) ; 				
					create_composite_dta = true ; 
			}
			//file name for .mgf file
			if (mobj_dta_generation_parameters->get_OutputType() == DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)		
			{
				strcpy(dta_processor->mch_mgf_filename, output_file_ch);
				strcat(dta_processor->mch_mgf_filename, ".mgf") ; 
				dta_processor->mfile_mgf.open(dta_processor->mch_mgf_filename, std::ios::out) ; 			
			}

			//Settings
			char averagine_formula[512] ;
			char tag_formula[512] ;
			averagine_formula[0] = '\0' ;
			tag_formula[0] = '\0' ;			
			DeconEngine::Utils::GetStr(mobj_transform_parameters->get_AveragineFormula(), averagine_formula) ;
			if (mobj_transform_parameters->get_TagFormula() != NULL)
				DeconEngine::Utils::GetStr(mobj_transform_parameters->get_TagFormula(), tag_formula) ;

			//Check if any dtas have to be ignored
			std::vector<int> vect_msn_ignore ; 			
			if (mobj_dta_generation_parameters->get_IgnoreMSnScans())
			{
				int numLevels = mobj_dta_generation_parameters->get_NumMSnLevelsToIgnore() ; 
				for (int levelNum = 0 ; levelNum < numLevels ; levelNum++)
				{	
					int level = mobj_dta_generation_parameters->get_MSnLevelToIgnore(levelNum) ; 
					vect_msn_ignore.push_back(level) ; 
				}
				sort(vect_msn_ignore.begin(), vect_msn_ignore.end()) ; 
			}

			

			
			dta_processor->SetDTAOptions(mobj_dta_generation_parameters->get_MinIonCount(), 
				mobj_dta_generation_parameters->get_MinScan(), mobj_dta_generation_parameters->get_MaxScan(), 
				mobj_dta_generation_parameters->get_MinMass(), mobj_dta_generation_parameters->get_MaxMass(), 
				create_log_file_only, create_composite_dta , mobj_dta_generation_parameters->get_ConsiderChargeValue(), mobj_dta_generation_parameters->get_ConsiderMultiplePrecursors(), 
				mobj_dta_generation_parameters->get_IsolationWindowSize(), mobj_dta_generation_parameters->get_IsProfileDataForMzXML()) ; 
			dta_processor->SetPeakProcessorOptions(mobj_peak_parameters->get_SignalToNoiseThreshold(), 0, thresholded, 
				(Engine::PeakProcessing::PEAK_FIT_TYPE)mobj_peak_parameters->get_PeakFitType()) ;
			dta_processor->SetMassTransformOptions(mobj_transform_parameters->get_MaxCharge(), mobj_transform_parameters->get_MaxMW(), 
				mobj_transform_parameters->get_MaxFit(), mobj_transform_parameters->get_MinS2N(), mobj_transform_parameters->get_CCMass(), 
				mobj_transform_parameters->get_DeleteIntensityThreshold(), mobj_transform_parameters->get_MinIntensityForScore(), 
				mobj_transform_parameters->get_NumPeaksForShoulder(), mobj_transform_parameters->get_UseMercuryCaching(), mobj_transform_parameters->get_O16O18Media(), averagine_formula, tag_formula, 
				mobj_transform_parameters->get_ThrashOrNot(), mobj_transform_parameters->get_CompleteFit(), 
				mobj_transform_parameters->get_CheckAllPatternsAgainstCharge1(),(Engine::HornTransform::IsotopicFittingType)mobj_transform_parameters->get_IsotopeFitType(), 
				*mobj_transform_parameters->get_ElementIsotopeComposition()->mobjAtomicInfo);

			System::String __gc *svm_file = mobj_dta_generation_parameters->get_SVMParamFile() ; 
			char svm_file_ch [256] ; 
			svm_file_ch[0] = '\0' ; 
			DeconEngine::Utils::GetStr(svm_file, svm_file_ch) ; 
			dta_processor->InitializeSVM(svm_file_ch);
			dta_processor->SetPeakParametersLowResolution(mobj_peak_parameters->get_PeakBackgroundRatio(), mobj_transform_parameters->get_PeptideMinBackgroundRatio()) ; 

            //begin process
			//stick in range
			int scan_num = mobj_dta_generation_parameters->get_MinScan();
			int msNScanIndex = 0;
			int num_scans ;
			int parent_scan ;
			double parent_mz = 0 ; 
			bool low_resolution = false ;

			if (mobj_dta_generation_parameters->get_MaxScan() <= dta_processor->mobj_raw_data_dta->GetNumScans())
				num_scans = mobj_dta_generation_parameters->get_MaxScan() ;
			else
				num_scans = dta_processor->mobj_raw_data_dta->GetNumScans();	

			while (scan_num <= num_scans)
			{	
				mint_percent_done = (scan_num*100)/num_scans ;					
				if (dta_processor->mobj_raw_data_dta->IsMSScan(scan_num))
				{						
                    //Get MS spectra					
					dta_processor->GetParentScanSpectra(scan_num, mobj_peak_parameters->get_PeakBackgroundRatio(), mobj_transform_parameters->get_PeptideMinBackgroundRatio() );																												
					for(int msN_scan = scan_num +1; msN_scan < num_scans && !dta_processor->mobj_raw_data_dta->IsMSScan(msN_scan)  ; msN_scan++)
					{
						//GetMS level and see if it is to be ignored
						if(mobj_dta_generation_parameters->get_IgnoreMSnScans())
						{							 
							int msN_level =  dta_processor->mobj_raw_data_dta->GetMSLevel(msN_scan) ; 
							bool found_msN_level = false ; 
							for (int index = 0 ; index < vect_msn_ignore.size(); index++)
							{
								if (msN_level == vect_msn_ignore[index])
								{
									found_msN_level = true ; 
									break ; 
								}
							}

							if (found_msN_level)
								continue ; 
						}						
						//Get msN spectra								
						dta_processor->GetMsNSpectra(msN_scan, mobj_peak_parameters->get_PeakBackgroundRatio(), mobj_transform_parameters->get_PeptideMinBackgroundRatio());										
						//Identify which is parent_scan
						parent_scan = dta_processor->mobj_raw_data_dta->GetParentScan(msN_scan);	
						// AM Modified to recieve new spectra everytime if (parent_scan != scan_num) //MSN data 
						dta_processor->GetParentScanSpectra(parent_scan, mobj_peak_parameters->get_PeakBackgroundRatio(), mobj_transform_parameters->get_PeptideMinBackgroundRatio() );																										
						
						if(dta_processor->IsFTData(parent_scan))
						{
							//Get charge and mono		
							bool dta_success = dta_processor->GenerateDTA(msN_scan, parent_scan) ; 													
							if (dta_success)
							{
								//write out dta
								if (mobj_dta_generation_parameters->get_OutputType()== DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)
									dta_processor->WriteToMGF(msN_scan, parent_scan) ;
						 		else
									dta_processor->WriteDTAFile(msN_scan, parent_scan);		
								
							}							
						}
						else
						{
							//Low res data	
							low_resolution = true ; 
							bool dta_success = dta_processor->GenerateDTALowRes(msN_scan, parent_scan, msNScanIndex) ; 
							if (dta_success)
							{							
								if (mobj_dta_generation_parameters->get_OutputType()== DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)
									dta_processor->WriteToMGF(msN_scan, parent_scan) ;
						 		else
									dta_processor->WriteDTAFile(msN_scan, parent_scan);						
							}						
						}
					}
					// reinitialize scan_count appropriately
					scan_num = msN_scan - 1;
				}
				else
				{				

					//Get msN spectra
					int msN_scan = scan_num ;
					 
					//GetMS level and see if it is to be ignored
					if(mobj_dta_generation_parameters->get_IgnoreMSnScans())
					{							 
						int msN_level =  dta_processor->mobj_raw_data_dta->GetMSLevel(msN_scan) ; 
						bool found_msN_level = false ; 
						for (int index = 0 ; index < vect_msn_ignore.size(); index++)
						{
							if (msN_level == vect_msn_ignore[index])
							{
								found_msN_level = true ; 
								break ; 
							}
						}

						if (found_msN_level)
						{
							scan_num++; // make sure an increment happens 
							continue ; 
						}
					}

					dta_processor->GetMsNSpectra(msN_scan, mobj_peak_parameters->get_PeakBackgroundRatio(), mobj_transform_parameters->get_PeptideMinBackgroundRatio());				

					//Identify which is parent_scan
					parent_scan = dta_processor->mobj_raw_data_dta->GetParentScan(msN_scan);	
					// check to see if valid parent which wont be in MRM cases where MSn begins at 1. 
					if (parent_scan < 1)
					{
						scan_num++ ; 
						continue ; //no dta is generated
					}

					// get parent data
					dta_processor->GetParentScanSpectra(parent_scan, mobj_peak_parameters->get_PeakBackgroundRatio(), mobj_transform_parameters->get_PeptideMinBackgroundRatio() );																										
					
					if(dta_processor->IsFTData(parent_scan))					
					{
						//Get charge and mono		
						bool dta_success = dta_processor->GenerateDTA(msN_scan, parent_scan) ; 													
						if (dta_success)
						{
							//write out dta
							if (mobj_dta_generation_parameters->get_OutputType()== DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)
								dta_processor->WriteToMGF(msN_scan, parent_scan) ;
						 	else
								dta_processor->WriteDTAFile(msN_scan, parent_scan);						
						}							
					}
					else
					{
						//Low res data	
						low_resolution = true ; 
						bool dta_success = dta_processor->GenerateDTALowRes(msN_scan, parent_scan, msNScanIndex) ; 
						if (dta_success)
						{							
							if (mobj_dta_generation_parameters->get_OutputType()== DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)
								dta_processor->WriteToMGF(msN_scan, parent_scan) ;
						 	else
								dta_processor->WriteDTAFile(msN_scan, parent_scan);						
						}						
					}
				}
				scan_num++;
			}

			if (low_resolution && !mobj_dta_generation_parameters->get_ConsiderChargeValue())
			{
				std::cout<<"Determining charge"<<std::endl;
				dta_processor->DetermineChargeForEachScan();
				std::cout<<"Generating DTAs for low-resolution data"<<std::endl;				
				if (mobj_dta_generation_parameters->get_OutputType()== DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)
					dta_processor->WriteLowResolutionMGFFile() ; 
				else
					dta_processor->WriteLowResolutionDTAFile();
			}
			
			// Write out log file
			dta_processor->WriteLogFile() ; 

			// Write out profile
			dta_processor->WriteProfileFile() ; 
			//Shutdown
			//dta_processor->mfile_log.close() ;
			if (mobj_dta_generation_parameters->get_OutputType() == DeconToolsV2::DTAGeneration::OUTPUT_TYPE::CDTA)
				dta_processor->mfile_comb_dta.close() ; 
			if (mobj_dta_generation_parameters->get_OutputType() == DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)		
				dta_processor->mfile_mgf.close() ; 
		

			//Done
 			menm_state = COMPLETE ;
		}
		catch (char *mesg)
		{
			if( dta_processor != NULL)
			{
				delete dta_processor ; 
			}
			System::String *exception_msg = new System::String(mesg) ; 
			throw new System::Exception(exception_msg) ; 
		}

		if(dta_processor != NULL)
		{
			delete dta_processor ; 
		}
	}
	
	void clsProcRunner::CreateTransformResultWithPeaksOnly()
	{
		if (mstr_file_name == NULL)
		{
			throw new System::Exception(S"File name is not set.") ;
		}
		if (mobj_peak_parameters == NULL)
		{
			throw new System::Exception(S"Peak parameters not set.") ;
		}
		mobj_results = CreateTransformResults(mstr_file_name, menm_file_type, mobj_peak_parameters, 
			mobj_transform_parameters, mobj_fticr_preprocess_parameters, true, false) ;
	}

	void clsProcRunner::CreateTransformResultWithNoPeaks()
	{
		if (mstr_file_name == NULL)
		{
			throw new System::Exception(S"File name is not set.") ;
		}
		if (mobj_peak_parameters == NULL)
		{
			throw new System::Exception(S"Peak parameters not set.") ;
		}
		if (mobj_transform_parameters == NULL)
		{
			throw new System::Exception(S"Horn Transform parameters not set.") ;
		}
		mobj_results = CreateTransformResults(mstr_file_name, menm_file_type, mobj_peak_parameters,
			mobj_transform_parameters, mobj_fticr_preprocess_parameters, false, true) ;
	}

	void clsProcRunner::CreateTransformResults()
	{
		if (mstr_file_name == NULL)
		{
			throw new System::Exception(S"File name is not set.") ;
		}
		if (mobj_peak_parameters == NULL)
		{
			throw new System::Exception(S"Peak parameters not set.") ;
		}
		if (mobj_transform_parameters == NULL)
		{
			throw new System::Exception(S"Horn Transform parameters not set.") ;
		}
		mobj_results = CreateTransformResults(mstr_file_name, menm_file_type, mobj_peak_parameters, 
			mobj_transform_parameters, mobj_fticr_preprocess_parameters, true, true) ;	
	}	

	void clsProcRunner::MemoryTest()
	{
		int how_many = 6000000 ; 
		int sizeofPeak = sizeof(Engine::PeakProcessing::Peak) ; 
		int sizeofLcmsPeak = sizeof(Engine::Results::LCMSPeak<Engine::PeakProcessing::Peak>) ;

		std::vector<Engine::Results::LCMSPeak<Engine::PeakProcessing::Peak> > mvect_peaks ; 
		mvect_peaks.reserve(how_many) ; 
		Engine::Results::LCMSPeak<Engine::PeakProcessing::Peak> lc_peak ; 
		Engine::PeakProcessing::Peak pk ; 
		for (int i = 0 ; i < how_many ; i++)
		{
			lc_peak.mobj_peak = pk ; 
			lc_peak.mint_scan_num = 0 ; 
			mvect_peaks.push_back(lc_peak) ; 
		}
		char *temp = new char [how_many *2*sizeofLcmsPeak] ; 
		char *temp1 = new char [how_many *2*sizeofLcmsPeak] ; 
		char *temp2 = new char [how_many *2*sizeofLcmsPeak] ; 
		if (temp2 == NULL)
		{
			std::cerr<<"UnSuccessful"<<std::flush ; 
		}
		delete [] temp ; 
		delete [] temp1 ; 
		delete [] temp2 ; 

		mvect_peaks.reserve(2*how_many) ; 
		std::cerr<<"Blah"<<std::endl ; 
	}
}