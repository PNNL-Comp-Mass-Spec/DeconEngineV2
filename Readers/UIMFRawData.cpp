#include "UIMFRawData.h"
#include <map> 
#include <iostream>
#include <float.h>
#include <fstream> 
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <ctime>

using namespace	std	; 
using namespace UIMFLibrary;

namespace Engine 
{
	namespace Readers
	{
		UIMFRawData::UIMFRawData(void)
		{
			mdbl_k0 = 0 ; 
			mdbl_t0 =  0 ; 
			mdbl_min_mz = 10 ; 
			mdbl_scan_interval = 1 ; 
			mdbl_bin_width = 0;
			mdbl_max_mz = 2500 ;
			mdbl_avg_tof_length = 0 ; 
			mint_frame_num = 0 ;
			mint_num_scans = 0;
			mint_num_bins = 0;
			mint_time_offset = -1;
			mbln_is_multiplexed_data = false ; 
			mbln_is_adc_data = false ; 
			mbln_is_file_open = false;
			mint_scans_per_frame = 0;

		}
		UIMFRawData::~UIMFRawData(void) 
		{
			if (mvect_data.size() != 0)
				mvect_data.clear() ;
			if (mvect_mxed_data.size() != 0 ) 
				mvect_mxed_data.clear() ; 
			if (mvect_scan_bpi.size() != 0)
				mvect_scan_bpi.clear() ;
			if (mvect_scan_bpi_adc.size() != 0)
				mvect_scan_bpi_adc.clear() ;
			//if (mvect_scan_start_index.size() != 0)
			//	mvect_scan_start_index.clear() ; 
			//if (mvect_scan_end_index.size() != 0)
			//	mvect_scan_end_index.clear() ; 
			if (mmap_bin_intensity_map.size() != 0)
				mmap_bin_intensity_map.clear() ; 
		}			
		

		DataReader* UIMFRawData::OpenUIMFFile(){
			DataReader *ReadUIMF = new DataReader();
			try{
				ReadUIMF->OpenUIMF(marr_filename);
				
				//ReadUIMF->runPragmas();

				mbln_is_file_open = true;
			}
			catch(char * str ){
				
			}
			
			return ReadUIMF;
		}

		void UIMFRawData::Load(char *file)
		{
			return;


	//		strcpy(marr_filename, file)	; 

	//		// Open UIMF file
	//		DataReader *ReadUIMF = new DataReader();
	//		ReadUIMF->OpenUIMF(marr_filename);

	//		// Get data_type, num_frames & num_scans
	//		System::String *data_type;
	//		data_type = System::Convert::ToString(ReadUIMF->GetGlobalParameters("TOFIntensityType"));
	//		mint_num_frames = System::Convert::ToInt32(ReadUIMF->GetGlobalParameters("NumFrames"));
	//		mint_startFrame = 1;
	//		
	//		//[gord] i don't think there is any point to this loop...  just need one line of it. 
	//		//[yan] This part is needed if StartFrame is greater than 1, 8/12/2009
	//		mint_num_scans_in_a_frame = 0;
	//		while (mint_num_scans_in_a_frame == 0)
	//		{
	//			mint_num_scans_in_a_frame = System::Convert::ToInt32(ReadUIMF->GetFrameParameters(mint_startFrame,"Scans"));
	//			mint_startFrame++;
	//		}
	//		mint_startFrame--;
	//		mint_endFrame = mint_num_frames + mint_startFrame - 1;
	//		int startScan = 0;
	//		int endScan = mint_num_scans_in_a_frame - 1; 

	//		//Get TIC
	//		mint_num_scans = mint_num_scans_in_a_frame * mint_num_frames;
	///*		if (System::String::Compare(data_type,"ADC") == 0)
	//		{
	//			int TICs __gc[] = new int __gc[mint_num_scans];
	//			ReadUIMF->GetTIC(TICs, 0,mint_startFrame, mint_endFrame, startScan, endScan);			
	//			for (int i = 0 ; i < mint_num_scans ; i++)
	//			{
	//				int temp_tic = TICs[i];
	//				mvect_scan_bpi.push_back(temp_tic) ; 
	//			}
	//		}
	//		else if (System::String::Compare(data_type,"FOLDED") == 0)
	//		{
	//			float TICs __gc[] = new float __gc[mint_num_scans];
	//			ReadUIMF->GetTIC(TICs, 0,mint_startFrame, mint_endFrame, startScan, endScan);			
	//			for (int i = 0 ; i < mint_num_scans ; i++)
	//			{
	//				float temp_tic = TICs[i];
	//				mvect_scan_bpi.push_back(temp_tic) ; 
	//			}
	//		}
	//		else if (System::String::Compare(data_type,"TDC") == 0)
	//		{
	//			short TICs __gc[] = new short __gc[mint_num_scans];
	//			ReadUIMF->GetTIC(TICs, 0,mint_startFrame, mint_endFrame, startScan, endScan);			
	//			for (int i = 0 ; i < mint_num_scans ; i++)
	//			{
	//				short temp_tic = TICs[i];
	//				mvect_scan_bpi.push_back(temp_tic) ; 
	//			}
	//		}
	//		else
	//		{
	//			double TICs __gc[] = new double __gc[mint_num_scans];
	//			ReadUIMF->GetTIC(TICs, 0,mint_startFrame, mint_endFrame, startScan, endScan);			
	//			for (int i = 0 ; i < mint_num_scans ; i++)
	//			{
	//				double temp_tic = TICs[i];
	//				mvect_scan_bpi.push_back(temp_tic) ; 
	//			}
	//		}*/
	//		// Close UIMF file
	//		ReadUIMF->CloseUIMF(marr_filename);

		}

	//returns the number of points in the spectrum
		int UIMFRawData::GetUIMFSpectrum( char *marr_filename, int scan_num)
		{

			return 0;




			//// Open UIMF file
			//DataReader *reader = new DataReader();
			//reader->OpenUIMF(marr_filename);
			//
			//// Get bin_width, num_bins, etc
			//if ( mdbl_bin_width == 0){
			//	mdbl_bin_width = System::Convert::ToInt32(reader->GetGlobalParameters("BinWidth"));
			//}

			//if (mint_num_bins == 0 ){
			//	mint_num_bins = System::Convert::ToInt32(reader->GetGlobalParameters("Bins"));
			//}

			//if (mint_time_offset == -1){
			//	mint_time_offset = System::Convert::ToInt32(reader->GetGlobalParameters("TimeOffset"));
			//}

			//if (mint_scans_per_frame == 0){
			//	mint_scans_per_frame = System::Convert::ToInt32(reader->GetFrameParameters(mint_startFrame,"Scans"));
			//}
			//
			////mint_max_scan_size = mint_num_scans * mint_num_bins ;			
			//mint_max_scan_size = 0;			
			//mint_frame_num = scan_num / mint_num_scans_in_a_frame + mint_startFrame;
			//int	scanNum = scan_num  % mint_scans_per_frame;
			//if ((mint_frame_num - mint_startFrame + 1) > mint_num_frames) 
			//{
			//	mint_frame_num = mint_num_frames + mint_startFrame - 1;
			//	scanNum = mint_num_scans_in_a_frame -1;
			//}
			//
			//// Get data_type, TOF_length, calibration values, & spectrum_length for each scan
			//System::String *data_type;
			//data_type = System::Convert::ToString(reader->GetGlobalParameters("TOFIntensityType"));
			//mdbl_avg_tof_length = System::Convert::ToDouble(reader->GetFrameParameters(mint_frame_num,"AverageTOFLength"));
			//mdbl_t0  = System::Convert::ToDouble(reader->GetFrameParameters(mint_frame_num,"CalibrationIntercept"));
			//mdbl_k0 =  System::Convert::ToDouble(reader->GetFrameParameters(mint_frame_num,"CalibrationSlope"));
			//int mint_spectrum_length = reader->GetCountPerSpectrum(mint_frame_num, scanNum);
			//
			//// Get Spectrum
			//if ( mint_spectrum_length != 0)
			//{
			//	UIMFRecord<int, int>  rec ; 
			//	UIMFRecord<int, float>  mxed_rec ;
			//	UIMFRecord<int, int>  adc_rec ;
			//	
			//	// Check data type
			//	if (System::String::Compare(data_type,"ADC") == 0)
			//	{			
			//		if (mvect_adc_data.size() != 0) mvect_adc_data.clear() ;
			//		int intensities __gc[] = new int __gc[mint_spectrum_length];
			//		int bins __gc[] = new int __gc[mint_spectrum_length];
			//		mint_spectrum_length = reader->GetSpectrum(mint_frame_num, scanNum, intensities, bins);				
			//		mvect_adc_data.reserve(mint_spectrum_length);
			//		mbln_is_multiplexed_data = false ; 
			//		mbln_is_adc_data  = true ; 
			//		for (int bin_num = 0 ; bin_num < mint_spectrum_length; bin_num++)
			//		{
			//			adc_rec.intensity = (int)intensities[bin_num];
			//			adc_rec.tof_bin = bins[bin_num];			
			//			mvect_adc_data.push_back(adc_rec) ; 
			//		}
			//	}
			//	else if (System::String::Compare(data_type,"FOLDED") == 0)
			//	{
			//		if (mvect_mxed_data.size() != 0) mvect_mxed_data.clear() ;
			//		float intensities __gc[] = new float __gc[mint_spectrum_length];
			//		int bins __gc[] = new int __gc[mint_spectrum_length];
			//		mint_spectrum_length = reader->GetSpectrum(mint_frame_num, scanNum, intensities, bins);				

			//		mvect_mxed_data.reserve(mint_spectrum_length);
			//		mbln_is_multiplexed_data = true ; 
			//		mbln_is_adc_data  = false ; 
			//		for (int bin_num = 0 ; bin_num < mint_spectrum_length; bin_num++)
			//		{
			//			mxed_rec.intensity = (float)intensities[bin_num];
			//			mxed_rec.tof_bin = bins[bin_num];				
			//			mvect_mxed_data.push_back(mxed_rec) ; 
			//		}
			//	}
			//	else if (System::String::Compare(data_type,"TDC") == 0)
			//	{
			//		if (mvect_mxed_data.size() != 0) mvect_mxed_data.clear() ;
			//		short intensities __gc[] = new short __gc[mint_spectrum_length];
			//		int bins __gc[] = new int __gc[mint_spectrum_length];
			//		mint_spectrum_length = reader->GetSpectrum(mint_frame_num, scanNum, intensities, bins);				

			//		mvect_mxed_data.reserve(mint_spectrum_length);
			//		mbln_is_multiplexed_data = true ; 
			//		mbln_is_adc_data  = false ; 
			//		for (int bin_num = 0 ; bin_num < mint_spectrum_length; bin_num++)
			//		{
			//			mxed_rec.intensity = (short)intensities[bin_num];
			//			mxed_rec.tof_bin = bins[bin_num];				
			//			mvect_mxed_data.push_back(mxed_rec) ; 
			//		}
			//	}
			//	else 
			//	{
			//		if (mvect_data.size() != 0) mvect_data.clear() ;
			//		double intensities __gc[] = new double __gc[mint_spectrum_length];
			//		int bins __gc[] = new int __gc[mint_spectrum_length];
			//		mint_spectrum_length = reader->GetSpectrum(mint_frame_num, scanNum, intensities, bins);				
			//		mvect_data.reserve(mint_spectrum_length);					
			//		for (int bin_num = 0 ; bin_num < mint_spectrum_length; bin_num++)
			//		{
			//			rec.intensity = (int)intensities[bin_num];
			//			rec.tof_bin = bins[bin_num];		
			//			mvect_data.push_back(rec) ; 
			//		}
			//	}
			//
			//	mint_scan_start_index = 0 ;
			//	mint_scan_end_index = mint_num_bins-1 ;
			//}
			//	
			//reader->CloseUIMF(marr_filename);
			//return mint_spectrum_length;
		}
		int UIMFRawData::GetFirstScanNum()
		{
			return 1 ; 
		}

		void UIMFRawData::SetFileName(const char * file_name){
			strcpy(marr_filename, file_name);
		}


		int UIMFRawData::GetNumScansInFrame(){
			return 0;
			
			/*if (mint_num_scans_in_a_frame <= 0){
				DataReader *ReadUIMF = new DataReader();
				ReadUIMF->OpenUIMF(marr_filename);
				mint_num_scans_in_a_frame = System::Convert::ToInt32(ReadUIMF->GetFrameParameters(1,"Scans"));

			}

			return mint_num_scans_in_a_frame;*/
		}

		int UIMFRawData::GetLastScanNum()
		{

			return 0;

			//if ( mint_num_scans == 0){

			//	//you have to retrieve it from the database
			//	DataReader *ReadUIMF = new DataReader();
			//	ReadUIMF->OpenUIMF(marr_filename);

			//	mint_num_frames = System::Convert::ToInt32(ReadUIMF->GetGlobalParameters("NumFrames"));
			//	mint_startFrame = 1;
			//	mint_num_scans_in_a_frame = 0;
			//	while (mint_num_scans_in_a_frame == 0)
			//	{
			//		mint_num_scans_in_a_frame = System::Convert::ToInt32(ReadUIMF->GetFrameParameters(mint_startFrame,"Scans"));
			//		mint_startFrame++;
			//	}
			//	mint_startFrame--;
			//	mint_endFrame = mint_num_frames + mint_startFrame - 1;
			//	int startScan = 0;
			//	int endScan = mint_num_scans_in_a_frame - 1; 

			//	mint_num_scans = mint_num_scans_in_a_frame * mint_num_frames;
			//	ReadUIMF->CloseUIMF(marr_filename);
			//}
			//return mint_num_scans ; 
		}
		
		int UIMFRawData::GetFrameNumber() 
		{
			return mint_frame_num ; 
		}

		double UIMFRawData::GetDriftTime(int scan_num) 
		{
			if (scan_num == mint_last_scan_num)
				return mdbl_drift_time/1000000 ;  //in musecs

			return ((scan_num * mdbl_avg_tof_length)/1000000) ; 			 
		}

		double UIMFRawData::GetScanTime(int scan_num)
		{
			return scan_num * mdbl_scan_interval ; 
		}

		const char* UIMFRawData::GetFileName()
		{
			return marr_filename ; 
		}

		void UIMFRawData::Close() 
		{
			mvect_data.clear() ; 
			mvect_scan_bpi.clear() ; 
			mvect_scan_bpi_adc.clear() ; 
			//mvect_scan_start_index.clear () ;
			//mvect_scan_end_index.clear () ;
			mvect_adc_data.clear() ; 
			mvect_mxed_data.clear() ; 
		}

		bool UIMFRawData::GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) 
		{
			return GetRawData(mzs, intensities,	scan_num, -1)  ; 
		}

		bool UIMFRawData::GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts)
		{
			bool raw_data_found = false;


			//Get Spectrum from UIMF file
			if ( (mint_spectrum_length = UIMFRawData::GetUIMFSpectrum(marr_filename, scan_num)) != 0){
				raw_data_found = true;
			}
			

			if ( raw_data_found){
				std::map<int, double> bin_intensity_map ; 
				int startIndex = 0;
				int stopIndex = mint_spectrum_length;
			
				//printf("Size of mvect_scan_start_index is %d\n" , mvect_scan_start_index.size());
				//fflush(stdout);

				UIMFRecord<int, int> rec ; 
				UIMFRecord<int, float> mxed_rec ;
				UIMFRecord<int, int> adc_rec ;
			
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
						adc_rec = mvect_adc_data[index] ;
						bin = adc_rec.tof_bin ; 
						intensity_val = adc_rec.intensity ; 
					}
					else
					{
						rec = mvect_data[index] ; 
						bin = rec.tof_bin ; 
						intensity_val = rec.intensity ; 
					}
					
					if (intensity_val > 0) 
					{
						double mz_val = GetMassFromBin(bin);
						mzs->push_back(mz_val);
						intensities->push_back(intensity_val);
	
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

				if (!bin_intensity_map.empty()){
					bin_intensity_map.clear();
				}

			}

			//I think we should clear the mvect_mxed_data, mvect_data, mvect_data

	
			return raw_data_found	; 
		}

		int	UIMFRawData::GetScanSize() 
		{
			return mint_max_scan_size ; 
		}

		int	UIMFRawData::GetNumScans()
		{
			return mint_num_scans ; 
		}

		double UIMFRawData::GetSignalRange(int scan_num)
		{
			return 0 ;
		}

		void UIMFRawData::GetTicFromFile(std::vector<double> *intensities, std::vector<double>	*scan_times, bool base_peak_tic)
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
				//if (intensity != 0 ){
					intensities->push_back(intensity) ; 
					scan_times->push_back(pt_num) ; 
				//}
			}
		}

		
		//this can only be done assuming that the frame calibration coefficients are similar
		//across different frames. Also before this emthod can be called, the vector of
		//data needs to be loaded in memory. Since we're summing across a range
		//we need to get all that data 
		void UIMFRawData::GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_scan, int stop_scan, double min_mz, double max_mz)
		{	
			std::map<int, double> bin_intensity_map ; 
			double mz ; 

			UIMFRecord<int, int> rec ; 
			UIMFRecord<int, float>mxed_rec ;
			UIMFRecord<int, int> adc_rec ;
			
			//clear the vector of data
			int startIndex = 0 ;

			// Start
			for (int scan_num = start_scan ; scan_num <= stop_scan ; scan_num++)
			{
				//this loads up the data in our data vector, so we can use it directly
				int mint_spectrum_length = this->GetUIMFSpectrum(marr_filename, scan_num);
				if (mint_spectrum_length ==0){
					continue;
				}

				//this piece of code needs to become a small private function,
				//it's being used in a number of places
				int stopIndex ; 
				if (mbln_is_multiplexed_data)
					stopIndex = mvect_mxed_data.size() ; 
				else if (mbln_is_adc_data)
					stopIndex = mvect_adc_data.size() ; 
				else
					stopIndex = mvect_data.size() ; 

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
					//if the mz value is within a specified range of values
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

			//int last_bin = 0 ; 
			//double last_intensity = 0 ; 
			for (std::map<int, double>::iterator iter = bin_intensity_map.begin() ; iter != bin_intensity_map.end() ; iter++)
			{			
				mz = this->GetMassFromBin((*iter).first) ;
				if (mz >= min_mz && mz <= max_mz)
				{
					mzs->push_back(mz) ; 
					intensities->push_back((*iter).second) ; 
					//last_bin = (int)(*iter).first ; 
					//last_intensity = (int)(*iter).second ; 
				}
			}
		}


		//should return whether there was any data or otherwise
		
		bool UIMFRawData::GetSummedFrameAndScanSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_frame, int end_frame, int ims_start_scan, int ims_end_scan, double min_mz, double max_mz ) {
			bool success = false;
			std::map<double, double> mz_intensity_map ; 
			double mz =  0;
			double intensity_val = 0;


			//std::cout<<"Size of map " << mz_intensity_map.size();
			//fflush(stdout);

			for ( int ims_scan = ims_start_scan; ims_scan < ims_end_scan; ims_scan++){

				//this gives you the spectra summed in the frame dimension

				success = GetSummedFrameSpectra(mzs, intensities, start_frame, end_frame, min_mz, max_mz, ims_scan);
				//std::cout<<"Printing intensities after summing frames with map size "<< mz_intensity_map.size()<<"\n";

		
				//std::cout<<"Mz size = "<<mzs->size()<<std::endl;

				//fflush(stdout);
			
				//load all the mzs and intensities into a hashmap
				for ( int i =0; i < mzs->size(); i++){
					mz = mzs->at(i);
					intensity_val = intensities->at(i);
					
					if (mz_intensity_map.find(mz) != mz_intensity_map.end())
					{
							mz_intensity_map[mz] += intensity_val ; 
					}
					else
					{
							mz_intensity_map[mz] = intensity_val ; 
					}
					//}
				}

				mzs->clear();
				intensities->clear();

			}


			for (std::map<double, double>::iterator iter = mz_intensity_map.begin() ; iter != mz_intensity_map.end() ; iter++)
			{			
				mz = (*iter).first ;
				mzs->push_back(mz) ; 
				intensities->push_back((*iter).second) ; 
				success = true;
					
			}
			mz_intensity_map.clear();
/*

			std::cout<<"Printing mza and intensities after summing spectra and scans\n";
			for ( int a = 0; a < intensities->size(); a++){
					std::cout<<"Intensities["<<a<<"]="<<intensities->at(a)<<"\n";
					std::cout<<"Mzs["<<a<<"]="<<mzs->at(a)<<"\n";

			}
*/

			return success;
		}



		bool UIMFRawData::GetSummedFrameSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_frame, int end_frame, double min_mz, double max_mz, int ims_scan_num ) {
			bool success = false;
			std::map<int, double> bin_intensity_map ; 
			double mz ; 

			UIMFRecord<int, int> rec ; 
			UIMFRecord<int, float>mxed_rec ;
			UIMFRecord<int, int> adc_rec ;
			
			//clear the vector of data
			int startIndex = 0 ;

			// Start
			for (int frame_num = start_frame; frame_num <= end_frame ; frame_num++)
			{
				int scan_num = ((frame_num-1) * mint_num_scans_in_a_frame) + ims_scan_num;
				//cout << "Rertieving scan num= " <<scan_num << endl;
				//this loads up the data in our data vector, so we can use it directly
				int mint_spectrum_length = this->GetUIMFSpectrum(marr_filename, scan_num);
				if (mint_spectrum_length ==0){
					continue;
				}

				//this piece of code needs to become a small private function,
				//it's being used in a number of places
				int stopIndex ; 
				if (mbln_is_multiplexed_data)
					stopIndex = mvect_mxed_data.size() ; 
				else if (mbln_is_adc_data)
					stopIndex = mvect_adc_data.size() ; 
				else
					stopIndex = mvect_data.size() ; 

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
					//if the mz value is within a specified range of values
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

			//int last_bin = 0 ; 
			//double last_intensity = 0 ; 
			for (std::map<int, double>::iterator iter = bin_intensity_map.begin() ; iter != bin_intensity_map.end() ; iter++)
			{			
				mz = this->GetMassFromBin((*iter).first) ;
				mzs->push_back(mz) ; 
				intensities->push_back((*iter).second) ; 
				success = true;
				
			}

			bin_intensity_map.clear();
			return success;
		}

		//returns the total number of frames in this dataset
		int UIMFRawData:: GetNumOfFrames(){
			return 0;

/*
			DataReader *ReadUIMF = new DataReader();
			ReadUIMF->OpenUIMF(marr_filename);
			int numOfFrames = System::Convert::ToInt32(ReadUIMF->GetGlobalParameters("NumFrames"));
			ReadUIMF->CloseUIMF(marr_filename);

			return numOfFrames;*/
		}


		void UIMFRawData::GetSummedSpectraSlidingWindow(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range )
		{
			bool success = false;
			double mz ; 

			UIMFRecord<int, int> rec ; 
			UIMFRecord<int, float>mxed_rec ;
			UIMFRecord<int, int> adc_rec ;

			//Maybe this comes in from the parameter file, we should check that 
			//in that case these would be additional arguments on this data

			//also why is this reversed???
			double min_mz = DBL_MAX ; 
			double max_mz = DBL_MIN ; 
			int min_mz_bin = 0 ; 
			int max_mz_bin = 0 ; 
			int bin  ; 

			int startIndex = 0 ;

			//where the vector of data ends
			int stopIndex ; 
			if (mbln_is_multiplexed_data)
				stopIndex = mvect_mxed_data.size() ; 
			else if (mbln_is_adc_data)
				stopIndex = mvect_adc_data.size() ;
			else
				stopIndex = mvect_data.size() ; 

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
				startIndex = 0; // mvect_scan_start_index[start_scan-1] ;
				if (mbln_is_multiplexed_data)
					stopIndex = mvect_mxed_data.size() ; 
				else if (mbln_is_adc_data)
					stopIndex = mvect_adc_data.size() ; 
				else
					stopIndex = mvect_data.size() ; 
				
				if (start_scan < mint_num_scans)
					stopIndex = mint_scan_end_index;//mvect_scan_start_index[start_scan] ;	

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
				startIndex = 0;//mvect_scan_start_index[stop_scan-1] ;
				stopIndex = mint_scan_end_index;// mvect_scan_start_index[stop_scan] ;	
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

		void UIMFRawData::InitializeSlidingWindow(int scan_range)
		{
			// Function that initializes the mmap_bin_intensity_map for summing using a sliding window
			// Sum from scans 1 to scan_range and set it to mmap_bin_intensity_map
			UIMFRecord<int, int> rec ; 
			UIMFRecord<int, float> mxed_rec ;
			UIMFRecord<int, int> adc_rec ;

			// Start
			for (int scan_num = 1 ; scan_num <= scan_range ; scan_num++)
			{
				if (scan_num > mint_num_scans || scan_num <= 0)
					return ; 

				if (mint_scan_start_index <0) 
					break ; 

				int startIndex = 0;//mvect_scan_start_index[scan_num-1] ;
				int stopIndex ; 
				if (mbln_is_multiplexed_data)
					stopIndex = mvect_mxed_data.size() ; 
				else if (mbln_is_adc_data)
					stopIndex = mvect_adc_data.size() ; 
				else
					stopIndex = mvect_data.size() ; 

				if (scan_num < mint_num_scans)
					stopIndex = mint_scan_end_index; //mvect_scan_start_index[scan_num] ;

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

		void UIMFRawData::GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range)
		{
			std::map<int, double> bin_intensity_map ; 
			double mz ; 

			UIMFRecord<int, int> rec ; 
			UIMFRecord<int, float>mxed_rec ;
			UIMFRecord<int, int> adc_rec ;

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
			int startIndex =0;//mvect_scan_start_index[current_scan-1] ;
			int stopIndex ; 
			if (mbln_is_multiplexed_data)
				stopIndex = mvect_mxed_data.size() ; 
			else if (mbln_is_adc_data)
				stopIndex = mvect_adc_data.size() ;
			else
				stopIndex = mvect_data.size() ; 
			
			stopIndex = mint_scan_end_index; //mvect_scan_start_index[current_scan] ; 
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
				int startIndex = 0;//mvect_scan_start_index[scan_num-1] ;

				int stopIndex = mvect_data.size() ; 

				if (scan_num < mint_num_scans)
					stopIndex = mint_scan_end_index; //mvect_scan_start_index[scan_num] ;

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
		double UIMFRawData::GetFramePressure(int frame_number){
			double pressure = 0;
			/*try{
				DataReader *ReadUIMF = new DataReader();
				ReadUIMF->OpenUIMF(marr_filename);
				double pressureBack = System::Convert::ToDouble(ReadUIMF->GetFrameParameters(mint_startFrame,"PressureBack"));
				double pressureFront = System::Convert::ToDouble(ReadUIMF->GetFrameParameters(mint_startFrame,"PressureFront"));

				pressure = (pressureBack+pressureFront)/2;
				ReadUIMF->CloseUIMF(marr_filename);
			}
			catch(char * e){

			}*/

			return pressure;

		}
		bool UIMFRawData::YanGetSummedFrameAndScanSpectra(DataReader* ReadUIMF, std::vector <double> *mzs, std::vector <double> *intensities, int start_frame, int end_frame, int ims_start_scan, int ims_end_scan, double min_mz, double max_mz, int num_bins, System::String *data_type) {

			bool success = false;

			if (System::String::Compare(data_type,"ADC") == 0)
			{
				int sum_int __gc[] = new int __gc[num_bins];
				double sum_mzs __gc[] = new double __gc[num_bins];
				int sum_spec_length = ReadUIMF->SumScans(sum_mzs,sum_int,0,start_frame,end_frame,ims_start_scan,ims_end_scan);						
				if (sum_spec_length > 0)
				{
					for (int i = 0; i < sum_spec_length; i++)
					{
						if (sum_mzs[i] >= min_mz && sum_mzs[i] <= max_mz && sum_int[i] > 0)
						{
							double temp_mz = sum_mzs[i];
							mzs->push_back(temp_mz);
							int temp_int = sum_int[i];
							intensities->push_back(temp_int) ; 
						}
					}
				}
				success = true;
			}
			else if (System::String::Compare(data_type,"FOLDED") == 0)
			{
				float sum_int __gc[] = new float __gc[num_bins];
				double sum_mzs __gc[] = new double __gc[num_bins];
				int sum_spec_length = ReadUIMF->SumScans(sum_mzs,sum_int,0,start_frame,end_frame,ims_start_scan,ims_end_scan);						
				if (sum_spec_length > 0)
				{
					for (int i = 0; i < sum_spec_length; i++)
					{
						if (sum_mzs[i] >= min_mz && sum_mzs[i] <= max_mz && sum_int[i] > 0)
						{
							double temp_mz = sum_mzs[i];
							mzs->push_back(temp_mz);
							float temp_int = sum_int[i];
							intensities->push_back(temp_int) ; 
						}
					}
				}
				success = true;
			}
			else if (System::String::Compare(data_type,"TDC") == 0)
			{
				short sum_int __gc[] = new short __gc[num_bins];
				double sum_mzs __gc[] = new double __gc[num_bins];
				int sum_spec_length = ReadUIMF->SumScans(sum_mzs,sum_int,0,start_frame,end_frame,ims_start_scan,ims_end_scan);						
				if (sum_spec_length > 0)
				{
					for (int i = 0; i < sum_spec_length; i++)
					{
						if (sum_mzs[i] >= min_mz && sum_mzs[i] <= max_mz && sum_int[i] > 0)
						{
							double temp_mz = sum_mzs[i];
							mzs->push_back(temp_mz);
							short temp_int = sum_int[i];
							intensities->push_back(temp_int) ; 
						}
					}
				}
				success = true;
			}
			else
			{
				double sum_int __gc[] = new double __gc[num_bins];
				double sum_mzs __gc[] = new double __gc[num_bins];
				int sum_spec_length = ReadUIMF->SumScans(sum_mzs,sum_int,0,start_frame,end_frame,ims_start_scan,ims_end_scan);						
				if (sum_spec_length > 0)
				{
					for (int i = 0; i < sum_spec_length; i++)
					{
						if (sum_mzs[i] >= min_mz && sum_mzs[i] <= max_mz && sum_int[i] > 0)
						{
							double temp_mz = sum_mzs[i];
							mzs->push_back(temp_mz);
							double temp_int = sum_int[i];
							intensities->push_back(temp_int) ; 
						}
					}
				}
				success = true;
			}

			// Close UIMF file
			//ReadUIMF->CloseUIMF(marr_filename);

			return success;
		}				
	}
}
