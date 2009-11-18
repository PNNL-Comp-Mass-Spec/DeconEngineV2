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

namespace DeconToolsV2
{
 namespace Readers
	{
		public __value enum ApodizationType {SQUARE =0, PARZEN, HANNING, WELCH, TRIANGLE, NOAPODIZATION } ; 
		public __value enum CalibrationType { A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C_OVERFCUBE = 0, 
			A_OVER_F_PLUS_B_OVER_FSQ, A_OVER_F_PLUS_B_OVER_FSQ_PLUS_CI_OVERFSQ, 
			A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C, AF_PLUS_B, F, A_OVER_F_PLUS_B,  
			A_OVER_F_PLUS_B_PLUS_CI, TIME_A_TSQ_PLUS_B_T_PLUS_C, BRUKER_CALMET, UNDEFINED} ; 


		public __gc class clsRawDataPreprocessOptions
		{
			ApodizationType menmApodizationType ; 
			CalibrationType menmCalibrationType ; 
			int mintApodizationPercent ; 
			double mdbl_apodization_min_x ; 
			double mdbl_apodization_max_x ; 
			short mshort_num_zeros ; 
			double mdbl_A ; 
			double mdbl_B ; 
			double mdbl_C ; 

		public:
			clsRawDataPreprocessOptions()
			{
				menmApodizationType = NOAPODIZATION ; 
				mintApodizationPercent = 50 ; 
				mdbl_apodization_min_x = 0 ;
				mdbl_apodization_max_x = 0.9437166 ;
				mshort_num_zeros = 0 ; 
				menmCalibrationType = UNDEFINED ; 
				mdbl_A = 0 ; 
				mdbl_B = 0 ; 
				mdbl_C = 0 ; 
			}
			~clsRawDataPreprocessOptions(void)
			{
			}

			virtual Object* Clone() ; 

			__property DeconToolsV2::Readers::ApodizationType get_ApodizationType()
			{
				return menmApodizationType ; 
			}
			__property void set_ApodizationType(DeconToolsV2::Readers::ApodizationType value)
			{
				menmApodizationType = value ; 
			}
			__property bool get_IsToBePreprocessed()
			{
				return menmApodizationType != NOAPODIZATION || menmCalibrationType != UNDEFINED ; 
			}
			__property double get_ApodizationMinX()
			{
				return mdbl_apodization_min_x ; 
			}
			__property void set_ApodizationMinX(double val)
			{
				mdbl_apodization_min_x = val ; 
			}
			__property double get_ApodizationMaxX()
			{
				return mdbl_apodization_max_x ; 
			}
			__property void set_ApodizationMaxX(double val)
			{
				mdbl_apodization_max_x = val ; 
			}
			__property int get_ApodizationPercent()
			{
				return mintApodizationPercent ; 
			}
			__property void set_ApodizationPercent(int val)
			{
				mintApodizationPercent = val ; 
			}
			__property short get_NumZeroFills()
			{
				return mshort_num_zeros ; 
			}
			__property void set_NumZeroFills(short val)
			{
				mshort_num_zeros = val ; 
			}
			__property bool get_ApplyCalibration()
			{
				return menmCalibrationType != UNDEFINED ; 
			}
			__property DeconToolsV2::Readers::CalibrationType get_CalibrationType()
			{
				return menmCalibrationType ; 
			}

			__property void set_CalibrationType(DeconToolsV2::Readers::CalibrationType type)
			{
				menmCalibrationType = type ; 
			}

			__property double get_A()
			{
				return mdbl_A ; 
			}

			__property void set_A(double a)
			{
				mdbl_A = a ; 
			}

			__property double get_B()
			{
				return mdbl_B ; 
			}

			__property void set_B(double b)
			{
				mdbl_B = b ; 
			}

			__property double get_C()
			{
				return mdbl_C ; 
			}

			__property void set_C(double c)
			{
				mdbl_C = c ; 
			}
			void SaveV1FTICRPreProcessOptions(System::Xml::XmlTextWriter *xwriter) ; 
			void LoadV1FTICRPreProcessOptions(System::Xml::XmlReader *rdr) ; 
		};
	}
}