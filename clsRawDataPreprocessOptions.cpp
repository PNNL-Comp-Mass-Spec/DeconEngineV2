// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "clsRawDataPreProcessOptions.h"

using namespace System ; 
using namespace System::Xml ; 

namespace DeconToolsV2
{
 namespace Readers
	{
		void clsRawDataPreprocessOptions::SaveV1FTICRPreProcessOptions(System::Xml::XmlTextWriter ^xwriter)
		{
			xwriter->WriteStartElement("FTICRRawPreProcessingOptions") ; 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("ApodizationType",this->ApodizationType.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("ApodizationMinX", Convert::ToString(this->ApodizationMinX)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("ApodizationMaxX", Convert::ToString(this->ApodizationMaxX)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("ApodizationPercent", Convert::ToString(this->ApodizationPercent)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("NumZeroFills", Convert::ToString(this->NumZeroFills)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteStartElement("Calibration"); 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("ApplyCalibration", Convert::ToString(this->ApplyCalibration())); 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("CalibrationType", this->CalibrationType.ToString()); 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("A", Convert::ToString(this->A)); 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("B", Convert::ToString(this->B)); 
			xwriter->WriteWhitespace("\n\t\t\t") ; 
			xwriter->WriteElementString("C", Convert::ToString(this->C)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteEndElement();
			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteEndElement();
			xwriter->WriteWhitespace("\n\t") ; 
		}

		Object^ clsRawDataPreprocessOptions::Clone()
		{
			clsRawDataPreprocessOptions ^new_params = gcnew clsRawDataPreprocessOptions() ; 
			new_params->ApodizationMaxX = this->ApodizationMaxX;
			new_params->ApodizationMinX = this->ApodizationMinX;
            new_params->ApodizationPercent = this->ApodizationPercent;
			new_params->ApodizationType = this->ApodizationType;
            new_params->NumZeroFills = this->NumZeroFills;
			new_params->CalibrationType = this->CalibrationType;
			new_params->A = this->A;
			new_params->B = this->B;
			new_params->C = this->C;
			return new_params ; 
		}


		void clsRawDataPreprocessOptions::LoadV1FTICRPreProcessOptions(System::Xml::XmlReader ^rdr)
		{
			//Add code to handle empty nodes.
			bool readingCalibrations ; 
			//Read each node in the tree.
			while (rdr->Read())
			{

				switch (rdr->NodeType)
				{
					case System::Xml::XmlNodeType::Element:
						if (rdr->Name->Equals("ApodizationType"))
						{
							if (rdr->IsEmptyElement)
							{
                                this->ApodizationType = Readers::ApodizationType::NOAPODIZATION;
								continue ; 
							}

							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew System::Exception ("Missing information for ApodizationType in parameter file") ; 
							}
							if (rdr->Value->Equals(DeconToolsV2::Readers::ApodizationType::HANNING.ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::HANNING ;
							}
							else if (rdr->Value->Equals(DeconToolsV2::Readers::ApodizationType::PARZEN.ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::PARZEN ;
							}
							else if (rdr->Value->Equals(DeconToolsV2::Readers::ApodizationType::SQUARE.ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::SQUARE ;
							}
							else if (rdr->Value->Equals(DeconToolsV2::Readers::ApodizationType::TRIANGLE.ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::TRIANGLE ;
							}
							else if (rdr->Value->Equals(DeconToolsV2::Readers::ApodizationType::WELCH.ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::WELCH ;
							}
							else
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::NOAPODIZATION ;
							}
						}
						else if (rdr->Name->Equals("ApodizationMinX"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No Apodization Min X value was specified in parameter file") ; 
							}
							else
							{
                                this->ApodizationMinX = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("ApodizationMaxX"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No Apodization Max X value was specified in parameter file") ; 
							}
							else
							{
                                this->ApodizationMaxX = Convert::ToDouble(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("ApodizationPercent"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No ApodizationPercent value was specified in parameter file") ; 
							}
							else
							{
                                this->ApodizationPercent = Convert::ToInt32(rdr->Value);
							}
						}
						else if (rdr->Name->Equals("NumZeroFills"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No NumZeroFills value was specified in parameter file") ; 
							}
							else
							{
                                this->NumZeroFills = Convert::ToInt16(rdr->Value);
							}
						}
					else if (rdr->Name->Equals("A"))
						{
							if (!readingCalibrations)
							{
								throw gcnew Exception ("Node A, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No value for calibration constant Awas specified in parameter file") ; 
							}
							else
							{
								this->A = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("B"))
						{
							if (!readingCalibrations)
							{
								throw gcnew Exception ("Node B, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No value for calibration constant B was specified in parameter file") ; 
							}
							else
							{
								this->B = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("C"))
						{
							if (!readingCalibrations)
							{
								throw gcnew Exception ("Node C, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew Exception ("No value for calibration constant C was specified in parameter file") ; 
							}
							else
							{
								this->C = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals("CalibrationType"))
						{
							if (!readingCalibrations)
							{
								throw gcnew Exception ("Node CalibrationType, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							System::Type ^type = DeconToolsV2::Readers::CalibrationType::typeid ; 
							//System::Type::GetType("DeconToolsV2::Readers::CalibrationType") ; 
							DeconToolsV2::Readers::CalibrationType calibrationType = *dynamic_cast<DeconToolsV2::Readers::CalibrationType^>(Enum::Parse(type, rdr->Value));
							this->CalibrationType = calibrationType ; 
						}
						else if (rdr->Name->Equals("Calibration"))
						{
							readingCalibrations = true ; 
						}
						break ; 
					case System::Xml::XmlNodeType::EndElement:
						if (rdr->Name->Equals("HornTransformParameters"))
							return ;
						if (!rdr->Name->Equals("Calibration"))
							break ;
						else
						{
							// calibration read. 
							readingCalibrations = false ; 
						}
					default:
						break ; 
				}
			}
		}

	}
}