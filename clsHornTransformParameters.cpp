// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0
#include ".\clshorntransformparameters.h"
#using <mscorlib.dll>

#using <System.Xml.dll>
namespace DeconToolsV2
{
	namespace HornTransform
	{
		clsHornTransformParameters::clsHornTransformParameters(void)
		{
			mshort_max_charge = 10 ;
			mshort_num_peaks_for_shoulder = 1 ;
			mdbl_max_mw = 10000 ; 
			mdbl_max_fit = 0.25 ; 
			mdbl_cc_mass = 1.00727638;
			mdbl_delete_threshold_intensity = 10 ; 
			mdbl_min_theoretical_intensity_for_score = 10 ;
			mbln_o16_o18_media = false ; 
			mdbl_min_s2n = 3 ; 
			mdbl_min_backgroun_ratio_for_peptide = 5 ; 
			mstr_averagine_formula = S"C4.9384 H7.7583 N1.3577 O1.4773 S0.0417";
			mstr_tag_formula = S"" ;
			mbln_o16_o18_media = false ; 
			mbln_thrash = true ; 
			mbln_complete_fit = false ; 
			mbln_use_mercury_caching = true ; 
			mobjElementIsotopes = new DeconToolsV2::clsElementIsotopes() ; 
			mbln_use_mz_range = true ; 
			mdbl_min_mz = 400 ; 
			mdbl_max_mz = 2000 ; 
			mint_min_scan = System::Int32::MinValue ; 
			mint_max_scan = System::Int32::MaxValue ; 
			mbln_use_scan_range = false ; 
			mbln_use_sg_smooth = false ; 
			mshort_sg_left = 2  ;
			mshort_sg_right = 2  ;
			mshort_sg_order = 2  ;
			menmFitType = DeconToolsV2::enmIsotopeFitType::AREA ;
			mbln_use_absolute_peptide_intensity_threshold = false ; 
			mdbl_absolute_peptide_intensity_threshold = 0 ; 
			mshort_num_zeros_to_fill = 3; 
			mbln_zero_fill = false ;
			mbln_check_against_charge1 = false ;
			mint_num_scans_to_sum_over = 0 ; 
			mbln_sum_spectra = false ; 
			mbln_process_msms = false ; 
			mbln_sum_spectra_across_frame_range = true;
			mint_num_frames_to_sum_over = 3;
			mbln_isActualMonoMZUsed = false;
			mdbl_leftFitStringencyFactor = 1;
			mdbl_rightFitStringencyFactor = 1;
			mbln_useRAPIDDeconvolution = false;
			mbln_replaceRAPIDScoreWithHornFitScore = false;
			menmExportFileType = enmExportFileType::TEXT;
			mshort_numPeaksUsedInAbundance = 1 ;
			mbln_detectPeaksWithNoDeconvolution= false;
			mbln_processMS = true;
		}

		clsHornTransformParameters::~clsHornTransformParameters(void)
		{
		}

		void clsHornTransformParameters::SaveV1HornTransformParameters(System::Xml::XmlTextWriter *xwriter)
		{
			xwriter->WriteStartElement(S"HornTransformParameters");
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"TagFormula",this->TagFormula) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"AveragineFormula",this->AveragineFormula) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"DeleteIntensityThreshold",this->DeleteIntensityThreshold.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MaxFit",this->MaxFit.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MinIntensityForScore",this->MinIntensityForScore.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"MaxCharge",this->MaxCharge.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MaxMW",this->MaxMW.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"NumPeaksForShoulder",this->NumPeaksForShoulder.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"O16O18Media",this->O16O18Media.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"PeptideMinBackgroundRatio",this->PeptideMinBackgroundRatio.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"UseAbsolutePeptideIntensity",this->UseAbsolutePeptideIntensity.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"AbsolutePeptideIntensity",this->AbsolutePeptideIntensity.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"ThrashOrNot",this->ThrashOrNot.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"CheckAllPatternsAgainstCharge1",this->CheckAllPatternsAgainstCharge1.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"CompleteFit",this->CompleteFit.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"CCMass",this->CCMass.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"IsotopeFitType",__box(this->IsotopeFitType)->ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"UseMercuryCaching",this->UseMercuryCaching.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"SumSpectra",this->SumSpectra.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 		

			xwriter->WriteElementString(S"SumSpectraAcrossScanRange",this->SumSpectraAcrossScanRange.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 		
	
			xwriter->WriteElementString(S"NumberOfScansToSumOver",this->NumScansToSumOver.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"NumberOfScansToAdvance",this->NumScansToAdvance.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"SumSpectraAcrossFrameRange",this->SumSpectraAcrossFrameRange.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 	

			xwriter->WriteElementString(S"NumberOfFramesToSumOver",this->NumFramesToSumOver.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 
			
			xwriter->WriteElementString(S"IsActualMonoMZUsed",this->IsActualMonoMZUsed.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"LeftFitStringencyFactor",this->LeftFitStringencyFactor.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"RightFitStringencyFactor",this->RightFitStringencyFactor.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"UseRAPIDDeconvolution",this->UseRAPIDDeconvolution.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"ReplaceRAPIDScoreWithHornFitScore",this->ReplaceRAPIDScoreWithHornFitScore.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ;

			xwriter->WriteElementString(S"NumPeaksUsedInAbundance",this->NumPeaksUsedInAbundance.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 

				xwriter->WriteEndElement();
			xwriter->WriteWhitespace(S"\n\t") ; 
		}

		void clsHornTransformParameters::SaveV1MiscellaneousParameters(System::Xml::XmlTextWriter *xwriter)
		{
			xwriter->WriteStartElement(S"Miscellaneous") ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"UseScanRange", Convert::ToString(this->UseScanRange)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MinScan", Convert::ToString(this->MinScan)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MaxScan", Convert::ToString(this->MaxScan)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"UseMZRange", Convert::ToString(this->UseMZRange)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MinMZ", Convert::ToString(this->MinMZ)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MaxMZ", Convert::ToString(this->MaxMZ)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"ApplySavitzkyGolay", Convert::ToString(this->UseSavitzkyGolaySmooth)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"SGNumLeft", Convert::ToString(this->SGNumLeft)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"SGNumRight", Convert::ToString(this->SGNumRight)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"SGOrder", Convert::ToString(this->SGOrder)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			
			xwriter->WriteElementString(S"ZeroFillDiscontinousAreas", Convert::ToString(this->ZeroFill)) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"NumZerosToFill", Convert::ToString(this->NumZerosToFill)) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			
			xwriter->WriteElementString(S"ProcessMSMS",this->ProcessMSMS.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"ExportFileType",__box(this->ExportFileType)->ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			
			xwriter->WriteElementString(S"DetectPeaksOnly_NoDeconvolution", this->DetectPeaksOnlyWithNoDeconvolution.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"Process_MS",this->ProcessMS.ToString()) ;
			xwriter->WriteWhitespace(S"\n\t") ; 


			xwriter->WriteEndElement();
			xwriter->WriteWhitespace(S"\n\t") ; 
			
		}

		void clsHornTransformParameters::LoadV1HornTransformParameters(XmlReader *rdr)
		{
			//Add code to handle empty nodes.

			//Read each node in the tree.
			while (rdr->Read())
			{

				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals(S"TagFormula"))
						{
							if (rdr->IsEmptyElement)
							{
								this->TagFormula = "" ; 
								continue ; 
							}

							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->TagFormula = rdr->Value ; 
						}
						else if (rdr->Name->Equals(S"UseMercuryCaching"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for caching of Mercury in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for caching of Mercury in parameter file") ; 
							}
							else
							{
								this->UseMercuryCaching = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"SumSpectra"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for SumSpectra in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for summing spectra in parameter file") ; 
							}
							else
							{
								this->SumSpectra = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"SumSpectraAcrossScanRange"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for SumSpectraAcrossScanRange in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for summing across scan range in parameter file") ; 
							}
							else
							{
								this->SumSpectraAcrossScanRange = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"SumSpectraAcrossFrameRange"))  //FIX this
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for SumSpectraAcrossFrameRange in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for summing across frame range in parameter file") ; 
							}
							else
							{
								this->SumSpectraAcrossFrameRange = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"CheckAllPatternsAgainstCharge1"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for CheckAllPatternsAgainstCharge1 in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for caching of Mercury in parameter file") ; 
							}
							else
							{
								this->CheckAllPatternsAgainstCharge1 = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"AveragineFormula"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No averagine formula provided in parameter file") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No averagine formula provided in parameter file") ; 
							}
							else
							{
								this->set_AveragineFormula(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"DeleteIntensityThreshold"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No DeleteIntensityThreshold was specified in parameter file") ; 
							}
							else
							{
								this->set_DeleteIntensityThreshold(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"MaxFit"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No MaxFit value was specified in parameter file") ; 
							}
							else
							{
								this->set_MaxFit(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"MinIntensityForScore"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No minimum intensity value was specified for score in parameter file") ; 
							}
							else
							{
								this->set_MinIntensityForScore(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"MaxCharge"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No maximum charge value was specified in parameter file") ; 
							}
							else
							{
								this->set_MaxCharge(Convert::ToInt16(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"MaxMW"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No maximum mass value was specified in parameter file") ; 
							}
							else
							{
								this->set_MaxMW(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"NumPeaksForShoulder"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for number of peaks for shoulder in parameter file") ; 
							}
							else
							{
								this->set_NumPeaksForShoulder(Convert::ToInt16(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"O16O18Media"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for O16/O18 media in parameter file") ; 
							}
							else
							{
								this->set_O16O18Media(Convert::ToBoolean(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"PeptideMinBackgroundRatio"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for min background ratio for peptide in parameter file") ; 
							}
							else
							{
								this->set_PeptideMinBackgroundRatio(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"UseAbsolutePeptideIntensity"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for use of absoluted intensity threshold in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for use of absolute intensity threshold in parameter file") ; 
							}
							else
							{
								this->UseAbsolutePeptideIntensity = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"AbsolutePeptideIntensity"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for absolute peptide threshold in parameter file") ; 
							}
							else
							{
								this->set_AbsolutePeptideIntensity(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"NumberOfScansToSumOver"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for number of peaks for shoulder in parameter file") ; 
							}
							else
							{
								this->set_NumScansToSumOver(Convert::ToInt16(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"NumberOfFramesToSumOver"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for number of frames to sum over in parameter file") ; 
							}
							else
							{
								this->set_NumFramesToSumOver(Convert::ToInt16(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"ThrashOrNot"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for thrashing in parameter file") ; 
							}
							else
							{
								this->set_ThrashOrNot(Convert::ToBoolean(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"CompleteFit"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for complete fit in parameter file") ; 
							}
							else
							{
								this->set_CompleteFit(Convert::ToBoolean(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"CCMass"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for charge carrier mass in parameter file") ; 
							}
							else
							{
								this->set_CCMass(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"IsotopeFitType"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new System::Exception (S"Missing information for IsotopeFitType in parameter file") ; 
							}
							if (rdr->Value->Equals(__box(DeconToolsV2::enmIsotopeFitType::AREA)->ToString()))
							{
								this->IsotopeFitType = DeconToolsV2::enmIsotopeFitType::AREA ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::enmIsotopeFitType::PEAK)->ToString()))
							{
								this->IsotopeFitType = DeconToolsV2::enmIsotopeFitType::PEAK ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::enmIsotopeFitType::CHISQ)->ToString()))
							{
								this->IsotopeFitType = DeconToolsV2::enmIsotopeFitType::CHISQ ;
							}
						}
						else if (rdr->Name->Equals(S"IsActualMonoMZUsed"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for parameter 'IsActualMonoMZUsed' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for parameter 'IsActualMonoMZUsed' in parameter file") ; 
							}
							else
							{
								this->IsActualMonoMZUsed = Convert::ToBoolean(rdr->Value) ; 
							}
						}


						else if (rdr->Name->Equals(S"LeftFitStringencyFactor"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for parameter 'LeftFitStringencyFactor' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for parameter 'LeftFitStringencyFactor' in parameter file") ; 
							}
							else
							{
								this->LeftFitStringencyFactor = Convert::ToDouble(rdr->Value) ; 
							}
						}


						else if (rdr->Name->Equals(S"RightFitStringencyFactor"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for parameter 'RightFitStringencyFactor' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for parameter 'RightFitStringencyFactor' in parameter file") ; 
							}
							else
							{
								this->RightFitStringencyFactor = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"UseRAPIDDeconvolution"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for parameter 'UseRAPIDDeconvolution' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for parameter 'UseRAPIDDeconvolution' in parameter file") ; 
							}
							else
							{
								this->UseRAPIDDeconvolution = Convert::ToBoolean(rdr->Value) ; 
							}
						}

						else if (rdr->Name->Equals(S"ReplaceRAPIDScoreWithHornFitScore"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new Exception (S"No information for parameter 'ReplaceRAPIDScoreWithHornFitScore' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information for parameter 'ReplaceRAPIDScoreWithHornFitScore' in parameter file") ; 
							}
							else
							{
								this->ReplaceRAPIDScoreWithHornFitScore = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"NumPeaksUsedInAbundance"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for 'NumPeaksUsedInAbundance' in parameter file") ; 
							}
							else
							{
								this->set_NumPeaksUsedInAbundance(Convert::ToInt16(rdr->Value)) ; 
							}
						}
						
						else if (rdr->Name->Equals(S"NumberOfScansToAdvance"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for 'NumScansToAdvance' in parameter file") ; 
							}
							else
							{
								this->set_NumScansToAdvance(Convert::ToInt32(rdr->Value)) ; 
							}
						}




						break ; 
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals(S"HornTransformParameters"))
							return ;
						break ; 
					default:
						break ; 
				}
			}
		}

		void clsHornTransformParameters::LoadV1MiscellaneousParameters(XmlReader *rdr)
		{

			//Read each node in the tree.
			while (rdr->Read())
			{
				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals(S"UseScanRange"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for using scan range") ; 
							}
							this->set_UseScanRange(Convert::ToBoolean(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MinScan"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for min scan number.") ; 
							}
							this->set_MinScan(Convert::ToInt32(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MaxScan"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for max scan number.") ; 
							}
							this->set_MaxScan(Convert::ToInt32(rdr->Value)) ; 
						}
						if (rdr->Name->Equals(S"UseMZRange"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for using mz range") ; 
							}
							this->set_UseMZRange(Convert::ToBoolean(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MinMZ"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for min m/z.") ; 
							}
							this->set_MinMZ(Convert::ToDouble(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MaxMZ"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for max m/z.") ; 
							}
							this->set_MaxMZ(Convert::ToDouble(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"ApplySavitzkyGolay"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for using Savitzky Golay") ; 
							}
							this->set_UseSavitzkyGolaySmooth(Convert::ToBoolean(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"SGNumLeft"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for # of left points in Savitzky Golay smoothing.") ; 
							}
							this->set_SGNumLeft(Convert::ToInt16(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"SGNumRight"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for # of right points in Savitzky Golay smoothing.") ; 
							}
							this->set_SGNumRight(Convert::ToInt16(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"SGOrder"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for order.") ; 
							}
							this->set_SGOrder(Convert::ToInt16(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"ZeroFillDiscontinousAreas"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for zero filling.") ; 
							}
							this->set_ZeroFill(Convert::ToBoolean(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"NumZerosToFill"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No information specified for zero filling.") ; 
							}
							this->set_NumZerosToFill(Convert::ToInt16(rdr->Value)) ; 
						}					
						else if (rdr->Name->Equals(S"ProcessMSMS"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for ProcessMSMS in parameter file") ; 
							}
							else
							{
								this->set_ProcessMSMS(Convert::ToBoolean(rdr->Value)) ; 
							}

						}
						else if (rdr->Name->Equals(S"ExportFileType"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new System::Exception (S"Missing information for ExportFileType in parameter file") ; 
							}
							if (rdr->Value->Equals(__box(enmExportFileType::TEXT)->ToString()))
							{
								this->ExportFileType = enmExportFileType::TEXT ;
							}
							else if (rdr->Value->Equals(__box(enmExportFileType::SQLITE)->ToString()))
							{
								this->ExportFileType = enmExportFileType::SQLITE;
							}
							else 
							{
								this->ExportFileType = enmExportFileType::TEXT;
							}
						}
						else if (rdr->Name->Equals(S"Process_MS"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for ProcessMS in parameter file") ; 
							}
							else
							{
								this->set_ProcessMS(Convert::ToBoolean(rdr->Value)) ; 
							}

						}
					    else if (rdr->Name->Equals(S"DetectPeaksOnly_NoDeconvolution"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No parameters was specified for 'DetectPeaksOnlyWithNoDeconvolution' in parameter file") ; 
							}
							else
							{
								this->set_DetectPeaksOnlyWithNoDeconvolution(Convert::ToBoolean(rdr->Value)) ; 
							}

						}


				
						break ; 
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals(S"Miscellaneous"))
							return ;
						break ; 
					default:
						break ; 
				}
			}
		}

	}
}
