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
		public __value enum PEAK_FIT_TYPE { APEX = 0, QUADRATIC, LORENTZIAN } ; 

		public __gc class clsPeakProcessorParameters: public System::ICloneable
		{
			double mdbl_SNThreshold ; 
			double mdbl_PeakBackgroundRatio ; 
			bool mbln_thresholded_data ; 
			PEAK_FIT_TYPE menm_FitType ;
			bool mbln_writePeaksToTextFile ; 


		public:
			clsPeakProcessorParameters(void);
			clsPeakProcessorParameters(double sn, double peak_bg_ratio, bool thresholded_data, PEAK_FIT_TYPE fit_type);
			~clsPeakProcessorParameters(void);
			void LoadV1PeakParameters(XmlReader *rdr) ; 
			void SaveV1PeakParameters(System::Xml::XmlTextWriter *xwriter) ; 

			virtual Object* Clone()
			{
				clsPeakProcessorParameters *new_params = new clsPeakProcessorParameters(mdbl_SNThreshold,
					mdbl_PeakBackgroundRatio, mbln_thresholded_data, menm_FitType) ; 
				return new_params ; 
			}

			__property bool get_ThresholdedData()
			{
				return mbln_thresholded_data ; 
			}
			__property void set_ThresholdedData(bool value)
			{
				mbln_thresholded_data = value ; 
			}

			__property double get_PeakBackgroundRatio()
			{
				return mdbl_PeakBackgroundRatio ; 
			}
			__property void set_PeakBackgroundRatio(double value)
			{
				mdbl_PeakBackgroundRatio = value ; 
			}

			__property double get_SignalToNoiseThreshold()
			{
				return mdbl_SNThreshold ; 
			}
			__property void set_SignalToNoiseThreshold(double value)
			{
				mdbl_SNThreshold = value ; 
			}

			__property PEAK_FIT_TYPE get_PeakFitType()
			{
				return menm_FitType ; 
			}
			__property void set_PeakFitType(PEAK_FIT_TYPE value)
			{
				menm_FitType = value ; 
			}

			__property bool get_WritePeaksToTextFile()
			{
				return mbln_writePeaksToTextFile;
			}
			__property void set_WritePeaksToTextFile(bool value)
			{
				mbln_writePeaksToTextFile = value;

			}


		};
	}
}
