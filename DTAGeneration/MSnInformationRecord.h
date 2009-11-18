#pragma once

namespace Engine
{
	namespace DTAProcessing
	{

		const int MAX_ISOTOPES = 16 ; 
		const int MAX_ID_LEN = 256 ; 
		//! class to store information that logs into the log file of DeconMSn
		class  MSnInformationRecord
		{
		public:			
			//! scan number of MSn_scan
			int mint_msn_scan_num ; 
			//! msNLevel of MSn_scan
			int mint_msn_scan_level ; 
			//! parent scam
			int mint_parent_scan_num ; 
			//! msNLevel of parent scan
			int mint_parent_scan_level ; 
			//! m/z value of parent
			double mdbl_parent_mz ;
			//! mono m/z value of parent
			double mdbl_mono_mz ; 
			//! charge state 
			short mshort_cs ;			
			//! fit value .
			double mdbl_fit ;
			//! monoisotopic mw of feature.
			double mdbl_mono_mw ;			
			//! intensity of monoisotopic peak observed.
			int mint_mono_intensity  ;
			//! intensity of parent peak observed.
			int mint_parent_intensity  ;
			//! default constructor
			MSnInformationRecord() ; 
			//! destructor.
			~MSnInformationRecord() ; 
		};
	}
}