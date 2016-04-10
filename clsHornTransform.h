// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
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
#include <vector>
#include <fstream>
#include "clsPeak.h"
#include "clsHornTransformResults.h"
#include "clsHornTransformParameters.h"
#include "PeakProcessor/PeakData.h"
#include "HornTransform/MassTransform.h" 
#include "clsElementIsotopes.h" 
#include "TheoreticalProfile/AtomicInformation.h" 
namespace DeconToolsV2
{
	namespace HornTransform
	{
		public ref class clsHornTransform
		{
			int mint_percent_done ; 
			System::String ^mstr_status_mesg ; 
			Engine::HornTransform::MassTransform *mobj_transform ; 
			clsHornTransformParameters^ mobj_transform_parameters ; 
			void SetIsotopeFitOptions(System::String ^str_averagine, System::String ^str_tag, bool thrash_or_not, bool complete_fit) ; 
			void SetOptions(short max_charge, double max_mw, double max_fit, double min_s2n, double cc_mass, 
				double delete_threshold_intensity, double min_theoretical_intensity_for_score, 
				short num_peaks_for_shoulder, bool use_caching, bool o16_o18_media, bool check_against_charge_1,
				double leftFitStringencyFactor, double rightFitStingencyFactor, bool isMonoMZUsed ) ; 
		public:

			clsHornTransformParameters^ TransformParameters()
			{
				return mobj_transform_parameters ; 
			}
			void TransformParameters(clsHornTransformParameters^ transform_parameters)
			{
				mobj_transform_parameters = dynamic_cast<clsHornTransformParameters ^>(transform_parameters->Clone()) ; 
				SetOptions(mobj_transform_parameters->MaxCharge, 
					mobj_transform_parameters->MaxMW, 
					mobj_transform_parameters->MaxFit, 
					mobj_transform_parameters->MinS2N, 
					mobj_transform_parameters->CCMass,
					mobj_transform_parameters->DeleteIntensityThreshold, 
					mobj_transform_parameters->MinIntensityForScore,
					mobj_transform_parameters->NumPeaksForShoulder,
					mobj_transform_parameters->UseMercuryCaching,  
					mobj_transform_parameters->O16O18Media, 
					mobj_transform_parameters->CheckAllPatternsAgainstCharge1,
					mobj_transform_parameters->LeftFitStringencyFactor,
					mobj_transform_parameters->RightFitStringencyFactor,
					mobj_transform_parameters->IsActualMonoMZUsed) ;
				
				SetIsotopeFitOptions(mobj_transform_parameters->AveragineFormula, mobj_transform_parameters->TagFormula,
					mobj_transform_parameters->ThrashOrNot, mobj_transform_parameters->CompleteFit) ; 
				const Engine::TheoreticalProfile::AtomicInformation *ptr_atomic_info = mobj_transform_parameters->ElementIsotopeComposition()->GetElementalIsotopeComposition() ; 
				if (mobj_transform != NULL)
				{
					mobj_transform->SetElementalIsotopeComposition(*ptr_atomic_info) ; 
					mobj_transform->SetIsotopeFitType((Engine::HornTransform::IsotopicFittingType) mobj_transform_parameters->IsotopeFitType) ; 
				}
			}

			int PercentDone()
			{
				return mint_percent_done ; 
			}

			System::String^ StatusMessage()
			{
				return mstr_status_mesg ; 
			}

			clsHornTransform(void);
			~clsHornTransform(void);
            void PerformTransform(float background_intensity, float min_peptide_intensity, array<float> ^ (&mzs), array<float> ^ (&intensities),
				array<DeconToolsV2::Peaks::clsPeak^>^ (&peaks), array<DeconToolsV2::HornTransform::clsHornTransformResults^>^ (&transformResults)) ; 
			
		};
	}
}