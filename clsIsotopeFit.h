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
#include "clsPeak.h" 
#include "HornTransform/AreaFit.h"
#include "HornTransform/PeakFit.h"
#include "HornTransform/ChisqFit.h"
#include "DeconEngineUtils.h"
namespace DeconToolsV2
{
	public enum class enmIsotopeFitType {PEAK = 0, AREA, CHISQ}; 
	
	public ref class clsIsotopeFit
	{
        enmIsotopeFitType m_IsotopeFitType;
		Engine::HornTransform::IsotopeFit *mobj_fit ; 

	public:
        double GetFitScore(array<float> ^ (&mzs), array<float> ^ (&intensities),
			array<DeconToolsV2::Peaks::clsPeak^>^ (&peaks), short charge, int peak_index,
			double delete_intensity_threshold, double min_intensity_for_score, 
			System::Collections::Hashtable^ elementCounts) ; 

		void SetOptions(System::String ^averagine_mf, System::String ^tag_mf, 
			double cc_mass, bool thrash_or_not, bool complete_fit)
		{
			char averagine_formula[512] ;
			char tag_formula[512] ;

			averagine_formula[0] = '\0' ;
			tag_formula[0] = '\0' ;

			if (averagine_mf != nullptr)
			{
				DeconEngine::Utils::GetStr(averagine_mf, averagine_formula) ;
			}
			if (tag_mf != nullptr)
			{
				DeconEngine::Utils::GetStr(tag_mf, tag_formula) ;
			}
			mobj_fit->SetOptions(averagine_formula, tag_formula, cc_mass, thrash_or_not, complete_fit) ; 
		}

		enmIsotopeFitType IsotopeFitType()
		{
			return m_IsotopeFitType ; 
		}

		void IsotopeFitType(enmIsotopeFitType type)
		{
            m_IsotopeFitType = type;
			Engine::HornTransform::IsotopeFit *new_fit ; 
            switch (m_IsotopeFitType)
			{
                case enmIsotopeFitType::PEAK:
					new_fit = new Engine::HornTransform::PeakFit() ; 
					break ; 
                case enmIsotopeFitType::AREA:
					new_fit = new Engine::HornTransform::AreaFit() ; 
					break ; 
                case enmIsotopeFitType::CHISQ:
					new_fit = new Engine::HornTransform::ChiSqFit() ; 
					break ;
				default:
					return ; 
					break ;
			}
			
			if (mobj_fit != NULL)
			{
				*new_fit = *mobj_fit ; 
				delete mobj_fit ;
			}
			mobj_fit = new_fit ; 
		}

		clsIsotopeFit(void);
		~clsIsotopeFit(void);
	};
}