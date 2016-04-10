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
#using <mscorlib.dll>
#using <System.Xml.dll>
using namespace System::Xml ; 

namespace DeconToolsV2
{
	namespace Peaks
	{
		//! enumeration for type of fit. 
		public enum class PEAK_FIT_TYPE { APEX = 0, QUADRATIC, LORENTZIAN } ; 

		public ref class clsPeakProcessorParameters: public System::ICloneable
		{

		public:
			clsPeakProcessorParameters(void);
			clsPeakProcessorParameters(double sn, double peak_bg_ratio, bool thresholded_data, PEAK_FIT_TYPE fit_type);
			~clsPeakProcessorParameters(void);
			void LoadV1PeakParameters(XmlReader ^rdr) ; 
			void SaveV1PeakParameters(System::Xml::XmlTextWriter ^xwriter) ; 

			virtual Object^ Clone()
			{
				clsPeakProcessorParameters ^new_params = gcnew clsPeakProcessorParameters(SignalToNoiseThreshold,
					PeakBackgroundRatio, ThresholdedData, PeakFitType) ; 
				return new_params ; 
			}

            property bool ThresholdedData;
            property double PeakBackgroundRatio;
            property double SignalToNoiseThreshold;
            property PEAK_FIT_TYPE PeakFitType;
            property bool WritePeaksToTextFile;

		};
	}
}
