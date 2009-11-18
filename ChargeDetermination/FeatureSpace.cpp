// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "FeatureSpace.h"

namespace Engine
{
	namespace ChargeDetermination
	{
		FeatureList::FeatureList(void)
		{
			for(int feature_num = 0; feature_num < num_features; feature_num ++)
				marr_features[feature_num] = 0;
		}			
		
		FeatureList::~FeatureList(void)
		{
			//delete marr_features;
		}

		void FeatureList::InitValues(std::vector<double> &features)
		{
			for(int feature_num = 0 ; feature_num < num_features; feature_num++)
				marr_features[feature_num] = features[feature_num];
		}

				
		int FeatureList::GetNumFeatures()
		{
			return num_features;
		}
		double FeatureList::GetValueAt(int index)
		{
			return marr_features[index];
			
		}
		void FeatureList::SetValueAt(int index, double value)
		{
			marr_features[index] = value;
		}
	}
}
