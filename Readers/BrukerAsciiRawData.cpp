// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0


#include "BrukerAsciiRawData.h"
#include "../Utilities/helpers.h" 
#include <fstream> 
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <float.h> 

namespace Engine 
{
	namespace Readers
	{
		BrukerAsciiRawData::~BrukerAsciiRawData(void)
		{
			Clear() ; 
		};

		BrukerAsciiRawData::BrukerAsciiRawData(void)
		{
			mint_file_handle =-1 ; 
			Clear() ; 
		};

		void BrukerAsciiRawData::Clear()
		{
			mint_last_scan_num = -1 ; 
			mint_num_spectra = 0 ; 
			mint_num_points_in_scan = 0 ;
			mvect_scan_start_position.clear() ; 
			if (mint_file_handle != -1)
			{
				_close(mint_file_handle) ; 
				mint_file_handle = -1 ;
			}
		}

		void BrukerAsciiRawData::Close() 
		{
			if (mint_file_handle != -1)
			{
				_close(mint_file_handle) ; 
				mint_file_handle = -1 ;
			}
		}

		const char* BrukerAsciiRawData::GetFileName()
		{
			return marr_file_name ; 
		}

		double BrukerAsciiRawData::GetScanTime(int scan_num)
		{
			if ((int) mvect_scan_time.size() <= scan_num)
				return mvect_scan_time[scan_num-1] ; 
			return 0 ; 
		}

		void BrukerAsciiRawData::Load(char *file_n) 
		{
			Clear() ; 

			strcpy(marr_file_name, file_n) ; 
			mint_file_handle = _open(marr_file_name, _O_RDONLY | _O_BINARY );
			mvect_scan_start_position.push_back(0) ; 
			mint_last_scan_num = 0 ; 
		}


		bool BrukerAsciiRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
			int num_pts = mint_num_points_in_scan ; 
			return GetRawData(mzs, intensities, scan_num, num_pts) ; 
		}

		double BrukerAsciiRawData::GetSignalRange(int scan_num) 
		{
			// only returns a value if the current scan is the one we are asking for. 
			if (mint_last_scan_num == scan_num)
				return mdbl_signal_range ; 
			return 0 ; 
		}

		bool BrukerAsciiRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
			// scan_nums are supposed to be 0 indexed for retrieval, but the function is expected to pass in 1 indexed. 
			// hence substract 1 from scan_num.


			// Do not allow 0 indexed entry.
			if (scan_num == 0)
				return false ; 

			scan_num = scan_num - 1 ; 

			__int64	startOffset ;
			__int64	stopOffset ;
			__int64	pos ;

			if (scan_num < mint_last_scan_num)
			{
				// we already know where to start reading because we tracked it before.
				startOffset = mvect_scan_start_position[scan_num] ; 
				stopOffset = mvect_scan_start_position[scan_num+1] ; 
			}
			else
			{
				// otherwise, scan through the scans till we get there.
				// yes the file will be scanned two times but we'll have to 
				// live with that for the simplicity of the read and to be able to 
				// use low level read commands which allow 64 bit reading. 

				startOffset = mvect_scan_start_position[mint_last_scan_num] ; 
				pos = _lseeki64(mint_file_handle, 0, SEEK_CUR);
				pos = _lseeki64(mint_file_handle, startOffset-pos, SEEK_CUR);

				while(!_eof(mint_file_handle) && mint_last_scan_num <= scan_num)
				{
					unsigned int num_read = _read(mint_file_handle, marr_buffer, MAX_SCAN_SIZE);

					if (num_read < 0)
						return false ; 
					// now go through this loaded data and set up positions first. 
					// start at index 1 because the end of the previous last loaded scan will be at the 
					// return character.
					for (unsigned int current_index = 1 ; current_index < num_read ; current_index++)
					{
						if (marr_buffer[current_index] == '\n')
						{
							mint_last_scan_num++ ; 
							mvect_scan_start_position.push_back(startOffset + current_index) ; 
						}
					}
				}
	
				// let the number of spectra grow if it wasn't set by other means. 
				if (mint_num_spectra < mint_last_scan_num)
					mint_num_spectra = mint_last_scan_num ;

				if (mint_last_scan_num <= scan_num)
				{
					// means this really was the last scan. 
					return false ; 
				}
				stopOffset = mvect_scan_start_position[scan_num+1] ; 
			}


			/* Seek the beginning of the file: */

			pos = _lseeki64(mint_file_handle, 0, SEEK_CUR);
			pos = _lseeki64(mint_file_handle, startOffset-pos, SEEK_CUR);

			if( pos == -1 )
			{
				std::cerr<<"Could not read data for scan = "<<scan_num<<" at location "<<startOffset<<std::endl ; 
				exit(1) ; 
			}

			// there's an extra carriage return in the end.
			int num_read = _read(mint_file_handle, marr_buffer, (unsigned int)(stopOffset - startOffset));
			double scan_time = atof(marr_buffer) ; 
			
			int index = -1 ; 
			int colNum = 0 ; 
			while(index < num_read && colNum != 7)
			{
				index++ ; 
				while(index < num_read && marr_buffer[index] != ',')
					index++ ; 
				colNum++ ;
			}
			if (colNum != 7)
				return false ; 
			
			int num_pts_in_scan = atoi(&marr_buffer[index+1]) ; 

			int pt_num = 0 ; 
			mzs->reserve(num_pts_in_scan) ; 
			intensities->reserve(num_pts_in_scan) ;

			index++ ; 
			while(index < num_read && marr_buffer[index] != ',')
				index++ ; 
			index++ ; 
			int startIndex1 = index  ; 

			char temp[32] ; 
			while(pt_num < num_pts_in_scan && index < num_read )
			{

				while(index < num_read && marr_buffer[index] != ' ')
					index++ ; 
				int startIndex2 = index ; 

				strncpy(temp, &marr_buffer[startIndex1], startIndex2-startIndex1);
				temp[startIndex2-startIndex1] = '\0' ; 
				double mz = atof(temp) ; 

				while(index < num_read && marr_buffer[index] != ',')
					index++ ; 
				startIndex1 = index + 1 ; 
				index++ ; 

				strncpy(temp, &marr_buffer[startIndex2], startIndex1-startIndex2-1);
				temp[startIndex1-startIndex2-1] = '\0' ; 
				int intensity = atoi(temp) ; 

				pt_num++ ; 
				mzs->push_back(mz) ; 
				intensities->push_back(intensity) ; 
			}

			return true ; 
		}

		int BrukerAsciiRawData::GetNumScans()
		{
			return mint_num_spectra ; 
		}
		int BrukerAsciiRawData::GetScanSize()
		{
			return mint_num_points_in_scan ; 
		}
		

		double BrukerAsciiRawData::GetBasePeakIntensity(std::vector<double> &mzs, std::vector<double> &intensities)
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

		double BrukerAsciiRawData::GetTotalIonCount(std::vector<double> &mzs, std::vector<double> &intensities)
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

		void BrukerAsciiRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{
			intensities->clear() ; 
			scan_times->clear() ; 

			std::vector<double> scan_mzs ; 
			std::vector<double> scan_intensities ; 

			// remember that we are not going to know the number of spectra to begin with at it will update itself each time.
			bool got_data = true ; 
			int scan_num = 1 ; 
			while(got_data)
			{
				// its time to read in that scan.
				got_data = GetRawData(&scan_mzs, &scan_intensities, scan_num) ; 
				if (!got_data)
					break ; 

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
				scan_num++ ; 
			}
		}

	}
}