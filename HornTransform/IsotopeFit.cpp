#include ".\IsotopeFit.h"
#include <iostream>
#include <time.h>
#include <fstream>
namespace Engine
{
	namespace HornTransform
	{
		IsotopeFit::IsotopeFit(void)
		{
			mdbl_cc_mass = 1.00727638 ; 
			mbln_thrash = false ; 
			mbln_complete_fit = false ; 
			mbln_use_isotope_distribution_caching = false ; 
			mint_find_peak_cached = 0 ; 
			mint_find_peak_calc = 0 ; 
			Init() ; 
			mobj_averagine.SetElementalIsotopeComposition(mobj_isotope_dist.mobj_elemental_isotope_composition) ; 
		}

		void IsotopeFit::Init()
		{
			Reset() ; 
			mint_distribution_processing_time = 0 ; 
			mint_fit_processing_time = 0 ;
		}
		IsotopeFit::~IsotopeFit(void)
		{
			int i = 0 ; 
		}

		IsotopeFit::IsotopeFit(const IsotopeFit &fit)
		{
			// only copies settings not variables.
			this->mbln_complete_fit = fit.mbln_complete_fit ; 
			this->mbln_thrash = fit.mbln_thrash ;
			this->mdbl_cc_mass = fit.mdbl_cc_mass ; 
			this->mobj_averagine = fit.mobj_averagine ;
			this->mobj_isotope_dist = fit.mobj_isotope_dist ; 
			Init() ; 
		}

		IsotopeFit& IsotopeFit::operator=(const IsotopeFit &fit)
		{
			// only copies settings not variables.
			this->mbln_complete_fit = fit.mbln_complete_fit ; 
			this->mbln_thrash = fit.mbln_thrash ;
			this->mdbl_cc_mass = fit.mdbl_cc_mass ; 
			this->mobj_averagine = fit.mobj_averagine ;
			this->mobj_isotope_dist = fit.mobj_isotope_dist ; 
			Init() ; 
			return (*this) ;
		}

		bool IsotopeFit::FindPeak(double min_mz, double max_mz, double &mz_value, double &intensity, bool debug)
		{
			// Disable timing (MEM 2013)
			// clock_t start_t = clock() ; 
			if (!mbln_last_value_was_cached)
			{
				bool found = mobj_isotope_dist.FindPeak(min_mz, max_mz, mz_value, intensity) ; 
				// Disable timing (MEM 2013)
				// mint_find_peak_calc += (clock() - start_t) ; 
				return found ; 
			}

			mz_value = 0 ; 
			intensity = 0 ; 

			int num_pts = (int) mvect_distribution_mzs.size() ; 
			//if (debug)
			//{
			//	std::cout<<" Num points = "<<num_pts<<" searching for min_mz = "<<min_mz<<std::endl ; 
			//}

			int index = mobj_pk_index.GetNearestBinary(mvect_distribution_mzs, min_mz, 0, num_pts-1) ; 
			if (index >= num_pts)
				return false ; 

			if (mvect_distribution_mzs[index] > min_mz)
			{
				while(index > 0 && mvect_distribution_mzs[index] > min_mz)
					index-- ; 
			}
			else
			{
				while(index < num_pts && mvect_distribution_mzs[index] < min_mz)
					index++ ; 
				index-- ; 
			}

			//find index of peak with maximum intensity
			int max_index = -1 ; 
			
			for ( ; index < num_pts ; index++)
			{
				double mz = mvect_distribution_mzs[index] ; 
				if (mz > max_mz)
					break ; 
				if (mz > min_mz)
				{
					if (mvect_distribution_intensities[index] > intensity)
					{
						max_index = index ; 
						intensity = mvect_distribution_intensities[index] ; 
					}
				}
			}
			if (max_index == -1)
			{
				// Disable timing (MEM 2013)
				// mint_find_peak_cached += (clock() - start_t) ; 
				return false ; 
			}

			double X2 = mvect_distribution_mzs[max_index] ; 
			double X1 = X2 - 1.0 / mobj_isotope_dist.mint_points_per_amu ; 
			double X3 = X2 + 1.0 / mobj_isotope_dist.mint_points_per_amu ; 

			if (max_index > 0 && max_index < num_pts-1)
			{
				double Y1 = mvect_distribution_intensities[max_index - 1] ; 
				double Y2 = mvect_distribution_intensities[max_index] ; 
				double Y3 = mvect_distribution_intensities[max_index + 1] ; 

				// if the points are cached, these could be single sticks with surrounding 
				// points below background. To avoid that case, lets just check
				// and see if the differences in the theoretical mz values is as
				// expected or not. 
				if (mvect_distribution_mzs[max_index - 1] > X2 - 2.0/mobj_isotope_dist.mint_points_per_amu 
					&& mvect_distribution_mzs[max_index + 1] < X2 + 2.0/mobj_isotope_dist.mint_points_per_amu)
				{
					double d ;
					d = (Y2 - Y1) * (X3 - X2) ;     //[gord] slope?  what is this?...  Denominator... see below
					d = d - (Y3 - Y2) * (X2 - X1) ;  //
			   
					if (d == 0)
						mz_value = X2 ; 
					else
						mz_value = ((X1 + X2) - ((Y2 - Y1) * (X3 - X2) * (X1 - X3)) / d) / 2.0 ;    //[gord] what's this doing?? Looks like a mid-point calculation
				}
				else
				{
					mz_value = X2 ; 
					intensity = mvect_distribution_intensities[max_index] ; 
				}
				// Disable timing (MEM 2013)
				// mint_find_peak_cached += (clock() - start_t) ; 
				return true ; 
			}

			mz_value = X2 ; 
			intensity = mvect_distribution_intensities[max_index] ; 
			
			// Disable timing (MEM 2013)
			// mint_find_peak_cached += (clock() - start_t) ; 
			return true ; 
		}

		
		bool IsotopeFit::IsIsotopeLinkedDistribution(double min_threshold)
		{
			int num_isotopes = mvect_isotope_mzs.size()  ; 
			int size = mvect_isotope_intensities.size() ; 
		 
			for (int isotope_num = 3 ; isotope_num < num_isotopes; isotope_num ++) 
			{
				double mz = mvect_isotope_mzs[isotope_num] ; 
				double intensity = mvect_isotope_intensities[isotope_num]  ; 
				if ((intensity-min_threshold) > 50)
					return true  ; 
			}

			return false ; 
		}

		
		
		//[gord]  the following is currently unused. The idea was to give weighting to the algorithm so that
		// the user could favor certain fitting parameters (i.e. space between isotopomers) over others
		double IsotopeFit::FindIsotopicDist(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, 
				IsotopeFitRecord &iso_record, double delete_intensity_threshold, 
				double spacingWeight, double spacingVar,
				double signalToNoiseWeight, double signalToNoiseThresh,
				double ratioWeight, double ratioThreshold, 
				double fitWeight, double fitThreshold, bool debug)
		{
			if (cs <= 0) {exit(1);}

			
			//Get theoretical distribution using Mercury algorithm
			double peak_mass = (pk.mdbl_mz - mdbl_cc_mass)* cs ; 
			double resolution = pk.mdbl_mz / pk.mdbl_FWHM ;
			GetIsotopeDistribution(peak_mass, cs,  resolution, mvect_distribution_mzs, mvect_distribution_intensities, 
				delete_intensity_threshold, debug) ;

			
			double theorMostAbundantPeakMZ = mobj_isotope_dist.mdbl_max_peak_mz ; 

			
			
			
			double delta =  pk.mdbl_mz - theorMostAbundantPeakMZ ; 
			
			PeakProcessing::Peak leftPeak;

			double spacingScore = 0;
			double signalToNoiseScore = 0;
			double ratioScore = 0;
			double fitScore = 0;
			double totalScore = 0;
			double maximumScore = spacingWeight + signalToNoiseWeight + ratioWeight + fitWeight;

			


			//this will select peaks to the left until 
			for (double dd = 1.003/cs ; dd <= 10.03/cs ; dd+= 1.003/cs)          
			{
				double theorLeftPeakMZ = 0;
				double theorLeftPeakIntensity = 0;
				pk_data.FindPeak(pk.mdbl_mz - dd - pk.mdbl_FWHM, pk.mdbl_mz - dd + pk.mdbl_FWHM, leftPeak) ;    //PeakProcessing.FindPeak
				mobj_isotope_dist.FindPeak(theorMostAbundantPeakMZ - dd - 0.2/cs, theorMostAbundantPeakMZ - dd + 0.2/cs, theorLeftPeakMZ, theorLeftPeakIntensity);




				
				if (leftPeak.mdbl_mz > 0)    //if there is an experimental peak...
				{
					//get spacing score
					spacingScore = spacingWeight * 1 ; 

					//get S/N score
					if (leftPeak.mdbl_SN > signalToNoiseThresh)
					{
						signalToNoiseScore = signalToNoiseWeight * 1 ;
					}

					//get Ratio score
					double leftPeakRatio = leftPeak.mdbl_intensity / pk.mdbl_intensity;
					double theorLeftPeakRatio = theorLeftPeakIntensity / 1;    //TODO: need to check if this most abundant theor peak's intensity is 1








				}

			
				//get Ratio score
				

				










			}


			


			//get S/N score

			

			//get Fit score

			//calculate maximum score

			//get overall score
			
			


			return 0;
		    
			




		}
		
		
		PeakProcessing::PeakData IsotopeFit::GetTheoreticalIsotopicDistributionPeakList(std::vector<double> *xvals, std::vector<double> *yvals)
		{
			PeakProcessing::PeakData peakList;
			PeakProcessing::PeakProcessor processor;
			processor.SetOptions(0.5, 1, false, (PeakProcessing::PEAK_FIT_TYPE)0);
			processor.DiscoverPeaks(xvals,yvals,0,10000);
			

			int numpeaks = processor.mobj_peak_data->GetNumPeaks();
			
			for (int i=0; i<numpeaks; i++)
			{
				PeakProcessing::Peak peak;
			    processor.mobj_peak_data->GetPeak(i,peak);
				peakList.AddPeak(peak);
			}

			return peakList;


			
		}


		/*
		This fitter is used by MassTransform.cpp.   It does more than just get a fit score. It first
		gets a fit score and then slides to the left until the fit score does not improve and then resets
		to the center point and then slides to the right until the fit score does not improve. Returns the
		best fit score and fills the isotopic profile (isotopeFitRecord)
		*/
		double IsotopeFit::GetFitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, 
				IsotopeFitRecord &iso_record, double delete_intensity_threshold, 
				double min_theoretical_intensity_for_score, double leftFitStringencyFactor, 
				double rightFitStringencyFactor, bool debug)
		{
			if (cs <= 0)
			{
				std::cout<<"Negative value for charge state. "<<cs<<std::endl ; 
				exit(1) ; 
			}
			//initialize 
			double peak_mass = (pk.mdbl_mz - mdbl_cc_mass)* cs ; 
			// by now the cc_mass, tag formula and media options in Mercury(Isotope generation)
			// should be set.    
			if (debug)
			{
				std::cout<<"\n\n-------------------- BEGIN TRANSFORM ---------------------------"<<cs<<std::endl ; 
				std::cout<<"Getting isotope distribution for mass = "<<peak_mass<<" mz = "<<pk.mdbl_mz<<" charge = "<<cs<<std::endl ; 
			}
			double resolution = pk.mdbl_mz / pk.mdbl_FWHM ;
			// DJ Jan 07 2007: Need to get all peaks down to delete interval so that range of deletion is correct. 
			//GetIsotopeDistribution(peak_mass, cs,  resolution, mvect_distribution_mzs, mvect_distribution_intensities, 
			//	min_theoretical_intensity_for_score, debug) ;
			GetIsotopeDistribution(peak_mass, cs,  resolution, mvect_distribution_mzs, mvect_distribution_intensities, 
				delete_intensity_threshold, debug) ;


			PeakProcessing::PeakData theorPeakData= IsotopeFit::GetTheoreticalIsotopicDistributionPeakList(&mvect_distribution_mzs, &mvect_distribution_intensities);

			int numpeaks = theorPeakData.GetNumPeaks();
			
			if (debug)
			{
				std::cout<<"---------------------------------------- THEORETICAL PEAKS ------------------"<<std::endl ;
				std::cout<<"Theoretical peak\t"<<"Index\t"<<"MZ\t"<<"Intensity\t"<<"FWHM\t"<<"SigNoise"<<std::endl;
				for (int i=0; i<numpeaks; i++)
				{
					PeakProcessing::Peak theorpeak;
					theorPeakData.GetPeak(i,theorpeak);
					std::cout<<"Theoretical peak\t"<<i<<"\t"<<theorpeak.mdbl_mz<<"\t"<<theorpeak.mdbl_intensity<<"\t"<<theorpeak.mdbl_FWHM<<"\t"<<theorpeak.mdbl_SN<<std::endl;
				}
				std::cout<<"---------------------------------------- END THEORETICAL PEAKS ------------------"<<std::endl;
			}




			
			// Anoop April 9 2007: For checking if the distribution does not overlap/link with any other distribution
			// Beginnings of deisotoping correction			
			//bool is_linked = false ; 
			//is_linked =  IsIsotopeLinkedDistribution(delete_intensity_threshold) ; 

			double delta =  pk.mdbl_mz - mobj_isotope_dist.mdbl_max_peak_mz ; 


			


			double fit ; 
			//if(debug)
			//	std::cout<<"Going for first fit"<<std::endl ; 
			fit = FitScore(pk_data, cs, pk, delta, min_theoretical_intensity_for_score, debug) ; 
			

			if (debug)
			{
				std::cout<<"Peak\tPeakIdx\tmz\tintens\tSN\tFWHM\tfit\tdelta"<<std::endl;

				std::cout<<"CENTER\t"<<pk.mint_peak_index<<"\t"<<pk.mdbl_mz<<"\t"<<pk.mdbl_intensity<<"\t"<<pk.mdbl_SN<<"\t"<<pk.mdbl_FWHM<<"\t"<<fit<<"\t"<<delta<<"\t"<<std::endl;
			}
			
			//if (debug)
			//	std::cout<<"\tFirst fit  = "<<fit<<" Intensity = "<<pk.mdbl_intensity
			//		<<" Charge = "<<cs<<" FWHM = "<<pk.mdbl_FWHM<<" delta = "<<delta<<std::endl ; 

			if (!mbln_thrash )
			{
				iso_record.mdbl_fit = fit ; 
				iso_record.mdbl_mz = pk.mdbl_mz ; 
				iso_record.mdbl_average_mw = mobj_isotope_dist.mdbl_average_mw + delta * cs ; 
				iso_record.mdbl_mono_mw = mobj_isotope_dist.mdbl_mono_mw + delta*cs ; 
				iso_record.mdbl_most_intense_mw = mobj_isotope_dist.mdbl_most_intense_mw + delta*cs ;
				iso_record.mdbl_delta_mz = delta ; 
				return fit ; 
			}

			double p1fit =-1, m1fit = -1  ;        // [gord]: this seems unused
			double Mpeak = mobj_isotope_dist.mdbl_max_peak_mz ; 
			PeakProcessing::Peak nxt_peak ; 

			double best_fit = fit ; 
			double best_delta = delta ; 
			double MaxY = pk.mdbl_intensity ;



			//------------- Slide to the LEFT --------------------------------------------------
			for (double dd = 1.003/cs ; dd <= 10.03/cs ; dd+= 1.003/cs)          
			{
				double mz, intensity ;    //[gord] should these be reset to '0'?
				
				//check for theoretical peak to the right of TheoreticalMaxPeak; store mz and intensity
				bool found_peak = FindPeak(Mpeak+dd - 0.2/cs, Mpeak+dd+ 0.2 /cs, mz, intensity, debug) ; 
				
				// if the above theoretical peak was found,  look one peak to the LEFT in the Experimental peaklist
				if (found_peak)  
				{
					pk_data.FindPeak(pk.mdbl_mz - dd - pk.mdbl_FWHM, pk.mdbl_mz - dd + pk.mdbl_FWHM, nxt_peak) ;    //PeakProcessing.FindPeak
				}

				//if (debug)
				//	std::cout<<"\t\t Move by "<<dd ; 

				if (mz > 0 && nxt_peak.mdbl_mz > 0)    //if there is a theoreticalPeak to the RIGHT of theoreticalMaxPeak AND there is an experimentalPeak to the LEFT of experimentalMaxPeak...
				{
					delta = pk.mdbl_mz - mz ;       // essentially, this shifts the theoretical over to the left and gets the delta; then check the fit
					PeakProcessing::Peak current_peak_copy = pk ;                   // in c++ this copy is created by value;
					current_peak_copy.mdbl_intensity = nxt_peak.mdbl_intensity ; 
					fit = FitScore(pk_data, cs, current_peak_copy, delta, min_theoretical_intensity_for_score) ; 
					if (debug)
					{
						//std::cout<<" isotopes. Fit ="<<fit<<" Charge = "<<cs<<" Intensity = "<<nxt_peak.mdbl_intensity<<" delta = "<<delta<<std::endl ;
						std::cout<<"LEFT\t"<<nxt_peak.mint_peak_index<<"\t"<<nxt_peak.mdbl_mz<<"\t"<<nxt_peak.mdbl_intensity<<"\t"<<nxt_peak.mdbl_SN<<"\t"<<nxt_peak.mdbl_FWHM<<"\t"<<fit<<"\t"<<delta<<std::endl;
					}

				}

				else
				{
					if (debug)
						std::cout<<"LEFT\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<std::endl;
					fit = best_fit + 1000 ;   // make the fit terrible
				}
				// TODO: Currently, if fix score is less than best_fit, iteration stops.  Future versions should continue attempted fitting if fit was within a specified range of the best fit
				// 26th February 2007 Deep Jaitly
				/*if (fit <= best_fit) 
				{
					if (nxt_peak.mdbl_intensity > pk.mdbl_intensity)
						pk.mdbl_intensity = nxt_peak.mdbl_intensity ; 
					MaxY = pk.mdbl_intensity ; 
					best_fit = fit ; 
					best_delta = delta ; 
				}*/
				double leftFitFactor = fit / best_fit;
				if (leftFitFactor <= leftFitStringencyFactor)
				{
					if (nxt_peak.mdbl_intensity > pk.mdbl_intensity)
						pk.mdbl_intensity = nxt_peak.mdbl_intensity ; 
					MaxY = pk.mdbl_intensity ; 
					best_fit = fit ; 
					best_delta = delta ; 
				}
				else
				{
					if (p1fit == -1)       //[gord]   what is this doing?  Peak1 fit??
						p1fit = fit ; 
					if (!mbln_complete_fit)
						break ; 
				}
			}

			//if (debug)
			//		std::cout<<"\n---------------- Sliding to the RIGHT -------------------------" ; 
			for (double dd = 1.003/cs ; dd <= 10.03/cs ; dd+= 1.003/cs)
			{
				double mz, intensity ;
				mz = 0 ; 
				intensity = 0 ; 

				////check for theoretical peak to the LEFT of TheoreticalMaxPeak; store mz and intensity
				bool found_peak = FindPeak(Mpeak - dd - 0.2/cs, Mpeak - dd + 0.2 /cs, mz, intensity, debug) ; 
				
				
				// if the above theoretical peak was found,  look one peak to the RIGHT in the Experimental peaklist
				if (found_peak)
				{
					pk_data.FindPeak(pk.mdbl_mz + dd - pk.mdbl_FWHM, pk.mdbl_mz + dd + pk.mdbl_FWHM, nxt_peak) ; 
				}
				//if (debug)
				//	std::cout<<"\t\t Move back by "<<dd ;
				if (mz > 0 && nxt_peak.mdbl_mz > 0)
				{
					delta = pk.mdbl_mz - mz ; 
					PeakProcessing::Peak current_peak_copy = pk ; 
					current_peak_copy.mdbl_intensity = nxt_peak.mdbl_intensity ; 
					fit = FitScore(pk_data, cs, current_peak_copy, delta, min_theoretical_intensity_for_score) ; 
					//fit = FitScore(pk_data, cs, nxt_peak.mdbl_intensity, delta) ; 
					if (debug)
					{
						//std::cout<<" isotopes. Fit ="<<fit<<" Charge = "<<cs<<" Intensity = "<<nxt_peak.mdbl_intensity<<" delta = "<<delta<<std::endl ; 
						std::cout<<"RIGHT\t"<<nxt_peak.mint_peak_index<<"\t"<<nxt_peak.mdbl_mz<<"\t"<<nxt_peak.mdbl_intensity<<"\t"<<nxt_peak.mdbl_SN<<"\t"<<nxt_peak.mdbl_FWHM<<"\t"<<fit<<"\t"<<delta<<std::endl;

					}
				}
				else
				{
					fit = best_fit + 1000 ;    //force it to be a bad fit
					if (debug)
					{
					//	std::cout<<"No peak found"<<std::endl ; 
						std::cout<<"RIGHT\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<"\t"<<-1<<std::endl;

					}
				}

				/*if (fit <= best_fit)
				{
				if (nxt_peak.mdbl_intensity > pk.mdbl_intensity)
				pk.mdbl_intensity = nxt_peak.mdbl_intensity ; 
				MaxY = pk.mdbl_intensity ; 
				best_fit = fit ; 
				best_delta = delta ; 
				}*/
				double rightFitFactor = fit / best_fit;
				if (rightFitFactor <= rightFitStringencyFactor)
				{
					if (nxt_peak.mdbl_intensity > pk.mdbl_intensity)
						pk.mdbl_intensity = nxt_peak.mdbl_intensity ; 
					MaxY = pk.mdbl_intensity ; 
					best_fit = fit ; 
					best_delta = delta ; 
				}
				
			
				else
				{
					if (m1fit == -1)
						m1fit = fit ; 
					if (!mbln_complete_fit)
						break ; 
				}
			}

			delta = best_delta ;

			double theorIntensityCutoff = 30;  // 

			double peakWidth = pk.mdbl_FWHM; 

			if (debug)
				{
						std::cout<<"Std delta = \t"<<delta<<std::endl;
				}


			//delta = CalculateDeltaFromSeveralObservedPeaks(delta, peakWidth, pk_data, theorPeakData, theorIntensityCutoff);

		if (debug)
				{
						std::cout<<"Weighted delta = \t"<<delta<<std::endl;
				}

			iso_record.mdbl_fit = best_fit ; 
			iso_record.mshort_cs = cs ; 
			iso_record.mdbl_mz = pk.mdbl_mz ; 
			iso_record.mdbl_delta_mz = delta ; 
			iso_record.mdbl_average_mw = mobj_isotope_dist.mdbl_average_mw + delta * cs ; 
			iso_record.mdbl_mono_mw = mobj_isotope_dist.mdbl_mono_mw + delta*cs ; 
			iso_record.mdbl_most_intense_mw = mobj_isotope_dist.mdbl_most_intense_mw + delta*cs ; 


			//iso_record.mbln_flag_isotope_link = is_linked ; 
			return best_fit ; 
		}

		double IsotopeFit::GetFitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, 
				TheoreticalProfile::MolecularFormula &formula, double delete_intensity_threshold, 
				double min_theoretical_intensity_for_score, bool debug)
		{
			if (cs <= 0)
			{
				std::cout<<"Negative value for charge state. "<<cs<<std::endl ; 
				exit(1) ; 
			}
			if (debug)
				std::cout<<"Getting isotope distribution for formula = "<<formula<<" mz = "<<pk.mdbl_mz<<" charge = "<<cs<<std::endl ; 

			double resolution = pk.mdbl_mz / pk.mdbl_FWHM ;

			mobj_isotope_dist.mdbl_cc_mass = mdbl_cc_mass ;
			mobj_isotope_dist.menm_ap_type = TheoreticalProfile::GAUSSIAN ; 

			mvect_distribution_intensities.clear() ; 
			mvect_distribution_mzs.clear() ; 

			mobj_isotope_dist.CalculateDistribution(cs, resolution, formula, mvect_distribution_mzs, mvect_distribution_intensities, 
				delete_intensity_threshold, mvect_isotope_mzs, mvect_isotope_intensities, debug) ; 

			double delta =  pk.mdbl_mz - mobj_isotope_dist.mdbl_max_peak_mz ; 
			double fit ; 
			if(debug)
				std::cout<<"Going for first fit"<<std::endl ; 
			fit = FitScore(pk_data, cs, pk, delta, min_theoretical_intensity_for_score, debug) ; 
			return fit ; 
		}

		void IsotopeFit::GetOptions(std::string &averagine_mf, std::string &tag_mf, bool &thrash_or_not, bool &complete_fit)
		{
			thrash_or_not = mbln_thrash ; 
			complete_fit = mbln_complete_fit ; 
			mobj_averagine.GetAveragineFormula(averagine_mf) ; 
			mobj_averagine.GetTagFormula(tag_mf) ; 
		}

		void IsotopeFit::SetOptions(std::string averagine_mf, std::string tag_mf, double cc_mass, bool thrash_or_not, bool complete_fit)
		{
			mobj_averagine.SetElementalIsotopeComposition(mobj_isotope_dist.mobj_elemental_isotope_composition) ; 
			mobj_averagine.SetAveragineFormula(averagine_mf) ; 
			mobj_averagine.SetTagFormula(tag_mf) ; 
			mbln_thrash = thrash_or_not ; 
			mbln_complete_fit = complete_fit ; 
			mdbl_cc_mass = cc_mass ; 
			mobj_mercury_cache.SetOptions(cc_mass, mobj_isotope_dist.mint_mercury_size) ; 
		}

		void IsotopeFit::GetZeroingMassRange(double &start_mz, double &stop_mz, double delta, double thresh, bool debug = false)
		{

			// this assumes that the last peak was not changed till now.
			int num_theoretical_points = (int) mvect_distribution_intensities.size() ; 
			double max_pk ; 
			double max_int = 0 ; 

			for(int i = 0 ; i < num_theoretical_points ; i++)
			{
				if (mvect_distribution_intensities[i] > max_int)
				{
					max_pk = mvect_distribution_mzs[i] ; 
					max_int = mvect_distribution_intensities[i] ; 
				}
			}


			for(int i = 0 ; i < num_theoretical_points ; i++)
			{
				double intensity = mvect_distribution_intensities[i] ;
				if (intensity > thresh)
				{
					start_mz = mvect_distribution_mzs[i] ; 
					stop_mz = start_mz ; 
					break ; 
				}
			}

			for(int i = num_theoretical_points-1 ; i > 0 ; i--)
			{
				if (mvect_distribution_intensities[i] > thresh)
				{
					stop_mz = mvect_distribution_mzs[i] ; 
					break ; 
				}
			}
			start_mz = start_mz + delta ; 
			stop_mz = stop_mz + delta ; 
			if (debug)
			{
				std::cout<<"\t Start MZ for deletion ="<<start_mz<<" Stop MZ for deletion = "<<stop_mz<<std::endl ;
			}

			return ; 
		}

		inline void IsotopeFit::GetIsotopeDistribution(double most_abundant_mass, short charge, double resolution, 
			std::vector<double>&mzs, std::vector<double>&intensities, double min_theoretical_intensity, 
			bool debug)
		{
			double current_mz = most_abundant_mass/charge + mdbl_cc_mass ;
			double FWHM = current_mz / resolution ; 
			
			// Disable timing (MEM 2013)
			// clock_t start_t = clock() ;

			//first check the UseCaching option; then check if it is in the cache or not. 
			//if it is in the cache, the data is retrieved and the method returns true
			//so If either is false then need to Calculate theor isotopic dist. 
			bool needToCalculateIsotopeDistribution = (!mbln_use_isotope_distribution_caching ||!mobj_mercury_cache.GetIsotopeDistributionCached(most_abundant_mass, charge, 
				FWHM, min_theoretical_intensity, mzs, intensities));
			
			if (needToCalculateIsotopeDistribution)   
			{
				mbln_last_value_was_cached = false ;
				mobj_averagine.GetAverageFormulaForMass(most_abundant_mass, mobj_empirical_formula) ; 
				long lng_charge = (long) charge ; 

				mobj_isotope_dist.mdbl_cc_mass = mdbl_cc_mass ;
				mobj_isotope_dist.menm_ap_type = TheoreticalProfile::GAUSSIAN ; 
				

				intensities.clear() ; 
				mzs.clear() ; 

				if (debug)
				{
					std::cout<<"Getting distribution for chemical ="<<mobj_empirical_formula<<std::endl ;
				}
				mobj_isotope_dist.CalculateDistribution(charge, resolution, mobj_empirical_formula, mzs, intensities, min_theoretical_intensity, 
					mvect_isotope_mzs, mvect_isotope_intensities, debug) ; 
				if (mbln_use_isotope_distribution_caching)
				{
					mobj_mercury_cache.CacheIsotopeDistribution(most_abundant_mass, mobj_isotope_dist.mdbl_most_intense_mw, 
						mobj_isotope_dist.mdbl_mono_mw, mobj_isotope_dist.mdbl_average_mw, mobj_isotope_dist.mdbl_max_peak_mz, 
						charge, FWHM, mobj_isotope_dist.mdbl_mass_variance, mobj_isotope_dist.mint_points_per_amu, min_theoretical_intensity,
						mvect_isotope_mzs, mvect_isotope_intensities) ;
				}
			}
			else
			{
				mbln_last_value_was_cached = true ;
				//mzs, intensities are fetched, get the average and mono mw stats.
				mobj_isotope_dist.mdbl_average_mw = mobj_mercury_cache.mdbl_average_mw ; 
				mobj_isotope_dist.mdbl_mono_mw = mobj_mercury_cache.mdbl_mono_mw ; 
				mobj_isotope_dist.mdbl_most_intense_mw = mobj_mercury_cache.mdbl_most_intense_mw ; 
				mobj_isotope_dist.mdbl_max_peak_mz =  mobj_mercury_cache.mdbl_most_intense_mw/charge + mdbl_cc_mass - ELECTRON_MASS ; 
			}
			
			// Disable timing (MEM 2013)
			// clock_t stop_t = clock() ; 
			// mint_distribution_processing_time  += (stop_t - start_t) ; 
			
			return ; 
		}


		void IsotopeFit::SetCCMass(double mass)
		{
			mdbl_cc_mass = mass ; 
			mobj_mercury_cache.SetOptions(mass, mobj_isotope_dist.mint_mercury_size) ; 
		}

		void IsotopeFit::SetElementalIsotopeComposition(const TheoreticalProfile::AtomicInformation &iso_comp)
		{
			mobj_isotope_dist.SetElementalIsotopeComposition(iso_comp) ; 
			mobj_averagine.SetElementalIsotopeComposition(iso_comp) ; 
		}
		void IsotopeFit::GetElementalIsotopeComposition(TheoreticalProfile::AtomicInformation &iso_comp)
		{
			iso_comp = mobj_isotope_dist.mobj_elemental_isotope_composition ;
		}

		double IsotopeFit::CalculateDeltaFromSeveralObservedPeaks(double startingDelta, double peakWidth, PeakProcessing::PeakData &obsPeakData, PeakProcessing::PeakData &theorPeakData, double theorIntensityCutOff)
		{
			//the idea is to use a selected number of theor peaks
			//and for each theor peak,  use the delta (mz offset) info
			//to find the obs peak data and determine the delta value for that peak.
			//accumulate delta values in an array and then calculate a weighted average
			
			int numTheorPeaks = theorPeakData.GetNumPeaks();

			PeakProcessing::PeakData filteredTheorPeakData;

			
			//filter the theor list
			int numFilteredTheorPeaks = 0;
			for (int i=0; i<numTheorPeaks; i++)
			{
				PeakProcessing::Peak peak;
				theorPeakData.GetPeak(i,peak);

				if (peak.mdbl_intensity>= theorIntensityCutOff)
				{
					filteredTheorPeakData.AddPeak(peak);
					numFilteredTheorPeaks++;
				}
			}

			if (numFilteredTheorPeaks==0)return startingDelta;

			

			double* deltaArray = new double[numFilteredTheorPeaks];
			double* intensityArray= new double [numFilteredTheorPeaks];
			double intensitySum = 0 ;

			double weightedSumOfDeltas = 0;

			for (int i=0; i<numFilteredTheorPeaks; i++)
			{
				PeakProcessing::Peak theorPeak;
				filteredTheorPeakData.GetPeak(i,theorPeak);

				double targetMZLower = theorPeak.mdbl_mz + startingDelta - peakWidth; 
				double targetMZUpper = theorPeak.mdbl_mz + startingDelta + peakWidth; 
				
				PeakProcessing::Peak foundPeak;
				obsPeakData.FindPeak(targetMZLower,targetMZUpper,foundPeak);

				if (foundPeak.mdbl_mz>0)
				{
					deltaArray[i] = foundPeak.mdbl_mz - theorPeak.mdbl_mz;
					intensityArray[i] = foundPeak.mdbl_intensity;
					intensitySum += foundPeak.mdbl_intensity;
				}
				else
				{
					deltaArray[i] = startingDelta;
					intensityArray[i] = 0;       //obs peak was not found; therefore assign 0 intensity (will have no effect on delta calc)
				}
			}

			if (intensitySum==0)return startingDelta;   // no obs peaks found at all;  return default


			//now perform a weighted average

			double weightedDelta = 0 ;
			for (int i=0; i< numFilteredTheorPeaks; i++)
			{
				weightedDelta += intensityArray[i] / intensitySum * deltaArray[i];
			}

			return weightedDelta; 

		}


		
	}
}