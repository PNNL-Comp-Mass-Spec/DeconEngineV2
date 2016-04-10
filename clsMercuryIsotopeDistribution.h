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
	public enum ApodizationType {Unknown, Gaussian, Lorentzian } ; 

	public ref class clsMercuryIsotopeDistribution
	{
	private:
		Engine::TheoreticalProfile::MercuryIsotopeDistribution *mMercuryIsotopeDistribution;
		double mResolution;
		int mChargeState;

	public:
		clsMercuryIsotopeDistribution();
		~clsMercuryIsotopeDistribution();

		void CalculateMasses(System::Collections::Hashtable^ elementCounts);

		// Call the mMercuryIsotopeDistribution method, passing in the currently set resolution and other 
		// settings.  Copy the result into an array of PointF.
		array<System::Drawing::PointF^>^ CalculateDistribution(System::Collections::Hashtable^ elementCounts);

		int MercurySize() {
			return mMercuryIsotopeDistribution->mint_mercury_size;
		}

		/// Sets the mercury size.  Only accepts multiples of 2.
		void MercurySize(int size) {
			if (size <= 0) {
				throw gcnew System::ArgumentOutOfRangeException("MercurySize", size,"Mercury size must be greater than 0");
			}

			bool goodSize = false;
			for (int i = 1; i <= 536870912; i *= 2) {
				if (size == i) {
					goodSize = true;
					break;
				}
			}
			if (!goodSize) {
				throw gcnew System::ArgumentException("MercurySize", "MercurySize must be a power of two.");
			}

			mMercuryIsotopeDistribution->mint_mercury_size = size;
		}

		double ChargeCarrierMass() {
			return mMercuryIsotopeDistribution->mdbl_cc_mass;
		}

		void ChargeCarrierMass(double cc_mass) {
			mMercuryIsotopeDistribution->mdbl_cc_mass = cc_mass;
		}

		ApodizationType ApodizationType() {
			short type = (short) mMercuryIsotopeDistribution->menm_ap_type;
			if (type == 1) {
				return ApodizationType::Gaussian;
			} else if (type == 2) {
				return ApodizationType::Lorentzian;
			} else {
				return ApodizationType::Unknown;
			}
		}

		void ApodizationType(DeconToolsV2::ApodizationType type) {
			if (type == ApodizationType::Gaussian) {
				mMercuryIsotopeDistribution->menm_ap_type = Engine::TheoreticalProfile::GAUSSIAN ;
			}
			else if (type == ApodizationType::Lorentzian) {
				mMercuryIsotopeDistribution->menm_ap_type = Engine::TheoreticalProfile::LORENTZIAN ;
			}
		}

		double Resolution() {
			return mResolution;
		}

		void Resolution(double res) {
			mResolution = res;
		}

		/* Gets and sets charge state property */
		short ChargeState() {
			return mChargeState;
		}

		void ChargeState(short charge) {
			mChargeState = charge;
		}

		/* Gets average molecular weight of the last call to CalculateDistribution */
		double AverageMolecularMass() {
			return mMercuryIsotopeDistribution->mdbl_average_mw;
		}

		/* Gets the monoisotopic molecular weight of the last call to CalculateDistribution */
		double MonoMolecularMass() {
			return mMercuryIsotopeDistribution->mdbl_mono_mw;
		}

		/* Gets the most abundant MZ of the last call to CalculateDistribution */
		double MostAbundantMZ() {
			return mMercuryIsotopeDistribution->mdbl_max_peak_mz;
		}

		/* Gets the mass variance of the last call to CalculateDistribution */
		double MassVariance() {
			return mMercuryIsotopeDistribution->GetMassVariance() ;
		}

		void ElementIsotopes(DeconToolsV2::clsElementIsotopes^ elementalIsotopes) 
		{
			mMercuryIsotopeDistribution->SetElementalIsotopeComposition(*elementalIsotopes->GetElementalIsotopeComposition()) ; 
		}
	} ;
}