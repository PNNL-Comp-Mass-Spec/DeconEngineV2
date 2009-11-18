#pragma once

#include "rawdata.h"
#include "../Utilities/SavGolSmoother.h" 

#include <map> 

namespace Engine {
 namespace Readers
{
	template <typename X, typename Y> struct TOFRecord
	{
		X tof_bin ; 
		Y intensity ; 
	} ; 	
	

	class  IMSRawData : public RawData
	{
		char marr_filename[MAX_FNAME_LEN] ; 
		double mdbl_k0 ; 
		double mdbl_t0 ; 
		double mdbl_min_mz ; 
		double mdbl_max_mz ; 
        
		int mint_num_scans ; 
		int mint_max_scan_size ; 
		double mdbl_scan_interval ; 
		double mdbl_elution_time ;
		double mdbl_drift_time ; 
		double mdbl_max_drift_time ; 
		int mint_start_bin ; 
		int mint_stop_bin ; 
		int mint_time_offset ; 
		int mint_bin_width ;
		int mint_last_scan_num ; 
		
		double mdbl_avg_tof_length ; 
		int mint_frame_num;

		bool mbln_is_multiplexed_data ; 
		bool mbln_is_adc_data ; 

		std::vector<int> mvect_scan_bpi_adc ; 
		std::vector<int> mvect_scan_start_index ; 
		std::vector<float> mvect_scan_bpi_mxed ; 
		std::vector<short> mvect_scan_bpi ; 

		std::vector<TOFRecord<int, short> > mvect_data ; 
		std::vector<TOFRecord<int, float> > mvect_mxed_data ; 
		std::vector<TOFRecord<int, int> > mvect_adc_data ; 

		std::map<int, double> mmap_bin_intensity_map ; 
		

		Utilities::SavGolSmoother mobj_savgol ; 
		inline double GetMassFromBin(int bin)
		{
			
			double bin_val = bin * 0.1 ; //(bin/16 (for index) * 16/10(to convert to ns))
//			double bin_val = bin * 1.6 ; for better resolution
			double t = bin_val + mint_time_offset ; 
			return mdbl_k0 * mdbl_k0 * (t - mdbl_t0) * (t - mdbl_t0) ;
		}
		
		
	public:
		virtual void Close() ; 
		virtual void Load(char *file) ; 
		FileType GetFileType() { return PNNL_IMS ; } ;  
		const char* GetFileName() ;

		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num) ; 
		bool GetRawData(std::vector <double> *mzs, std::vector<double> *intensities, int scan_num, int num_pts) ; 
		double GetScanTime(int scan_num) ; 
		int GetScanSize() ; 
		IMSRawData(void) ;
		~IMSRawData(void) ;
		int GetNumScans() ; 
		double GetDriftTime(int scan_num) ; 
		int GetNumScansLoaded() { return GetNumScans() ; } 
		int GetFrameNumber() ;
		void GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range) ; 
		void GetSummedSpectra(std::vector <double> *mzs, std::vector <double> *intensities, int start_scan, int stop_scan, double min_mz, double max_mz)  ; 
		
		double GetSignalRange(int scan_num) ; 
		void GetTicFromFile(std::vector<double> *intensities, std::vector<double> *scan_times, bool base_peak_tic) ; 

		int GetFirstScanNum() ;
		int GetLastScanNum() ; 

		void GetSummedSpectraSlidingWindow(std::vector <double> *mzs, std::vector <double> *intensities, int current_scan, int scan_range ) ;

		void InitializeSlidingWindow(int scan_range) ; 		

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