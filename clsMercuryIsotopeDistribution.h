// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#using <mscorlib.dll>
#using <System.Drawing.DLL>

#include "clsElementIsotopes.h"
#include "TheoreticalProfile/MercuryIsotopeDistribution.h"
#include <iostream>

namespace DeconToolsV2
{
	public __value enum ApodizationType {Unknown, Gaussian, Lorentzian } ; 

	public __gc class clsMercuryIsotopeDistribution
	{
	private:
		Engine::TheoreticalProfile::MercuryIsotopeDistribution __nogc *mMercuryIsotopeDistribution;
		double mResolution;
		int mChargeState;

	public:
		clsMercuryIsotopeDistribution();
		~clsMercuryIsotopeDistribution();

		void CalculateMasses(System::Collections::Hashtable* elementCounts);

		// Call the mMercuryIsotopeDistribution method, passing in the currently set resolution and other 
		// settings.  Copy the result into an array of PointF.
		System::Drawing::PointF CalculateDistribution(System::Collections::Hashtable* elementCounts)[];

		__property int get_MercurySize() {
			return mMercuryIsotopeDistribution->mint_mercury_size;
		}

		/// Sets the mercury size.  Only accepts multiples of 2.
		__property void set_MercurySize(int size) {
			if (size <= 0) {
				throw new System::ArgumentOutOfRangeException(S"MercurySize", __box(size),S"Mercury size must be greater than 0");
			}

			bool goodSize = false;
			for (int i = 1; i <= 536870912; i *= 2) {
				if (size == i) {
					goodSize = true;
					break;
				}
			}
			if (!goodSize) {
				throw new System::ArgumentException(S"MercurySize", S"MercurySize must be a power of two.");
			}

			mMercuryIsotopeDistribution->mint_mercury_size = size;
		}

		__property double get_ChargeCarrierMass() {
			return mMercuryIsotopeDistribution->mdbl_cc_mass;
		}

		__property void set_ChargeCarrierMass(double cc_mass) {
			mMercuryIsotopeDistribution->mdbl_cc_mass = cc_mass;
		}

		__property ApodizationType get_ApodizationType() {
			short type = (short) mMercuryIsotopeDistribution->menm_ap_type;
			if (type == 1) {
				return ApodizationType::Gaussian;
			} else if (type == 2) {
				return ApodizationType::Lorentzian;
			} else {
				return ApodizationType::Unknown;
			}
		}

		__property void set_ApodizationType(DeconToolsV2::ApodizationType type) {
			if (type == ApodizationType::Gaussian) {
				mMercuryIsotopeDistribution->menm_ap_type = Engine::TheoreticalProfile::GAUSSIAN ;
			}
			else if (type == ApodizationType::Lorentzian) {
				mMercuryIsotopeDistribution->menm_ap_type = Engine::TheoreticalProfile::LORENTZIAN ;
			}
		}

		__property double get_Resolution() {
			return mResolution;
		}

		__property void set_Resolution(double res) {
			mResolution = res;
		}

		/* Gets and sets charge state property */
		__property short get_ChargeState() {
			return mChargeState;
		}

		__property void set_ChargeState(short charge) {
			mChargeState = charge;
		}

		/* Gets average molecular weight of the last call to CalculateDistribution */
		__property double get_AverageMolecularMass() {
			return mMercuryIsotopeDistribution->mdbl_average_mw;
		}

		/* Gets the monoisotopic molecular weight of the last call to CalculateDistribution */
		__property double get_MonoMolecularMass() {
			return mMercuryIsotopeDistribution->mdbl_mono_mw;
		}

		/* Gets the most abundant MZ of the last call to CalculateDistribution */
		__property double get_MostAbundantMZ() {
			return mMercuryIsotopeDistribution->mdbl_max_peak_mz;
		}

		/* Gets the mass variance of the last call to CalculateDistribution */
		__property double get_MassVariance() {
			return mMercuryIsotopeDistribution->GetMassVariance() ;
		}

		__property void set_ElementIsotopes(DeconToolsV2::clsElementIsotopes* elementalIsotopes) 
		{
			mMercuryIsotopeDistribution->SetElementalIsotopeComposition(*elementalIsotopes->GetElementalIsotopeComposition()) ; 
		}
	} ;
}