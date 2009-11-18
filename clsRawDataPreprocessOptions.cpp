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
#using <mscorlib.dll>

using namespace System ; 
using namespace System::Xml ; 

namespace DeconToolsV2
{
 namespace Readers
	{
		void clsRawDataPreprocessOptions::SaveV1FTICRPreProcessOptions(System::Xml::XmlTextWriter *xwriter)
		{
			xwriter->WriteStartElement(S"FTICRRawPreProcessingOptions") ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"ApodizationType",__box(this->ApodizationType)->ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"ApodizationMinX", Convert::ToString(this->ApodizationMinX)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"ApodizationMaxX", Convert::ToString(this->ApodizationMaxX)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"ApodizationPercent", Convert::ToString(this->ApodizationPercent)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"NumZeroFills", Convert::ToString(this->NumZeroFills)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteStartElement(S"Calibration"); 
			xwriter->WriteWhitespace(S"\n\t\t\t") ; 
			xwriter->WriteElementString(S"ApplyCalibration", Convert::ToString(this->ApplyCalibration)); 
			xwriter->WriteWhitespace(S"\n\t\t\t") ; 
			xwriter->WriteElementString(S"CalibrationType", __box(this->CalibrationType)->ToString()); 
			xwriter->WriteWhitespace(S"\n\t\t\t") ; 
			xwriter->WriteElementString(S"A", Convert::ToString(this->A)); 
			xwriter->WriteWhitespace(S"\n\t\t\t") ; 
			xwriter->WriteElementString(S"B", Convert::ToString(this->B)); 
			xwriter->WriteWhitespace(S"\n\t\t\t") ; 
			xwriter->WriteElementString(S"C", Convert::ToString(this->C)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteEndElement();
			xwriter->WriteWhitespace(S"\n\t") ; 
			xwriter->WriteEndElement();
			xwriter->WriteWhitespace(S"\n\t") ; 
		}

		Object* clsRawDataPreprocessOptions::Clone()
		{
			clsRawDataPreprocessOptions *new_params = new clsRawDataPreprocessOptions() ; 
			new_params->set_ApodizationMaxX(this->get_ApodizationMaxX()) ; 
			new_params->set_ApodizationMinX(this->get_ApodizationMinX()) ; 
			new_params->set_ApodizationPercent(this->get_ApodizationPercent()) ; 
			new_params->set_ApodizationType(this->get_ApodizationType()) ; 
			new_params->set_NumZeroFills(this->get_NumZeroFills()) ; 
			new_params->set_CalibrationType(this->get_CalibrationType()) ; 
			new_params->set_A(this->get_A()) ; 
			new_params->set_B(this->get_B()) ; 
			new_params->set_C(this->get_C()) ; 
			return new_params ; 
		}


		void clsRawDataPreprocessOptions::LoadV1FTICRPreProcessOptions(System::Xml::XmlReader *rdr)
		{
			//Add code to handle empty nodes.
			bool readingCalibrations ; 
			//Read each node in the tree.
			while (rdr->Read())
			{

				switch (rdr->NodeType)
				{
					case System::Xml::XmlNodeType::Element:
						if (rdr->Name->Equals(S"ApodizationType"))
						{
							if (rdr->IsEmptyElement)
							{
								this->set_ApodizationType(NOAPODIZATION) ; 
								continue ; 
							}

							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new System::Exception (S"Missing information for ApodizationType in parameter file") ; 
							}
							if (rdr->Value->Equals(__box(DeconToolsV2::Readers::ApodizationType::HANNING)->ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::HANNING ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::Readers::ApodizationType::PARZEN)->ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::PARZEN ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::Readers::ApodizationType::SQUARE)->ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::SQUARE ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::Readers::ApodizationType::TRIANGLE)->ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::TRIANGLE ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::Readers::ApodizationType::WELCH)->ToString()))
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::WELCH ;
							}
							else
							{
								this->ApodizationType = DeconToolsV2::Readers::ApodizationType::NOAPODIZATION ;
							}
						}
						else if (rdr->Name->Equals(S"ApodizationMinX"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No Apodization Min X value was specified in parameter file") ; 
							}
							else
							{
								this->set_ApodizationMinX(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"ApodizationMaxX"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No Apodization Max X value was specified in parameter file") ; 
							}
							else
							{
								this->set_ApodizationMaxX(Convert::ToDouble(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"ApodizationPercent"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No ApodizationPercent value was specified in parameter file") ; 
							}
							else
							{
								this->set_ApodizationPercent(Convert::ToInt32(rdr->Value)) ; 
							}
						}
						else if (rdr->Name->Equals(S"NumZeroFills"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No NumZeroFills value was specified in parameter file") ; 
							}
							else
							{
								this->set_NumZeroFills(Convert::ToInt16(rdr->Value)) ; 
							}
						}
					else if (rdr->Name->Equals(S"A"))
						{
							if (!readingCalibrations)
							{
								throw new Exception (S"Node A, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No value for calibration constant Awas specified in parameter file") ; 
							}
							else
							{
								this->A = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"B"))
						{
							if (!readingCalibrations)
							{
								throw new Exception (S"Node B, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No value for calibration constant B was specified in parameter file") ; 
							}
							else
							{
								this->B = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"C"))
						{
							if (!readingCalibrations)
							{
								throw new Exception (S"Node C, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new Exception (S"No value for calibration constant C was specified in parameter file") ; 
							}
							else
							{
								this->C = Convert::ToDouble(rdr->Value) ; 
							}
						}
						else if (rdr->Name->Equals(S"CalibrationType"))
						{
							if (!readingCalibrations)
							{
								throw new Exception (S"Node CalibrationType, needs to be inside node: Calibration.") ; 
							}
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}

							System::Type *type = __typeof(DeconToolsV2::Readers::CalibrationType) ; 
							//System::Type::GetType(S"DeconToolsV2::Readers::CalibrationType") ; 
							DeconToolsV2::Readers::CalibrationType calibrationType = *dynamic_cast<__box DeconToolsV2::Readers::CalibrationType*>(Enum::Parse(type, rdr->Value));
							this->CalibrationType = calibrationType ; 
						}
						else if (rdr->Name->Equals(S"Calibration"))
						{
							readingCalibrations = true ; 
						}
						break ; 
					case System::Xml::XmlNodeType::EndElement:
						if (rdr->Name->Equals(S"HornTransformParameters"))
							return ;
						if (!rdr->Name->Equals(S"Calibration"))
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