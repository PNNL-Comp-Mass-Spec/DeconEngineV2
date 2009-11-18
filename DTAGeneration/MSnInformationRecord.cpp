#include "MSnInformationRecord.h" 

namespace Engine
{
	namespace DTAProcessing
	{

		MSnInformationRecord::MSnInformationRecord()
		{	
			mint_msn_scan_num = 0 ; 
			mint_parent_scan_num = 0 ; 

			mint_msn_scan_level = 0 ; 
			mint_parent_scan_level = 0 ; 

			mdbl_parent_mz = 0 ;
			mdbl_mono_mz = 0 ; 

			mint_parent_intensity = 0 ; 
			mint_mono_intensity = 0 ; 

			mshort_cs = -1 ;
			mdbl_fit = 1 ;			
			mdbl_mono_mw = 0 ;
			
		}

		MSnInformationRecord::~MSnInformationRecord()		
		{
		}
	}
}