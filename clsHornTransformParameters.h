// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0
#pragma once
#include "clsElementIsotopes.h" 
#include "clsIsotopeFit.h"

#using <System.Xml.dll>
using namespace System::Xml ; 

namespace DeconToolsV2
{
	namespace HornTransform
	{

		public __value enum enmExportFileType {TEXT = 0, SQLITE}; 

		public __gc class clsHornTransformParameters: public System::ICloneable
		{
			static System::String *DEFAULT_ISOTOPE_FILE = S"isotope.xml" ; 
			short mshort_max_charge  ;
			short mshort_num_peaks_for_shoulder ;
			double mdbl_max_mw ; 
			double mdbl_max_fit ; 
			double mdbl_cc_mass ;
			double mdbl_delete_threshold_intensity ; 
			double mdbl_min_theoretical_intensity_for_score ;
			double mdbl_min_s2n ;
			double mdbl_min_backgroun_ratio_for_peptide ; 
			enmIsotopeFitType menmFitType ;
			bool mbln_use_absolute_peptide_intensity_threshold ; 
			double mdbl_absolute_peptide_intensity_threshold ; 
			bool mbln_process_msms ; 
			bool mbln_use_mz_range ; 
			double mdbl_min_mz ; 
			double mdbl_max_mz ; 

			System::String *mstr_averagine_formula ;
			System::String *mstr_tag_formula ;
			bool mbln_o16_o18_media ; 
			bool mbln_thrash ; 
			bool mbln_complete_fit ;
			bool mbln_use_mercury_caching ; 
			DeconToolsV2::clsElementIsotopes *mobjElementIsotopes ; 
			int mint_min_scan ; 
			int mint_max_scan ; 
			//! whether or not to use a range of scans
			bool mbln_use_scan_range ; 
			bool mbln_use_sg_smooth ; 
			bool mbln_zero_fill ; 
			short mshort_num_zeros_to_fill ; 
			short mshort_sg_left ;
			short mshort_sg_right ;
			short mshort_sg_order ;
			bool mbln_check_against_charge1 ; 
			int  mint_num_scans_to_sum_over ; 
			bool mbln_sum_spectra ; 
			bool mbln_sum_spectra_across_scan_range ; 
			//gord added
			bool mbln_isActualMonoMZUsed;
			double mdbl_leftFitStringencyFactor;
			double mdbl_rightFitStringencyFactor;
			int mint_num_frames_to_sum_over;
			bool mbln_sum_spectra_across_frame_range ;

			bool mbln_useRAPIDDeconvolution;
			bool mbln_replaceRAPIDScoreWithHornFitScore;
			enmExportFileType menmExportFileType;
			short mshort_numPeaksUsedInAbundance;     // when abundance for an IsosResult is reported... this is how many peaks are summed. Typically the most abundant peak's abundance is reported. 

			int mint_numScansToAdvance;
			bool mbln_detectPeaksWithNoDeconvolution;
			bool mbln_processMS;    // parameter that will allow skipping of MSLevel data





		public:
			clsHornTransformParameters(void);
			~clsHornTransformParameters(void);
			void SaveV1HornTransformParameters(System::Xml::XmlTextWriter *xwriter) ; 
			void SaveV1MiscellaneousParameters(System::Xml::XmlTextWriter *xwriter) ; 

			void LoadV1HornTransformParameters(System::Xml::XmlReader *rdr) ; 
			void LoadV1MiscellaneousParameters(System::Xml::XmlReader *rdr) ; 

			virtual Object* Clone()
			{
				clsHornTransformParameters *new_params = new clsHornTransformParameters() ; 
				new_params->set_AveragineFormula(static_cast<System::String*>(this->get_AveragineFormula()->Clone())) ; 
				new_params->set_CCMass(this->get_CCMass()) ; 
				new_params->set_CompleteFit(this->get_CompleteFit()) ; 
				new_params->set_DeleteIntensityThreshold(this->get_DeleteIntensityThreshold()) ; 
				new_params->set_ElementIsotopeComposition(static_cast<DeconToolsV2::clsElementIsotopes*>(this->get_ElementIsotopeComposition()->Clone())) ; 
				new_params->set_MaxCharge(this->get_MaxCharge()) ; 
				new_params->set_MaxFit(this->get_MaxFit()) ; 
				new_params->set_MaxMW(this->get_MaxMW()) ; 
				new_params->set_MinIntensityForScore(this->get_MinIntensityForScore()) ; 
				new_params->set_MinS2N(this->get_MinS2N()) ; 
				new_params->set_NumPeaksForShoulder(this->get_NumPeaksForShoulder()) ; 
				new_params->set_O16O18Media(this->get_O16O18Media()) ; 
				new_params->set_PeptideMinBackgroundRatio(this->get_PeptideMinBackgroundRatio()) ; 
				new_params->set_TagFormula(static_cast<System::String *>(this->get_TagFormula()->Clone())) ; 
				new_params->set_ThrashOrNot(this->get_ThrashOrNot()) ; 
				new_params->set_IsotopeFitType(this->get_IsotopeFitType()) ; 
				new_params->set_UseMercuryCaching(this->get_UseMercuryCaching()) ; 

				new_params->set_UseMZRange(this->get_UseMZRange()) ; 
				new_params->set_MinMZ(this->get_MinMZ()) ; 
				new_params->set_MaxMZ(this->get_MaxMZ()) ; 
				new_params->set_UseMercuryCaching(this->get_UseMercuryCaching()) ; 
				new_params->set_MinScan(this->get_MinScan()) ; 
				new_params->set_MaxScan(this->get_MaxScan()) ; 
				new_params->set_UseScanRange(this->get_UseScanRange()) ; 
				new_params->set_CheckAllPatternsAgainstCharge1(this->get_CheckAllPatternsAgainstCharge1()) ; 

				new_params->set_UseSavitzkyGolaySmooth(this->get_UseSavitzkyGolaySmooth()) ; 
				new_params->set_SGNumLeft(this->get_SGNumLeft()) ; 
				new_params->set_SGNumRight(this->get_SGNumRight()) ; 
				new_params->set_SGOrder(this->get_SGOrder()) ; 

				new_params->set_ZeroFill(this->get_ZeroFill()) ; 
				new_params->set_NumZerosToFill(this->get_NumZerosToFill()) ; 

				new_params->set_AbsolutePeptideIntensity(this->get_AbsolutePeptideIntensity());
				new_params->set_UseAbsolutePeptideIntensity(this->get_UseAbsolutePeptideIntensity());
				new_params->set_NumScansToSumOver(this->get_NumScansToSumOver()) ; 
				new_params->set_SumSpectra(this->get_SumSpectra()) ; 
				new_params->set_SumSpectraAcrossScanRange(this->get_SumSpectraAcrossScanRange()) ; 
				new_params->set_NumFramesToSumOver(this->get_NumFramesToSumOver()) ; 
				new_params->set_SumSpectraAcrossFrameRange(this->get_SumSpectraAcrossFrameRange()) ; 
				new_params->set_ProcessMSMS(this->get_ProcessMSMS()) ; 
				new_params->set_IsActualMonoMZUsed(this->get_IsActualMonoMZUsed()) ;
				new_params->set_LeftFitStringencyFactor(this->get_LeftFitStringencyFactor()) ;
				new_params->set_RightFitStringencyFactor(this->get_RightFitStringencyFactor()) ;
				new_params->set_UseRAPIDDeconvolution(this->get_UseRAPIDDeconvolution()) ; 
				new_params->set_ReplaceRAPIDScoreWithHornFitScore(this->get_ReplaceRAPIDScoreWithHornFitScore()) ; 
				new_params->set_ExportFileType(this->get_ExportFileType());
				new_params->set_NumPeaksUsedInAbundance(this->get_NumPeaksUsedInAbundance());
				new_params->set_NumScansToAdvance(this->get_NumScansToAdvance());
			    new_params->set_DetectPeaksOnlyWithNoDeconvolution(this->get_DetectPeaksOnlyWithNoDeconvolution());
				new_params->set_ProcessMS(this->get_ProcessMS());

			


				return new_params ; 
			}

			__property bool get_IsActualMonoMZUsed()
			{
				return mbln_isActualMonoMZUsed;
			}

			__property void set_IsActualMonoMZUsed(bool value)
			{
				mbln_isActualMonoMZUsed = value; 
			}

			__property double get_LeftFitStringencyFactor()
			{
				return mdbl_leftFitStringencyFactor;
			}

			__property void set_LeftFitStringencyFactor(double value)
			{
				mdbl_leftFitStringencyFactor = value;
			}

			__property double get_RightFitStringencyFactor()
			{
				return mdbl_rightFitStringencyFactor;
			}

			__property void set_RightFitStringencyFactor(double value)
			{
				mdbl_rightFitStringencyFactor = value;
			}



			__property short get_SGNumLeft()
			{
				return mshort_sg_left ; 
			}

			__property void set_SGNumLeft(short value)
			{
				mshort_sg_left = value ; 
			}

			__property short get_SGNumRight()
			{
				return mshort_sg_right ; 
			}

			__property void set_SGNumRight(short value)
			{
				mshort_sg_right = value ; 
			}

			__property short get_SGOrder()
			{
				return mshort_sg_order ; 
			}

			__property void set_SGOrder(short value)
			{
				mshort_sg_order = value ; 
			}

			__property bool get_UseSavitzkyGolaySmooth()
			{
				return mbln_use_sg_smooth ; 
			}

			__property void set_UseSavitzkyGolaySmooth(bool value)
			{
				mbln_use_sg_smooth = value ; 
			}


			__property int get_MinScan()
			{
				return mint_min_scan ; 
			}

			__property void set_MinScan(int value)
			{
				mint_min_scan = value ; 
			}

			__property int get_MaxScan()
			{
				return mint_max_scan ; 
			}

			__property void set_MaxScan(int value)
			{
				mint_max_scan = value ; 
			}

			__property bool get_UseMercuryCaching()
			{
				return mbln_use_mercury_caching ; 
			}

			__property void set_UseMercuryCaching(bool value)
			{
				mbln_use_mercury_caching = value ; 
			}
			__property bool get_UseScanRange()
			{
				return mbln_use_scan_range ; 
			}

			__property void set_UseScanRange(bool value)
			{
				mbln_use_scan_range = value ; 
			}
			
			__property bool get_SumSpectra()
			{	
				return mbln_sum_spectra ; 
			}	

			__property void set_SumSpectra(bool value)
			{
				mbln_sum_spectra = value ; 
			}

			
			__property void set_SumSpectraAcrossScanRange(bool value)
			{
				mbln_sum_spectra_across_scan_range = value ; 
			}

			__property bool get_SumSpectraAcrossScanRange()
			{
				return mbln_sum_spectra_across_scan_range ; 
			}
		
			__property int get_NumScansToSumOver()
			{
				return mint_num_scans_to_sum_over ; 
			}
			__property void set_NumScansToSumOver(int value)
			{
				mint_num_scans_to_sum_over = value ; 
			}

			__property int get_NumFramesToSumOver()
			{
				return mint_num_frames_to_sum_over;
			}

			__property void set_NumFramesToSumOver(int value)
			{
				mint_num_frames_to_sum_over = value;
			}

			__property void set_SumSpectraAcrossFrameRange(bool value)
			{
				mbln_sum_spectra_across_frame_range = value;
			}

			__property bool get_SumSpectraAcrossFrameRange()
			{
				return mbln_sum_spectra_across_frame_range;
			}
			
			__property bool get_UseMZRange()
			{
				return mbln_use_mz_range ; 
			}

			__property void set_UseMZRange(bool value)
			{
				mbln_use_mz_range = value ; 
			}

			__property double get_MinMZ()
			{
				return mdbl_min_mz ; 
			}
			__property void set_MinMZ(double value)
			{
				mdbl_min_mz = value ; 
			}

			__property double get_MaxMZ()
			{
				return mdbl_max_mz ; 
			}
			__property void set_MaxMZ(double value)
			{
				mdbl_max_mz = value ; 
			}

			__property short get_MaxCharge()
			{
				return mshort_max_charge ; 
			}
			__property void set_MaxCharge(short value)
			{
				mshort_max_charge = value ; 
			}
			__property double get_MinS2N()
			{
				return mdbl_min_s2n ; 
			}
			__property void set_MinS2N(double value)
			{
				mdbl_min_s2n = value ; 
			}
			__property double get_PeptideMinBackgroundRatio()
			{
				return mdbl_min_backgroun_ratio_for_peptide ; 
			}
			__property void set_PeptideMinBackgroundRatio(double value)
			{
				mdbl_min_backgroun_ratio_for_peptide = value ; 
			}
			__property short get_NumPeaksForShoulder()
			{
				return mshort_num_peaks_for_shoulder ; 
			}
			__property void set_NumPeaksForShoulder(short value)
			{
				mshort_num_peaks_for_shoulder = value ; 
			}
			__property double get_MaxMW()
			{
				return mdbl_max_mw; 
			}
			__property void set_MaxMW(double value)
			{
				mdbl_max_mw = value ; 
			}
			__property double get_MaxFit()
			{
				return mdbl_max_fit; 
			}
			__property void set_MaxFit(double value)
			{
				mdbl_max_fit = value ; 
			}
			__property double get_CCMass()
			{
				return mdbl_cc_mass ; 
			}
			__property void set_CCMass(double value)
			{
				mdbl_cc_mass = value ; 
			}
			__property double get_DeleteIntensityThreshold()
			{
				return mdbl_delete_threshold_intensity ; 
			}
			__property void set_DeleteIntensityThreshold(double value)
			{
				mdbl_delete_threshold_intensity = value ; 
			}
			__property void set_ZeroFill(bool value)
			{
				mbln_zero_fill = value ; 
			}

			__property bool get_ZeroFill()
			{
				return mbln_zero_fill ; 
			}

			__property short get_NumZerosToFill()
			{
				return mshort_num_zeros_to_fill ; 
			}
			
			__property void set_NumZerosToFill(short value) 
			{
				mshort_num_zeros_to_fill = value ; 
			}

			__property double get_MinIntensityForScore()
			{
				return mdbl_min_theoretical_intensity_for_score ; 
			}
			__property void set_MinIntensityForScore(double value)
			{
				mdbl_min_theoretical_intensity_for_score = value ; 
			}
			__property bool get_O16O18Media()
			{
				return mbln_o16_o18_media ; 
			}
			__property void set_O16O18Media(bool value)
			{
				mbln_o16_o18_media = value ; 
			}
			__property System::String* get_AveragineFormula()
			{
				return mstr_averagine_formula ; 
			}
			__property void set_AveragineFormula(System::String* value)
			{
				mstr_averagine_formula = value ; 
			}
			__property System::String* get_TagFormula()
			{
				return mstr_tag_formula ; 
			}
			__property void set_TagFormula(System::String* value)
			{
				mstr_tag_formula = value ; 
			}
			__property bool get_ThrashOrNot()
			{
				return mbln_thrash ; 
			}
			__property void set_ThrashOrNot(bool value)
			{
				mbln_thrash = value ; 
			}
			__property bool get_CompleteFit()
			{
				return mbln_complete_fit ; 
			}
			__property void set_CompleteFit(bool value)
			{
				mbln_complete_fit = value ; 
			}
			__property bool get_ProcessMSMS()
			{
				return mbln_process_msms ; 
			}
			__property void set_ProcessMSMS(bool value)
			{
				mbln_process_msms = value ; 
			}
			__property bool get_CheckAllPatternsAgainstCharge1() 
			{
				return mbln_check_against_charge1 ; 
			}
			__property void set_CheckAllPatternsAgainstCharge1(bool value) 
			{
				mbln_check_against_charge1 = value ; 
			}

			__property clsElementIsotopes* get_ElementIsotopeComposition()
			{
				if (mobjElementIsotopes == NULL)
					mobjElementIsotopes = new DeconToolsV2::clsElementIsotopes() ; 
				return mobjElementIsotopes ; 
			}

			__property void set_ElementIsotopeComposition(clsElementIsotopes *atomic_info)
			{
				if (atomic_info != mobjElementIsotopes)
					mobjElementIsotopes->Assign(atomic_info) ; 
			}

			__property void set_IsotopeFitType(enmIsotopeFitType type)
			{
				menmFitType = type ; 
			}
			__property enmIsotopeFitType get_IsotopeFitType()
			{
				return menmFitType ; 
			}
			__property bool get_UseAbsolutePeptideIntensity()
			{
				return mbln_use_absolute_peptide_intensity_threshold ; 
			}
			__property void set_UseAbsolutePeptideIntensity(bool value)
			{
				mbln_use_absolute_peptide_intensity_threshold = value ; 
			}
			__property double get_AbsolutePeptideIntensity()
			{
				return mdbl_absolute_peptide_intensity_threshold ; 
			}

			__property void set_AbsolutePeptideIntensity(double value)
			{
				mdbl_absolute_peptide_intensity_threshold = value ; 
			}

			__property bool get_UseRAPIDDeconvolution()
			{
				return mbln_useRAPIDDeconvolution ; 
			}
			__property void set_UseRAPIDDeconvolution(bool value)
			{
				mbln_useRAPIDDeconvolution = value ; 
			}
			__property bool get_ReplaceRAPIDScoreWithHornFitScore()
			{
				return mbln_replaceRAPIDScoreWithHornFitScore ; 
			}
			__property void set_ReplaceRAPIDScoreWithHornFitScore(bool value)
			{
				mbln_replaceRAPIDScoreWithHornFitScore = value ; 
			}
			__property enmExportFileType get_ExportFileType()
			{
				return menmExportFileType;
			}
			__property void set_ExportFileType(enmExportFileType value)
			{
				menmExportFileType = value;
			}
			__property short get_NumPeaksUsedInAbundance()
			{
				return mshort_numPeaksUsedInAbundance ;
			}
			__property void set_NumPeaksUsedInAbundance(short value)
			{
				mshort_numPeaksUsedInAbundance = value ; 
			}
			__property short get_NumScansToAdvance()
			{
				return mint_numScansToAdvance ;
			}
			__property void set_NumScansToAdvance(short value)
			{
				mint_numScansToAdvance = value ; 
			}
			__property bool get_DetectPeaksOnlyWithNoDeconvolution()
			{
				return mbln_detectPeaksWithNoDeconvolution;
			}
			__property void set_DetectPeaksOnlyWithNoDeconvolution(bool value)
			{
				mbln_detectPeaksWithNoDeconvolution = value;
			}
			__property bool get_ProcessMS()
			{
				return mbln_processMS;
			}
			__property void set_ProcessMS(bool value)
			{
				mbln_processMS = value;
			}






		}
	; }
}
