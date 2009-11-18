// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "DeconEngineUtils.h"
#include "Utilities/SavGolSmoother.h" 
#include "Utilities/Interpolation.h" 
#include "Utilities/Apodization.h"
#include "Utilities/FFT32.h" 
namespace DeconEngine
{
	void Utils::SetData(std::vector<double> &vectData, float (&data) __gc [])
	{
		vectData.clear() ; 
		int numPoints = data->Length ; 

		for (int ptNum = 0 ; ptNum < numPoints ; ptNum++)
		{
			vectData.push_back((double)data[ptNum]) ; 
		}
	}

	void Utils::SetData(std::vector<float> &vectData, float (&data) __gc [])
	{
		vectData.clear() ; 
		int numPoints = data->Length ; 

		for (int ptNum = 0 ; ptNum < numPoints ; ptNum++)
		{
			float val = data[ptNum] ; 
			vectData.push_back(val) ; 
		}
	}

	void Utils::GetData(std::vector<float> &vectData, float (&data) __gc [])
	{
		int numPoints = vectData.size() ; 
		data = new float __gc [numPoints] ;

		for (int ptNum = 0 ; ptNum < numPoints ; ptNum++)
		{
			data[ptNum] = vectData[ptNum] ; 
		}
	}

	void Utils::GetData(std::vector<double> &vectData, float (&data) __gc [])
	{
		int numPoints = vectData.size() ; 
		data = new float __gc [numPoints] ;

		for (int ptNum = 0 ; ptNum < numPoints ; ptNum++)
		{
			data[ptNum] = (float) vectData[ptNum] ; 
		}
	}

	void Utils::GetStr(System::String *src, char *dest)
	{
		if (src == 0 || src == "" || src->get_Length() == 0)
		{
			dest[0] = '\0' ; 
			return ; 
		}

		int len = src->get_Length() ; 
		for (int i = 0 ; i < len ; i++)
		{
			dest[i] = (char) src->Chars[i] ; 
		}
		dest[len] = '\0' ; 
	}

	void Utils::GetStr(const char *src, System::String **dest)
	{
		if (src == NULL)
		{
			*dest = NULL ; 
			return ; 
		}
		if (strlen(src) == 0)
		{
			*dest = new System::String("") ; 
			return ; 
		}

		*dest = new System::String(src) ; 
		return ; 
	}

	double Utils::GetAverage(float (&intensities) __gc [], float maxIntensity)
	{
		int numPts = intensities->Length  ; 
		if (numPts == 0)
			return 0 ; 

		double backgroundIntensity = 0 ; 
		int numPtsUsed = 0 ;
		for (int i = 0 ; i < numPts ; i++)
		{
			if (intensities[i] <= maxIntensity && intensities[i] != 0)
			{
				backgroundIntensity += intensities[i] ; 
				numPtsUsed++ ; 
			}
		}
		return backgroundIntensity / numPtsUsed ; 
	}

	double Utils::GetAverage(std::vector<double> &intensities, float maxIntensity)
	{
		int numPts = intensities.size()  ; 
		if (numPts == 0)
			return 0 ; 

		double backgroundIntensity = 0 ; 
		int numPtsUsed = 0 ;
		for (int i = 0 ; i < numPts ; i++)
		{
			if (intensities[i] <= maxIntensity && intensities[i] != 0)
			{
				backgroundIntensity += intensities[i] ; 
				numPtsUsed++ ; 
			}
		}
		return backgroundIntensity / numPtsUsed ; 
	}

	double Utils::GetTIC(double min_mz, double max_mz, std::vector<double> &mzs, std::vector<double> &intensities, float minIntensity, 
		double &bpi, double &bp_mz)
	{
		int numPts = intensities.size()  ; 
		if (numPts == 0)
			return 0 ; 

		double sum = 0 ; 
		int numPtsUsed = 0 ;
		bpi = 0 ; 
		for (int i = 0 ; i < numPts ; i++)
		{
			if (mzs[i] >= min_mz && mzs[i] < max_mz && intensities[i] >= minIntensity)
			{
				sum += intensities[i] ;
				if (intensities[i] > bpi)
				{
					bpi = intensities[i] ; 
					bp_mz = mzs[i] ; 
				}
			}
		}
		return sum ; 
	}

	void Utils::ConvertElementTableToFormula(Engine::TheoreticalProfile::AtomicInformation &elemental_isotope_composition, System::Collections::Hashtable* elementCounts, 
			Engine::TheoreticalProfile::MolecularFormula &formula)
	{
		char element_char[16] ; 
		System::Collections::IEnumerator* elements = elementCounts->Keys->GetEnumerator();

		formula.Clear() ; 
		Engine::TheoreticalProfile::AtomicCount atomic_count ; 

		while(elements->MoveNext()) 
		{
			// Get the next element symbol in the table
			System::String* element = __try_cast<System::String*>(elements->Current);
			DeconEngine::Utils::GetStr(element, element_char) ; 
			// Put it in a character array
			int count = *static_cast<__box int*>(elementCounts->Item[element]);

			// find the index of the element in the AtomicInformation
			int index = elemental_isotope_composition.GetElementIndex(element_char) ; 
			if (index == -1) 
			{
				throw new System::ApplicationException(System::String::Concat(S"Unknown element ", element));
			} 
			else 
			{
				atomic_count.mint_index = index ;
				atomic_count.mdbl_num_copies = count ; 
				double mono_mass = elemental_isotope_composition.mvect_elemental_isotopes[index].marr_isotope_mass[0] * count ; 
				double avg_mass = elemental_isotope_composition.mvect_elemental_isotopes[index].mdbl_average_mass * count ; 
				formula.AddAtomicCount(atomic_count, mono_mass, avg_mass ) ; 
			}
		}
	}
	void Utils::SetPeaks(Engine::PeakProcessing::PeakData &pk_data, DeconToolsV2::Peaks::clsPeak* (&peaks) __gc [])
	{
		Engine::PeakProcessing::Peak enginePeak ; 
		for (int pkNum = 0 ; pkNum < peaks->Length ; pkNum++)
		{
			enginePeak.mdbl_FWHM = peaks[pkNum]->mdbl_FWHM ; 
			enginePeak.mdbl_intensity = peaks[pkNum]->mdbl_intensity ; 
			enginePeak.mdbl_mz = peaks[pkNum]->mdbl_mz ; 
			enginePeak.mdbl_SN = peaks[pkNum]->mdbl_SN ; 
			enginePeak.mint_data_index = peaks[pkNum]->mint_data_index ; 
			enginePeak.mint_peak_index = peaks[pkNum]->mint_peak_index ; 
			pk_data.AddPeak(enginePeak) ; 
		}
		pk_data.InitializeUnprocessedPeakData() ; 

	}


	void Utils::SavitzkyGolaySmooth(short num_left, short num_right, short order, float (&mzs) __gc [], float (&intensities) __gc [])
	{
		Engine::Utilities::SavGolSmoother *sgSmoother = __nogc new  Engine::Utilities::SavGolSmoother(num_left, num_right, order) ; 
		std::vector<double> vectX  ; 
		std::vector<double> vectY  ; 
		int num_pts = mzs->Length ; 

		for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
		{
			vectX.push_back((double)mzs[pt_num]) ; 
			vectY.push_back((double)intensities[pt_num]) ; 
		}
		sgSmoother->Smooth(&vectX, &vectY) ; 
		for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
		{
			mzs[pt_num] = (float)vectX[pt_num] ; 
			intensities[pt_num] = (float) vectY[pt_num] ; 
		}
		delete sgSmoother ; 
	}
	int Utils::ZeroFillUnevenData(float (&mzs) __gc [], float (&intensities) __gc [], int maxPtsToAdd)
	{
		std::vector<float> vectX  ; 
		std::vector<float> vectY  ; 
		int num_pts = mzs->Length ; 

		for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
		{
			float currentMZ = mzs[pt_num] ; 
			float currentIntensity = intensities[pt_num] ; 
			vectX.push_back(currentMZ) ; 
			vectY.push_back(currentIntensity) ; 
		}
		int numPtsX = vectX.size() ; 
		Engine::Utilities::Interpolation __nogc *interp = __nogc new Engine::Utilities::Interpolation() ; 
		interp->ZeroFillMissing(vectX, vectY, maxPtsToAdd) ; 

		int num_pts_new = (int) vectX.size() ; 
		intensities = new float __gc [num_pts_new] ;
		mzs = new float __gc [num_pts_new] ;

		for (int i = 0 ; i < num_pts_new ; i++)
		{
			mzs[i] = vectX[i] ; 
			intensities[i] = vectY[i] ; 
		}
		delete interp ; 
		return num_pts_new ; 
	}

	void Utils::Apodize(double minX, double maxX, double sampleRate, int apexPositionPercent, float (&intensities) __gc [], DeconToolsV2::Readers::ApodizationType type)
	{
		float __nogc *arrIntensities = __nogc new float [intensities->Length] ; 
		for (int i = 0 ; i < intensities->Length ; i++)
			arrIntensities[i] = intensities[i] ; 
		Engine::Utilities::Apodization::Apodize(minX, maxX, sampleRate, false, (Engine::Utilities::ApodizationType) type, arrIntensities, 
			intensities->Length, apexPositionPercent) ; 
		for (int i = 0 ; i < intensities->Length ; i++)
			intensities[i] = arrIntensities[i] ; 
		delete [] arrIntensities ; 
	}

	void Utils::UnApodize(float (&intensities) __gc[], DeconToolsV2::Readers::ApodizationType type)
	{
	}
	void Utils::FourierTransform(float (&intensities) __gc [])
	{
		float __nogc *arrIntensities = __nogc new float [intensities->Length] ; 
		for (int i = 0 ; i < intensities->Length ; i++)
			arrIntensities[i] = intensities[i] ; 
		Engine::FFT::realft(intensities->Length, arrIntensities, 1) ; 
		for (int i = 0 ; i < intensities->Length ; i++)
			intensities[i] = arrIntensities[i] ; 
		delete [] arrIntensities ; 
	}
	void Utils::InverseFourierTransform(float (&intensities) __gc [])
	{
		float __nogc *arrIntensities = __nogc new float [intensities->Length] ; 
		for (int i = 0 ; i < intensities->Length ; i++)
			arrIntensities[i] = intensities[i] ; 
		Engine::FFT::realft(intensities->Length, arrIntensities, -1) ; 
		for (int i = 0 ; i < intensities->Length ; i++)
			intensities[i] = arrIntensities[i] ; 
		delete [] arrIntensities ; 
	}
}