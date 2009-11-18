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
#include "RawData.h"
namespace Engine 
{
	namespace Readers
	{
		class   ReaderFactory
		{
		public:
			static RawData* GetRawData(FileType file_type) ; 
			static RawData* GetRawData(FileType file_type, char *file_name) ; 
			static void GetRawData(RawData **raw_data, FileType file_type) ; 
		} ; 
	}
}