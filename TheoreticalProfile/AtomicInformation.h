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
#include <vector>


namespace Engine
{
	namespace TheoreticalProfile
	{
		const int MAX_TAG_LEN = 256 ; 

		const int num_elements = 103 ; 
		const double ELECTRON_MASS = 0.00054858;
		
		class  ElementalIsotopes
		{
		public:
			//! Elemental symbol
			char   marr_symbol[3];
			char   marr_name[32];
			int mint_num_isotopes ; 
			int mint_atomicity;
			
			// Tin has 10 isotopes!!! Rather deal with extra fragmented 
			// memory than put separate allocations on heap each time.
			// especially because the number of elements is not changing any time
			// soon till the aliens take over.
			double marr_isotope_mass[10] ;
			double marr_isotope_prob[10] ; 
			double mdbl_average_mass ; 
			double mdbl_mass_variance ;

			ElementalIsotopes();
			ElementalIsotopes(const ElementalIsotopes &a);
			ElementalIsotopes& operator=(const ElementalIsotopes &a);
			~ElementalIsotopes(); 
		};

		class  AtomicInformation
		{
			void SetDefaultIsotopeDistribution() ; 
		public:
			AtomicInformation& operator=(const AtomicInformation &a) ; 
	#pragma warning(disable: 4251)
			std::vector<ElementalIsotopes> mvect_elemental_isotopes ; 
	#pragma warning(default: 4251)
			AtomicInformation();
			// Copy the given atomic information into a new copy
			AtomicInformation(AtomicInformation &a);
			~AtomicInformation();
			void LoadData(const char *file_name);
			void LoadData( ) ;
			int GetNumElements() ; 
			void WriteData(const char *file_name);			
			int GetElementIndex(char *symbol) ; 

		} ;
	}
}