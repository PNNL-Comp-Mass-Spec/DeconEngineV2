// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include ".\clstransformfile.h"
#using <mscorlib.dll>
#include "DeconEngineUtils.h"
namespace DeconToolsV2
{
	clsTransformFile::clsTransformFile(void)
	{
	}

	clsTransformFile::~clsTransformFile(void)
	{
	}

	void clsTransformFile::ReadFile(System::String *file_name, DeconToolsV2::Results::clsTransformResults *results, 
		Peaks::clsPeakProcessorParameters *peak_parameters, HornTransform::clsHornTransformParameters *transform_parameters)
	{
	}

	void clsTransformFile::WritePeakParameters(std::fstream &fout, Peaks::clsPeakProcessorParameters *peak_parameters)
	{
		// In a wierd pseudo xml/raw file format, we will write out tags in text
		// but keep all the data in binary. 
		// We will not use a specialized SAX parser because our vocabulary on the XML file 
		// will not be increasing to a point where we actually want to go through the effort
		// of incorporating an XML parser with our binary data. 
		if (peak_parameters != NULL)
		{
			fout<<"<PeakParameters>" ; 
			fout<<"<MinBackgroundRatio>"<<peak_parameters->get_PeakBackgroundRatio()<<"</MinBackgroundRatio>" ; 
			
			fout<<"<PeakFitType>" ; 
			switch(peak_parameters->get_PeakFitType())
			{
				case Peaks::APEX:
					fout<<"APEX" ; 
					break ; 
				case Peaks::QUADRATIC:
					fout<<"QUADRATIC" ; 
					break ; 
				case Peaks::LORENTZIAN:
					fout<<"LORENTZIAN" ; 
					break ; 
				default:
					break ; 
			}
			fout<<"</PeakFitType>";
			
			fout<<"<S2NThreshold>"<<peak_parameters->get_SignalToNoiseThreshold()<<"</S2NThreshold>"; 
			fout<<"</PeakParameters>" ; 
		}

	}


	void clsTransformFile::WriteTransformParameters(std::fstream &fout, 
					HornTransform::clsHornTransformParameters *transform_parameters)
	{
		if (transform_parameters != NULL)
		{
			fout<<"<HornTransformParameters>" ; 
			fout<<"<MaxCharge>"<<transform_parameters->get_MaxCharge()<<"</MaxCharge>" ; 
			fout<<"<NumPeaksForShoulder>"<<transform_parameters->get_NumPeaksForShoulder()<<"</NumPeaksForShoulder>" ; 
			fout<<"<MaxMW>"<<transform_parameters->get_MaxMW()<<"</MaxMW>" ; 
			fout<<"<MaxFit>"<<transform_parameters->get_MaxFit()<<"</MaxFit>" ; 
			fout<<"<CCMass>"<<transform_parameters->get_CCMass()<<"</CCMass>" ; 
			fout<<"<DeleteIntensityThreshold>"<<transform_parameters->get_DeleteIntensityThreshold()<<"</DeleteIntensityThreshold>" ; 
			fout<<"<MinTheoreticalIntensityForScore>"<<transform_parameters->get_MinIntensityForScore()<<"</MinTheoreticalIntensityForScore>" ; 
			fout<<"<O16O18Media>"<<transform_parameters->get_O16O18Media()<<"</O16O18Media>" ; 
			fout<<"</HornTransformParameters>" ; 
		}
	}


	void clsTransformFile::WritePeaks(std::fstream &fout, DeconToolsV2::Results::clsTransformResults *results)
	{
		const int PK_SIZE = sizeof(Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo>) ; 
		int num_peaks = results->GetNumPeaks() ; 
		fout<<"<PeakResults><NumPeaks>"<<num_peaks<<"</NumPeaks>" ;
		fout<<"<Peaks>" ; 

		char *peaks = new char [num_peaks * PK_SIZE] ; 

		for (int pk_num = 0 ; pk_num < num_peaks ; pk_num++)
		{
			*((Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo> *)&peaks[pk_num])
				= results->mobj_lcms_results->GetPeak(pk_num) ; 
		}
		fout.write(peaks, num_peaks * PK_SIZE) ; 
		fout<<"</Peaks>" ; 
		delete [] peaks ; 
	}

	void clsTransformFile::WriteFile(System::String *file_name, DeconToolsV2::Results::clsTransformResults *results, 
		Peaks::clsPeakProcessorParameters *peak_parameters, HornTransform::clsHornTransformParameters *transform_parameters)
	{
		char file_name_ch[512] ; 
		DeconEngine::Utils::GetStr(file_name, file_name_ch) ; 
		WriteFile(file_name_ch, results, peak_parameters, transform_parameters) ; 
	}


	
	
	void clsTransformFile::WriteFile(char *file_name, DeconToolsV2::Results::clsTransformResults *results, 
		Peaks::clsPeakProcessorParameters *peak_parameters, HornTransform::clsHornTransformParameters *transform_parameters)
	{
		std::fstream fout(file_name, std::ios::binary) ; 

		// Write out descriptors. Some information is unavailable for now.
		// i.e. what is the start position of the peaks data block and 
		// the transform data block. Fill in place holders for now.
		// Once the data is actually written out, we can return to the stream and
		// write it. 
		float version_num = 1;
		fout<<"<Version>"<<version_num<<"</version>" ; 
		WritePeakParameters(fout, peak_parameters) ; 
		WriteTransformParameters(fout, transform_parameters) ; 
		WritePeaks(fout, results) ;
		fout.close() ; 
	}
}