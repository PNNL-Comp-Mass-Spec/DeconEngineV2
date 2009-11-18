// Written by Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include <iostream>
#include <vector>

namespace Engine
{
	namespace ChargeDetermination
	{
		const int num_features = 19;
		class FeatureList
		{
			private:								
				double marr_features[num_features];				

			public:

				FeatureList(void);
				~FeatureList(void);				
				void InitValues(std::vector<double> &features);				
				int GetNumFeatures();
				double GetValueAt(int index);
				void SetValueAt(int index, double value);

		};
	}
}

