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
#using <System.Xml.dll>

namespace DeconToolsV2
{
 namespace Readers
	{
		public enum class ApodizationType {SQUARE =0, PARZEN, HANNING, WELCH, TRIANGLE, NOAPODIZATION } ; 
		public enum class CalibrationType { A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C_OVERFCUBE = 0, 
			A_OVER_F_PLUS_B_OVER_FSQ, A_OVER_F_PLUS_B_OVER_FSQ_PLUS_CI_OVERFSQ, 
			A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C, AF_PLUS_B, F, A_OVER_F_PLUS_B,  
			A_OVER_F_PLUS_B_PLUS_CI, TIME_A_TSQ_PLUS_B_T_PLUS_C, BRUKER_CALMET, UNDEFINED} ; 


		public ref class clsRawDataPreprocessOptions
		{
		public:
			clsRawDataPreprocessOptions()
			{
                ApodizationType = Readers::ApodizationType::NOAPODIZATION;
				ApodizationPercent = 50 ; 
				ApodizationMinX = 0 ;
				ApodizationMaxX = 0.9437166 ;
				NumZeroFills = 0 ; 
				CalibrationType = CalibrationType::UNDEFINED ; 
				A = 0 ; 
				B = 0 ; 
				C = 0 ; 
			}
			~clsRawDataPreprocessOptions(void)
			{
			}

			virtual Object^ Clone() ; 

            property DeconToolsV2::Readers::ApodizationType ApodizationType;
			bool IsToBePreprocessed()
			{
                return ApodizationType != Readers::ApodizationType::NOAPODIZATION || CalibrationType != CalibrationType::UNDEFINED;
			}
            property double ApodizationMinX;
            property double ApodizationMaxX;
            property int ApodizationPercent;
            property short NumZeroFills;
			bool ApplyCalibration()
			{
				return CalibrationType != CalibrationType::UNDEFINED ; 
			}
            DeconToolsV2::Readers::CalibrationType CalibrationType;
            property double A;
            property double B;
            property double C;
			void SaveV1FTICRPreProcessOptions(System::Xml::XmlTextWriter ^xwriter) ; 
			void LoadV1FTICRPreProcessOptions(System::Xml::XmlReader ^rdr) ; 
		};
	}
}