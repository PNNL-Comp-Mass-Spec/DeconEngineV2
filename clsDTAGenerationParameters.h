// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
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
#include<stdlib.h> 
#include<vector>
#include<iostream>
#include<iterator>
#include<vcclr.h>

namespace DeconToolsV2
{
	namespace DTAGeneration
	{
		public __value enum OUTPUT_TYPE {DTA = 0, MGF, LOG, CDTA, MZXML } ; 
		

		public __gc class clsDTAGenerationParameters: public System::ICloneable
		{
		
			int mint_min_ion_count ;
			int mint_min_scan ; 
			int mint_max_scan ;
			double mdbl_min_mass ;
			double mdbl_max_mass ;			
			int mint_consider_charge ; 			
			int mint_window_size_to_check  ; 
			double mdbl_cc_mass ;			
			System::String *mstr_svm_file_name  ; 
			
			
			bool mbln_is_profile_data_for_mzXML ; 
			bool mbln_consider_multiple_precursors ; 
			
			bool mbln_ignore_msn_scans ; 			
			int mint_num_msn_levels_to_ignore ; 
		
			std::vector<int> *mvect_msn_levels_to_ignore ;

			int mint_isolation_window_size ; 

			OUTPUT_TYPE menm_output_type ; 
		
		
		public:
			clsDTAGenerationParameters(void);
			

			~clsDTAGenerationParameters(void);
			
			void SaveV1DTAGenerationParameters(System::Xml::XmlTextWriter *xwriter) ; 
			

			void LoadV1DTAGenerationParameters(System::Xml::XmlReader *xrdr) ; 
			

			virtual Object* Clone()
			{
				clsDTAGenerationParameters *new_params = new clsDTAGenerationParameters() ;

				new_params->set_CCMass(this->get_CCMass()) ; 
				new_params->set_ConsiderChargeValue(this->get_ConsiderChargeValue()) ; 
				new_params->set_MaxMass(this->get_MaxMass()) ; 
				new_params->set_MaxScan((int)this->get_MaxScan()) ; 
				new_params->set_MinMass(this->get_MinMass()) ;
				new_params->set_MinScan((int)this->get_MinMass()) ;				
				new_params->set_WindowSizetoCheck(this->get_WindowSizetoCheck());
				new_params->set_MinIonCount(this->get_MinIonCount()) ; 
				new_params->set_OutputType(this->get_OutputType()) ; 
				new_params->set_SVMParamFile(this->mstr_svm_file_name) ; 
				new_params->set_ConsiderMultiplePrecursors(this->get_ConsiderMultiplePrecursors()) ; 
				new_params->set_IsolationWindowSize(this->get_IsolationWindowSize()) ;
				new_params->set_IsProfileDataForMzXML(this->get_IsProfileDataForMzXML()) ; 
				new_params->set_IgnoreMSnScans(this->get_IgnoreMSnScans()) ; 
				new_params->set_NumMSnLevelsToIgnore(this->get_NumMSnLevelsToIgnore()) ; 				
	
				return new_params ; 
			}

			
			__property int get_MSnLevelToIgnore(int index) 
			{
				return (*mvect_msn_levels_to_ignore)[index] ; 
			}
			
			__property void set_MSnLevelToIgnore(int value )
			{
				(*mvect_msn_levels_to_ignore).push_back(value) ; 
			}

			__property bool get_IgnoreMSnScans()
			{
				return mbln_ignore_msn_scans ; 
			}
			__property void set_IgnoreMSnScans(bool value)
			{
				mbln_ignore_msn_scans = value ; 
			}
			__property int get_NumMSnLevelsToIgnore()
			{
				return mint_num_msn_levels_to_ignore  ; 
			}
			__property void set_NumMSnLevelsToIgnore(int value)
			{
				mint_num_msn_levels_to_ignore = value ; 
			}			
			__property bool get_IsProfileDataForMzXML()
			{
				return mbln_is_profile_data_for_mzXML ; 
			}
			__property void set_IsProfileDataForMzXML(bool value)
			{
				mbln_is_profile_data_for_mzXML = value; 
			}

			__property int get_MinScan()
			{
				return mint_min_scan ; 
			}

			__property int get_MinIonCount()
			{
				return mint_min_ion_count ; 
			}
			
			__property int get_MaxScan()
			{
				return mint_max_scan ; 			
			}
			
			__property double get_MinMass() 
			{
				return mdbl_min_mass ; 
			}
			
			__property double get_MaxMass()
			{
				return mdbl_max_mass ;
			}

			__property void set_MinScan(int value)
			{
				mint_min_scan = value ; 
			}

			__property void set_MaxScan(int value)
			{
				mint_max_scan = value ; 
			}

			__property void set_MinMass(double value)
			{
				mdbl_min_mass = value ; 
			}

			__property void set_MaxMass(double value)
			{
				mdbl_max_mass = value ; 
			}

			__property void set_MinIonCount( int value)
			{
				mint_min_ion_count = value ; 
			}

			__property void set_SVMParamFile(System::String *file_name) 
			{
				mstr_svm_file_name  = file_name ; 
			}

			__property System::String * get_SVMParamFile()
			{
				return mstr_svm_file_name ; 
			}

			__property void set_ConsiderMultiplePrecursors(bool value)
			{
				mbln_consider_multiple_precursors = value ; 
			}
				
			__property bool get_ConsiderMultiplePrecursors()
			{
				return mbln_consider_multiple_precursors ; 
			}

			__property void set_IsolationWindowSize(int value)
			{
				mint_isolation_window_size = value ; 
			}

			__property int get_IsolationWindowSize()
			{
				return mint_isolation_window_size ; 
			}

			__property int get_ConsiderChargeValue()
			{
				return mint_consider_charge ; 
			}

			__property void set_ConsiderChargeValue(int value)
			{
				mint_consider_charge = value ;  
			}
		
			__property double get_CCMass()
			{
				return mdbl_cc_mass ; 
			}

			__property void set_CCMass(double value)
			{
				mdbl_cc_mass = value ; 
			}

			__property int get_WindowSizetoCheck()
			{
				return mint_window_size_to_check ;
			}

			__property void set_WindowSizetoCheck(int value)
			{
				mint_window_size_to_check = value ;
			}

			__property OUTPUT_TYPE get_OutputType()
			{
				return menm_output_type ; 
			}

			__property void set_OutputType(OUTPUT_TYPE value)
			{
				menm_output_type = value ;
			}
		
		};
	}	
}