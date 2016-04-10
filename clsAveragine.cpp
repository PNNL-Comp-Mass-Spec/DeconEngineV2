// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\clsAveragine.h"
#include "DeconEngineUtils.h"
#using <mscorlib.dll>

#using <System.Xml.dll>
namespace DeconToolsV2
{
	namespace HornTransform
	{
		clsAveragine::clsAveragine(void)
		{
			mobjAveragine = new Engine::TheoreticalProfile::Averagine() ; 
			mobjElementIsotopes = gcnew clsElementIsotopes() ; 
			mobjAveragine->SetElementalIsotopeComposition(*mobjElementIsotopes->GetElementalIsotopeComposition()) ; 
		}

		clsAveragine::~clsAveragine(void)
		{
		}

		System::String^ clsAveragine::GenerateAveragineFormula(double averageMass, System::String ^averagineFormula, 
			System::String ^tagFormula)
		{
			char averagine_formula[512] ;
			char tag_formula[512] ;

			averagine_formula[0] = '\0' ;
			tag_formula[0] = '\0' ;

			DeconEngine::Utils::GetStr(averagineFormula, averagine_formula) ;
			if (tagFormula != nullptr)
			{
				DeconEngine::Utils::GetStr(tagFormula, tag_formula) ;
			}
			mobjAveragine->SetAveragineFormula(averagine_formula) ; 
			mobjAveragine->SetTagFormula(tag_formula); 
			Engine::TheoreticalProfile::MolecularFormula empirical_formula ; 
			mobjAveragine->GetAverageFormulaForMass(averageMass, empirical_formula) ; 
			// Convert to String. 
			System::String ^empiricalFormula = gcnew System::String("") ; 
			
			const Engine::TheoreticalProfile::AtomicInformation *atomicInfo = mobjElementIsotopes->GetElementalIsotopeComposition() ;

			int numElements = empirical_formula.mvect_elemental_composition.size() ; 
			for (int elementNum = 0 ; elementNum < numElements ; elementNum++)
			{
				Engine::TheoreticalProfile::AtomicCount atomic_count = 
					empirical_formula.mvect_elemental_composition[elementNum] ; 
                empiricalFormula = System::String::Concat(empiricalFormula, System::Convert::ToString(atomicInfo->mvect_elemental_isotopes[atomic_count.mint_index].marr_symbol),
					System::Convert::ToString(Convert::ToInt32(atomic_count.mdbl_num_copies))) ; 
			}
			return empiricalFormula ;
		}

	}
}
