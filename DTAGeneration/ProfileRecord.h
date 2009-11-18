#pragma once

namespace Engine
{
	namespace DTAProcessing
	{		
		//! class to store information that logs into the log file of DeconMSn
		class  ProfileRecord
		{
		public:			
			//! scan number of MSn_scan
			int mint_msn_scan_num ; 			 
			//! parent scan
			int mint_parent_scan_num ; 
			//! ion injection value		
			double mdbl_agc_time ;
			//! tic
			double mdbl_tic_val  ;
			//! default constructor
			ProfileRecord() ; 
			//! destructor.
			~ProfileRecord() ; 
		};
	}
}