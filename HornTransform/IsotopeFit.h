#pragma once
#include "../PeakProcessor/PeakProcessor.h" 
#include "../PeakProcessor/PeakStatistician.h" 
#include "IsotopeFitRecord.h"
#include "../TheoreticalProfile/Averagine.h"
#include "../TheoreticalProfile/MercuryIsotopeDistribution.h"
#include "MercuryCache.h"
#include <vector> 


namespace Engine	
{
	namespace HornTransform
	{	
		enum IsotopicFittingType { LEASTSQPEAK = 0 , LEASTSQAREA , CHISQ } ; 

		//! Base class for calculating isotope fit values between theoretical distrbution and observed one.
		/*!
			This class is the base class for the isotope fitting. There are three types of fits:
			Area Fit, Peak Fit, and Chi Sq. Fit.
		*/
		class  IsotopeFit
		{
			int mint_distribution_processing_time  ;
			bool mbln_use_isotope_distribution_caching ; 
			bool mbln_last_value_was_cached ; 
			MercuryCache mobj_mercury_cache ; 
			void Init() ; 
		protected:
			int mint_fit_processing_time  ;
			int mint_find_peak_cached ; 
			int mint_find_peak_calc ; 
			TheoreticalProfile::MolecularFormula mobj_empirical_formula ; 
	#pragma warning(disable:4251)
			//! std::vector to store m/z values from the theoretical distribution. Corresponding intensities are stored in IsotopeFit::mvect_distribution_intensities 
			std::vector<double> mvect_distribution_mzs ; 
			//! std::vector to store intensities from the theoretical distribution. Corresponding mzs are stored in IsotopeFit::mvect_distribution_intensities 
			std::vector<double> mvect_distribution_intensities ; 
			std::vector<double> mvect_isotope_mzs ; 
			std::vector<double> mvect_isotope_intensities ; 
			//! variable to help find points of a givan m/z quickly in a sorted array.
			PeakProcessing::PeakIndex<double> mobj_pk_index ; 
	#pragma warning(default:4251)

			//! this variable helps keep track of the last min m/z value we looked at with the function IsotopeFit::GetPointIntensity
			/*!
				\sa IsotopeFit::GetPointIntensity
			*/
			double mdbl_mz1 ; 
			//! this variable helps keep track of the last max m/z value we looked at with the function IsotopeFit::GetPointIntensity
			/*!
				\sa IsotopeFit::GetPointIntensity
			*/
			double mdbl_mz2 ;
			//! this variable helps keep track of the last intensity value at the lower m/z which we looked at with the function IsotopeFit::GetPointIntensity
			/*!
				\sa IsotopeFit::GetPointIntensity
			*/
			double mdbl_int1 ; 
			//! this variable helps keep track of the last intensity value at the higher m/z which we looked at with the function IsotopeFit::GetPointIntensity
			/*!
				\sa IsotopeFit::GetPointIntensity
			*/
			double mdbl_int2 ;
			//! keeps track of the last index looked at with IsotopeFit::GetPointIntensity
			/*!
				\sa IsotopeFit::GetPointIntensity
			*/
			int mint_last_point_index ; 

			TheoreticalProfile::Averagine mobj_averagine ; 
			//! mass of the charge carrier. 
			double mdbl_cc_mass ; 
			//! flag to control thrashing about the most intense peak. See details of THRASH algorithm by Horn et. al.
			bool mbln_thrash ; 
			//! flag to make the fit function look at all possible isotopes to thrash to. If this is set to false, thrashing stops as soon as we reach a missing isotopic peak.
			bool mbln_complete_fit ; 



			//! Gets the isotope distribution for the given most abundance mass and charge with provided resolution.
			/*!
				\param most_abundant_mass Specifies the mass of the observed distribution which is believed to represent the most 
				intense isotope.
				\param charge charge of the species
				\param resolution resolution at which the theoretical profile should be generated. 
				\param mzs vector for output of mz values .
				\param intensities vector for output of intensity values .
				\param min_theoretical_intensity intensity of the minimum point to be provided in the vectors as output. 
			*/
			void GetIsotopeDistribution(double most_abundant_mass, short charge, double resolution, std::vector<double>&mzs, std::vector<double>&intensities,
				double min_theoretical_intensity, bool debug);

		public:

			IsotopeFit(const IsotopeFit &other_one) ; 
			IsotopeFit& operator=(const IsotopeFit &fit) ; 
			//! variable to do the calculation of isotopic distribution from mass, using averagine and mercury.
			TheoreticalProfile::MercuryIsotopeDistribution mobj_isotope_dist ; 
			//! default constructor.
			IsotopeFit(void);
			//! destructor.
			~IsotopeFit(void);

			//! get options for the isotope fit. It also gets the options for theoretical isotope generation.
			/*
				\param averagine_mf is the averagine molecular formula.
				\param tag_mf is the molecular formula of the labeling tag used to label peptide ("" if no tag was used).
				\param thrash_or_not specifies whether or not to do thrashing. See details of THRASH by Horn et. al.
				\param complete_fit: if thrashing is enable, we may want to thrash not just one or two isotopes (as the score improves
				 but to all possible isotopes. If this value is true, then the thrashing continues to all isotopes looking for 
				 better scores. If false and thrash_or_not is true, then thrashing only continues as long as the fit score
				 keeps increasing. If thrash_or_not is false, none is performed.
			*/
			void GetOptions(std::string &averagine_mf, std::string &tag_mf, bool &thrash_or_not, bool &complete_fit) ; 

			//! set options for the isotope fit. It also sets the options for theoretical isotope generation.
			/*
				\param averagine_mf is the averagine molecular formula.
				\param tag_mf is the molecular formula of the labeling tag used to label peptide ("" if no tag was used).
				\param cc_mass is the charge carrier mass.
				\param intensity_thresh specifies the minimum normalized intensity for a point to be considered in the fit function.
				\param thrash_or_not specifies whether or not to do thrashing. See details of THRASH by Horn et. al.
				\param complete_fit: if thrashing is enable, we may want to thrash not just one or two isotopes (as the score improves
				 but to all possible isotopes. If this value is true, then the thrashing continues to all isotopes looking for 
				 better scores. If false and thrash_or_not is true, then thrashing only continues as long as the fit score
				 keeps increasing. If thrash_or_not is false, none is performed.
			*/
			void SetOptions(std::string averagine_mf, std::string tag_mf, double cc_mass, bool thrash_or_not, bool complete_fit) ; 
			//! checks if any of the isotopes in the distribution ispossible part of a different distribution
			/*
				\param min_threshold - threshold for that spectrum
			*/
			bool IsIsotopeLinkedDistribution(double min_threshold) ; 
			
			PeakProcessing::PeakData GetTheoreticalIsotopicDistributionPeakList(std::vector<double> *xvals, std::vector<double> *yvals);


			//will calculate the delta mz (referenced to the theor) based on several of the observed peaks
			double CalculateDeltaFromSeveralObservedPeaks(double startingDelta, double peakWidth, PeakProcessing::PeakData &obsPeakData, PeakProcessing::PeakData &theorPeakData, double theorIntensityCutOff);



			/*

			*/
			double FindIsotopicDist(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, 
				IsotopeFitRecord &iso_record, double delete_intensity_threshold, 
				double spacingWeight, double spacingVar,
				double signalToNoiseWeight, double signalToNoiseThresh,
				double ratioWeight, double ratioThreshold, 
				double fitWeight, double fitThreshold, bool debug = false) ;

			
			//! calculates the fit score for a peak.
			/*
				\param pk_data  variable which stores the data itself
				\param cs  charge state at which we want to compute the peak.
				\param pk  peak for which we want to compute the fit function.
				\param iso_record stores the result of the fit. 
				\param delete_intensity_threshold: intensity of least isotope to delete. 
				\param min_theoretical_intensity_for_score minimum intensity of point to consider for scoring purposes. 
			*/
			double GetFitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, 
				IsotopeFitRecord &iso_record, double delete_intensity_threshold, 
				double min_theoretical_intensity_for_score, double leftFitStringencyFactor, double rightFitStringencyFactor,
				bool debug = false) ; 
			//! calculates the fit score for a peak against a molecular formula.
			/*
				\param pk_data  variable which stores the data itself
				\param cs  charge state at which we want to compute the peak.
				\param pk  peak for which we want to compute the fit function.
				\param formula stores the formula we want to fit. 
				\param delete_intensity_threshold intensity of least isotope to delete. 
				\param min_theoretical_intensity_for_score minimum intensity of point to consider for scoring purposes. 
			*/
			double GetFitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, 
				TheoreticalProfile::MolecularFormula &formula, double delete_intensity_threshold,
				double min_theoretical_intensity_for_score, bool debug = false) ; 

			//! calculates the fit score between the theoretical distribution stored and the observed data. Normalizes the observed intensity by specified intensity.
			/*
				\param pk_data  variable which stores the data itself
				\param cs  charge state at which we want to compute the peak.
				\param pk  peak for which we want to compute the fit function.
				\param mz_delta specifies the mass delta between theoretical and observed m/z with the best fit so far.
				\param min_intensity minimum intensity for score
			*/
			virtual double FitScore(PeakProcessing::PeakData &pk_data, short cs, PeakProcessing::Peak &pk, 
				double mz_delta, double min_intensity_for_score, bool debug = false ) = 0   ;
			//! calculates the fit score between the theoretical distribution stored and the observed data. Normalizes the observed intensity by specified intensity.
			/*
				\param pk_data  variable which stores the data itself
				\param cs  charge state at which we want to compute the peak.
				\param normalizer intensity to normalize the peaks to. assumes that if peak with intensity = normalizer was present, it would be normalized to 100
				\param mz_delta specifies the mass delta between theoretical and observed m/z. The we are looking to score against the feature in the observed data at theoeretical m/z + mz_delta
				\param min_intensity minimum intensity for score
				\param debug prints debugging information if this is set to true.
			*/
			virtual double FitScore(PeakProcessing::PeakData &pk_data, short cs, double normalizer, 
				double mz_delta, double min_intensity_for_score, bool debug = false ) = 0   ;

			//! gets the intensity for a given mz. 
			/*!
				We look for the intensity at a given m/z value in the raw data std::vector ptr_vect_mzs 
				(the intensities are stored in the corresponding raw data intensity std::vector ptr_vect_inensities). 
				If the value does not exist, we interpolate the intensities of points before and after this m/z value.
				\param mz the m/z value for which we want to find the intensity. 
				\param ptr_vect_mzs pointer to std::vector with observed m/z values.
				\param ptr_vect_intensities pointer to std::vector with observed intensity values.
				\return returns the intensity of the peak which has the given m/z. If the exact peak is not present, 
				then we interpolate the intensity. If the m/z value is greater than the maximum mz or less, 
				then the minimum m/z, 0 is returned.
			*/
			inline double GetPointIntensity(double mz, std::vector<double> *ptr_vect_mzs, std::vector<double> *ptr_vect_intensities)
			{
				if ((mdbl_mz1 < mz && mdbl_mz2 < mz) || (mdbl_mz1 > mz && mdbl_mz2 > mz))
				{
					int num_pts = (int) ptr_vect_mzs->size() ; 
					if (mint_last_point_index >= num_pts)
						mint_last_point_index = -1 ; 

					 //since points are more likely to be searched in order. 
					if (mint_last_point_index != -1 && mdbl_mz2 < mz)
					{
						if (mint_last_point_index < num_pts-1 && (*ptr_vect_mzs)[mint_last_point_index+1] > mz)
							mint_last_point_index++ ; 
						else
							mint_last_point_index = mobj_pk_index.GetNearestBinary(*ptr_vect_mzs, mz, mint_last_point_index, num_pts-1) ; 
					}
					else
						mint_last_point_index = mobj_pk_index.GetNearestBinary(*ptr_vect_mzs, mz, 0, num_pts-1) ; 
					if (mint_last_point_index >= num_pts)
						return 0 ; 

					if (mint_last_point_index < 0)
						mint_last_point_index = 0 ; 

					if ((*ptr_vect_mzs)[mint_last_point_index] > mz)
					{
						while(mint_last_point_index > 0 && (*ptr_vect_mzs)[mint_last_point_index] > mz)
							mint_last_point_index-- ; 
					}
					else
					{
						while(mint_last_point_index < num_pts && (*ptr_vect_mzs)[mint_last_point_index] < mz)
							mint_last_point_index++ ; 
						mint_last_point_index-- ; 
					}

					if (mint_last_point_index == num_pts -1)
						mint_last_point_index = num_pts - 2 ; 

					mdbl_mz1 = (*ptr_vect_mzs)[mint_last_point_index] ; 
					mdbl_mz2 = (*ptr_vect_mzs)[mint_last_point_index+1] ; 

					mdbl_int1 = (*ptr_vect_intensities)[mint_last_point_index] ; 
					mdbl_int2 = (*ptr_vect_intensities)[mint_last_point_index+1] ; 
				}
				if (mdbl_mz1 == mdbl_mz2)
					return mdbl_int1 ; 

				return ((mz - mdbl_mz1) / (mdbl_mz2 - mdbl_mz1)) * (mdbl_int2 - mdbl_int1) + mdbl_int1 ; 
			}
			//! specifies the mass range of the theoretical distribution which covers all points that are of intensity greater than specified value.
			/*!
				\param start_mz variable to store the value of the starting m/z
				\param end_mz variable to store the value of the ending m/z
				\param delta variable to store the mz delta between theoretical profile and observed.
				\param thresh specifies the threshold intensity that the mass range should necessarily cover.
				\param debug whether to print debug messages or not. (false by default).
				\remarks This function is used to find out the mass range that should be used to zero out peaks after the deisotoping of that peak is complete. For this we need to know how much of the mass range the peak would cover. 
			*/
			void GetZeroingMassRange(double &start_mz, double &stop_mz, double delta, double thresh, bool debug) ; // returns the mass range for the last

			void SetCCMass(double mass) ; 
			//! Sets the theoretical isotope composition for all the elements.
			/*!
				\param iso_comp Specifies the new AtomicInformation object. 
			*/
			void SetElementalIsotopeComposition(const TheoreticalProfile::AtomicInformation &iso_comp) ; 
			void GetElementalIsotopeComposition(TheoreticalProfile::AtomicInformation &iso_comp) ; 
			void GetProcessingTime(int &isotope_distribution_time, int &fit_time, int &find_peak_calc, int &find_peak_cached) 
			{ 
				isotope_distribution_time = mint_distribution_processing_time ; 
				fit_time = mint_fit_processing_time ; 
				find_peak_calc = mint_find_peak_calc ; 
				find_peak_cached = mint_find_peak_cached ; 
			} 
			void SetUseCaching(bool use) { mbln_use_isotope_distribution_caching = use ; } ; 
			bool GetUseCaching(){return mbln_use_isotope_distribution_caching;} ; 
			bool FindPeak(double min_mz, double max_mz, double &mz_value, double &intensity, bool debug) ;
			void Reset() 
			{ 
				mint_last_point_index = -1 ; 
				mdbl_mz1 = mdbl_mz2 = 0 ; 
				mdbl_int1 = mdbl_int2 = 0 ; 
			}
		};
	}
}