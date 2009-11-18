#pragma once

#include "rawdata.h"
#include "../Utilities/SavGolSmoother.h" 

#include <map> 

namespace Engine {
 namespace Readers
{
    template <typename X, typename Y> struct UIMFRecord
	{
		X tof_bin ; 
		Y intensity ; 
	} ; 	

	class  UIMFRawData : public RawData
	{
		char marr_filename[MAX_FNAME_LEN] ; 
		double mdbl_k0 ; 
		double mdbl_t0 ; 
		double mdbl_min_mz ; 
		double mdbl_max_mz ;

		int mint_num_scans ; 
		long mint_max_scan_size ; 
		double mdbl_scan_interval ; 
		double mdbl_elution_time ;
		double mdbl_drift_time ; 
		double mdbl_max_drift_time ; 
		int mint_num_frames;
		int mint_num_scans_in_a_frame;
		int mint_startFrame;
		int mint_endFrame;
		int mint_start_bin ; 
		int mint_stop_bin ; 
		int mint_time_offset ; 
		int mdbl_bin_width ;
		int mint_last_scan_num ; 
		int mint_spectrum_length;
		int mint_scans_per_frame;

		double mdbl_avg_tof_length ; 
		int mint_frame_num;
		int mint_num_bins;

		bool mbln_is_multiplexed_data ; 
		bool mbln_is_adc_data ; 
		bool mbln_is_file_open;

		std::vector<int> mvect_scan_bpi_adc ; 
		int mint_scan_start_index ; 
		int mint_scan_end_index ; 
		std::vector<float> mvect_scan_bpi_mxed ; 
		std::vector<System::Int32> mvect_scan_bpi ; 

		std::vector<UIMFRecord<int, int> > mvect_data ; 
		std::vector<UIMFRecord<int, float> > mvect_mxed_data ; 
		std::vector<UIMFRecord<int, int> > mvect_adc_data ; 
		
		std::map<int, double> mmap_bin_intensity_map ; 
			
		Utilities::SavGolSmoother mobj_savgol ; 
		
		
	public:
		virtual void Close() ; 
		virtual void Load(char *file) ; 
		FileType GetFileType() { return PNNL_UIMF ; } ;  
		const char* GetFileName() ;
		void SetFileName(const char * file_name);
		
		UIMFLibrary::DataReader* OpenUIMFFile();
		void CloseUIMFFile();
		int GetUIMFSpectrum(char *marr_filename, int scan_num);
		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) ; 
		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 
		double GetScanTime(int scan_num) ;
		double GetFramePressure(int frame_num);
		int GetNumScansInFrame();
		int GetScanSize() ; 
		UIMFRawData(void) ;
		~UIMFRawData(void) ;
		int GetNumScans() ; 
		int GetNumOfFrames();
		double GetDriftTime(int scan_num) ; 
		int GetNumScansLoaded() { return GetNumScans() ; } 
		int GetFrameNumber() ;
		void GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range) ; 
		void GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_scan, int stop_scan, double min_mz, double max_mz)  ; 
		bool GetSummedFrameSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_frame, int stop_frame, double min_mz, double max_mz, int scan_num);
		bool GetSummedFrameAndScanSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_frame, int end_frame, int start_scan, int end_scan, double min_mz, double max_mz );
		bool YanGetSummedFrameAndScanSpectra(UIMFLibrary::DataReader* ReadUIMF, std::vector <double> *mzs, std::vector <double> *intensities, int start_frame, int end_frame, int start_scan, int end_scan, double min_mz, double max_mz, int num_bins, System::String *data_type);
		int GetTICandbpi(int frameNum, int scanNum, double &bpi, double &bpi_mz);
		double GetSignalRange(int scan_num) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ; 

		int GetFirstScanNum() ;
		int GetLastScanNum() ; 

		void GetSummedSpectraSlidingWindow(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range ) ;

		void InitializeSlidingWindow(int scan_range) ; 		
		inline double GetMassFromBin(int bin)
		{
			double t = bin * mdbl_bin_width / 1000.0; 
			return mdbl_k0 * mdbl_k0 * (t - mdbl_t0) * (t - mdbl_t0) ;
		}
		int GetParentScan(int scan_num)
		{
			//future work 
			return 0;
		}
		bool IsMSScan(int scan_num)
		{
			//future work
			return true;
		}		
		double GetParentMz(int scan_num)
		{
			//future work
			return 0;
		}
		bool IsProfileScan(int scan_num)
		{
			return true;
		}
		int GetMSLevel(int scan_num)
		{
			//future work 
			return 1;
		}		
	};
}
}