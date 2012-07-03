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
			mdbl_SNThreshold = 3.0 ; 
			mdbl_PeakBackgroundRatio = 5.0 ; 
			menm_FitType = PEAK_FIT_TYPE::QUADRATIC ; 
		}

		clsPeakProcessorParameters::clsPeakProcessorParameters(double sn, 
			double peak_bg_ratio, bool thresholded_data, PEAK_FIT_TYPE fit_type)
		{
			mdbl_SNThreshold = sn ; 
			mdbl_PeakBackgroundRatio = peak_bg_ratio ; 
			menm_FitType = fit_type ; 
			mbln_thresholded_data = thresholded_data ; 
		}

		void clsPeakProcessorParameters::SaveV1PeakParameters(System::Xml::XmlTextWriter *xwriter)
		{
			xwriter->WriteWhitespace(S"\n\t") ;
			xwriter->WriteStartElement(S"PeakParameters");
			xwriter->WriteWhitespace(S"\n\t\t") ; 

			xwriter->WriteElementString(S"PeakBackgroundRatio", System::Convert::ToString(this->PeakBackgroundRatio)); 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"SignalToNoiseThreshold", this->SignalToNoiseThreshold.ToString()) ; 
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"PeakFitType",__box(this->PeakFitType)->ToString()) ;
			xwriter->WriteWhitespace(S"\n\t\t") ; 
			xwriter->WriteElementString(S"WritePeaksToTextFile",__box(this->WritePeaksToTextFile)->ToString()) ;
		
			xwriter->WriteWhitespace(S"\n\t") ; 
			xwriter->WriteEndElement();
			
		}

		void clsPeakProcessorParameters::LoadV1PeakParameters(XmlReader *rdr)
		{

			//Read each node in the tree.
			while (rdr->Read())
			{
				switch (rdr->NodeType)
				{
					case XmlNodeType::Element:
						if (rdr->Name->Equals(S"PeakBackgroundRatio"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new System::Exception (S"Missing information for PeakBackgroundRatio in parameter file") ; 
							}
							this->set_PeakBackgroundRatio(System::Convert::ToDouble(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"SignalToNoiseThreshold"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new System::Exception (S"Missing information for SignalToNoiseThreshold in parameter file") ; 
							}
							this->set_SignalToNoiseThreshold(System::Convert::ToDouble(rdr->Value)) ; 
						}
						else if (rdr->Name->Equals(S"PeakFitType"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new System::Exception (S"Missing information for SignalToNoiseThreshold in parameter file") ; 
							}
							if (rdr->Value->Equals(__box(DeconToolsV2::Peaks::PEAK_FIT_TYPE::QUADRATIC)->ToString()))
							{
								this->PeakFitType = DeconToolsV2::Peaks::PEAK_FIT_TYPE::QUADRATIC ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::Peaks::PEAK_FIT_TYPE::LORENTZIAN)->ToString()))
							{
								this->PeakFitType = DeconToolsV2::Peaks::PEAK_FIT_TYPE::LORENTZIAN ;
							}
							else if (rdr->Value->Equals(__box(DeconToolsV2::Peaks::PEAK_FIT_TYPE::APEX)->ToString()))
							{
								this->PeakFitType = DeconToolsV2::Peaks::PEAK_FIT_TYPE::APEX ;
							}
						}
						else if (rdr->Name->Equals(S"WritePeaksToTextFile"))
						{
							rdr->Read() ; 
							while(rdr->NodeType == XmlNodeType::Whitespace || rdr->NodeType == XmlNodeType::SignificantWhitespace)
							{
								rdr->Read() ; 
							}
							if (rdr->NodeType != XmlNodeType::Text)
							{
								throw new System::Exception (S"Missing information for 'WritePeaksToTextFile' parameter in parameter file") ; 
							}
							this->set_WritePeaksToTextFile(System::Convert::ToBoolean(rdr->Value)) ; 
						}
						break ; 
					case XmlNodeType::EndElement:
						if (rdr->Name->Equals(S"PeakParameters"))
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