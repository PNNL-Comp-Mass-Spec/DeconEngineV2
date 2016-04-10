// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\clspeakprocessorparameters.h"
namespace DeconToolsV2
{
	namespace Peaks
	{
		clsPeakProcessorParameters::clsPeakProcessorParameters(void)
		{
			SignalToNoiseThreshold = 3.0 ; 
			PeakBackgroundRatio = 5.0 ; 
			PeakFitType = PEAK_FIT_TYPE::QUADRATIC ; 
		}

		clsPeakProcessorParameters::clsPeakProcessorParameters(double sn, 
			double peak_bg_ratio, bool thresholded_data, PEAK_FIT_TYPE fit_type)
		{
			SignalToNoiseThreshold = sn ; 
			PeakBackgroundRatio = peak_bg_ratio ; 
			PeakFitType = fit_type ; 
			ThresholdedData = thresholded_data ; 
		}

		void clsPeakProcessorParameters::SaveV1PeakParameters(System::Xml::XmlTextWriter ^xwriter)
		{
			xwriter->WriteWhitespace("\n\t") ;
			xwriter->WriteStartElement("PeakParameters");
			xwriter->WriteWhitespace("\n\t\t") ; 

			xwriter->WriteElementString("PeakBackgroundRatio", System::Convert::ToString(this->PeakBackgroundRatio)); 
			xwriter->WriteWhitespace("\n\t\t") ; 
            xwriter->WriteElementString("SignalToNoiseThreshold", System::Convert::ToString(this->SignalToNoiseThreshold));
			xwriter->WriteWhitespace("\n\t\t") ; 
			xwriter->WriteElementString("PeakFitType",this->PeakFitType.ToString()) ;
			xwriter->WriteWhitespace("\n\t\t") ; 
            xwriter->WriteElementString("WritePeaksToTextFile", System::Convert::ToString(this->WritePeaksToTextFile));
		
			xwriter->WriteWhitespace("\n\t") ; 
			xwriter->WriteEndElement();
			
		}

		void clsPeakProcessorParameters::LoadV1PeakParameters(XmlReader ^rdr)
		{

			//Read each node in the tree.
			while (rdr->Read())
			{
				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals("PeakBackgroundRatio"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew System::Exception ("Missing information for PeakBackgroundRatio in parameter file") ; 
							}
                            this->PeakBackgroundRatio = System::Convert::ToDouble(rdr->Value);
						}
						else if (rdr->Name->Equals("SignalToNoiseThreshold"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew System::Exception ("Missing information for SignalToNoiseThreshold in parameter file") ; 
							}
                            this->SignalToNoiseThreshold = System::Convert::ToDouble(rdr->Value);
						}
						else if (rdr->Name->Equals("PeakFitType"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew System::Exception ("Missing information for SignalToNoiseThreshold in parameter file") ; 
							}
							if (rdr->Value->Equals(DeconToolsV2::Peaks::PEAK_FIT_TYPE::QUADRATIC.ToString()))
							{
                                this->PeakFitType = DeconToolsV2::Peaks::PEAK_FIT_TYPE::QUADRATIC;
							}
							else if (rdr->Value->Equals(DeconToolsV2::Peaks::PEAK_FIT_TYPE::LORENTZIAN.ToString()))
							{
                                this->PeakFitType = DeconToolsV2::Peaks::PEAK_FIT_TYPE::LORENTZIAN;
							}
							else if (rdr->Value->Equals(DeconToolsV2::Peaks::PEAK_FIT_TYPE::APEX.ToString()))
							{
                                this->PeakFitType = DeconToolsV2::Peaks::PEAK_FIT_TYPE::APEX;
							}
						}
						else if (rdr->Name->Equals("WritePeaksToTextFile"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw gcnew System::Exception ("Missing information for 'WritePeaksToTextFile' parameter in parameter file") ; 
							}
                            this->WritePeaksToTextFile = System::Convert::ToBoolean(rdr->Value);
						}
						break ; 
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals("PeakParameters"))
							return ;
						break ; 
					default:
						break ; 
				}
			}
		}


		clsPeakProcessorParameters::~clsPeakProcessorParameters(void)
		{
		}
	}
}