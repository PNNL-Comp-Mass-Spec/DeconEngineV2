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

#using <System.Xml.dll>
namespace DeconToolsV2
{
	namespace HornTransform
	{
		clsHornTransformParameters::clsHornTransformParameters(void)
		{
			MaxCharge = 10 ;
			NumPeaksForShoulder = 1 ;
			MaxMW = 10000 ; 
			MaxFit = 0.25 ; 
			CCMass = 1.00727638;
			DeleteIntensityThreshold = 10 ; 
			MinIntensityForScore = 10 ;
			O16O18Media = false ; 
			MinS2N = 3 ; 
			PeptideMinBackgroundRatio = 5 ; 
			AveragineFormula = "C4.9384 H7.7583 N1.3577 O1.4773 S0.0417";
			TagFormula = "" ;
			O16O18Media = false ; 
			ThrashOrNot = true ; 
			CompleteFit = false ; 
			UseMercuryCaching = true ; 
			mobjElementIsotopes = gcnew DeconToolsV2::clsElementIsotopes() ; 
			UseMZRange = true ; 
			MinMZ = 400 ; 
			MaxMZ = 2000 ; 
			MinScan = System::Int32::MinValue ; 
			MaxScan = System::Int32::MaxValue ; 
			UseScanRange = false ; 
			UseSavitzkyGolaySmooth = false ; 
			SGNumLeft = 2  ;
			SGNumRight = 2  ;
			SGOrder = 2  ;
			IsotopeFitType = DeconToolsV2::enmIsotopeFitType::AREA ;
			UseAbsolutePeptideIntensity = false ; 
			AbsolutePeptideIntensity = 0 ; 
			NumZerosToFill = 3; 
			ZeroFill = false ;
			CheckAllPatternsAgainstCharge1 = false ;
			NumScansToSumOver = 0 ; 
			SumSpectra = false ; 
			ProcessMSMS = false ; 
			SumSpectraAcrossFrameRange = true;
			NumFramesToSumOver = 3;
			IsActualMonoMZUsed = false;
			LeftFitStringencyFactor = 1;
			RightFitStringencyFactor = 1;
			UseRAPIDDeconvolution = false;
			ReplaceRAPIDScoreWithHornFitScore = false;
			ExportFileType = enmExportFileType::TEXT;
			NumPeaksUsedInAbundance = 1 ;
			DetectPeaksOnlyWithNoDeconvolution= false;
			ProcessMS = true;
			ScanBasedWorkflowType = "standard";
			SaturationThreshold = 90000;
		}

		clsHornTransformParameters::~clsHornTransformParameters(void)
		{
		}

		void clsHornTransformParameters::SaveV1HornTransformParameters(System::Xml::XmlTextWriter ^xwriter)
		{
			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteStartElement("HornTransformParameters");
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("TagFormula",this->TagFormula) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("AveragineFormula",this->AveragineFormula) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("DeleteIntensityThreshold",this->DeleteIntensityThreshold.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MaxFit",this->MaxFit.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MinIntensityForScore",this->MinIntensityForScore.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("MaxCharge",this->MaxCharge.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MaxMW",this->MaxMW.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("NumPeaksForShoulder",this->NumPeaksForShoulder.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("O16O18Media",this->O16O18Media.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("PeptideMinBackgroundRatio",this->PeptideMinBackgroundRatio.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("UseAbsolutePeptideIntensity",this->UseAbsolutePeptideIntensity.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("AbsolutePeptideIntensity",this->AbsolutePeptideIntensity.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("ThrashOrNot",this->ThrashOrNot.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("CheckAllPatternsAgainstCharge1",this->CheckAllPatternsAgainstCharge1.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("CompleteFit",this->CompleteFit.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("CCMass",this->CCMass.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("IsotopeFitType",this->IsotopeFitType.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("UseMercuryCaching",this->UseMercuryCaching.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("SumSpectra",this->SumSpectra.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("SumSpectraAcrossScanRange",this->SumSpectraAcrossScanRange.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("NumberOfScansToSumOver",this->NumScansToSumOver.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("NumberOfScansToAdvance",this->NumScansToAdvance.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("SumSpectraAcrossFrameRange",this->SumSpectraAcrossFrameRange.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("NumberOfFramesToSumOver",this->NumFramesToSumOver.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		
			
			xwriter->WriteElementString("IsActualMonoMZUsed",this->IsActualMonoMZUsed.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("LeftFitStringencyFactor",this->LeftFitStringencyFactor.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("RightFitStringencyFactor",this->RightFitStringencyFactor.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("UseRAPIDDeconvolution",this->UseRAPIDDeconvolution.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("ReplaceRAPIDScoreWithHornFitScore",this->ReplaceRAPIDScoreWithHornFitScore.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ;

			xwriter->WriteElementString("NumPeaksUsedInAbundance",this->NumPeaksUsedInAbundance.ToString()) ;

			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteEndElement();
			
		}

		void clsHornTransformParameters::SaveV1MiscellaneousParameters(System::Xml::XmlTextWriter ^xwriter)
		{
			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteStartElement("Miscellaneous") ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("UseScanRange", Convert::ToString(this->UseScanRange)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MinScan", Convert::ToString(this->MinScan)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MaxScan", Convert::ToString(this->MaxScan)); 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("UseMZRange", Convert::ToString(this->UseMZRange)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MinMZ", Convert::ToString(this->MinMZ)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MaxMZ", Convert::ToString(this->MaxMZ)); 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("ApplySavitzkyGolay", Convert::ToString(this->UseSavitzkyGolaySmooth)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("SGNumLeft", Convert::ToString(this->SGNumLeft)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("SGNumRight", Convert::ToString(this->SGNumRight)); 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("SGOrder", Convert::ToString(this->SGOrder)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			
			xwriter->WriteElementString("ZeroFillDiscontinousAreas", Convert::ToString(this->ZeroFill)) ;
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("NumZerosToFill", Convert::ToString(this->NumZerosToFill)) ;
			xwriter->WriteWhitespace("\n\t\t") ; 
			
			xwriter->WriteElementString("ProcessMSMS",this->ProcessMSMS.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("ExportFileType",this->ExportFileType.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 
			
			xwriter->WriteElementString("DetectPeaksOnly_NoDeconvolution", this->DetectPeaksOnlyWithNoDeconvolution.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("Process_MS",this->ProcessMS.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("ScanBasedWorkflowType",this->ScanBasedWorkflowType) ;
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("SaturationThreshold",this->SaturationThreshold.ToString()) ;
			
			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteEndElement();
			
		}

		void clsHornTransformParameters::LoadV1HornTransformParameters(XmlReader ^rdr)
		{
			//Add code to handle empty nodes.

			//Read each node in the tree.
			while (rdr->Read())
			{

				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals("TagFormula"))
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
						else if (rdr->Name->Equals("UseMercuryCaching"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for caching of Mercury in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for caching of Mercury in parameter file") ; 
							}
							else
							{
								this->UseMercuryCaching = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("SumSpectra"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for SumSpectra in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for summing spectra in parameter file") ; 
							}
							else
							{
								this->SumSpectra = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("SumSpectraAcrossScanRange"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for SumSpectraAcrossScanRange in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for summing across scan range in parameter file") ; 
							}
							else
							{
								this->SumSpectraAcrossScanRange = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("SumSpectraAcrossFrameRange"))  //FIX this
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for SumSpectraAcrossFrameRange in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for summing across frame range in parameter file") ; 
							}
							else
							{
								this->SumSpectraAcrossFrameRange = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("CheckAllPatternsAgainstCharge1"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for CheckAllPatternsAgainstCharge1 in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for caching of Mercury in parameter file") ; 
							}
							else
							{
								this->CheckAllPatternsAgainstCharge1 = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("AveragineFormula"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No averagine formula provided in parameter file") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No averagine formula provided in parameter file") ; 
							}
							else
							{
                                this->AveragineFormula = rdr->Value;
							}
						}
						else if (rdr->Name->Equals("DeleteIntensityThreshold"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No DeleteIntensityThreshold was specified in parameter file") ; 
							}
							else
							{
                                this->DeleteIntensityThreshold = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("MaxFit"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No MaxFit value was specified in parameter file") ; 
							}
							else
							{
                                this->MaxFit = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("MinIntensityForScore"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No minimum intensity value was specified for score in parameter file") ; 
							}
							else
							{
                                this->MinIntensityForScore = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("MaxCharge"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No maximum charge value was specified in parameter file") ; 
							}
							else
							{
                                this->MaxCharge = Convert::ToInt16(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("MaxMW"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No maximum mass value was specified in parameter file") ; 
							}
							else
							{
                                this->MaxMW = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("NumPeaksForShoulder"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for number of peaks for shoulder in parameter file") ; 
							}
							else
							{
                                this->NumPeaksForShoulder = Convert::ToInt16(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("O16O18Media"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for O16/O18 media in parameter file") ; 
							}
							else
							{
                                this->O16O18Media = Convert::ToBoolean(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("PeptideMinBackgroundRatio"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for min background ratio for peptide in parameter file") ; 
							}
							else
							{
                                this->PeptideMinBackgroundRatio = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("UseAbsolutePeptideIntensity"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for use of absoluted intensity threshold in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for use of absolute intensity threshold in parameter file") ; 
							}
							else
							{
								this->UseAbsolutePeptideIntensity = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("AbsolutePeptideIntensity"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for absolute peptide threshold in parameter file") ; 
							}
							else
							{
                                this->AbsolutePeptideIntensity = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("NumberOfScansToSumOver"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for number of peaks for shoulder in parameter file") ; 
							}
							else
							{
                                this->NumScansToSumOver = Convert::ToInt16(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("NumberOfFramesToSumOver"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for number of frames to sum over in parameter file") ; 
							}
							else
							{
                                this->NumFramesToSumOver = Convert::ToInt16(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("ThrashOrNot"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for thrashing in parameter file") ; 
							}
							else
							{
                                this->ThrashOrNot = Convert::ToBoolean(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("CompleteFit"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for complete fit in parameter file") ; 
							}
							else
							{
                                this->CompleteFit = Convert::ToBoolean(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("CCMass"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for charge carrier mass in parameter file") ; 
							}
							else
							{
                                this->CCMass = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("IsotopeFitType"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew System::Exception ("Missing information for IsotopeFitType in parameter file") ; 
							}
							if (rdr->Value->Equals(DeconToolsV2::enmIsotopeFitType::AREA.ToString()))
							{
								this->IsotopeFitType = DeconToolsV2::enmIsotopeFitType::AREA ;
							}
							else if (rdr->Value->Equals(DeconToolsV2::enmIsotopeFitType::PEAK.ToString()))
							{
								this->IsotopeFitType = DeconToolsV2::enmIsotopeFitType::PEAK ;
							}
							else if (rdr->Value->Equals(DeconToolsV2::enmIsotopeFitType::CHISQ.ToString()))
							{
								this->IsotopeFitType = DeconToolsV2::enmIsotopeFitType::CHISQ ;
							}
						}
						else if (rdr->Name->Equals("IsActualMonoMZUsed"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for parameter 'IsActualMonoMZUsed' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for parameter 'IsActualMonoMZUsed' in parameter file") ; 
							}
							else
							{
								this->IsActualMonoMZUsed = Convert::ToBoolean(rdr->Value) ; 
							}
						}


						else if (rdr->Name->Equals("LeftFitStringencyFactor"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for parameter 'LeftFitStringencyFactor' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for parameter 'LeftFitStringencyFactor' in parameter file") ; 
							}
							else
							{
								this->LeftFitStringencyFactor = Convert::ToDouble(rdr->Value) ; 
							}
						}


						else if (rdr->Name->Equals("RightFitStringencyFactor"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for parameter 'RightFitStringencyFactor' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for parameter 'RightFitStringencyFactor' in parameter file") ; 
							}
							else
							{
								this->RightFitStringencyFactor = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("UseRAPIDDeconvolution"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for parameter 'UseRAPIDDeconvolution' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for parameter 'UseRAPIDDeconvolution' in parameter file") ; 
							}
							else
							{
								this->UseRAPIDDeconvolution = Convert::ToBoolean(rdr->Value) ; 
							}
						}

						else if (rdr->Name->Equals("ReplaceRAPIDScoreWithHornFitScore"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew Exception ("No information for parameter 'ReplaceRAPIDScoreWithHornFitScore' in parameter file") ; 
							}
							rdr->Read() ; 

							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information for parameter 'ReplaceRAPIDScoreWithHornFitScore' in parameter file") ; 
							}
							else
							{
								this->ReplaceRAPIDScoreWithHornFitScore = Convert::ToBoolean(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("NumPeaksUsedInAbundance"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for 'NumPeaksUsedInAbundance' in parameter file") ; 
							}
							else
							{
                                this->NumPeaksUsedInAbundance = Convert::ToInt16(rdr->Value);
							}
						}
						
						else if (rdr->Name->Equals("NumberOfScansToAdvance"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for 'NumScansToAdvance' in parameter file") ; 
							}
							else
							{
                                this->NumScansToAdvance = Convert::ToInt32(rdr->Value);
							}
						}




						break ; 
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals("HornTransformParameters"))
							return ;
						break ; 
					default:
						break ; 
				}
			}
		}

		void clsHornTransformParameters::LoadV1MiscellaneousParameters(XmlReader ^rdr)
		{

			//Read each node in the tree.
			while (rdr->Read())
			{
				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals("UseScanRange"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for using scan range") ; 
							}
                            this->UseScanRange = Convert::ToBoolean(rdr->Value);
						}
						else if (rdr->Name->Equals("MinScan"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for min scan number.") ; 
							}
                            this->MinScan = Convert::ToInt32(rdr->Value);
						}
						else if (rdr->Name->Equals("MaxScan"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for max scan number.") ; 
							}
                            this->MaxScan = Convert::ToInt32(rdr->Value);
						}
						if (rdr->Name->Equals("UseMZRange"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for using mz range") ; 
							}
                            this->UseMZRange = Convert::ToBoolean(rdr->Value);
						}
						else if (rdr->Name->Equals("MinMZ"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for min m/z.") ; 
							}
                            this->MinMZ = Convert::ToDouble(rdr->Value);
						}
						else if (rdr->Name->Equals("MaxMZ"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for max m/z.") ; 
							}
                            this->MaxMZ = Convert::ToDouble(rdr->Value);
						}
						else if (rdr->Name->Equals("ApplySavitzkyGolay"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for using Savitzky Golay") ; 
							}
                            this->UseSavitzkyGolaySmooth = Convert::ToBoolean(rdr->Value);
						}
						else if (rdr->Name->Equals("SGNumLeft"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for # of left points in Savitzky Golay smoothing.") ; 
							}
                            this->SGNumLeft = Convert::ToInt16(rdr->Value);
						}
						else if (rdr->Name->Equals("SGNumRight"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for # of right points in Savitzky Golay smoothing.") ; 
							}
                            this->SGNumRight = Convert::ToInt16(rdr->Value);
						}
						else if (rdr->Name->Equals("SGOrder"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for order.") ; 
							}
                            this->SGOrder = Convert::ToInt16(rdr->Value);
						}
						else if (rdr->Name->Equals("ZeroFillDiscontinousAreas"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for zero filling.") ; 
							}
                            this->ZeroFill = Convert::ToBoolean(rdr->Value);
						}
						else if (rdr->Name->Equals("NumZerosToFill"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No information specified for zero filling.") ; 
							}
                            this->NumZerosToFill = Convert::ToInt16(rdr->Value);
						}					
						else if (rdr->Name->Equals("ProcessMSMS"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for ProcessMSMS in parameter file") ; 
							}
							else
							{
                                this->ProcessMSMS = Convert::ToBoolean(rdr->Value);
							}

						}
						else if (rdr->Name->Equals("ExportFileType"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew System::Exception ("Missing information for ExportFileType in parameter file") ; 
							}
							if (rdr->Value->Equals(enmExportFileType::TEXT.ToString()))
							{
								this->ExportFileType = enmExportFileType::TEXT ;
							}
							else if (rdr->Value->Equals(enmExportFileType::SQLITE.ToString()))
							{
								this->ExportFileType = enmExportFileType::SQLITE;
							}
							else 
							{
								this->ExportFileType = enmExportFileType::TEXT;
							}
						}
						else if (rdr->Name->Equals("Process_MS"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for ProcessMS in parameter file") ; 
							}
							else
							{
                                this->ProcessMS = Convert::ToBoolean(rdr->Value);
							}

						}
					    else if (rdr->Name->Equals("DetectPeaksOnly_NoDeconvolution"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters was specified for 'DetectPeaksOnlyWithNoDeconvolution' in parameter file") ; 
							}
							else
							{
                                this->DetectPeaksOnlyWithNoDeconvolution = Convert::ToBoolean(rdr->Value);
							}

						}
						else if (rdr->Name->Equals("ScanBasedWorkflowType"))
						{
							if (rdr->IsEmptyElement)
							{
								this->ScanBasedWorkflowType = "" ; 
								continue ; 
							}

							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->ScanBasedWorkflowType = rdr->Value ; 
						}
						else if (rdr->Name->Equals("SaturationThreshold"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No parameters were specified for 'SaturationThreshold'") ; 
							}
							else
							{
                                this->SaturationThreshold = Convert::ToDouble(rdr->Value);
							}
						}
				
						break ; 
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals("Miscellaneous"))
							return ;
						break ; 
					default:
						break ; 
				}
			}
		}

	}
}
