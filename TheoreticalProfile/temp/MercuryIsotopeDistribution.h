#pragma once
#include "MolecularFormula.h"
#include <vector>

namespace Engine
{
	namespace TheoreticalProfile
	{
		const double PI = 3.14159265358979323846 ; 
		const double TWOPI = 2*3.14159265358979323846 ;		
		
		enum enmApodizationType	{ GAUSSIAN = 1, LORENTZIAN, UNGAUSSIAN } ; 

		/*!
			MERCURY is an isotope distribution generator based on Fourier      
			transform methods.  The calculation is performed by generating the 
			transformed mass (or mu-domain) for the input molecule which is    
			then inverse Fourier transformed into the mass spectrum.  The FFT  
			routine is taken from Numerical Recipes in C, 2nd ed. by Press,    
			Teukolsky, Vetterling, and Flannery, Cambridge Univ. Press. This   
			program can be used to swiftly calculate high resolution and       
			ultrahigh resolution (a mass defect spectrum of a single isotopic  
			peak) distributions.   The program outputs an ASCII file of mass   
			intensity pairs. The output file does not account for the electron 
			mass, but the interactive display does. (We'll fix that in a later 
			version.) When running an ultrahigh resolution calculation, do not 
			use zero charge or you will get bogus output. Also, when it asks   
			for the mass shift in ultrahigh resolution mode, it expects you to 
			feed it a negative number. The high resolution calculations are    
			very fast (typically a second on a 66 MHz '486), but ultrahigh     
			resolution calculations are rather slow (several  minutes for a    
			single isotope peak.) Overall, the program is good, but could be   
			simplified and also optimized to run several times faster.         
			                                                                    
			\arg Algorithm by : Alan L. Rockwood                                    
			\arg Original Program by   : Steven L. Van Orden
		*/
		class  MercuryIsotopeDistribution
		{
	#pragma warning(disable:4251)
			std::vector<double> mvect_intensity ; 
			std::vector<double> mvect_mz ; 
			std::vector<double> mvect_frequency_data ; 
	#pragma warning(default:4251)
			int mint_mass_range ; 
			double mdbl_mass_variance ; 
			double mdbl_min_mz ; 
			double mdbl_max_mz ; 
		public:
			double mdbl_max_peak_mz ; 
			double GetMassVariance() { return mdbl_mass_variance ; } ;
			AtomicInformation mobj_elemental_isotope_composition ; 
			double mdbl_cc_mass ; 
			// average mass calculated for current peptide. It includes the charge 
			// specified for the species.
			double mdbl_average_mw ; 
			// monoisotopic mass calculated for current peptide. It includes the charge 
			// specified for the species.
			double mdbl_mono_mw ; 
			double mdbl_most_intense_mw ; 
			enmApodizationType menm_ap_type ; 
			int mint_mercury_size ;				
			int mint_points_per_amu ; 

			MercuryIsotopeDistribution(void);
			MercuryIsotopeDistribution(char *isotope_file_name) ; 
			void SetElementalIsotopeComposition(const AtomicInformation &iso_comp) ; 
			~MercuryIsotopeDistribution(void) {};
			void CalculateDistribution(short charge, double resolution, MolecularFormula &formula, 
				std::vector<double> &vect_x, std::vector<double> &vect_y, double threshold, bool debug) ; 
			void CalcVariancesAndMassRange(short charge, MolecularFormula &formula) ; 

			MercuryIsotopeDistribution& operator=(const MercuryIsotopeDistribution &merc_distribution) ; 
			void CalculateMasses(MolecularFormula &formula) ;
			void CalcFrequencies(short charge, int num_points, MolecularFormula &formula) ;
			void Apodize(int num_points, double Resolution, double sub) ;
			void OutputData(int num_points, int charge, std::vector<double> &vect_x, std::vector<double> &vect_y, double threshold) ;
			double NormalizeToPercentIons(std::vector<double> &vect_y) ; 
			bool FindPeak(double min_mz, double max_mz, double &mz, double &intensity) ; 
		};
	}
}