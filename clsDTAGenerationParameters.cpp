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
			MinIonCount = 35;
			MinScan = 1 ; 
			MaxScan = 1000000;
			ConsiderChargeValue = 0 ; 			
			WindowSizetoCheck = 5 ; 			
			MinMass = 200 ;
			MaxMass = 5000;			
			CCMass = 1.00727638;							
			OutputType = OUTPUT_TYPE::DTA; 		
			SVMParamFile = "svm_params.xml" ; 
			ConsiderMultiplePrecursors = false ; 
			IsolationWindowSize = 3 ; 
			IsProfileDataForMzXML = false ; 
			IgnoreMSnScans = false ; 
			NumMSnLevelsToIgnore = 0 ;			
			mvect_msn_levels_to_ignore = new std::vector<int> ; 
		}		

		clsDTAGenerationParameters::~clsDTAGenerationParameters(void)
		{
		}

	/*	void clsDTAGenerationParameters::GetMSnLevelToIgnore(int (&msnLevelsToIgnore)__gc[])
		{
			int numLevels = this->NumMSnLevelsToIgnore() ; 
			msnLevelsToIgnore = new int __gc[numLevels] ; 

			for (int levelNum = 0 ; levelNum < numLevels ; levelNum++) 
			{
				int level = (*mvect_msn_levels_to_ignore)[levelNum] ; 
				msnLevelsToIgnore[levelNum] = level ; 
			}
		}*/

		void clsDTAGenerationParameters::SaveV1DTAGenerationParameters(System::Xml::XmlTextWriter ^xwriter)
		{
			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteStartElement("DTAGenerationParameters");
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MinScan", System::Convert::ToString(this->MinScan)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MaxScan", System::Convert::ToString(this->MaxScan)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MinMass", System::Convert::ToString(this->MinMass)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("MaxMass", System::Convert::ToString(this->MaxMass)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("MinIonCount", System::Convert::ToString(this->MinIonCount)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			if (this->ConsiderChargeValue > 0)
			{
				xwriter->WriteElementString("ConsiderCharge", System::Convert::ToString(true)) ; 
				xwriter->WriteWhitespace("\n\t\t") ; 
			}
			else
			{
				xwriter->WriteElementString("ConsiderCharge", System::Convert::ToString(false)) ; 
				xwriter->WriteWhitespace("\n\t\t") ; 			
			}
			xwriter->WriteElementString("ChargeValueToConsider", System::Convert::ToString(this->ConsiderChargeValue)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("CCMass", System::Convert::ToString(this->CCMass)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 		

			xwriter->WriteElementString("WindowSizeToCheck", System::Convert::ToString(this->WindowSizetoCheck)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			
			xwriter->WriteElementString("ConsiderMultiplePrecursors", System::Convert::ToString(this->ConsiderMultiplePrecursors)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("IsolationWindowSize", System::Convert::ToString(this->IsolationWindowSize)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("OutputType", this->OutputType.ToString()); 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("IsProfileDataForMzXML", System::Convert::ToString(this->IsProfileDataForMzXML)) ; 
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("IgnoreMSnScans", System::Convert::ToString(this->IgnoreMSnScans)); 
			
			
			if(this->IgnoreMSnScans)
			{
				int numLevels = this->NumMSnLevelsToIgnore ; 
				for (int levelNum = 0 ; levelNum < numLevels ; levelNum++)
				{
					int level = (*mvect_msn_levels_to_ignore)[levelNum] ; 
					xwriter->WriteWhitespace("\n\t\t") ; 
					xwriter->WriteElementString("MSnLevelToIgnore", System::Convert::ToString(level)); 
					
				}
			}			
			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteEndElement();
					
		}

		void clsDTAGenerationParameters::LoadV1DTAGenerationParameters(XmlReader ^rdr)
		{

			while(rdr->Read())
			{
				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals("MinMass"))
						{
							if (rdr->IsEmptyElement)
							{
							throw gcnew System::Exception ("No information for minimum mass in parameter file") ; 								
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
                            this->MinMass = System::Convert::ToDouble(rdr->Value);
						}
						else if (rdr->Name->Equals("MaxMass"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for maximum mass in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
                            this->MaxMass = System::Convert::ToDouble(rdr->Value);
						}
						else if (rdr->Name->Equals("MinScan"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for minimum scan in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
                            this->MinScan = System::Convert::ToInt32(rdr->Value);
						}
						else if (rdr->Name->Equals("MaxScan"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for maximum scan in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
                            this->MaxScan = System::Convert::ToInt32(rdr->Value);
						}
						else if (rdr->Name->Equals("MinIonCount"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for minimum ion count in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->MinIonCount = System::Convert::ToInt16(rdr->Value);
						}						
						else if (rdr->Name->Equals("ChargeValueToConsider"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for considering charge in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->ConsiderChargeValue = System::Convert::ToInt16(rdr->Value);
						}
						else if (rdr->Name->Equals("ConsiderMultiplePrecursors"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception("No information for considering multiple precursors") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->ConsiderMultiplePrecursors = System::Convert::ToBoolean(rdr->Value);
						}
						else if (rdr->Name->Equals("IsolationWindowSize"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception("No information for isolation window size") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->IsolationWindowSize = System::Convert::ToInt16(rdr->Value);
						}
						else if (rdr->Name->Equals("IsProfileDataForMzXML"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information mzXML profile data in parameter file") ; 																
							}
							rdr->Read(); 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
                            this->IsProfileDataForMzXML = System::Convert::ToBoolean(rdr->Value);
						}	
						else if (rdr->Name->Equals("IgnoreMSnScans"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information about ignoring MSn scans") ; 																
							}
							rdr->Read(); 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
                            this->IgnoreMSnScans = System::Convert::ToBoolean(rdr->Value);
						}
						else if (rdr->Name->Equals("MSnLevelToIgnore"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information about which MSn level to ignore") ; 																
							}
							rdr->Read(); 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							NumMSnLevelsToIgnore++ ; 
							int level  = System::Convert::ToInt16(rdr->Value) ; 
							(*mvect_msn_levels_to_ignore).push_back(level) ; 
						}
						else if (rdr->Name->Equals("CCMass"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for charge carrier mass in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
						this->CCMass = System::Convert::ToDouble(rdr->Value) ; 
						}					
						else if (rdr->Name->Equals("WindowSizeToCheck"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for window size to check in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							this->WindowSizetoCheck = System::Convert::ToInt16(rdr->Value); 
						}
						else if (rdr->Name->Equals("OutputType"))
						{
							if (rdr->IsEmptyElement)
							{
								throw gcnew System::Exception ("No information for output type in parameter file") ; 																
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->Value->Equals(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::DTA.ToString()))
							{
								this->OutputType = DeconToolsV2::DTAGeneration::OUTPUT_TYPE::DTA;
							}
							else if (rdr->Value->Equals(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::LOG.ToString()))
							{
								this->OutputType = DeconToolsV2::DTAGeneration::OUTPUT_TYPE::LOG;
							}
							else if (rdr->Value->Equals(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::CDTA.ToString()))
							{
								this->OutputType = DeconToolsV2::DTAGeneration::OUTPUT_TYPE::CDTA;
							} 
							else if (rdr->Value->Equals(DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF.ToString()))
							{
								this->OutputType = DeconToolsV2::DTAGeneration::OUTPUT_TYPE::MGF;
							} 
						}
						break;
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals("DTAGenerationParameters"))
							return ; 
						break ; 
					default:
						break ;
				}
			}
		}	
	}
}