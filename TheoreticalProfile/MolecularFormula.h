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
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <iostream>

#include "AtomicInformation.h" 
namespace Engine
{
	namespace TheoreticalProfile
	{
		class MolecularFormulaError
		{
			public:
			std::string mstr_message ; 
			std::string mstr_location ; 

			MolecularFormulaError(std::string error_message, std::string location)
			{
				mstr_message = error_message ; 
				mstr_location = location ; 
			}
			~MolecularFormulaError()
			{
			}
		} ; 

		class AtomicCount
		{
		public:
			// index of this element in list of elements used.
			int mint_index ;
			// Number of copies of the above element in compound.
			// we have set this to be a double to allow for normalized values.
			double mdbl_num_copies ; 
		} ; 

		class MolecularFormula
		{
		public:
			int mint_num_elements ; 
			double mdbl_total_atom_count ; 
			double mdbl_monoisotopic_mass ; 
			double mdbl_average_mass ; 
			std::string mstr_formula ; 
			MolecularFormula& operator=(const MolecularFormula&) ; 
			std::vector<AtomicCount> mvect_elemental_composition ;
			MolecularFormula() ;
			void Clear() ; 
			bool IsAssigned() { return mint_num_elements != 0 ; } ; 
			MolecularFormula(std::string formula, AtomicInformation &atomic_information) ;
			void SetMolecularFormula(std::string formula, AtomicInformation &atomic_information) ; 
			void AddAtomicCount(AtomicCount &cnt, double mono_mass, double avg_mass) ; 
			~MolecularFormula() ; 
		};
		std::ostream& operator << (std::ostream &out, const MolecularFormula &formula);

	}
}