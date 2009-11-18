// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

// DeconEngine.h

#pragma once
#using <mscorlib.dll>
#using <System.Xml.dll>

using namespace System;
#include "TheoreticalProfile/AtomicInformation.h"


namespace DeconToolsV2
{
public __gc class clsElementIsotopes: public ICloneable

	{
	public:
		Engine::TheoreticalProfile::AtomicInformation __nogc *mobjAtomicInfo ; 
		clsElementIsotopes() ; 
		clsElementIsotopes(System::String *file_name) ; 

		virtual Object* Clone() ;
		virtual clsElementIsotopes* Assign(clsElementIsotopes* otherOne) ; 

		~clsElementIsotopes() ; 
		void Write(System::String *file_name);
		void Load(System::String *file_name);
		void GetElementalIsotope(int index, Int32& atomicity, Int32& num_isotopes, 
			System::String* (&element_name), System::String* (&element_symbol),
			float __gc& average_mass, float __gc& mass_variance, float (&isotope_mass) __gc[], 
			float (&isotope_prob) __gc[]) ; 
		void UpdateElementalIsotope(int index, Int32& atomicity, Int32& isotope_num, 
			System::String* (&element_name), System::String* (&element_symbol),
			double  __gc& isotope_mass, double __gc &isotope_prob) ;
		int GetNumberOfElements();
		void SetElementalIsotopeComposition(Engine::TheoreticalProfile::AtomicInformation 
			__nogc *atomic_info) ; 
		const Engine::TheoreticalProfile::AtomicInformation* GetElementalIsotopeComposition() ; 
		void LoadV1ElementIsotopes(System::Xml::XmlReader *rdr) ; 
		void SaveV1ElementIsotopes(System::Xml::XmlTextWriter *xwriter) ; 
	};	
}
