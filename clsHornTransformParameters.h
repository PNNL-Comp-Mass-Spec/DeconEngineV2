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

		public enum class enmExportFileType {TEXT = 0, SQLITE}; 

	public ref class clsHornTransformParameters: public System::ICloneable
		{
			static System::String ^DEFAULT_ISOTOPE_FILE = "isotope.xml" ;
			DeconToolsV2::clsElementIsotopes ^mobjElementIsotopes ;

		public:
			clsHornTransformParameters(void);
			~clsHornTransformParameters(void);
			void SaveV1HornTransformParameters(System::Xml::XmlTextWriter ^xwriter) ; 
			void SaveV1MiscellaneousParameters(System::Xml::XmlTextWriter ^xwriter) ; 

			void LoadV1HornTransformParameters(System::Xml::XmlReader ^rdr) ; 
			void LoadV1MiscellaneousParameters(System::Xml::XmlReader ^rdr) ; 

			virtual Object^ Clone()
			{
				clsHornTransformParameters ^new_params = gcnew clsHornTransformParameters; 
				new_params->AveragineFormula = static_cast<System::String^>(this->AveragineFormula->Clone()); 
				new_params->CCMass = this->CCMass; 
                new_params->CompleteFit = this->CompleteFit;
                new_params->DeleteIntensityThreshold = this->DeleteIntensityThreshold;
                new_params->ElementIsotopeComposition(static_cast<DeconToolsV2::clsElementIsotopes^>(this->ElementIsotopeComposition()->Clone()));
                new_params->MaxCharge = this->MaxCharge;
                new_params->MaxFit = this->MaxFit;
                new_params->MaxMW = this->MaxMW;
                new_params->MinIntensityForScore = this->MinIntensityForScore;
                new_params->MinS2N = this->MinS2N;
                new_params->NumPeaksForShoulder = this->NumPeaksForShoulder;
                new_params->O16O18Media = this->O16O18Media;
                new_params->PeptideMinBackgroundRatio = this->PeptideMinBackgroundRatio;
                new_params->TagFormula = static_cast<System::String ^>(this->TagFormula->Clone());
                new_params->ThrashOrNot = this->ThrashOrNot;
                new_params->IsotopeFitType = this->IsotopeFitType;
                new_params->UseMercuryCaching = this->UseMercuryCaching;

                new_params->UseMZRange = this->UseMZRange;
                new_params->MinMZ = this->MinMZ;
                new_params->MaxMZ = this->MaxMZ;
                new_params->UseMercuryCaching = this->UseMercuryCaching;
				new_params->MinScan = this->MinScan;
				new_params->MaxScan = this->MaxScan;
                new_params->UseScanRange = this->UseScanRange;
                new_params->CheckAllPatternsAgainstCharge1 = this->CheckAllPatternsAgainstCharge1;

                new_params->UseSavitzkyGolaySmooth = this->UseSavitzkyGolaySmooth;
                new_params->SGNumLeft = this->SGNumLeft;
                new_params->SGNumRight = this->SGNumRight;
                new_params->SGOrder = this->SGOrder;

                new_params->ZeroFill = this->ZeroFill;
                new_params->NumZerosToFill = this->NumZerosToFill;

                new_params->AbsolutePeptideIntensity = this->AbsolutePeptideIntensity;
                new_params->UseAbsolutePeptideIntensity = this->UseAbsolutePeptideIntensity;
                new_params->NumScansToSumOver = this->NumScansToSumOver;
                new_params->SumSpectra = this->SumSpectra;
                new_params->SumSpectraAcrossScanRange = this->SumSpectraAcrossScanRange;
                new_params->NumFramesToSumOver = this->NumFramesToSumOver;
                new_params->SumSpectraAcrossFrameRange = this->SumSpectraAcrossFrameRange;
                new_params->ProcessMSMS = this->ProcessMSMS;
                new_params->IsActualMonoMZUsed = this->IsActualMonoMZUsed;
                new_params->LeftFitStringencyFactor = this->LeftFitStringencyFactor;
                new_params->RightFitStringencyFactor = this->RightFitStringencyFactor;
                new_params->UseRAPIDDeconvolution = this->UseRAPIDDeconvolution;
                new_params->ReplaceRAPIDScoreWithHornFitScore = this->ReplaceRAPIDScoreWithHornFitScore;
                new_params->ExportFileType = this->ExportFileType;
                new_params->NumPeaksUsedInAbundance = this->NumPeaksUsedInAbundance;
                new_params->NumScansToAdvance = this->NumScansToAdvance;
                new_params->DetectPeaksOnlyWithNoDeconvolution = this->DetectPeaksOnlyWithNoDeconvolution;
                new_params->ProcessMS = this->ProcessMS;
                new_params->ScanBasedWorkflowType = static_cast<System::String ^>(this->ScanBasedWorkflowType->Clone());


				return new_params; 
			}

            property bool IsActualMonoMZUsed;
            property double LeftFitStringencyFactor;
            property double RightFitStringencyFactor;
            property short SGNumLeft;
            property short SGNumRight;
            property short SGOrder;
            property bool UseSavitzkyGolaySmooth;
            property int MinScan;
            property int MaxScan;
            property bool UseMercuryCaching;
            //! whether or not to use a range of scans
            property bool UseScanRange;
			property bool SumSpectra;
            property bool SumSpectraAcrossScanRange;
		    property int NumScansToSumOver;
            property int NumFramesToSumOver;
            property bool SumSpectraAcrossFrameRange;
			property bool UseMZRange;
            property double MinMZ;
            property double MaxMZ;

            property short MaxCharge;
            property double MinS2N;
            property double PeptideMinBackgroundRatio;
            property short NumPeaksForShoulder;
            property double MaxMW;
            property double MaxFit;
            property double CCMass;
            property double DeleteIntensityThreshold;
            property bool ZeroFill;
            property short NumZerosToFill;
            property double MinIntensityForScore;
            property bool O16O18Media;
            property System::String^ AveragineFormula;
            property System::String^ TagFormula;
            property bool ThrashOrNot;
            property bool CompleteFit;
            property bool ProcessMSMS;
            property bool CheckAllPatternsAgainstCharge1;

			clsElementIsotopes^ ElementIsotopeComposition()
			{
				if (mobjElementIsotopes == nullptr)
					mobjElementIsotopes = gcnew DeconToolsV2::clsElementIsotopes() ; 
				return mobjElementIsotopes ; 
			}

			void ElementIsotopeComposition(clsElementIsotopes ^atomic_info)
			{
				if (atomic_info != mobjElementIsotopes)
					mobjElementIsotopes->Assign(atomic_info) ; 
			}
            
            property enmIsotopeFitType IsotopeFitType;
            property bool UseAbsolutePeptideIntensity;
            property double AbsolutePeptideIntensity;
            property bool UseRAPIDDeconvolution;
            property bool ReplaceRAPIDScoreWithHornFitScore;
            property enmExportFileType ExportFileType;
            property short NumPeaksUsedInAbundance; // when abundance for an IsosResult is reported... this is how many peaks are summed. Typically the most abundant peak's abundance is reported. 
            property short NumScansToAdvance;
            property bool DetectPeaksOnlyWithNoDeconvolution;
            property bool ProcessMS; // parameter that will allow skipping of MSLevel data
            property System::String^ ScanBasedWorkflowType;
            property double SaturationThreshold;
		}
	; }
}
