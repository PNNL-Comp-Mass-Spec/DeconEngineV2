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
#include "MolecularFormula.h"

namespace Engine
{
	namespace TheoreticalProfile
	{
		class Averagine
		{

			MolecularFormula mobj_averagine_formula ; 
			MolecularFormula mobj_tag_formula ; 
			AtomicInformation mobj_element_isotope_abundance ; 
			// If a chemical labelling tag is applied to the molecular formula, 
			// it needs to be necessarily added to the molecular formula (after substraction by 
			// its mass).
			bool mbln_use_tag ;
			double mdbl_tag_mass ;
			double mdbl_averagine_mass ; 
			int mint_hydrogen_element_index ; 

		public:
			Averagine() ; 
			~Averagine() ; 
			Averagine& operator=(const Averagine&) ; 
			void GetAverageFormulaForMass(double MW, MolecularFormula &empirical_formula) ; 
			void SetAveragineFormula(std::string averagine_formula) ; 
			void SetTagFormula(std::string tag_formula) ; 
			void SetElementalIsotopeComposition(const AtomicInformation &element_isotopes) ; 

			void GetAveragineFormula(std::string &averagine_formula)  ;
			void GetTagFormula(std::string &tag_formula)  ;
		};
	}
}