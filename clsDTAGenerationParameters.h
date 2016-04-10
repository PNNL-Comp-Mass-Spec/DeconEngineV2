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
		public enum class OUTPUT_TYPE {DTA = 0, MGF, LOG, CDTA, MZXML } ; 
		

		public ref class clsDTAGenerationParameters: public System::ICloneable
		{
			std::vector<int> *mvect_msn_levels_to_ignore ;
		
		public:
			clsDTAGenerationParameters(void);
			

			~clsDTAGenerationParameters(void);
			
			void SaveV1DTAGenerationParameters(System::Xml::XmlTextWriter ^xwriter) ; 
			

			void LoadV1DTAGenerationParameters(System::Xml::XmlReader ^xrdr) ; 
			

			virtual Object^ Clone()
			{
				clsDTAGenerationParameters ^new_params = gcnew clsDTAGenerationParameters() ;

                new_params->CCMass = this->CCMass;
                new_params->ConsiderChargeValue = this->ConsiderChargeValue;
                new_params->MaxMass = this->MaxMass;
                new_params->MaxScan = (int)this->MaxScan;
                new_params->MinMass = this->MinMass;
                new_params->MinScan = (int)this->MinMass;
                new_params->WindowSizetoCheck = this->WindowSizetoCheck;
                new_params->MinIonCount = this->MinIonCount;
                new_params->OutputType = this->OutputType;
                new_params->SVMParamFile = this->SVMParamFile;
                new_params->ConsiderMultiplePrecursors = this->ConsiderMultiplePrecursors;
                new_params->IsolationWindowSize = this->IsolationWindowSize;
                new_params->IsProfileDataForMzXML = this->IsProfileDataForMzXML;
                new_params->IgnoreMSnScans = this->IgnoreMSnScans;
                new_params->NumMSnLevelsToIgnore = this->NumMSnLevelsToIgnore;
	
				return new_params ; 
			}

			
			int get_MSnLevelToIgnore(int index) 
			{
				return (*mvect_msn_levels_to_ignore)[index] ; 
			}
			
			void set_MSnLevelToIgnore(int value )
			{
				(*mvect_msn_levels_to_ignore).push_back(value) ; 
			}

            property bool IgnoreMSnScans;
            property int NumMSnLevelsToIgnore;
            property bool IsProfileDataForMzXML;
            property int MinScan;
            property int MinIonCount;
            property int MaxScan;
            property double MinMass;
            property double MaxMass;
            property System::String ^ SVMParamFile;
			property bool ConsiderMultiplePrecursors;
            property int IsolationWindowSize;
            property int ConsiderChargeValue;
            property double CCMass;
            property int WindowSizetoCheck;
            property OUTPUT_TYPE OutputType;
		
		};
	}	
}