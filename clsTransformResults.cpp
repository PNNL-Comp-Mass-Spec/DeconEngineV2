// Written by Navdeep Jaitly and Anoop Mayampurath for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "clstransformresults.h"
#using <mscorlib.dll>
#include <algorithm>
namespace DeconToolsV2 
{
	namespace Results
	{
		clsTransformResults::clsTransformResults(void)
		{
			mobj_lcms_results = NULL ; 
			menmFileType = DeconToolsV2::Readers::FileType::UNDEFINED ; 
		}

		clsTransformResults::~clsTransformResults(void)
		{
			if (mobj_lcms_results != NULL)
			{
				delete mobj_lcms_results ; 
				mobj_lcms_results = NULL ; 
			}
		}
		
		void clsTransformResults::SetLCMSTransformResults(Engine::Results::LCMSTransformResults *results)
		{
			if (mobj_lcms_results != NULL)
			{
				delete mobj_lcms_results ; 
				mobj_lcms_results = NULL ; 
			}
			mobj_lcms_results = results ; 
		}

		int clsTransformResults::GetMinScan()
		{
			if(mobj_lcms_results == NULL)
				return -1 ; 
			return mobj_lcms_results->GetMinScan() ;
		}

		int clsTransformResults::GetMaxScan()
		{
			if(mobj_lcms_results == NULL)
				return -1 ; 
			return mobj_lcms_results->GetMaxScan() ;
		}

		bool clsTransformResults::IsDeisotoped()
		{
			if (mobj_lcms_results == NULL)
				throw gcnew System::Exception("No results stored.") ;
			return mobj_lcms_results->IsDeisotoped() ; 
		}

		
		// Used by the stl algorithm sort to sort std::vector of peaks in descending order of mdbl_intensity.
		bool RawPeaksIntensityComparison(Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo> &pk1, Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo> &pk2)
		{
			if (pk1.mobj_peak.mflt_intensity < pk2.mobj_peak.mflt_intensity)
				return true ; 
			if (pk1.mobj_peak.mflt_intensity > pk2.mobj_peak.mflt_intensity)
				return false ; 
			return pk1.mobj_peak.mflt_mz < pk2.mobj_peak.mflt_mz ; 
		}

		void clsTransformResults::GetRawDataSortedInIntensity(array<DeconToolsV2::Results::clsLCMSPeak^>^ (&lcms_peaks))
		{
			if (mobj_lcms_results == NULL)
			{
				lcms_peaks = nullptr ; 
				return ; 
			}

			std::vector<Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo> > vectPeaks ; 
			int num_peaks = mobj_lcms_results->GetNumPeaks() ; 
			vectPeaks.reserve(num_peaks) ; 
			mobj_lcms_results->GetAllPeaks(vectPeaks) ; 
			std::sort(vectPeaks.begin(), vectPeaks.end(), &RawPeaksIntensityComparison) ; 

			lcms_peaks = gcnew array<DeconToolsV2::Results::clsLCMSPeak^>(num_peaks); 
			int min_scan = mobj_lcms_results->GetMinScan() ; 
			int max_scan = mobj_lcms_results->GetMaxScan() ; 
			Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo> pk ; 

			for (int pk_num = 0 ; pk_num < num_peaks ; pk_num++)
			{
				mint_percent_done = (pk_num * 100)/num_peaks ; 
				pk = vectPeaks[pk_num] ; 
				lcms_peaks[pk_num] = gcnew clsLCMSPeak(pk.mint_scan_num, pk.mobj_peak.mflt_mz, pk.mobj_peak.mflt_intensity) ; 
			}
			mint_percent_done = 100 ; 
		}

		void clsTransformResults::GetRawData(array<DeconToolsV2::Results::clsLCMSPeak^>^ (&lcms_peaks))
		{
			if (mobj_lcms_results == NULL)
			{
				lcms_peaks = nullptr ; 
				return ; 
			}

			int num_peaks = mobj_lcms_results->GetNumPeaks() ; 
            lcms_peaks = gcnew array<DeconToolsV2::Results::clsLCMSPeak^>(num_peaks);
			int min_scan = mobj_lcms_results->GetMinScan() ; 
			int max_scan = mobj_lcms_results->GetMaxScan() ; 
			Engine::Results::LCMSPeak<Engine::Results::PeakMinInfo> pk ; 

			for (int pk_num = 0 ; pk_num < num_peaks ; pk_num++)
			{
				mint_percent_done = (pk_num * 100)/num_peaks ; 
				pk = mobj_lcms_results->GetPeak(pk_num) ; 
				lcms_peaks[pk_num] = gcnew clsLCMSPeak(pk.mint_scan_num, pk.mobj_peak.mflt_mz, pk.mobj_peak.mflt_intensity) ; 
			}
			mint_percent_done = 100 ; 
		}
		
		
        void clsTransformResults::GetSIC(int min_scan, int max_scan, float mz, float mz_tolerance, array<float> ^ (&peak_intensities))
		{
			std::vector<float> vect_intensities ; 
			mobj_lcms_results->GetSIC(min_scan, max_scan, mz-mz_tolerance, mz+mz_tolerance, vect_intensities) ; 
			int num_scans = max_scan - min_scan + 1 ; 
            peak_intensities = gcnew array<float>(num_scans);

			for (int scan_num = min_scan ; scan_num <= max_scan ; scan_num++)
			{
				peak_intensities[scan_num-min_scan] = vect_intensities[scan_num-min_scan] ; 
			}
		}

        void clsTransformResults::GetScanPeaks(int scan_num, array<float> ^ (&peak_mzs), array<float> ^ (&peak_intensities))
		{
			std::vector<float> vect_mzs ; 
			std::vector<float> vect_intensities ; 

			mobj_lcms_results->GetScanPeaks(scan_num, vect_mzs, vect_intensities) ; 

			int num_pts = vect_intensities.size() ; 
            peak_mzs = gcnew array<float>(num_pts);
            peak_intensities = gcnew array<float>(num_pts);

			for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
			{
				peak_mzs[pt_num] = vect_mzs[pt_num] ; 
				peak_intensities[pt_num] = vect_intensities[pt_num] ; 
			}
		}

		int clsTransformResults::GetNumPeaks()
		{
			return mobj_lcms_results->GetNumPeaks() ; 
		}

		void clsTransformResults::ReadResults(System::String ^fileName)
		{
			if (mobj_lcms_results == NULL)
			{
				mobj_lcms_results = new Engine::Results::LCMSTransformResults() ; 
			}

			char fileNameCh[512] ; 
			DeconEngine::Utils::GetStr(fileName, fileNameCh) ; 
			mobj_lcms_results->LoadResults(fileNameCh) ; 
		}

		void clsTransformResults::WriteResults(System::String ^fileName, bool save_signal_range)
		{
			char fileNameCh[512] ; 
			try
			{
				DeconEngine::Utils::GetStr(fileName, fileNameCh) ; 
				mobj_lcms_results->SaveResults(fileNameCh, save_signal_range) ; 
			}
			catch (char *mesg)
			{
				System::String ^exception_msg = gcnew System::String(mesg) ; 
				throw gcnew System::Exception(exception_msg) ; 
			}
		}

		void clsTransformResults::WriteScanResults(System::String ^fileName)
		{
			char fileNameCh[512] ; 
			try
			{
				DeconEngine::Utils::GetStr(fileName, fileNameCh) ; 
				if (menmFileType != DeconToolsV2::Readers::FileType::ICR2LSRAWDATA)
					mobj_lcms_results->SaveScanResults(fileNameCh, false) ; 
				else
					mobj_lcms_results->SaveScanResults(fileNameCh, true) ; 
			}
			catch (char *mesg)
			{
				System::String ^exception_msg = gcnew System::String(mesg) ; 
				throw gcnew System::Exception(exception_msg) ; 
			}
		}

	}
}