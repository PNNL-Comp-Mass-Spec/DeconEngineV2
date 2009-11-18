// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "Averagine.h" 
#include <fstream>

namespace Engine
{
	namespace TheoreticalProfile
	{
		const char *DEFAULT_PROTEIN_AVERAGINE_FORMULA = "C4.9384 H7.7583 N1.3577 O1.4773 S0.0417" ; 
		const char *DEFAULT_RNA_AVERAGINE_FORMULA = "C3.9 H4.9 N1.5 O2.4 P0.4" ; 
		const char *DEFAULT_DNA_AVERAGINE_FORMULA = "C3.8 H4.7 N1.5 O2.8 P0.4" ; 

		Averagine::Averagine()
		{
			mbln_use_tag = false ; 
			mdbl_averagine_mass = 0 ; 
			mdbl_tag_mass = 0 ; 
		}
		Averagine::~Averagine()
		{
		}

		Averagine& Averagine::operator=(const Averagine& a)
		{
			mbln_use_tag = a.mbln_use_tag ; 
			mdbl_averagine_mass = a.mdbl_averagine_mass ; 
			mdbl_tag_mass = a.mdbl_tag_mass ; 
			mobj_averagine_formula = a.mobj_averagine_formula ;
			mobj_tag_formula = a.mobj_tag_formula ; 
			return *this ; 
		}

		void Averagine::GetAverageFormulaForMass(double MW, MolecularFormula &empirical_formula)
		{
			// still need to add tag mass. 
			if (mbln_use_tag && MW > mdbl_tag_mass)
			{
				MW -= mdbl_tag_mass ; 
			}

			int numElements = mobj_averagine_formula.mint_num_elements ; 
			double totalElementCount = mobj_averagine_formula.mdbl_total_atom_count ; 
			empirical_formula.mvect_elemental_composition.clear() ; 

			double averageMass = 0 ; 
			double monoMass = 0 ; 
			int hydrogenIndex = -1 ; 
			double totalAtomCount = 0 ; 

			for (int elementNum = 0 ; elementNum < numElements ; elementNum++)
			{
				AtomicCount elementCount = mobj_averagine_formula.mvect_elemental_composition[elementNum] ; 
				int numAtoms = (int)((MW/mdbl_averagine_mass) * elementCount.mdbl_num_copies + 0.5)  ; 
				if (_stricmp(mobj_element_isotope_abundance.mvect_elemental_isotopes[elementCount.mint_index].marr_symbol, "H") ==0)
				{
					// Hydrogen is used as the remainder mass, so we skip it in the formula calculation by assigning it to 0
					// for now and later below assigning the remainder to it.
					hydrogenIndex = elementNum ; 
					numAtoms = 0 ; 
				}
				else
				{
					double elementAvgMass = mobj_element_isotope_abundance.mvect_elemental_isotopes[elementCount.mint_index].mdbl_average_mass ;
					double elementMonoMass = mobj_element_isotope_abundance.mvect_elemental_isotopes[elementCount.mint_index].marr_isotope_mass[0] ;
					averageMass += elementAvgMass * numAtoms ;
					monoMass += elementMonoMass * numAtoms ;
				}
				totalAtomCount += numAtoms ;
				elementCount.mdbl_num_copies = numAtoms ; 
				empirical_formula.mvect_elemental_composition.push_back(elementCount) ; 
			}

			// if tag mass is enabled, add it to the formula.
			if (mbln_use_tag)
			{
				int numElementsTag = mobj_tag_formula.mint_num_elements ; 
				double totalElementCountTag = mobj_tag_formula.mdbl_total_atom_count ; 
				for (int elementNum = 0 ; elementNum < numElementsTag ; elementNum++)
				{
					AtomicCount elementCount = mobj_tag_formula.mvect_elemental_composition[elementNum] ; 
					int numAtoms = (int) elementCount.mdbl_num_copies  ; 
					double elementAvgMass = mobj_element_isotope_abundance.mvect_elemental_isotopes[elementCount.mint_index].mdbl_average_mass ;
					double elementMonoMass = mobj_element_isotope_abundance.mvect_elemental_isotopes[elementCount.mint_index].marr_isotope_mass[0] ;
					averageMass += elementAvgMass * numAtoms ;
					monoMass += elementMonoMass * numAtoms ;
					totalAtomCount += numAtoms ;
					elementCount.mdbl_num_copies = numAtoms ; 
					// now go through each elementCount. If already added, then just add to its count.
					// otherwise, create new. 
					int elementNumAlreadyPushed = -1 ; 
					for (int elementNumPushed = 0 ; elementNumPushed < (int) empirical_formula.mvect_elemental_composition.size() ; 
						elementNumPushed++)
					{
						if (empirical_formula.mvect_elemental_composition[elementNumPushed].mint_index == elementCount.mint_index)
						{
							elementNumAlreadyPushed = elementNumPushed ; 
							break ; 
						}
					}
					if (elementNumAlreadyPushed == -1)
						empirical_formula.mvect_elemental_composition.push_back(elementCount) ; 
					else
						empirical_formula.mvect_elemental_composition[elementNumAlreadyPushed].mdbl_num_copies += elementCount.mdbl_num_copies ; 
				}
			}

			// now whatever's left over in mass, is assigned to hydrogen because it is not expected to cause 
			// much of a distortion in the isotope profile.
			double remainderMass = MW - averageMass ; 
			if (mbln_use_tag)
				remainderMass += mdbl_tag_mass ; 
			int numHydrogens = (int) (remainderMass/mobj_element_isotope_abundance.mvect_elemental_isotopes[mint_hydrogen_element_index].mdbl_average_mass +0.5) ; 

			averageMass += numHydrogens * mobj_element_isotope_abundance.mvect_elemental_isotopes[mint_hydrogen_element_index].mdbl_average_mass  ;
			monoMass += numHydrogens * mobj_element_isotope_abundance.mvect_elemental_isotopes[mint_hydrogen_element_index].marr_isotope_mass[0] ;
			totalAtomCount += numHydrogens ;

			if (numHydrogens > 0)
			{
				if (hydrogenIndex != -1)
				{
					empirical_formula.mvect_elemental_composition[hydrogenIndex].mdbl_num_copies += numHydrogens ; 
				}
				else
				{
					AtomicCount hydrogenCount ; 
					hydrogenCount.mint_index = mint_hydrogen_element_index ; 
					hydrogenCount.mdbl_num_copies = numHydrogens ; 
					empirical_formula.mvect_elemental_composition.push_back(hydrogenCount) ; 
				}
			}
			empirical_formula.mint_num_elements = empirical_formula.mvect_elemental_composition.size() ; 
			empirical_formula.mdbl_monoisotopic_mass = monoMass ; 
			empirical_formula.mdbl_average_mass = averageMass ;
			empirical_formula.mdbl_total_atom_count = totalAtomCount ; 
		}

		void Averagine::GetAveragineFormula(std::string &averagine_formula)
		{
			averagine_formula = mobj_averagine_formula.mstr_formula ; 
		}

		void Averagine::GetTagFormula(std::string &tag_formula)
		{
			tag_formula = mobj_tag_formula.mstr_formula ; 
		}

		void Averagine::SetAveragineFormula(std::string averagine_formula)
		{
			mobj_averagine_formula.SetMolecularFormula(averagine_formula, mobj_element_isotope_abundance) ; 
			mdbl_averagine_mass = mobj_averagine_formula.mdbl_monoisotopic_mass ; 
		}

		void Averagine::SetTagFormula(std::string tag_formula)
		{
			mbln_use_tag = false ; 
			mdbl_tag_mass = 0 ; 

			if (tag_formula.length() != 0)
			{
				mbln_use_tag = true ; 
				mobj_tag_formula.SetMolecularFormula(tag_formula, mobj_element_isotope_abundance) ; 
				mdbl_tag_mass = mobj_tag_formula.mdbl_average_mass ; 
			}
		}

		void Averagine::SetElementalIsotopeComposition(const AtomicInformation &element_isotopes)
		{
			mobj_element_isotope_abundance = element_isotopes ; 
			mint_hydrogen_element_index = mobj_element_isotope_abundance.GetElementIndex("H") ; 
			if (!mobj_averagine_formula.IsAssigned())
			{
				SetAveragineFormula(DEFAULT_PROTEIN_AVERAGINE_FORMULA) ; 
			}
		}

	}
}
