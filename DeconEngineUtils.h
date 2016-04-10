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
#using <mscorlib.dll>
#include <vector>
#include "TheoreticalProfile/MolecularFormula.h"
#include "clsPeak.h"
#include "PeakProcessor/PeakProcessor.h"
#include "clsRawDataPreprocessOptions.h" 

namespace DeconEngine
{
	public ref class Utils
	{
	public:
        static void SetData(std::vector<double> &vectData, array<float> ^ (&data));
        static void SetData(std::vector<float> &vectData, array<float> ^ (&data));
        static void GetData(std::vector<double> &vectData, array<float> ^ (&data));
        static void GetData(std::vector<float> &vectData, array<float> ^ (&data));
		static void GetStr(System::String ^src, char *dest) ;
		static void GetStr(const char *src, System::String ^*dest) ; 
        static double GetAverage(array<float> ^ (&intensities), float maxIntensity);
		static double GetAverage(std::vector<double> &intensities, float maxIntensity) ; 
		static double Utils::GetTIC(double min_mz, double max_mz, std::vector<double> &mzs, std::vector<double> &intensities, float minIntensity, 
			double &bpi, double &bp_mz) ; 
		static void ConvertElementTableToFormula(Engine::TheoreticalProfile::AtomicInformation &elemental_isotope_composition, 
			System::Collections::Hashtable^ elementCounts, Engine::TheoreticalProfile::MolecularFormula &formula) ; 
		static void SetPeaks(Engine::PeakProcessing::PeakData &pk_data, array<DeconToolsV2::Peaks::clsPeak^>^ (&peaks)) ; 
        static void SavitzkyGolaySmooth(short num_left, short num_right, short order, array<float> ^ (&mzs), array<float> ^ (&intensities));
        static int ZeroFillUnevenData(array<float> ^ (&mzs), array<float> ^ (&intensities), int maxPtsToAdd);
        static void Apodize(double minX, double maxX, double sampleRate, int apexPositionPercent, array<float> ^ (&intensities),
			DeconToolsV2::Readers::ApodizationType type) ; 
        static void UnApodize(array<float> ^ (&intensities), DeconToolsV2::Readers::ApodizationType type);
        static void FourierTransform(array<float> ^ (&intensities));
        static void InverseFourierTransform(array<float> ^ (&intensities));
	} ; 
}