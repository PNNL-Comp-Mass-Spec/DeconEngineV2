#include "IsotopeFitRecord.h" 

namespace Engine
{
	namespace HornTransform
	{

		IsotopeFitRecord::IsotopeFitRecord()
		{
			mint_abundance = 0 ;
			mshort_cs = -1 ;
			mdbl_mz = 0 ;
			mdbl_fit = 1 ;
			mdbl_average_mw = 0 ;
			mdbl_mono_mw = 0 ;
			mdbl_most_intense_mw = 0 ; 

			mdbl_fwhm = 0 ; 
			mdbl_sn = 0 ; 
			mint_mono_intensity = 0 ;
			mint_iplus2_intensity = 0 ;

			mint_peak_index = -1 ; 
			mint_num_isotopes_observed = 0 ; 
			//mbln_flag_isotope_link = false ; 
		}

		IsotopeFitRecord::~IsotopeFitRecord()
		{
		}
	}
}