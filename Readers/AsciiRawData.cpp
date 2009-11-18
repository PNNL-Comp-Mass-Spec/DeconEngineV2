// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0


#include "AsciiRawData.h"
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
		AsciiRawData::~AsciiRawData(void)
		{
			Clear() ; 
		};

		AsciiRawData::AsciiRawData(void)
		{
			mint_file_handle =-1 ; 
			Clear() ; 
		};

		void AsciiRawData::Clear()
		{
			mint_percent_done = 0 ; 
			mint_num_spectra = 0 ; 
			mint_num_points_in_scan = 0 ;
			mvect_scan_start_position.clear() ; 
			if (mint_file_handle != -1)
			{
				_close(mint_file_handle) ; 
				mint_file_handle = -1 ;
			}
		}

		void AsciiRawData::Close() 
		{
			if (mint_file_handle != -1)
			{
				_close(mint_file_handle) ; 
				mint_file_handle = -1 ;
			}
		}

		const char* AsciiRawData::GetFileName()
		{
			return marr_file_name ; 
		}

		double AsciiRawData::GetScanTime(int scan_num)
		{
			if ((int) mvect_scan_time.size() <= scan_num)
				return mvect_scan_time[scan_num-1] ; 
			return 0 ; 
		}

		void AsciiRawData::Load(char *file_n) 
		{
			Clear() ; 
			strcpy(marr_file_name, file_n) ; 
			mint_file_handle = _open(marr_file_name, _O_RDONLY | _O_BINARY );

			__int64 end_pos = _lseeki64(mint_file_handle, 0, SEEK_END );
			__int64 start_pos = _lseeki64(mint_file_handle, 0, SEEK_SET );

			__int64	pos = 0 ;

			int mint_last_scan = -1 ; 
			int leftover_length = 0 ; 

			float scan_time = 0 ;
			float last_scan_time = -1 * FLT_MAX ; 

			double mz=0, intensity = 0 ; 
			char *temp_buffer = new char[MAX_SCAN_SIZE] ; 
			std::vector<double> vect_mz ; 
			std::vector<double> vect_intensity ; 
			unsigned int line_start_index = 0 ; 

			mchar_delimiter = ',' ; 
			unsigned int num_read ;
			bool first_line = true ; 

			int temp_copy_length = 511 ; 
			char *temp_copy = new char [temp_copy_length+1] ; 

			while(!_eof(mint_file_handle))
			{
				num_read = _read(mint_file_handle, &temp_buffer[leftover_length], MAX_SCAN_SIZE-leftover_length) + leftover_length ;
				for (unsigned int current_index = 0 ; current_index < num_read ; current_index++)
				{
					if (temp_buffer[current_index] == '\n')
					{
						if (first_line)
						{
							// look for mchar_delimiter.. 
							first_line = false ; 
							int pt_index = 0 ; 
							while(pt_index < current_index)
							{
								if (temp_buffer[pt_index] != '.' && 
										(temp_buffer[pt_index] < '0' || temp_buffer[pt_index] > '9'))
								{
									mchar_delimiter = temp_buffer[pt_index] ; 
									break ; 
								}
								pt_index++ ; 
							}
						}
						mint_percent_done = (int) ((100.0 * ((pos + current_index)*1.0)) / (end_pos+1.0)) ; 

						unsigned int current_len = current_index - line_start_index  ; 
						if (current_len > temp_copy_length)
						{
							delete [ ] temp_copy ;
							temp_copy_length = current_len ; 
							temp_copy = new char [temp_copy_length+1] ; 
						}
						memcpy(temp_copy, &temp_buffer[line_start_index], current_len) ; 
						temp_copy[current_len] = '\0' ; 

						mz = atof(temp_copy) ; 
						unsigned int next_val_index = 1 ; 
						while(next_val_index < current_len && temp_copy[next_val_index] != mchar_delimiter)
							next_val_index++ ; 
						intensity = atof(&temp_copy[next_val_index+1]) ; 

						next_val_index++ ; 
						while(next_val_index < current_len && temp_copy[next_val_index] != mchar_delimiter)
							next_val_index++ ; 
						if (next_val_index < current_len)
							scan_time = (float) atof(&temp_copy[next_val_index+1]) ; 
						else
							// no scan provided. Just set to 0.
							scan_time = 0.0 ; 

						if (last_scan_time != scan_time)
						{
							if (last_scan_time != -1 * FLT_MAX)
							{
								double bpi_mz = 0 ;
								double bpi = GetBasePeakIntensity(vect_mz, vect_intensity, bpi_mz) ; 
								double tic = GetTotalIonCount(vect_mz, vect_intensity) ; 

								mvect_scan_time.push_back(scan_time) ; 
								mvect_scan_tic.push_back(tic) ; 
								mvect_scan_bpi.push_back(bpi) ; 
								mvect_scan_bpi_mz.push_back(bpi_mz) ; 
							}

							mvect_scan_start_position.push_back(line_start_index + pos - leftover_length) ; 
							last_scan_time = scan_time ; 
							mint_num_spectra++ ; 
							vect_mz.clear() ; 
							vect_intensity.clear() ; 
						}
						vect_mz.push_back(mz) ; 
						vect_intensity.push_back(intensity) ; 
						line_start_index = current_index + 1 ; 
					}
				}
				
				leftover_length = num_read - line_start_index ;
				int num_copied = 0 ; 
				while(line_start_index < num_read)
				{
					temp_buffer[num_copied++] = temp_buffer[line_start_index++] ; 
				}
				line_start_index = 0 ; 
				if (num_copied != leftover_length)
				{
					std::cerr<<"PROBLEMO"<<std::endl ; 
				}
				pos+= (num_read-leftover_length) ; 
			}
			delete [] temp_copy ; 

			// last line ? 
			if (leftover_length != 0)
			{
				// there is one line left behind. 
				mz = atof(&temp_buffer[line_start_index]) ; 

				unsigned int next_val_index = line_start_index+1 ; 

				while(next_val_index < num_read && temp_buffer[next_val_index] != mchar_delimiter)
					next_val_index++ ; 
				intensity = (float)atof(&temp_buffer[next_val_index+1]) ; 

				next_val_index++ ; 
				while(next_val_index < num_read && temp_buffer[next_val_index] != mchar_delimiter)
					next_val_index++ ; 
				scan_time = (float)atof(&temp_buffer[next_val_index+1]) ; 

				if (last_scan_time != scan_time)
				{
					double bpi_mz = 0 ;
					double bpi = GetBasePeakIntensity(vect_mz, vect_intensity, bpi_mz) ; 
					double tic = GetTotalIonCount(vect_mz, vect_intensity) ; 

					mvect_scan_time.push_back(scan_time) ; 
					mvect_scan_tic.push_back(tic) ; 
					mvect_scan_bpi.push_back(bpi) ; 
					mvect_scan_bpi_mz.push_back(bpi_mz) ; 

					mvect_scan_start_position.push_back(line_start_index + pos - leftover_length) ; 
					last_scan_time = scan_time ; 
					mint_num_spectra++ ; 
					vect_mz.clear() ; 
					vect_intensity.clear() ; 

					mvect_scan_start_position.push_back(pos) ; 
					last_scan_time = scan_time ; 
					mint_num_spectra++ ; 
				}
				vect_mz.push_back(mz) ; 
				vect_intensity.push_back(intensity) ; 
			}
			mvect_scan_start_position.push_back(pos+leftover_length) ; 

			if (last_scan_time == scan_time)
			{
				double bpi_mz = 0 ;
				double bpi = GetBasePeakIntensity(vect_mz, vect_intensity, bpi_mz) ; 
				double tic = GetTotalIonCount(vect_mz, vect_intensity) ; 

				mvect_scan_time.push_back(scan_time) ; 
				mvect_scan_tic.push_back(tic) ; 
				mvect_scan_bpi.push_back(bpi) ; 
				mvect_scan_bpi_mz.push_back(bpi_mz) ; 

				mvect_scan_start_position.push_back(line_start_index + pos - leftover_length) ; 
				last_scan_time = scan_time ; 
				mint_num_spectra++ ; 
				vect_mz.clear() ; 
				vect_intensity.clear() ; 
			}

			delete [] temp_buffer ; 

		}


		bool AsciiRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num)
		{
			int num_pts = mint_num_points_in_scan ; 
			return GetRawData(mzs, intensities, scan_num, num_pts) ; 
		}

		double AsciiRawData::GetSignalRange(int scan_num) 
		{
			// only returns a value if the current scan is the one we are asking for. 
			return 0 ; 
		}

		bool AsciiRawData::GetRawData(std::vector<double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
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

			// we already know where to start reading because we tracked it before.
			startOffset = mvect_scan_start_position[scan_num] ; 
			stopOffset = mvect_scan_start_position[scan_num+1] ; 

			//pos = _lseeki64(mint_file_handle, 0, SEEK_CUR);
			//pos = _lseeki64(mint_file_handle, startOffset-pos, SEEK_CUR);

			pos = _lseeki64(mint_file_handle, startOffset, SEEK_SET);

			// there's an extra carriage return in the end.
			char *temp_buffer = new char[stopOffset - startOffset+1] ; 
			int num_read = _read(mint_file_handle, temp_buffer, (int)(stopOffset - startOffset));
			temp_buffer[num_read] = '\0' ;

			int pt_num = 0 ; 

			int read_scan_num = 0 ;
			double mz=0, intensity = 0 ; 
			unsigned int line_start_index = 0 ; 

			int temp_copy_length = 512 ; 
			char *temp_copy = new char [temp_copy_length+1] ; 
			for (unsigned int current_index = 0 ; current_index < num_read ; current_index++)
			{
				if (temp_buffer[current_index] == '\n')
				{
					unsigned int current_len = current_index - line_start_index  ; 
					if (current_len > temp_copy_length)
					{
						delete [ ] temp_copy ;
						temp_copy_length = current_len ; 
						temp_copy = new char [temp_copy_length+1] ; 
					}
					memcpy(temp_copy, &temp_buffer[line_start_index], current_len) ; 
					temp_copy[current_len] = '\0' ; 

					mz = atof(temp_copy) ; 
					unsigned int next_val_index = 1 ; 
					while(next_val_index < current_len && temp_copy[next_val_index] != mchar_delimiter)
						next_val_index++ ; 
					intensity = atof(&temp_copy[next_val_index+1]) ; 

					mzs->push_back(mz) ; 
					intensities->push_back(intensity) ; 

					line_start_index = current_index + 1 ; 
				}
			}
			if (line_start_index < (unsigned int) num_read)
			{
				mz = atof(&temp_buffer[line_start_index]) ; 

				unsigned int next_val_index = line_start_index+1 ; 

				while(next_val_index < num_read && temp_buffer[next_val_index] != mchar_delimiter)
					next_val_index++ ; 
				intensity = atof(&temp_buffer[next_val_index+1]) ; 
				mzs->push_back(mz) ; 
				intensities->push_back(intensity) ;
			}

			delete [] temp_copy ; 
			delete [] temp_buffer ; 
			return true ; 
		}

		int AsciiRawData::GetNumScans()
		{
			return mint_num_spectra ; 
		}

		int AsciiRawData::GetFirstScanNum()
		{
			return 1 ; 
		}

		int AsciiRawData::GetScanSize()
		{
			return mint_num_points_in_scan ; 
		}
		

		double AsciiRawData::GetBasePeakIntensity(std::vector<double> &mzs, std::vector<double> &intensities, double &bpi_mz)
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
					bpi_mz = mzs[pt_num] ; 
				}
			}
			return max_intensity ; 
		}

		double AsciiRawData::GetTotalIonCount(std::vector<double> &mzs, std::vector<double> &intensities)
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

		void AsciiRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic)
		{
			intensities->clear() ; 
			scan_times->clear() ; 

			for (int scan_num = 0 ; scan_num < (int) mvect_scan_tic.size() ; scan_num++)
			{
				scan_times->insert(scan_times->begin(), mvect_scan_time.begin(), mvect_scan_time.end()) ; 
				if (base_peak_tic)
				{
					intensities->insert(intensities->begin(), mvect_scan_bpi.begin(), mvect_scan_bpi.end()) ; 
				}
				else
				{
					intensities->insert(intensities->begin(), mvect_scan_tic.begin(), mvect_scan_tic.end()) ; 
				}
			}
		}

	}
}