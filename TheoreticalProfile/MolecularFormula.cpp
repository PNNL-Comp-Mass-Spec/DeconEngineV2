// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "MolecularFormula.h"
#include <fstream> 

namespace Engine
{
	namespace TheoreticalProfile
	{
		std::ostream& operator << (std::ostream &out, const MolecularFormula &formula)
		{
			out<<" monomw ="<<formula.mdbl_monoisotopic_mass<<" averagemw="<<formula.mdbl_average_mass<<std::endl ; 
			for (unsigned int element_num = 0 ; element_num < formula.mvect_elemental_composition.size() ; element_num++)
			{
				out<<formula.mvect_elemental_composition[element_num].mint_index<<" Num Atoms = "<<formula.mvect_elemental_composition[element_num].mdbl_num_copies<<std::endl ; 
			}
			return out ;
		}
		MolecularFormula::MolecularFormula()
		{
			mint_num_elements = 0 ; 
			mdbl_total_atom_count = 0 ; 
			mdbl_monoisotopic_mass = 0 ; 
			mdbl_average_mass = 0 ;
			mstr_formula = "" ; 
		}

		void MolecularFormula::Clear()
		{
			mint_num_elements = 0 ; 
			mdbl_total_atom_count = 0 ; 
			mdbl_monoisotopic_mass = 0 ; 
			mdbl_average_mass = 0 ;
			mstr_formula = "" ; 
			mvect_elemental_composition.clear() ; 
		}

		MolecularFormula::MolecularFormula(std::string formula, AtomicInformation &atomic_information)
		{
			SetMolecularFormula(formula, atomic_information) ; 
		}

		MolecularFormula& MolecularFormula::operator=(const MolecularFormula& formula)
		{
			mint_num_elements = formula.mint_num_elements ; 
			mstr_formula = formula.mstr_formula ; 
			mdbl_total_atom_count = formula.mdbl_total_atom_count ; 
			mdbl_monoisotopic_mass = formula.mdbl_monoisotopic_mass ; 
			mvect_elemental_composition.clear() ;
			mvect_elemental_composition.insert(mvect_elemental_composition.begin(), formula.mvect_elemental_composition.begin(), 
				formula.mvect_elemental_composition.end()) ; 
			return (*this) ; 
		}


		void MolecularFormula::SetMolecularFormula(std::string mol_formula, AtomicInformation &atomic_information)
		{
			mstr_formula = mol_formula ; 
			const char *formula = mol_formula.c_str() ; 
			mdbl_total_atom_count = 0 ; 


			mvect_elemental_composition.clear() ; 
			int formula_length = strlen(formula) ; 
			int index = 0 ; 
			char atomic_symbol[32] ; 
			int num_atoms = atomic_information.GetNumElements() ; 
			AtomicCount current_atom ; 
			mdbl_monoisotopic_mass = 0 ; 
			int start_index, stop_index ; 
			char symbol_char ;

			while(index < formula_length)
			{
				while(index < formula_length && (formula[index] == ' ' || formula[index] == '\t'))
				{
					index++ ; 
				}

				start_index = index ; 
				stop_index = start_index ; 
				symbol_char = formula[stop_index] ; 
				if(!(symbol_char >= 'A' && symbol_char <= 'Z') && 
						!(symbol_char >= 'a' && symbol_char <= 'z'))
				{
					char error_str[1024] ; 
					strcpy(error_str, "Molecular Formula specified was incorrect at position: ") ; 
					_itoa(stop_index+1, error_str + stop_index, 10) ; 
					strcat(error_str, ". Should have element symbol there") ; 
					throw MolecularFormulaError(error_str, "Line 59 in MolecularFormula.cpp") ; 
				}

				while((symbol_char >= 'A' && symbol_char <= 'Z') || 
						(symbol_char >= 'a' && symbol_char <= 'z'))
				{
					stop_index++ ; 
					if (stop_index == formula_length)
						break ; 
					symbol_char = formula[stop_index] ; 
				}

				strncpy(atomic_symbol, &formula[start_index], stop_index - start_index) ; 
				atomic_symbol[stop_index-start_index] = '\0' ; 
				index = stop_index ; 
				while(index < formula_length && (formula[index] == ' ' || formula[index] == '\t'))
				{
					index++ ; 
				}
				double count = 0 ; 
				if (index == formula_length)
				{
					// assume that the last symbol had a 1. 
					count = 1 ; 
				}
				start_index = index ; 
				stop_index = start_index ; 
				symbol_char = formula[stop_index] ; 
				bool decimal_found = false ; 
				while((symbol_char >= '0' && symbol_char <= '9') || symbol_char == '.')
				{
					if (symbol_char == '.')
					{
						if(decimal_found == true)
						{
							// theres an error. two decimals.
							char error_str[1024] ; 
							strcpy(error_str, "Molecular Formula specified was incorrect at position: ") ; 
							_itoa(stop_index+1, error_str + stop_index, 10) ; 
							strcat(error_str, ". Two decimal points present") ; 
							throw MolecularFormulaError(error_str, "Line 100 in MolecularFormula.cpp") ; 
						}
						else
						{
							decimal_found = true ; 
						}
					}
					stop_index++ ; 
					if (stop_index == formula_length)
						break ; 
					symbol_char = formula[stop_index] ; 
				}
				if (start_index == stop_index)
				{
					count = 1 ; 
				}
				else
				{
					count = atof(&formula[start_index]) ; 
				}
				// now we should have a symbol and a count. Lets get the atomicity of the atom.
				int element_index = atomic_information.GetElementIndex(atomic_symbol) ; 
				if (element_index == -1)
				{
					// theres an error. two decimals.
					char error_str[1024] ; 
					strcpy(error_str, "Molecular Formula specified was incorrect. Symbol in formula was not recognize from elements provided: ") ; 
					strcat(error_str, atomic_symbol) ; 
					throw MolecularFormulaError(error_str, "Line 131 in MolecularFormula.cpp") ; 
				}
				current_atom.mint_index = element_index ; 
				current_atom.mdbl_num_copies = count ; 
				mvect_elemental_composition.push_back(current_atom) ; 
				index = stop_index ; 
				mdbl_total_atom_count += count ; 
				mdbl_monoisotopic_mass += atomic_information.mvect_elemental_isotopes[element_index].marr_isotope_mass[0] * count ;
				mdbl_average_mass += atomic_information.mvect_elemental_isotopes[element_index].mdbl_average_mass * count ;
			}
			mint_num_elements = mvect_elemental_composition.size() ; 
		}

		MolecularFormula::~MolecularFormula()
		{
		}

		void MolecularFormula::AddAtomicCount(AtomicCount &cnt, double mono_mass, double avg_mass)
		{
			int num_elements = (int) mvect_elemental_composition.size() ;
			for (int elem_num = 0 ; elem_num < num_elements ; elem_num++)
			{
				if (mvect_elemental_composition[elem_num].mint_index == cnt.mint_index)
				{
					mvect_elemental_composition[elem_num].mdbl_num_copies += cnt.mdbl_num_copies ; 
					mdbl_monoisotopic_mass += mono_mass ; 
					mdbl_average_mass += avg_mass ; 
					mdbl_total_atom_count += cnt.mdbl_num_copies ; 
					return ; 
				}
			}
			mvect_elemental_composition.push_back(cnt) ; 
			mdbl_monoisotopic_mass += mono_mass ; 
			mdbl_average_mass += avg_mass ; 
			mdbl_total_atom_count += cnt.mdbl_num_copies ; 
			mint_num_elements++ ; 
		}
	}
}