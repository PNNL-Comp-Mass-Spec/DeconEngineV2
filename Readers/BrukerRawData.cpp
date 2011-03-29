// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0


#include "brukerrawdata.h"
#include "../Utilities/helpers.h" 
#include <fstream> 
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <float.h> 
#include <stdio.h>
#include <errno.h>


namespace Engine 
{
	namespace Readers
	{
		BrukerRawData::~BrukerRawData(void)
		{			
			if (marr_data_block != 0)
			{
				delete [] marr_data_block ;
				delete [] marr_temp_data_block ;
				delete [] marr_data_block_copy ; 
				marr_data_block = 0 ; 
				marr_temp_data_block = 0 ; 
				marr_data_block_copy = 0 ;
			} ;
			if (marr_serName != NULL)
			{
				delete [] marr_serName ; 
				marr_serName = 0 ; 
			}
			if (marr_headerName != NULL)
			{
				delete [] marr_headerName ; 
				marr_headerName = 0 ; 
			}

			mint_last_scan_num = -1 ; 
		};

		BrukerRawData::BrukerRawData(void)
		{
			marr_data_block = NULL ; 
			marr_data_block_copy = NULL ; 
			marr_temp_data_block = NULL ; 
			marr_headerName = new char[512] ; 
			marr_serName = new char[512] ; 
			mint_last_scan_num = -1 ; 
		};

		const char* BrukerRawData::GetFileName()
		{
			return marr_serName ; 
		}


		void  BrukerRawData::SetCalibrator(Engine::Calibrations::CCalibrator *calib)
				{
					mobj_calibrator = calib ;
					SetDataSize(mobj_calibrator->GetSize());
					mint_num_spectra = GetNumSpectraFromFileSizeInfo();
				}

		void BrukerRawData::Open(char *header_n, char *ser_file_name)
		{
	

			if (IsDir(ser_file_name))
			{
				strcpy(marr_serName, ser_file_name) ; 
				strcpy(marr_headerName, ser_file_name) ; 
				strcat(marr_headerName, "\\acqus") ; 
				strcat(marr_serName, "\\ser") ; 

			}
			else
			{
				// assuming an acqu file was selected.. go back to the name of the folder.
				char fileName[512] ;
				int pos = strlen(ser_file_name)-1 ; 
				while(pos >= 0 && ser_file_name[pos] != '\\' 
					&& ser_file_name[pos] != '\/')
				{
					pos-- ; 
				}
				strncpy(marr_serName, ser_file_name, pos) ; 
				strncpy(marr_headerName, ser_file_name, pos) ; 

				strcat(&marr_headerName[pos], "\\acqus") ; 
				strcat(&marr_serName[pos], "\\ser") ; 

				strcpy(fileName, ser_file_name) ; 

			}

			FindHeaderParams() ; 
			mint_num_spectra = GetNumSpectraFromFileSizeInfo();

		}

		double BrukerRawData::GetScanTime(int scan_num)
		{
			return scan_num ; 
		}

		void BrukerRawData::Load(char *file_n) 
		{
			Open("acqus", file_n) ; 
		}



	




		void BrukerRawData::SetDataSize(int sz) 
		{
			mint_num_points_in_scan = sz ; 
			if (marr_data_block != 0)
				delete [] marr_data_block ; 
			marr_data_block = new int [mint_num_points_in_scan] ; 
			if (marr_data_block_copy != 0)
				delete [] marr_data_block_copy ; 
			marr_data_block_copy = new int [mint_num_points_in_scan] ; 

			if (marr_temp_data_block != 0)
				delete [] marr_temp_data_block ; 
			marr_temp_data_block = new float [mint_num_points_in_scan] ; 
		}

		int BrukerRawData::FindHeaderParams()
		{
			std::string line;
			std::string sub;
			std::string::size_type pos;
			double ML1, ML2, SW_h, FR_low ; 
			int byte_order ; 
			int TD, NF ; 
			

			std::ifstream acqusHeader(marr_headerName);  // Open header
			
			if(!acqusHeader.is_open())
				return -1; // File does not exist!

			// Read first line
			getline(acqusHeader,line);
			while(acqusHeader)
			{	
				// Find the name,value pair within the acqus header file.
				if(pos=line.find("##$ML1= ") != -1) { pos+=7; ML1 = Helpers::to_double(line.substr(pos,line.length()-pos-1)); }
				if(pos=line.find("##$ML2= ") != -1) { pos+=7; ML2 = Helpers::to_double(line.substr(pos,line.length()-pos-1)); }
				if(pos=line.find("##$SW_h= ") != -1) { pos+=8; SW_h = Helpers::to_double(line.substr(pos,line.length()-pos-1)); }
				if(pos=line.find("##$BYTORDA= ") != -1) { pos+=12; byte_order = Helpers::to_int(line.substr(pos,line.length()-pos-1)); }
				if(pos=line.find("##$TD= ") != -1) 
				{ 
					pos+=6; 
					TD = Helpers::to_int(line.substr(pos,line.length()-pos-1)); 
				}
				if(pos=line.find("##$FR_low= ") != -1) { pos+=9; FR_low = Helpers::to_double(line.substr(pos,line.length()-pos-1)); }
				if(pos=line.find("##$NF= ") != -1) { pos+=6; NF = Helpers::to_int(line.substr(pos,line.length()-pos-1)); }
				getline(acqusHeader,line);
			}

			mint_num_spectra = NF ; 
			// Done.
			acqusHeader.close();

			// It's in ICR-2LS...
			SW_h *= 2.0;
			if(SW_h > FR_low)
				FR_low = 0.0;

			Calibrations::CCalibrator *calib = new Calibrations::CCalibrator(A_OVER_F_PLUS_B) ; 

			SetDataSize(TD) ; 
			calib->SetSize(TD) ; 
			calib->SetLowMassFrequency(FR_low) ; 
			calib->SetSampleRate(SW_h);
			calib->SetCalibrationEquationParams(ML1,ML2,0.0);
			SetCalibrator(calib) ; 

			return 0; // success


			//[gord] delete later - this has been moved to method: GetNumSpectraFromFileSizeInfo, called elsewhere
			//// Now we need to get the size of the data so we can find out the number of scans. 
			//int fh;
			//fh = _open(marr_serName, _O_RDONLY | _O_BINARY );
			//if (fh ==  ENOENT || fh == -1 )
			//{
			//	// try using the fid extention instead of the .ser business.
			//	int len = strlen(marr_serName) ; 
			//	marr_serName[len-3] = 'f' ; 
			//	marr_serName[len-2] = 'i' ; 
			//	marr_serName[len-1] = 'd' ; 
			//	fh = _open(marr_serName, _O_RDONLY | _O_BINARY );
			//}
			///* Seek the beginning of the file: */
			//__int64 pos64 = 0 ;
			//pos64 = _lseeki64(fh, 0, SEEK_END );
			//__int64		blockSizeInBytes = (__int64) (sizeof(int) * mint_num_points_in_scan) ; 
			//mint_num_spectra = (int)((pos64+2) / blockSizeInBytes) ; // add 2 just in case we have an exact multiple - 1.

			//_close( fh );
		}

		int	BrukerRawData::ReadSpectraFloats(int spectra_num)
		{
			int fh;
			__int64	startOffset;
			__int64 pos ; 
			int		blockSizeInBytes;
			// Prepare for data read
			blockSizeInBytes = sizeof(int) * mint_num_points_in_scan ;
			startOffset = ((_int64)spectra_num) *  ((__int64)blockSizeInBytes) ;
			// Read the data from the SER file into "intBlock"

			fh = _open(marr_serName, _O_RDONLY | _O_BINARY );
			if (fh == -1)
			{
				std::cerr<<" Could not open "<<marr_serName<<" perhaps it does not exist. Exiting"<<std::endl ; 
				exit(1) ; 
			}
			/* Seek the beginning of the file: */
			pos = _lseeki64(fh, startOffset, SEEK_SET );

			if( pos == -1 )
			{
				std::cerr<<"Could not read data for scan = "<<spectra_num<<" at location "<<startOffset<<std::endl ; 
				exit(1) ; 
			}

			int num_read = _read(fh, marr_data_block, blockSizeInBytes );
			_close( fh );
			return num_read ; 
		}

		bool BrukerRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
			int num_pts = mint_num_points_in_scan ; 
			return GetRawData(mzs, intensities, scan_num, num_pts) ; 
		}
		int BrukerRawData::GetParentScan(int scan_num)
		{
			//future work 
			return 0;
		}
		bool BrukerRawData::IsMSScan(int scan_num)
		{
			//future work
			return true;
		}

		int BrukerRawData:: GetMSLevel(int scan_num)
		{
			//future work 
			return 1;
		}
		
		double BrukerRawData::GetParentMz(int scan_num)
		{
			//future work
			return 0;
		}

		bool BrukerRawData::IsProfileScan(int scan_num) 
		{
			//future work
			return true;
		}
		

		double BrukerRawData::GetSignalRange(int scan_num) 
		{
			// only returns a value if the current scan is the one we are asking for. 
			if (mint_last_scan_num == scan_num)
				return mdbl_signal_range ; 
			return 0 ; 
		}

		bool BrukerRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
			// scan_nums are supposed to be 0 indexed for retrieval, but the function is expected to pass in 1 indexed. 
			// hence substract 1 from scan_num.
			

			// Do not allow 0 indexed entry.
			if (scan_num == 0)
				return false ; 

			scan_num = scan_num - 1 ; 
			const int int_size = sizeof(int) ; 
			if (mint_last_scan_num != scan_num)
			{
				int num_read = ReadSpectraFloats(scan_num) ; 
				mint_last_scan_num = scan_num ; 
				if (num_read <= 0)
					return false ; 
				memcpy((char *)marr_data_block_copy, (char *)marr_data_block, mint_num_points_in_scan*int_size) ; 
			}
			else
			{
				memcpy((char *)marr_data_block, (char *)marr_data_block_copy, num_pts*int_size) ; 
			}

			double min_intensity = DBL_MAX ; 
			double max_intensity = DBL_MIN ; 
			for (int i = 0 ; i < num_pts ; i++)
			{
				marr_temp_data_block[i] = (float) marr_data_block[i] ; 
				if (marr_data_block[i] < min_intensity)
					min_intensity = marr_data_block[i] ; 
				if (marr_data_block[i] > max_intensity)
					max_intensity = marr_data_block[i] ; 
			}
			mdbl_signal_range = max_intensity - min_intensity ; 
			int n = mobj_calibrator->GetRawPointsApplyFFT(marr_temp_data_block, mzs, intensities, num_pts) ; 
			// lets only take points whose mz is less than MAX_MZ.
			// these are going to be sorted so just start at the right end. 
			
			int pt_num;
			for (pt_num = 0 ; pt_num < num_pts ; pt_num++)
			{
				if((*mzs)[pt_num] > MAX_MZ)
					break ; 
			}
			mzs->resize(pt_num) ; 
			intensities->resize(pt_num) ; 
			return true ; 
		}

		int BrukerRawData::GetNumScans()
		{
			return mint_num_spectra ; 
		}
		int BrukerRawData::GetScanSize()
		{
			return mint_num_points_in_scan ; 
		}

		double BrukerRawData::GetBasePeakIntensity(std::vector<double> &mzs, std::vector<double> &intensities)
		{
			int num_pts = intensities.size() ; 
			if (num_pts == 0)
				return 0 ; 
			double max_intensity = -1 * DBL_MAX ; 
			for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
			{
				if (intensities[pt_num] > max_intensity && mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
				{
					max_intensity = intensities[pt_num] ; 
				}
			}
			return max_intensity ; 
		}

		double BrukerRawData::GetTotalIonCount(std::vector<double> &mzs, std::vector<double> &intensities)
		{

			int num_pts = intensities.size() ; 
			if (num_pts == 0)
				return 0 ; 

			double intensity_sum = 0 ; 
			for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
			{
				if (mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
				{
					intensity_sum += intensities[pt_num] ;
				}
			}
			double bg_intensity = intensity_sum / num_pts ; 

			int num_pts_considered = 0 ; 
			double min_intensity = bg_intensity * BACKGROUND_RATIO_FOR_TIC ; 

			intensity_sum = 0 ; 
			for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
			{
				if (intensities[pt_num] > min_intensity && mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
				{
					intensity_sum += intensities[pt_num] ;
				}
			}
			return intensity_sum ; 
		}

		void BrukerRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{
			intensities->clear() ; 
			scan_times->clear() ; 

			std::vector<double> scan_mzs ; 
			std::vector<double> scan_intensities ; 

			for (int scan_num = 1 ; scan_num < mint_num_spectra ; scan_num++)
			{
				// its time to read in that scan.
				bool got_data = GetRawData(&scan_mzs, &scan_intensities, scan_num) ; 
				if (!got_data)
					continue ; 

				scan_times->push_back(scan_num) ; 
				if (base_peak_tic)
				{
					double bp_intensity = GetBasePeakIntensity(scan_mzs, scan_intensities) ; 

					intensities->push_back(bp_intensity) ; 
				}
				else
				{
					double tic_intensity = GetTotalIonCount(scan_mzs, scan_intensities) ; 
					intensities->push_back(tic_intensity) ; 
				}
			}
		}

		int BrukerRawData::GetNumSpectraFromFileSizeInfo()
		{

			int numSpectra = 0;

			if (mint_num_points_in_scan==0)return 0 ;      // prevent divide by 0

			int fh;
			fh = _open(marr_serName, _O_RDONLY | _O_BINARY );
			if (fh ==  ENOENT || fh == -1 )
			{
				// try using the fid extention instead of the .ser business.
				int len = strlen(marr_serName) ; 
				marr_serName[len-3] = 'f' ; 
				marr_serName[len-2] = 'i' ; 
				marr_serName[len-1] = 'd' ; 
				fh = _open(marr_serName, _O_RDONLY | _O_BINARY );
			}
			/* Seek the beginning of the file: */
			__int64 pos64 = 0 ;
			pos64 = _lseeki64(fh, 0, SEEK_END );
			__int64		blockSizeInBytes = (__int64) (sizeof(int) * mint_num_points_in_scan) ; 
			numSpectra = (int)((pos64+2) / blockSizeInBytes) ; // add 2 just in case we have an exact multiple - 1.

			_close( fh );

			return numSpectra;


		}


	}
}
