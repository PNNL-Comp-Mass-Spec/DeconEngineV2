#include "ProfileRecord.h" 

namespace Engine
{
	namespace DTAProcessing
	{

		ProfileRecord::ProfileRecord()
		{	
			mint_msn_scan_num = 0 ; 
			mint_parent_scan_num = 0 ; 
			mdbl_agc_time = -1.0 ; 
			mdbl_tic_val = -1.0; 
		}

		ProfileRecord::~ProfileRecord()		
		{
		}
	}
}