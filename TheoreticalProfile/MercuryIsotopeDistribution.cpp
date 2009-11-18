// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "MercuryIsotopeDistribution.h" 


namespace Engine
{
	namespace TheoreticalProfile
	{

		MercuryIsotopeDistribution::MercuryIsotopeDistribution(void)
		{
			mdbl_cc_mass = 1.00727638;
			menm_ap_type = GAUSSIAN ; 
			mint_mercury_size = 8192 ; 
		}

		MercuryIsotopeDistribution::MercuryIsotopeDistribution(char *isotope_file_name)
		{
			mobj_elemental_isotope_composition.LoadData(isotope_file_name) ; 
			mdbl_cc_mass = 1.00727638;
			menm_ap_type = GAUSSIAN ; 
			mint_mercury_size = 8192 ; 

		}

		void MercuryIsotopeDistribution::SetElementalIsotopeComposition(const AtomicInformation &iso_comp)
		{
			mobj_elemental_isotope_composition = iso_comp ; 
		}

			
		MercuryIsotopeDistribution& MercuryIsotopeDistribution::operator=(const MercuryIsotopeDistribution &merc_distribution)
		{
			this->mobj_elemental_isotope_composition = merc_distribution.mobj_elemental_isotope_composition ; 
			this->mdbl_cc_mass = merc_distribution.mdbl_cc_mass ; 
			this->menm_ap_type = merc_distribution.menm_ap_type ; 
			this->mint_mercury_size = merc_distribution.mint_mercury_size ; 
			this->mint_points_per_amu = merc_distribution.mint_points_per_amu ; 
			return (*this) ; 
		}

	}
}