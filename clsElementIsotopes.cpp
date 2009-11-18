// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "clsElementIsotopes.h"
#include "DeconEngineUtils.h"
using namespace System::Xml ;

namespace DeconToolsV2
{
	clsElementIsotopes::clsElementIsotopes()
	{
		mobjAtomicInfo = new Engine::TheoreticalProfile::AtomicInformation() ;
	}

	clsElementIsotopes::clsElementIsotopes(System::String *file_name)
	{
		mobjAtomicInfo = new Engine::TheoreticalProfile::AtomicInformation() ; 	
		try
		{
			Load(file_name) ; 
		}
		catch (char *mesg)
		{
			System::String *exception_msg = new System::String(mesg) ; 
			throw new System::Exception(exception_msg) ; 
		}
	}

	clsElementIsotopes::~clsElementIsotopes()
	{
		if (mobjAtomicInfo != NULL)
			delete mobjAtomicInfo ; 
		mobjAtomicInfo = NULL ; 
	}

	Object* clsElementIsotopes::Clone()
	{
		clsElementIsotopes *elem_isotopes = new clsElementIsotopes() ; 
		*(elem_isotopes->mobjAtomicInfo) = *this->GetElementalIsotopeComposition() ; 
		return elem_isotopes ; 
	}

	clsElementIsotopes* clsElementIsotopes::Assign(clsElementIsotopes* otherOne)
	{
		if (mobjAtomicInfo != NULL)
		{
			delete mobjAtomicInfo ; 
			mobjAtomicInfo = NULL ; 
		}
		mobjAtomicInfo = new Engine::TheoreticalProfile::AtomicInformation() ;
		*mobjAtomicInfo = *otherOne->GetElementalIsotopeComposition() ;
		return this ; 
	}

	void clsElementIsotopes::Load(System::String *file_name)
	{
		char file_name_ch[256];
		DeconEngine::Utils::GetStr(file_name, file_name_ch);
		mobjAtomicInfo->LoadData(file_name_ch);			
	}

	void clsElementIsotopes::Write(System::String *file_name)
	{
		char file_name_ch[256];			
		DeconEngine::Utils::GetStr(file_name, file_name_ch);
		mobjAtomicInfo->WriteData(file_name_ch);
		
	}

	int clsElementIsotopes::GetNumberOfElements()
	{
		int size = mobjAtomicInfo->mvect_elemental_isotopes.size();
		return size;
	}

	void clsElementIsotopes::GetElementalIsotope(int index, Int32& atomicity, Int32& num_isotopes, 
		System::String* (&element_name), System::String* (&element_symbol),
		float __gc& average_mass, float __gc& mass_variance, float (&isotope_mass) __gc[], 
		float (&isotope_prob) __gc[]) 
	{
		Engine::TheoreticalProfile::ElementalIsotopes element_isotopes = mobjAtomicInfo->mvect_elemental_isotopes[index];	

		atomicity =  element_isotopes.mint_atomicity;
		num_isotopes = element_isotopes.mint_num_isotopes ;

		element_name = new String(element_isotopes.marr_name) ;
		element_symbol = new  String(element_isotopes.marr_symbol);

		isotope_mass = new float __gc[num_isotopes];
		isotope_prob = new float __gc[num_isotopes];			

		for (int isotope_num = 0; isotope_num < num_isotopes ; isotope_num++)
		{
			isotope_mass[isotope_num] = (float)element_isotopes.marr_isotope_mass[isotope_num] ;
			isotope_prob[isotope_num] = (float)element_isotopes.marr_isotope_prob[isotope_num];
		}
		
		average_mass = (float)element_isotopes.mdbl_average_mass;
		mass_variance = (float)element_isotopes.mdbl_mass_variance;
	}		

	void clsElementIsotopes::UpdateElementalIsotope(int index, Int32& atomicity, Int32& isotope_num, 
		System::String* (&element_name), System::String* (&element_symbol), double __gc& isotope_mass, 
		double __gc &isotope_prob) 
	{
		
		Engine::TheoreticalProfile::ElementalIsotopes element_isotopes = mobjAtomicInfo->mvect_elemental_isotopes[index];	
		
		element_isotopes.mint_atomicity = atomicity;

		DeconEngine::Utils::GetStr(element_name, element_isotopes.marr_name);
		DeconEngine::Utils::GetStr(element_symbol, element_isotopes.marr_symbol);

		element_isotopes.marr_isotope_mass[isotope_num] =  isotope_mass;
		element_isotopes.marr_isotope_prob[isotope_num] =  isotope_prob;
		element_isotopes.mdbl_average_mass = 0 ; 
		for (int isoNum = 0 ; isoNum < element_isotopes.mint_num_isotopes ; isoNum++)
			element_isotopes.mdbl_average_mass += element_isotopes.marr_isotope_mass[isoNum] * element_isotopes.marr_isotope_prob[isoNum] ; 

		mobjAtomicInfo->mvect_elemental_isotopes[index] = element_isotopes;
	}

	void clsElementIsotopes::SetElementalIsotopeComposition(Engine::TheoreticalProfile::AtomicInformation 
	__nogc *atomic_info)
	{
		if (mobjAtomicInfo != NULL)
		{
			delete mobjAtomicInfo ; 
			mobjAtomicInfo = NULL ; 
		}
		mobjAtomicInfo = atomic_info ; 
	}

	const Engine::TheoreticalProfile::AtomicInformation* clsElementIsotopes::GetElementalIsotopeComposition()
	{
		return mobjAtomicInfo ; 
	}

	void clsElementIsotopes::SaveV1ElementIsotopes(System::Xml::XmlTextWriter *xwriter)
	{
		System::String *elementSymbol = S"" ;
		System::String *elementName = S"";
		int atomicity = 0 ;
		int numIsotopes = 0 ;
		float isotope_mass __gc [] = new float __gc [1];
		float isotope_probability __gc [] = new float __gc [1];
		float average_mass = 0 ;
		float mass_variance = 0 ;

		int numElements = GetNumberOfElements();			

		xwriter->WriteStartElement("ElementIsotopes");
		xwriter->WriteWhitespace("\n\t\t") ; 
		xwriter->WriteElementString("NumElements",numElements.ToString()) ; 

		for (int element_num = 0; element_num<numElements; element_num++)
		{

			GetElementalIsotope(element_num, atomicity, numIsotopes, elementName, elementSymbol,
				average_mass, mass_variance, isotope_mass, isotope_probability );

			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteStartElement("Element");
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("Symbol",elementSymbol) ; 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("Name",elementName) ; 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("NumIsotopes",numIsotopes.ToString()) ; 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("Atomicity",atomicity.ToString()) ; 
			
			for (int isotope_num=0; isotope_num<numIsotopes; isotope_num++)
			{
				xwriter->WriteWhitespace("\n\t\t\t") ; 
				xwriter->WriteStartElement("Isotope");
				xwriter->WriteWhitespace("\n\t\t\t\t") ; 
				xwriter->WriteElementString("Mass", isotope_mass[isotope_num].ToString()) ;
				xwriter->WriteWhitespace("\n\t\t\t\t") ; 
				xwriter->WriteElementString("Probability", isotope_probability[isotope_num].ToString());					
				xwriter->WriteWhitespace("\n\t\t\t") ; 
				xwriter->WriteEndElement() ; 
			}
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteEndElement() ; 
		}
		xwriter->WriteWhitespace("\n\t") ; 
		xwriter->WriteEndElement() ; 
		xwriter->WriteWhitespace("\n") ; 
	}

	void clsElementIsotopes::LoadV1ElementIsotopes(System::Xml::XmlReader *rdr)
	{
		int num_elements = 0, num_isotopes = 0, atomicity = 0, isotope_num = 0, element_num = 0 ; 
		double isotope_mass = 0 , isotope_probability = 0 ; 
		System::String *symbol, *name ; 
		//Read each node in the tree.
		while (rdr->Read())
		{
			switch (rdr->NodeType)
			{
				case XmlNodeType::Element:
					if (rdr->Name->Equals(S"NumElements"))
					{
						rdr->Read() ; 
						while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
						{
							rdr->Read() ; 
						}
						if (rdr->NodeType != XmlNodeType::Text)
						{
							throw new System::Exception (S"Missing information for number of elements in parameter file") ; 
						}
						num_elements = System::Convert::ToInt32(rdr->Value) ; 
						isotope_num = 0 ; 
					}
					else if (rdr->Name->Equals(S"Element"))
					{
						num_isotopes = 0 ;
						isotope_num = 0 ; 
					}
					else if (rdr->Name->Equals(S"NumIsotopes"))
					{
						rdr->Read() ; 
						while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
						{
							rdr->Read() ; 
						}
						if (rdr->NodeType != XmlNodeType::Text)
						{
							throw new System::Exception (S"Missing information for NumIsotopes in parameter file") ; 
						}
						num_isotopes = System::Convert::ToInt32(rdr->Value) ; 
					}
					else if (rdr->Name->Equals(S"Atomicity"))
					{
						rdr->Read() ; 
						while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
						{
							rdr->Read() ; 
						}
						if (rdr->NodeType != XmlNodeType::Text)
						{
							throw new System::Exception (S"Missing information for Atomicity in parameter file") ; 
						}
						atomicity = System::Convert::ToInt32(rdr->Value) ; 
					}
					else if (rdr->Name->Equals(S"Symbol"))
					{
						rdr->Read() ; 
						while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
						{
							rdr->Read() ; 
						}
						if (rdr->NodeType != XmlNodeType::Text)
						{
							throw new System::Exception (S"Missing information for Symbol in parameter file") ; 
						}
						symbol = rdr->Value ; 
					}
					else if (rdr->Name->Equals(S"Name"))
					{
						rdr->Read() ; 
						while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
						{
							rdr->Read() ; 
						}
						if (rdr->NodeType != XmlNodeType::Text)
						{
							throw new System::Exception (S"Missing information for Element Name in parameter file") ; 
						}
						name = rdr->Value ; 
					}
					else if (rdr->Name->Equals(S"Isotope"))
					{
						isotope_mass = 0 ; 
						isotope_probability = 0 ; 
					}
					else if (rdr->Name->Equals(S"Mass"))
					{
						rdr->Read() ; 
						while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
						{
							rdr->Read() ; 
						}
						if (rdr->NodeType != XmlNodeType::Text)
						{
							throw new System::Exception (S"Missing information for Mass in parameter file") ; 
						}
						isotope_mass = System::Convert::ToSingle(rdr->Value) ; 
					}
					else if (rdr->Name->Equals(S"Probability"))
					{
						rdr->Read() ; 
						while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
						{
							rdr->Read() ; 
						}
						if (rdr->NodeType != XmlNodeType::Text)
						{
							throw new System::Exception (S"Missing information for Probability in parameter file") ; 
						}
						isotope_probability = System::Convert::ToSingle(rdr->Value) ; 
					}
					break ; 
				case XmlNodeType::EndElement:
					if (rdr->Name->Equals(S"Isotope"))
					{
						UpdateElementalIsotope(element_num, atomicity, isotope_num, name, symbol, isotope_mass, isotope_probability) ; 
						isotope_num++ ; 
					}
					else if (rdr->Name->Equals(S"Element"))
					{
						/*Console::Write(System::Convert::ToString(element_num)) ; 
						Console::WriteLine(S" ") ; 
						Console::WriteLine(name) ; */
						element_num++ ;
					}
					else if (rdr->Name->Equals(S"ElementIsotopes"))
					{
						return ; 
					}
					break ; 
				default:
					break ; 
			}
		}
	}

}
