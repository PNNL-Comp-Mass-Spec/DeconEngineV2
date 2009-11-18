// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\clsdtagenerationparameters.h"


namespace DeconToolsV2
{
	namespace DTAGeneration
	{
		
		clsDTAGenerationParameters::clsDTAGenerationParameters(void)
		{
			mint_min_ion_count = 35;
			mint_min_scan = 1 ; 
			mint_max_scan = 1000000;
			mint_consider_charge = 0 ; 			
			mint_window_size_to_check = 5 ; 			
			mdbl_min_mass = 200 ;
			mdbl_max_mass = 5000;			
			mdbl_cc_mass = 1.00727638;							
			menm_output_type = OUTPUT_TYPE::DTA; 		
			mstr_svm_file_name = "svm_params.xml" ; 
			mbln_consider_multiple_precursors = false ; 
			mint_isolation_window_size = 3 ; 
			mbln_is_profile_data_for_mzXML = false ; 
			mbln_ignore_msn_scans = false ; 
			mint_num_msn_levels_to_ignore = 0 ;			
			mvect_msn_levels_to_ignore = new std::vector<int> ; 
		}		

		clsDTAGenerationParameters::~clsDTAGenerationParameters(void)
		{
		}

	/*	void clsDTAGenerationParameters::GetMSnLevelToIgnore(int (&msnLevelsToIgnore)__gc[])
		{
			int numLevels = this->get_NumMSnLevelsToIgnore() ; 
			msnLevelsToIgnore = new int __gc[numLevels] ; 

			for (int levelNum = 0 ; levelNum < numLevels ; levelNum++) 
			{
				int level = (*mvect_msn_levels_to_ignore)[levelNum] ; 
				msnLevelsToIgnore[levelNum] = level ; 
			}
		}*/

		void clsDTAGenerationParameters::SaveV1DTAGenerationParameters(System::Xml::XmlTextWriter *xwriter)
		{
			xwriter->WriteStartElement(S"DTAGenerationParameters");
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MinScan", System::Convert::ToString(this->MinScan)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MaxScan", System::Convert::ToString(this->MaxScan)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MinMass", System::Convert::ToString(this->MinMass)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"MaxMass", System::Convert::ToString(this->MaxMass)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"MinIonCount", System::Convert::ToString(this->MinIonCount)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			if (this->ConsiderChargeValue > 0)
			{
				xwriter->WriteElementString(S"ConsiderCharge", System::Convert::ToString(true)) ; 
				xwriter->WriteWhitespace(S"\n\t\t") ; 
			}
			else
			{
				xwriter->WriteElementString(S"ConsiderCharge", System::Convert::ToString(false)) ; 
				xwriter->WriteWhitespace(S"\n\t\t") ; 			
			}
			xwriter->WriteElementString(S"ChargeValueToConsider", System::Convert::ToString(this->ConsiderChargeValue)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"CCMass", System::Convert::ToString(this->CCMass)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 		

			xwriter->WriteElementString(S"WindowSizeToCheck", System::Convert::ToString(this->WindowSizetoCheck)) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			
			xwriter->WriteElementString(S"ConsiderMultiplePrecursors", System::Convert::ToString(this->ConsiderMultiplePrecursors)) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"IsolationWindowSize", System::Convert::ToString(this->IsolationWindowSize)) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"OutputType", __box(this->OutputType)->ToString()); 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"IsProfileDataForMzXML", System::Convert::ToString(this->IsProfileDataForMzXML)) ; 
			xwriter->WriteWhitespace(S"\n\t") ; 

			xwriter->WriteElementString(S"IgnoreMSnScans", System::Convert::ToString(this->IgnoreMSnScans)); 
			xwriter->WriteWhitespace(S"\n\t") ; 
			
			if(this->IgnoreMSnScans)
			{
				int numLevels = this->NumMSnLevelsToIgnore ; 
				for (int levelNum = 0 ; levelNum < numLevels ; levelNum++)
				{
					int level = (*mvect_msn_levels_to_ignore)[levelNum] ; 
					xwriter->WriteElementString(S"MSnLevelToIgnore", System::Convert::ToString(level)); 
					xwriter->WriteWhitespace(S"\n\t") ; 
				}
			}			
			xwriter->WriteEndElement();
			xwriter->WriteWhitespace(S"\n\t") ; 		
		}

		void clsDTAGenerationParameters::LoadV1DTAGenerationParameters(XmlReader *rdr)
		{

			while(rdr->Read())
			{
				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals(S"MinMass"))
						{
							if (rdr->IsEmptyElement)
							{
							throw new System::Exception (S"No information for minimum mass in parameter file") ; 								
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_MinMass(System::Convert::ToDouble(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MaxMass"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for maximum mass in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_MaxMass(System::Convert::ToDouble(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MinScan"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for minimum scan in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_MinScan(System::Convert::ToInt32(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MaxScan"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for maximum scan in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_MaxScan(System::Convert::ToInt32(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MinIonCount"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for minimum ion count in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_MinIonCount(System::Convert::ToInt16(rdr->Value)) ; 
						}						
						else if (rdr->Name->Equals(S"ChargeValueToConsider"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for considering charge in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_ConsiderChargeValue(System::Convert::ToInt16(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"ConsiderMultiplePrecursors"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception(S"No information for considering multiple precursors") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_ConsiderMultiplePrecursors(System::Convert::ToBoolean(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"IsolationWindowSize"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception(S"No information for isolation window size") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_IsolationWindowSize(System::Convert::ToInt16(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"IsProfileDataForMzXML"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information mzXML profile data in parameter file") ; 																
							}
							rdr->Read(); 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_IsProfileDataForMzXML(System::Convert::ToBoolean(rdr->Value)) ; 
						}	
						else if (rdr->Name->Equals(S"IgnoreMSnScans"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information about ignoring MSn scans") ; 																
							}
							rdr->Read(); 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_IgnoreMSnScans(System::Convert::ToBoolean(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"MSnLevelToIgnore"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information about which MSn level to ignore") ; 																
							}
							rdr->Read(); 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							mint_num_msn_levels_to_ignore++ ; 
							int level  = System::Convert::ToInt16(rdr->Value) ; 
							(*mvect_msn_levels_to_ignore).push_back(level) ; 
						}
						else if (rdr->Name->Equals(S"CCMass"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for charge carrier mass in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
						this->set_CCMass(System::Convert::ToDouble(rdr->Value)) ; 
						}					
						else if (rdr->Name->Equals(S"WindowSizeToCheck"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for window size to check in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->set_WindowSizetoCheck(System::Convert::ToInt16(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"OutputType"))
						{
							if (rdr->IsEmptyElement)
							{
								throw new System::Exception (S"No information for output type in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->Value->Equals(__box(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::DTA)->ToString()))
							{
								this->set_OutputType(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::DTA)  ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::LOG)->ToString()))
							{
								this->set_OutputType(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::LOG)  ;								
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::CDTA)->ToString()))
							{
								this->set_OutputType(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::CDTA)  ;								
							} 
							else if (rdr->Value->Equals(__box(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)->ToString()))
							{
								this->set_OutputType(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF)  ;								
							} 
						}
						break;
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals(S"DTAGenerationParameters"))
							return ; 
						break ; 
					default:
						break ;
				}
			}
		}	
	}
}