#include "IMSRawData.h"
#include <map> 
#include <iostream>
#include <float.h>
#include <fstream> 
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>

using namespace	std	; 

namespace Engine 
{
	namespace Readers
	{
		IMSRawData::IMSRawData(void)
		{
			mdbl_k0 = 0 ; 
			mdbl_t0 =  0 ; 
			mdbl_min_mz = 10 ; 
			mdbl_scan_interval = 1 ; 
			mdbl_max_mz = 2500 ;
			mdbl_avg_tof_length = 0 ; 
			mint_frame_num = 0 ;
			mbln_is_multiplexed_data = false ; 
			mbln_is_adc_data = false ; 
		}
		IMSRawData::~IMSRawData(void) 
		{
			if (mvect_data.size() != 0)
				mvect_data.clear() ;
			if (mvect_mxed_data.size() != 0 ) 
				mvect_mxed_data.clear() ; 
			if (mvect_scan_bpi.size() != 0)
				mvect_scan_bpi.clear() ;
			if (mvect_scan_bpi_adc.size() != 0)
				mvect_scan_bpi_adc.clear() ;
			if (mvect_scan_start_index.size() != 0)
				mvect_scan_start_index.clear() ; 
			if (mmap_bin_intensity_map.size() != 0)
				mmap_bin_intensity_map.clear() ; 
		}			
		
		void IMSRawData::Load(char *file)
		{			
			const int SIZE_BUF = 512 ; 						
			mint_max_scan_size = 0 ; 
			strcpy(marr_filename, file)	; 
			char stringBuf[SIZE_BUF];
			char *pStr;
			int dataType = 0 ;
			double agilent_t0 ; 
			double agilent_k0 ;
			FILE* pFile ; 
			pFile = fopen(marr_filename, "r") ; 

			if (pFile == NULL)
			{
				char mesg [256] ; 
				strcpy(mesg, " Could not open ") ; 
				strcat(mesg, marr_filename) ; 
				strcat (mesg, " perhaps it does not exist.") ; 
				throw mesg ; 
			}

			char last_char = 0 ; 

			// Read the header in, and initialize values
			while (!feof(pFile))
			{
				fgets(stringBuf, SIZE_BUF, pFile) ; 				
				if ((pStr = (char *) strstr(stringBuf, "TimeOffset: ")))
					sscanf(pStr + 11, "%d", &mint_time_offset);      
				if ((pStr = (char *) strstr(stringBuf, "CalibrationSlope: ")))
					sscanf(pStr + 17, "%lf ", &agilent_k0);      
				if ((pStr = (char *) strstr(stringBuf, "CalibrationIntercept: ")))
					sscanf(pStr + 21, "%lf ", &agilent_t0);     
				if ((pStr = (char *) strstr(stringBuf, "FrameNumber: ")))
					sscanf(pStr+12, "%d", &mint_frame_num) ; 
				if ((pStr = (char *) strstr(stringBuf, "AverageTOFLength: ")))
					sscanf(pStr+17,"%lf", &mdbl_avg_tof_length) ; 	
				if ((pStr = (char *) strstr(stringBuf, "DataSubType: float")))
				{
					mbln_is_multiplexed_data = true ; 
					mbln_is_adc_data  = false ; 
				}
				if ((pStr = (char *) strstr(stringBuf, "DataSubType: int")))
				{
					mbln_is_multiplexed_data = false ; 	
					mbln_is_adc_data = true ; 
				}			


				int len = strlen(stringBuf) ; 
				bool foundEndBlock = false ; 
				for (int charIndex = 0 ; charIndex < len ; charIndex++)
				{
					if (stringBuf[charIndex] == 0x1B)
					{
						foundEndBlock = true ; 
						break ;
					}
				}
				if (foundEndBlock)
					break ; 
			}
			fclose(pFile) ; 
			//delete pFile ;  Anoop  - causes debug assertion in debug mode


			mdbl_t0 = agilent_t0 * 1000 ; //convert to ns
			mdbl_k0 = agilent_k0/1000 ; 		
		
			
			//open file and skip through header
			int fh;
			fh = _open(marr_filename, _O_RDONLY | _O_BINARY );
			if (fh == -1)
			{
				char mesg [256] ; 
				strcpy(mesg, " Could not open ") ; 
				strcat(mesg, marr_filename) ; 
				strcat (mesg, " perhaps it does not exist.") ; 
				throw mesg ; 
			}
			while(!_eof(fh) && last_char!= 0x1B)
			{
				int num_read = _read(fh, &last_char, 1);				 
			}

			if (_eof(fh))
			{
			   _close( fh );			
				return ; 
			}

			
			const int INT_SIZE = sizeof(int) ;
			const int FLT_SIZE = sizeof(float) ; 
			const int SHRT_SIZE = sizeof(short) ; 


			TOFRecord<int, short> rec ; 
			TOFRecord<int, float> mxed_rec ;
			TOFRecord<int, int> adc_rec ;

			//Get TOF size based on data ype
			// Standard IMS - short
			// Multiplexed IMS - float 
			// AGC IMS - int 

			int tof_rec_size = 0 ; 
			if (mbln_is_multiplexed_data)
				tof_rec_size = FLT_SIZE + INT_SIZE ; 
			else if (mbln_is_adc_data)
				tof_rec_size = 2* INT_SIZE ; 
			else
				tof_rec_size = SHRT_SIZE + INT_SIZE ; 
			
			
			//Read in number of IMS scans			
			int num_read = _read(fh, &mint_num_scans, INT_SIZE);			

			// Read in the lengths and the tic values. 
			int *int_vals = new int [2*mint_num_scans] ; 
			if (mbln_is_multiplexed_data)
				num_read = _read(fh, int_vals, (INT_SIZE + FLT_SIZE) * mint_num_scans);
			else if (mbln_is_adc_data)
				num_read = _read(fh, int_vals, (2 * INT_SIZE) * mint_num_scans);
			else
				num_read = _read(fh, int_vals, INT_SIZE * 2 * mint_num_scans);
		
			// now the values are fetched, lets save them. 
			int num_pts_so_far = 0 ; 
			for (int scan_num = 0 ; scan_num < mint_num_scans ; scan_num++)
			{
				int temp  = int_vals[2*scan_num] ; 
				temp = int_vals[scan_num*2+1] ; 				
				mvect_scan_bpi.push_back(int_vals[2*scan_num]) ; 
				mvect_scan_bpi_adc.push_back(int_vals[2*scan_num]) ; 
				mvect_scan_start_index.push_back(num_pts_so_far) ;
				num_pts_so_far += int_vals[scan_num*2+1] / tof_rec_size; 
			}
			mvect_scan_start_index.push_back(num_pts_so_far) ; 
			
			int data_size = tof_rec_size  * num_pts_so_far ; 

			unsigned char *temp = new unsigned char [(int)data_size] ; 
			num_read = _read(fh, temp, data_size);


			mvect_data.reserve(num_pts_so_far) ; 
			int current_scan = 0 ; 
			std::cerr.precision(6) ; 
			std::cerr.setf(std::ios::fixed, std::ios::floatfield);
			//std::cerr<<"Loading "<<file<<" # of points = "<<num_pts_so_far<<" Read points = "<<num_read/TOFREC_SIZE<<std::endl ; 

			// Now copy into TOFRecords and stuff them into the vector
            for (int pt_num = 0 ; pt_num < num_pts_so_far ; pt_num++)
			{
			try
			{

				while (mvect_scan_start_index[current_scan] < pt_num)
					current_scan++  ;
			
				// ok Byte alignment issues is a pain. To avoid pain, 
				//TOFRecord *ptr = (TOFRecord *) &temp[pt_num*TOFREC_SIZE] ;
				if (mbln_is_multiplexed_data)						
				{
					memcpy(&mxed_rec, &temp[pt_num*tof_rec_size], tof_rec_size) ; 	
					mvect_mxed_data.push_back(mxed_rec) ; 
					if (mxed_rec.tof_bin > mint_max_scan_size)
						mint_max_scan_size = mxed_rec.tof_bin ; 
				}
				else if (mbln_is_adc_data)
				{
					memcpy(&adc_rec, &temp[pt_num*tof_rec_size], tof_rec_size) ; 	
					mvect_adc_data.push_back(adc_rec) ; 
					
					if (adc_rec.tof_bin > mint_max_scan_size)
						mint_max_scan_size = adc_rec.tof_bin ; 
				}
				else
				{
					memcpy(&rec, &temp[pt_num*tof_rec_size], tof_rec_size) ; 	
					mvect_data.push_back(rec) ; 
					if (rec.tof_bin > mint_max_scan_size)
						mint_max_scan_size = rec.tof_bin ; 
				}
			}
					
			catch (std::exception &e)
			{
				std::cerr<<e.what()<<std::endl ; 					
			}
			catch (std::out_of_range &e)
			{
				std::cerr<<e.what()<<std::endl ; 					
			}
			catch (std::overflow_error &e)			
			{
				std::cerr<<e.what()<<std::endl ; 					
			}
			}

			mint_max_scan_size++ ; 

			delete [] temp ; 
			delete [] int_vals ; 
			_close( fh );			
			

			mint_start_bin = INT_MAX ; 
			mint_stop_bin = 0 ; 
			
			for (int i = 0 ; i < mint_max_scan_size ; i++)
			{
				//ignoring /16 to increase resolution
				int index_shifted = i/16 ; 
				double mz_val = GetMassFromBin(i) ;

				if (mz_val < mdbl_max_mz)
				{
					mint_stop_bin = index_shifted ; 
				}
				else
				{
					break ; 
				}
				if (mz_val > mdbl_min_mz && index_shifted < mint_start_bin)
				{
					mint_start_bin = index_shifted ; 
				}
			}	
			return ; 
		}
		

		int IMSRawData::GetFirstScanNum()
		{
			return 1 ; 
		}


		int IMSRawData::GetLastScanNum()
		{
			return mint_num_scans ; 
		}
		
		int IMSRawData::GetFrameNumber() 
		{
			return mint_frame_num ; 
		}

		double IMSRawData::GetDriftTime(int scan_num) 
		{
			if (scan_num == mint_last_scan_num)
				return mdbl_drift_time/1000000 ;  //in musecs

			return ((scan_num * mdbl_avg_tof_length)/1000000) ; 			 
		}

		double IMSRawData::GetScanTime(int scan_num)
		{
			/*if (scan_num == mint_last_scan_num)
				return mdbl_elution_time/1000000000 ; // returns elution time in secs

			double lc_elution_time ; 
			double max_drift_time ; 
			max_drift_time = mint_num_scans * mdbl_avg_tof_length ; 
			lc_elution_time = (mint_frame_num * max_drift_time) + (scan_num * mdbl_avg_tof_length) ; 			 
			return lc_elution_time/1000000000 ; */

			return scan_num * mdbl_scan_interval ; 
		}

		const char* IMSRawData::GetFileName()
		{
			return marr_filename ; 
		}

		void IMSRawData::Close() 
		{
			mvect_data.clear() ; 
			mvect_scan_bpi.clear() ; 
			mvect_scan_bpi_adc.clear() ; 
			mvect_scan_start_index.clear () ;
			mvect_adc_data.clear() ; 
			mvect_mxed_data.clear() ; 
		}

		bool IMSRawData::GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) 
		{
			bool found_data = GetRawData(mzs, intensities,	scan_num, -1) ;	
			if (!found_data)
				return found_data ;
			//mobj_savgol.Smooth(mzs, intensities) ; 
			return true ; 
		}

		bool IMSRawData::GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
			std::map<int, double> bin_intensity_map ; 
			// get all the points for now. 
			if (scan_num > mint_num_scans || scan_num <= 0)
				return false ; 

			int startIndex = mvect_scan_start_index[scan_num-1] ;
			int stopIndex ; 
			if (mbln_is_multiplexed_data)
				stopIndex = mvect_mxed_data.size() ; 
			else if (mbln_is_adc_data)
				stopIndex = mvect_adc_data.size() ; 
			else
                stopIndex = mvect_data.size() ; 

			if (scan_num < mint_num_scans)
				stopIndex = mvect_scan_start_index[scan_num] ;

			if (stopIndex == 0)
				return false ; 

			TOFRecord<int, short> rec ; 
			TOFRecord<int, float> mxed_rec ;
			TOFRecord<int, int> adc_rec ;
			
			mint_last_scan_num = scan_num ; 
			mdbl_drift_time = scan_num * mdbl_avg_tof_length ;
			mdbl_max_drift_time = mint_num_scans * mdbl_avg_tof_length ; 
			mdbl_elution_time = (mint_frame_num * mdbl_max_drift_time) + mdbl_drift_time ; 

			for (int index = startIndex ; index < stopIndex ; index++)
			{	
				int bin = 0 ; 
				double intensity_val =  0 ; 
				if(mbln_is_multiplexed_data)
				{
					mxed_rec =  mvect_mxed_data[index] ;
					bin = mxed_rec.tof_bin ; 
					intensity_val = mxed_rec.intensity ; 
				}
				else if (mbln_is_adc_data)
				{
					adc_rec =  mvect_adc_data[index] ;
					bin = adc_rec.tof_bin ; 
					intensity_val = adc_rec.intensity ; 
				}
				else
				{
					rec = mvect_data[index] ; 
					bin = rec.tof_bin ; 
					intensity_val = rec.intensity ; 
				}

				if (bin_intensity_map.find(bin) != bin_intensity_map.end())
				{
					bin_intensity_map[bin] += intensity_val ; 
				}
				else
				{
					bin_intensity_map[bin] = intensity_val ; 
				}
				
			}

			// now copy all the m/z values into vector. 
			int last_bin = 0 ; 
			int last_intensity = 0 ;
			double mz_val = 0 ;
			bool first = true ; 
			for (std::map<int, double>::iterator iter = bin_intensity_map.begin() ;
				iter != bin_intensity_map.end() ; iter++)
			{				
				if ((*iter).first > last_bin + 1 && !first) //need to revisit this
				{
					if (last_intensity != 0)
					{
						mz_val = this->GetMassFromBin(last_bin+1) ; 
						//mzs->push_back(mz_val) ; 
						//intensities->push_back(0) ;
						last_bin++;
					}
					while(last_bin < (*iter).first-1)
					{
						mz_val = this->GetMassFromBin(last_bin+1) ; 
						//mzs->push_back(mz_val) ; 
						//intensities->push_back(0) ;
						last_bin++;
					}
				}
				mz_val = this->GetMassFromBin((*iter).first) ; 
				mzs->push_back(mz_val) ; 

				intensities->push_back((*iter).second) ;

				last_bin = (int) (*iter).first ; 
				last_intensity = (int) (*iter).second ; 
				first = false ; 
			}

			return true	; 
		}

		int	IMSRawData::GetScanSize() 
		{
			return mint_max_scan_size ; 
		}

		int	IMSRawData::GetNumScans()
		{			return mint_num_scans ; 
		}

		double IMSRawData::GetSignalRange(int scan_num)
		{
			return 0 ;
		}

		void IMSRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double>	*scan_times, bool base_peak_tic)
		{
			int num_pts = mvect_scan_bpi.size() ; 
			intensities->reserve(num_pts) ; 
			scan_times->reserve(num_pts) ; 
			for (int pt_num = 0 ; pt_num < num_pts ; pt_num++)
			{
				double intensity; 
				if (mbln_is_adc_data)
				{
 					intensity = (double) mvect_scan_bpi_adc[pt_num]; 
				}
				else if (mbln_is_multiplexed_data)
				{
					intensity = (double) mvect_scan_bpi_mxed[pt_num] ; 
				}
				else
				{
				  intensity = mvect_scan_bpi[pt_num] ; 
				}
				intensities->push_back(intensity) ; 
				scan_times->push_back(pt_num) ; 
			}
		}

		void IMSRawData::GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_scan, int stop_scan, double min_mz, double max_mz)
		{

			std::map<int, double> bin_intensity_map ; 
			double mz ; 

			TOFRecord<int, short> rec ; 
			TOFRecord<int, float>mxed_rec ;
			TOFRecord<int, int> adc_rec ;

			// Start
			for (int scan_num = start_scan ; scan_num <= stop_scan ; scan_num++)
			{
				if (scan_num > mint_num_scans || scan_num <= 0)
					return ; 

				if (mvect_scan_start_index[scan_num-1] <0) 
					break ; 


				int startIndex = mvect_scan_start_index[scan_num-1] ;
				int stopIndex ; 
				if (mbln_is_multiplexed_data)
					stopIndex = mvect_mxed_data.size() ; 
				else if (mbln_is_adc_data)
					stopIndex = mvect_adc_data.size() ; 
				else
					stopIndex = mvect_data.size() ; 

				if (scan_num < mint_num_scans)
					stopIndex = mvect_scan_start_index[scan_num] ;

				for (int index = startIndex ; index < stopIndex ; index++)
				{

					int bin = 0 ; 
					double intensity_val =  0 ; 
					if(mbln_is_multiplexed_data)
					{
						mxed_rec =  mvect_mxed_data[index] ;
						bin = mxed_rec.tof_bin ; 
						intensity_val = mxed_rec.intensity ; 
					}
					else if (mbln_is_adc_data)
					{
						adc_rec =  mvect_adc_data[index] ;
						bin = adc_rec.tof_bin ; 
						intensity_val = adc_rec.intensity ; 
					}
					else
					{
						rec = mvect_data[index] ; 
						bin = rec.tof_bin ; 
						intensity_val = rec.intensity ; 
					}
				
					mz = GetMassFromBin(bin) ; 

					if (mz >= min_mz && mz <= max_mz)
					{
						if (bin_intensity_map.find(bin) != bin_intensity_map.end())
						{
							bin_intensity_map[bin] += intensity_val ; 
						}
						else
						{
							bin_intensity_map[bin] = intensity_val ; 
						}
					}
				}
			}

			int last_bin = 0 ; 
			double last_intensity = 0 ; 
			for (std::map<int, double>::iterator iter = bin_intensity_map.begin() ; iter != bin_intensity_map.end() ; iter++)
			{			
				mz = this->GetMassFromBin((*iter).first) ;
				if (mz >= min_mz && mz <= max_mz)
				{
					mzs->push_back(mz) ; 
					intensities->push_back((*iter).second) ; 
					last_bin = (int)(*iter).first ; 
					last_intensity = (int)(*iter).second ; 
				}
			}
		}

		void IMSRawData::GetSummedSpectraSlidingWindow(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range )
		{
			double mz ; 

			TOFRecord<int, short> rec ; 
			TOFRecord<int, float>mxed_rec ;
			TOFRecord<int, int> adc_rec ;

			double min_mz = DBL_MAX ; 
			double max_mz = DBL_MIN ; 
			int min_mz_bin = 0 ; 
			int max_mz_bin = 0 ; 
			int bin  ; 

			// get mz_range to sum over
			int startIndex = mvect_scan_start_index[current_scan-1] ;
			int stopIndex ; 
			if (mbln_is_multiplexed_data)
				stopIndex = mvect_mxed_data.size() ; 
			else if (mbln_is_adc_data)
				stopIndex = mvect_adc_data.size() ;
			else
				stopIndex = mvect_data.size() ; 
			if (current_scan < mint_num_scans)
				stopIndex = mvect_scan_start_index[current_scan] ;	

			//get min and max values for m/z and bin
			for (int index = startIndex ; index < stopIndex ; index++)
			{				
				if (mbln_is_multiplexed_data)
				{
					mxed_rec = mvect_mxed_data[index] ;
					bin = mxed_rec.tof_bin ; 
					mz = GetMassFromBin(bin) ; 
					if(mz < min_mz) 
					{
						min_mz = mz; 
						min_mz_bin = bin ; 
					}
					if (mz > max_mz) 
					{					
						max_mz = mz ; 
						max_mz_bin = bin ; 
					}
				}
				else if (mbln_is_adc_data)
				{			
					adc_rec = mvect_adc_data[index] ; 					
					bin = adc_rec.tof_bin ; 
					mz = GetMassFromBin(bin) ; 
					if(mz < min_mz) 
					{
						min_mz = mz; 
						min_mz_bin = bin ; 
					}
					if (mz > max_mz) 
					{					
						max_mz = mz ; 
						max_mz_bin = bin ; 
					}
				}
				else
				{
					rec = mvect_data[index] ; 
					bin = rec.tof_bin ; 
					mz = GetMassFromBin(bin) ; 
					if(mz < min_mz) 
					{
						min_mz = mz; 
						min_mz_bin = bin ; 
					}
					if (mz > max_mz) 
					{					
						max_mz = mz ; 
						max_mz_bin = bin ; 
					}
				}
			}

			// now start the summing
			int start_scan = current_scan - scan_range-1 ;
			int stop_scan = current_scan + scan_range ; 

			// Subtract start_scan from bin_intensity_map
			if (start_scan > 1)
			{				
				startIndex = mvect_scan_start_index[start_scan-1] ;
				if (mbln_is_multiplexed_data)
					stopIndex = mvect_mxed_data.size() ; 
				else if (mbln_is_adc_data)
					stopIndex = mvect_adc_data.size() ; 
				else
					stopIndex = mvect_data.size() ; 
				
				if (start_scan < mint_num_scans)
					stopIndex = mvect_scan_start_index[start_scan] ;	

				for (int index = startIndex ; index < stopIndex ; index++)
				{
					int bin = 0 ; 
					double intensity_val =  0 ; 
					if(mbln_is_multiplexed_data)
					{
						mxed_rec =  mvect_mxed_data[index] ;
						bin = mxed_rec.tof_bin ; 
						intensity_val = mxed_rec.intensity ; 
					}
					else if (mbln_is_adc_data)
					{
						adc_rec =  mvect_adc_data[index] ;
						bin = adc_rec.tof_bin ; 
						intensity_val = adc_rec.intensity ; 
					}
					else
					{
						rec = mvect_data[index] ; 
						bin = rec.tof_bin ; 
						intensity_val = rec.intensity ; 
					}
					if (mmap_bin_intensity_map[bin] > 0 )
						mmap_bin_intensity_map[bin] -= intensity_val ; 					
					else
						mmap_bin_intensity_map[bin] = 0 ; 					
				}	
			}			
			
			// now add the second scan to the bin
			if (stop_scan < mint_num_scans)
			{
				startIndex = mvect_scan_start_index[stop_scan-1] ;
				stopIndex = mvect_scan_start_index[stop_scan] ;	
				for (int index = startIndex ; index < stopIndex ; index++)
				{
					int bin = 0 ; 
					double intensity_val =  0 ; 
					if(mbln_is_multiplexed_data)
					{
						mxed_rec =  mvect_mxed_data[index] ;
						bin = mxed_rec.tof_bin ; 
						intensity_val = mxed_rec.intensity ; 
					}
					else if (mbln_is_adc_data)
					{
						adc_rec =  mvect_adc_data[index] ;
						bin = adc_rec.tof_bin ; 
						intensity_val = adc_rec.intensity ; 
					}
					else
					{
						rec = mvect_data[index] ; 
						bin = rec.tof_bin ; 
						intensity_val = rec.intensity ; 
					}
					int this_intensity = 0 ; 

					if (mmap_bin_intensity_map.find(bin) != mmap_bin_intensity_map.end())
					{
						this_intensity = mmap_bin_intensity_map[bin] ;
						mmap_bin_intensity_map[bin] += intensity_val ; 					
						this_intensity = mmap_bin_intensity_map[bin] ;
					}
					else
					{
						this_intensity = mmap_bin_intensity_map[bin] ;						
						mmap_bin_intensity_map[bin] = intensity_val ; 
						this_intensity = mmap_bin_intensity_map[bin] ;
					}
				}
			}

			// now choose the mz range we care about
			int last_bin = 0 ; 
			int last_intensity = 0 ; 
			
			for (std::map<int, double>::iterator iter = mmap_bin_intensity_map.lower_bound(min_mz_bin) ; iter != mmap_bin_intensity_map.upper_bound(max_mz_bin) ; iter++)
			{
				mz = this->GetMassFromBin((*iter).first) ;
				if (mz >= min_mz && mz <= max_mz)
				{
					mzs->push_back(mz) ; 
					intensities->push_back((*iter).second) ; 
					last_bin = (int)(*iter).first ; 
					last_intensity = (int)(*iter).second ; 
				}
			}
			

		}

		void IMSRawData::InitializeSlidingWindow(int scan_range)
		{
			// Function that initializes the mmap_bin_intensity_map for summing using a sliding window
			// Sum from scans 1 to scan_range and set it to mmap_bin_intensity_map
			TOFRecord<int, short> rec ; 
			TOFRecord<int, float> mxed_rec ;
			TOFRecord<int, int> adc_rec ;

			// Start
			for (int scan_num = 1 ; scan_num <= scan_range ; scan_num++)
			{
				if (scan_num > mint_num_scans || scan_num <= 0)
					return ; 

				if (mvect_scan_start_index[scan_num-1] <0) 
					break ; 

				int startIndex = mvect_scan_start_index[scan_num-1] ;
				int stopIndex ; 
				if (mbln_is_multiplexed_data)
					stopIndex = mvect_mxed_data.size() ; 
				else if (mbln_is_adc_data)
					stopIndex = mvect_adc_data.size() ; 
				else
					stopIndex = mvect_data.size() ; 

				if (scan_num < mint_num_scans)
					stopIndex = mvect_scan_start_index[scan_num] ;

				for (int index = startIndex ; index < stopIndex ; index++)
				{
					int bin = 0 ; 
					double intensity_val =  0 ; 
					if(mbln_is_multiplexed_data)
					{
						mxed_rec =  mvect_mxed_data[index] ;
						bin = mxed_rec.tof_bin ; 
						intensity_val = mxed_rec.intensity ; 
					}
					else if (mbln_is_adc_data)
					{
						adc_rec =  mvect_adc_data[index] ;
						bin = adc_rec.tof_bin ; 
						intensity_val = adc_rec.intensity ; 
					}
					else
					{
						rec = mvect_data[index] ; 
						bin = rec.tof_bin ; 
						intensity_val = rec.intensity ; 
					}				
					// sum over all mzs do not bother about mz range
					if (mmap_bin_intensity_map.find(bin) != mmap_bin_intensity_map.end())					
						mmap_bin_intensity_map[bin] += intensity_val ; 
					else
						mmap_bin_intensity_map[bin] = intensity_val ; 
				}
			}
		}

		void IMSRawData::GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range)
		{
			std::map<int, double> bin_intensity_map ; 
			double mz ; 

			TOFRecord<int, short> rec ; 
			TOFRecord<int, float>mxed_rec ;
			TOFRecord<int, int> adc_rec ;

			double min_mz = DBL_MAX ; 
			double max_mz = DBL_MIN ; 
			int bin  ; 
			
			int start_scan = current_scan - scan_range ;
			int stop_scan = current_scan + scan_range ; 

			//check 
			if (start_scan < 1)
					start_scan = 1 ; 
			if (stop_scan > mint_num_scans)
					stop_scan = mint_num_scans ;  

			// Setting m/z range to view
			int startIndex = mvect_scan_start_index[current_scan-1] ;
			int stopIndex ; 
			if (mbln_is_multiplexed_data)
				stopIndex = mvect_mxed_data.size() ; 
			else if (mbln_is_adc_data)
				stopIndex = mvect_adc_data.size() ;
			else
				stopIndex = mvect_data.size() ; 
			
			stopIndex = mvect_scan_start_index[current_scan] ; 
			for (int index = startIndex ; index < stopIndex ; index++)
			{
				
				if (mbln_is_multiplexed_data)
				{
					mxed_rec = mvect_mxed_data[index] ;
					bin = mxed_rec.tof_bin ; 
					mz = GetMassFromBin(bin) ; 
					if(mz < min_mz) 
						min_mz = mz; 
					if (mz > max_mz) 
						max_mz = mz ; 
				}
				else if (mbln_is_adc_data)
				{			
					adc_rec = mvect_adc_data[index] ; 					
					bin = adc_rec.tof_bin ; 
					mz = GetMassFromBin(bin) ; 
					if(mz < min_mz) 
						min_mz = mz; 
					if (mz > max_mz) 
						max_mz = mz ; 
				}
				else
				{
					rec = mvect_data[index] ; 
					bin = rec.tof_bin ; 
					mz = GetMassFromBin(bin) ; 
					if(mz < min_mz) 
						min_mz = mz; 
					if (mz > max_mz) 
						max_mz = mz ;
				}
			}

			for (int scan_num = start_scan ; scan_num <= stop_scan ; scan_num++)
			{
				//if it goes out of bounds, then return
				if (scan_num > mint_num_scans || scan_num <= 0)
					return ; 

				int startIndex = mvect_scan_start_index[scan_num-1] ;
				int stopIndex = mvect_data.size() ; 

				if (scan_num < mint_num_scans)
					stopIndex = mvect_scan_start_index[scan_num] ;

				for (int index = startIndex ; index < stopIndex ; index++)
				{
					int bin = 0 ; 
					double intensity_val =  0 ; 
					if(mbln_is_multiplexed_data)
					{
						mxed_rec =  mvect_mxed_data[index] ;
						bin = mxed_rec.tof_bin ; 
						intensity_val = mxed_rec.intensity ; 
					}
					else if (mbln_is_adc_data)
					{
						adc_rec =  mvect_adc_data[index] ;
						bin = adc_rec.tof_bin ; 
						intensity_val = adc_rec.intensity ; 
					}
					else
					{
						rec = mvect_data[index] ; 
						bin = rec.tof_bin ; 
						intensity_val = rec.intensity ; 
					}
				
					mz = GetMassFromBin(bin) ; 

					if (mz >= min_mz && mz <= max_mz)
					{
						if (bin_intensity_map.find(bin) != bin_intensity_map.end())
						{
							bin_intensity_map[bin] += intensity_val ; 
						}
						else
						{
							bin_intensity_map[bin] = intensity_val ; 
						}
					}

				}
			}

			int last_bin = 0 ; 
			int last_intensity = 0 ; 
			for (std::map<int, double>::iterator iter = bin_intensity_map.begin() ; iter != bin_intensity_map.end() ; iter++)
			{
				mz = this->GetMassFromBin((*iter).first) ;
				if (mz >= min_mz && mz <= max_mz)
				{
					mzs->push_back(mz) ; 
					intensities->push_back((*iter).second) ; 
					last_bin = (int)(*iter).first ; 
					last_intensity = (int)(*iter).second ; 
				}
			}
		}
	}
}
