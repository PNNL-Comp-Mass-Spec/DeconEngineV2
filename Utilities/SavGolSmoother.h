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
#include <vector> 

namespace Engine
{
	namespace Utilities
	{
		class  SavGolSmoother
		{
			//! number of points to the left while applying Savitzky Golay filter.
			int mint_Nleft_golay ; 
			//! the order of the Savitzky Golay smoothing filter.
			int mint_golay_order ; 
			//! the number of points to the right while applying Savitzky Golay filter.
			int mint_Nright_golay ; 

			int mint_num_coeffs ; 

			std::vector<double> mvect_temp_x ; 
			std::vector<double> mvect_temp_y ; 
			std::vector<double> mvect_coefficients ; 

		public:
			SavGolSmoother() ; 
			SavGolSmoother(int num_left, int num_right, int order) ; 
			void SetOptions(int num_left, int num_right, int order) ; 
			~SavGolSmoother() ; 
			void Smooth(std::vector<double> *mzs, std::vector<double> *intensities) ;
		};
	}
}