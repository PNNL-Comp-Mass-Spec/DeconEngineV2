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
#include <exception> 

namespace Engine
{
	namespace Exception
	{
		class InterpolationException : public std::exception
		{
		public:
			InterpolationException(const char* const &mesg): std::exception(mesg)
			{
			}; 
			~InterpolationException() 
			{
			}
		};
	}
}