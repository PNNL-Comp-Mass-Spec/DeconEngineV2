#include <iostream>
#include <time.h>
#include <float.h>
#include <math.h>
#include <fstream>
#include "MercuryIsotopeDistribution.h" 
#include "../Utilities/FFt32.h" 


namespace Engine
{
	namespace TheoreticalProfile
	{

		MercuryIsotopeDistribution::MercuryIsotopeDistribution(void)
		{
			mobj_elemental_isotope_composition.LoadData() ; 
			mdbl_cc_mass = 1.00782 ; 
			menm_ap_type = GAUSSIAN ; 
			mint_mercury_size = 8192 ; 
		}

		MercuryIsotopeDistribution::MercuryIsotopeDistribution(char *isotope_file_name)
		{
			mobj_elemental_isotope_composition.LoadData(isotope_file_name) ; 
			mdbl_cc_mass = 1.00782 ; 
			menm_ap_type = GAUSSIAN ; 
			mint_mercury_size = 8192 ; 

		}

		void MercuryIsotopeDistribution::SetElementalIsotopeComposition(const AtomicInformation &iso_comp)
		{
			mobj_elemental_isotope_composition = iso_comp ; 
		}

		void MercuryIsotopeDistribution::CalcFrequencies(short charge, int num_points, MolecularFormula &formula)
		{
			long   i;
			int    j,k;
			double real,imag,freq,X,theta,r,tempr;

			int num_elements_in_entity = formula.mint_num_elements ; 
			/* Calculate first half of Frequency Domain (+)masses */
			for (i=1; i<=num_points/2; i++)
			{
				freq = (double)(i-1)/mint_mass_range;
				r = 1;
				theta = 0;
				for (j=0; j<num_elements_in_entity; j++)
				{
					int element_index = formula.mvect_elemental_composition[j].mint_index ;
					int atomicity =  (int) formula.mvect_elemental_composition[j].mdbl_num_copies ;
					int num_isotopes = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].mint_num_isotopes ; 
					double average_mass = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].mdbl_average_mass ; 
					real = imag = 0.0;
					for (k=0; k < num_isotopes ; k++)
					{
						double wrap_freq = 0 ;
						double isotope_abundance = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].marr_isotope_prob[k] ; 
						if (num_isotopes > 1)
						{
							wrap_freq = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].marr_isotope_mass[k]/charge - average_mass/charge ;
							if (wrap_freq < 0)
								wrap_freq += mint_mass_range ; 
						}
						X = 2 * PI * wrap_freq * freq;
						real += isotope_abundance * cos(X);
						imag += isotope_abundance * sin(X);
					}
					/* Convert to polar coordinates, r then theta */
					tempr = sqrt(real*real+imag*imag);
					r *= pow(tempr,atomicity);
					if (real > 0) 
						theta += atomicity * atan(imag/real);
					else if (real < 0) 
						theta += atomicity * (atan(imag/real) + PI);
					else if (imag > 0) 
						theta += atomicity * PI / 2;
					else 
						theta += atomicity * -PI / 2; 
				}
				/* Convert back to real:imag coordinates and store */
				mvect_frequency_data[2*i-1l] = r * cos(theta);  /* real data in odd index */
				mvect_frequency_data[2*i] = r * sin(theta);        /* imag data in even index */
			}  /* end for(i) */
	 
			/* Calculate second half of Frequency Domain (-)masses */
			for (i=num_points/2+1; i<=num_points; i++)
			{
				freq = (double)(i-num_points-1)/mint_mass_range;
				r = 1;
				theta = 0;

				for (j=0; j<num_elements_in_entity; j++)
				{
					int element_index = formula.mvect_elemental_composition[j].mint_index ;
					int atomicity =  (int) formula.mvect_elemental_composition[j].mdbl_num_copies ;
					int num_isotopes = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].mint_num_isotopes ; 

					double average_mass = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].mdbl_average_mass ; 

					real = imag = 0;
					for (k=0; k<num_isotopes; k++)
					{
						double wrap_freq = 0 ;
						double isotope_abundance = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].marr_isotope_prob[k] ; 
						if (num_isotopes > 1)
						{
							wrap_freq = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].marr_isotope_mass[k]/charge - average_mass/charge ;
							if (wrap_freq < 0)
								wrap_freq += mint_mass_range ; 
						}

						X = 2 * PI * wrap_freq * freq;
						real += isotope_abundance * cos(X);
						imag += isotope_abundance * sin(X);
					}
			 
					/* Convert to polar coordinates, r then theta */
					tempr = sqrt(real*real+imag*imag);
					r *= pow(tempr,atomicity);
					if (real > 0) theta += atomicity * atan(imag/real);
					else if (real < 0) theta += atomicity * (atan(imag/real) + PI);
						else if (imag > 0) theta += atomicity * PI / 2;
							else theta -= atomicity * PI / 2;


				}  /* end for(j) */

				/* Convert back to real:imag coordinates and store */
				mvect_frequency_data[2*i-1] = r * cos(theta); /* real data in even index */
				mvect_frequency_data[2*i] = r * sin(theta);   /* imag data in odd index */
			 
			}  /* end of for(i) */
			 
		}

		/*************************************************/
		/* FUNCTION Apodize - called by main()           */
		/*    ApType = 1 : Gaussian                      */
		/*           = 2 : Lorentzian                    */
		/*           = 3 : ??                            */
		/*           = -1 : Unapodize Gaussian           */
		/*************************************************/
		void MercuryIsotopeDistribution::Apodize(int num_points, double Resolution, double sub)
		{
			long i;
			double ApVal, expdenom;

			switch(menm_ap_type)
			{
				case GAUSSIAN:
					for (i=1; i<=num_points; i++)
					{
						expdenom = ((double)num_points/sub)*((double)num_points/sub);
						if (i <= num_points/2) ApVal = exp(-(i-1)*(i-1)/expdenom);
						else ApVal = exp(-(num_points-i-1)*(num_points-i-1)/expdenom);

						double freq = mvect_frequency_data[2*i-1] ; 
						double val = freq * ApVal; 
						mvect_frequency_data[2*i-1] = val ;
						freq = mvect_frequency_data[2*i] ; 
						val = freq * ApVal; 
						mvect_frequency_data[2*i] = val ;
					}
					break ; 
				case LORENTZIAN: /* Lorentzian */
					for (i=1; i<=num_points; i++)
					{
						if (i <= num_points/2) 
							ApVal = exp(-(double)(i-1)*(sub/5000.0));
						else 
							ApVal = exp(-(double)(num_points-i)*(sub/5000.0));
						mvect_frequency_data[2*i-1] *= ApVal;
						mvect_frequency_data[2*i] *= ApVal;
					}
					break ; 
				case UNGAUSSIAN:  /* Unapodize Gaussian */
					for (i=1; i<=num_points; i++)
					{
						expdenom = ((double)num_points/sub)*((double)num_points/sub);
						if (i <= num_points/2) ApVal = exp(-(i-1)*(i-1)/expdenom);
						else ApVal = exp(-(num_points-i-1)*(num_points-i-1)/expdenom);
						mvect_frequency_data[2*i-1] /= ApVal;
						mvect_frequency_data[2*i] /= ApVal;
					}
					break ; 
			}  /* End of Apodize() */
		}

		void MercuryIsotopeDistribution::OutputData(int num_points, int charge, std::vector<double> &vect_x, std::vector<double> &vect_y, double threshold)
		{
			int i;
			double maxint=0;

			/* Normalize intensity to 0%-100% scale */
			for (i=1; i<2*num_points; i+=2)
			{
				double intensity = mvect_frequency_data[i] ; 
				if (intensity > maxint) 
					maxint = intensity ;
			}
			for (i=1; i<2*num_points; i+=2)
			{
				double intensity = mvect_frequency_data[i] ; 
				mvect_frequency_data[i] = 100 * intensity/maxint;
			}
			double X1 ; 
			double X2 ; 
			double X3 ; 

			double Y1 ; 
			double Y2 ; 
			double Y3 ; 

			int index = 0 ; 

			mvect_intensity.clear() ; 
			mvect_mz.clear() ; 

			if (charge == 0)
				charge = 1 ; 

			for (i=num_points/2+1; i<=num_points; i++)
			{
				double mz = (double)(i-num_points-1)/mint_points_per_amu + mdbl_average_mw / charge + (mdbl_cc_mass - ELECTRON_MASS) ;
				double intensity = mvect_frequency_data[2*i-1] ;
				mvect_mz.push_back(mz) ; 
				mvect_intensity.push_back(intensity) ;
			}
			for (i=1; i<=num_points/2; i++)
			{
				double mz = (double)(i-1)/mint_points_per_amu + mdbl_average_mw / charge + (mdbl_cc_mass - ELECTRON_MASS) ;
				double intensity = mvect_frequency_data[2*i-1] ;
				mvect_mz.push_back(mz) ; 
				mvect_intensity.push_back(intensity) ;
			}

			double highest_intensity = -1 * DBL_MIN ; 

			vect_x.clear();
			vect_y.clear() ; 
			int n1 = (int)mvect_intensity.size() ; 
			for (i=0; i<num_points; i++)
			{
				double intensity = mvect_intensity[i] ; 
				double mz = mvect_mz[i] ; 
				if (intensity > threshold)
				{
					vect_y.push_back(intensity) ;
					vect_x.push_back(mz) ;
				}
				if (intensity > highest_intensity)
				{
					highest_intensity = intensity ; 

					if (i > 0 )
					{
						X1 = mvect_mz[i-1] ; 
						Y1 = mvect_intensity[i-1] ; 
					}
					else
					{
						X1 = mz - 1.0/(charge * mint_points_per_amu) ;
						Y1 = 0 ; 
					}

					Y2 = intensity ;
					X2 = mz ; 

					if (i < num_points - 1)
					{
						X3 = mvect_mz[i+1] ; 
						Y3 = mvect_intensity[i + 1] ; 
					}
					else
					{
						X3 = mz + 1.0/(charge * mint_points_per_amu) ;
						Y3 = Y2 ; 
					}
				}
			}
			double d = ((Y2 - Y1) * (X3 - X2) - (Y3 - Y2) * (X2 - X1)) ;
	   
			if (d == 0)
				mdbl_max_peak_mz = X2 ;
			else
				mdbl_max_peak_mz = ((X1 + X2) - ((Y2 - Y1) * (X3 - X2) * (X1 - X3)) / ((Y2 - Y1) * (X3 - X2) - (Y3 - Y2) * (X2 - X1))) / 2.0 ; 
			// remember that the mono isotopic mass is calculated using theoretical values rather than 
			// fit with the fourier transformed point. Hence even if the monoisotopic mass is the most intense
			// its value might not match exactly with mdbl_most_intense_mw, hence check if they are the same
			// and set it.
			mdbl_most_intense_mw = mdbl_max_peak_mz * charge - mdbl_cc_mass * charge + ELECTRON_MASS * charge ; 
			if (abs(mdbl_most_intense_mw - mdbl_mono_mw) < 0.5 * 1.003/charge)
				mdbl_most_intense_mw = mdbl_mono_mw ; 

		}  /* End of OutputData() */


			
		void MercuryIsotopeDistribution::CalculateDistribution(short charge, double resolution, MolecularFormula &formula, 
			std::vector<double> &vect_x, std::vector<double> &vect_y, double threshold, bool debug = false )
		{
			/* Calculate mono and average mass */
			CalculateMasses(formula);
			if (debug)
			{
				std::cerr<<"MonoMW ="<<mdbl_mono_mw<<" AverageMW ="<<mdbl_average_mw<<std::endl ; 
			}

			/* Calculate mass range to use based on molecular variance */
			CalcVariancesAndMassRange(charge, formula);
			if (debug)
			{
				std::cerr<<"Variance ="<<mdbl_mass_variance<<" Mass Range ="<<mint_mass_range<<std::endl ; 
			}

			mdbl_min_mz = mdbl_average_mw / charge + (mdbl_cc_mass - ELECTRON_MASS) - (mint_mass_range * 1.0)/ 2 ; 
			mdbl_max_mz = mdbl_min_mz + mint_mass_range ; 

			mint_points_per_amu = mint_mercury_size / mint_mass_range;  /* Use maximum of 2048 real, 2048 imag points */

			// calculate Ap_subscript to from requesed Res
			double Ap_subscript ;
			if(charge==0) 
				Ap_subscript = ((mdbl_average_mw/resolution) * mint_mercury_size *2.0)/mint_mass_range ;
			else 
				Ap_subscript = ((mdbl_average_mw/(resolution*abs(charge))) * mint_mercury_size*2.0)/mint_mass_range;
			
			/* Allocate memory for Axis arrays */
			int num_points = mint_mass_range * mint_points_per_amu ;
			mvect_frequency_data.clear() ; 
			mvect_frequency_data.resize(2*num_points+1) ; 

			if (charge == 0)
				charge = 1 ; 

			if (debug)
			{
				std::cerr<<"MINMZ = "<<mdbl_min_mz<<" MAXMZ = "<<mdbl_max_mz<<std::endl ; 
				std::cerr<<"Num Points per AMU = "<<mint_points_per_amu<<std::endl ; 
			}

			CalcFrequencies(charge, num_points, formula);

			/* Apodize data */
			double Resolution = 1;  /* Resolution used in apodization. Not used yet */
			Apodize(num_points,Resolution,Ap_subscript);


			Engine::FFT::Four1(num_points, mvect_frequency_data,-1);  // myers changes this line to 	Realft(FreqData,NumPoints,-1);

			OutputData(num_points, charge, vect_x, vect_y, threshold);

			//NormalizeToPercentIons(vect_y) ; 
		}


		inline void MercuryIsotopeDistribution::CalcVariancesAndMassRange(short charge, MolecularFormula &formula)
		{
			mdbl_mass_variance = 0 ; 
			int num_elements_found = formula.mint_num_elements ; 
			for (int element_num=0; element_num < num_elements_found; element_num++)
			{
				int element_index = formula.mvect_elemental_composition[element_num].mint_index ; 
				int atomicity = (int) formula.mvect_elemental_composition[element_num].mdbl_num_copies ; 
				double elemental_variance = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].mdbl_mass_variance ;
				mdbl_mass_variance += elemental_variance * atomicity ; 
			}

			if (charge == 0)
				mint_mass_range = (int)(sqrt(1 + mdbl_mass_variance)*10);
			else  
				mint_mass_range = (int) (sqrt(1 + mdbl_mass_variance)*10.0/(float)charge);  /* +/- 5 sd's : Multiply charged */

			/* Set to nearest (upper) power of 2 */
			for (int i=1024; i>0; i/=2)
			{
				if (i < mint_mass_range)
				{
					mint_mass_range = i * 2 ;
					i = 0 ;
				}
			}
			if (mint_mass_range <= 0) 
				mint_mass_range = 1 ;
		}

		double MercuryIsotopeDistribution::NormalizeToPercentIons(std::vector <double> &vect_y)
		{
			int    i,LastDelta;
			double Sum;
			int num_points = (int)vect_y.size() ; 

			Sum=0.0;
			LastDelta=0;
			for(i=1;i<num_points;i++)
			{
				if(fabs(vect_y[i]) >= fabs(vect_y[i-1]))
				{
					if(fabs(vect_y[i]) > fabs(vect_y[i-1])) LastDelta = 1;
				}
				else
				{
					if(LastDelta == 1)
					{
						Sum += vect_y[i-1];
					}
					LastDelta = -1;
				}
			}
			for(i=0;i<num_points;i++) vect_y[i] /= Sum/100.0;
			return(Sum);
		}

		bool MercuryIsotopeDistribution::FindPeak(double min_mz, double max_mz, double &mz_value, double &intensity) 
		{
			int max_index = -1 ; 
			for (int i = 0 ; i < mint_mercury_size ; i++)
			{
				double mz = mdbl_min_mz + (i*1.0) / mint_points_per_amu ; 
				if (mz > mdbl_max_mz)
					break ; 
				if (mz > mdbl_min_mz)
				{
					if (mvect_intensity[i] > intensity)
					{
						max_index = i ; 
						intensity = mvect_intensity[i] ; 
					}
				}
			}
			if (max_index == -1)
				return false ; 

			double X1 = mdbl_min_mz + ((i-1)* 1.0) / mint_points_per_amu ; 
			double X2 = X1 + 1.0 / mint_points_per_amu ; 
			double X3 = X2 + 1.0 / mint_points_per_amu ; 

			double Y1 = mvect_intensity[i - 1] ; 
			double Y2 = mvect_intensity[i] ; 
			double Y3 = mvect_intensity[i + 1] ; 

			double d ;
			d = (Y2 - Y1) * (X3 - X2) ; 
			d = d - (Y3 - Y2) * (X2 - X1) ;
	   
			if (d == 0)
				mz_value = X2 ; 
			else
				mz_value = ((X1 + X2) - ((Y2 - Y1) * (X3 - X2) * (X1 - X3)) / d) / 2.0 ; 
			return true ; 
		}

		inline void MercuryIsotopeDistribution::CalculateMasses(MolecularFormula &formula) 
		{
			mdbl_mono_mw = 0 ;
			mdbl_average_mw = 0 ; 
			int num_elements_found = formula.mint_num_elements ; 
			for (int j=0; j < num_elements_found ; j++)
			{
				int element_index = formula.mvect_elemental_composition[j].mint_index ; 
				int atomicity = (int) formula.mvect_elemental_composition[j].mdbl_num_copies ; 

				if (atomicity == 0)
					continue ; 
				int num_isotopes = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].mint_num_isotopes ; 
				double mono_mw = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].marr_isotope_mass[0] ;
				double avg_mw = mobj_elemental_isotope_composition.mvect_elemental_isotopes[element_index].mdbl_average_mass ;
				mdbl_mono_mw += atomicity * mono_mw ;
				mdbl_average_mw += atomicity * avg_mw ;
			}
		}

		MercuryIsotopeDistribution& MercuryIsotopeDistribution::operator=(const MercuryIsotopeDistribution &merc_distribution)
		{
			this->mobj_elemental_isotope_composition = merc_distribution.mobj_elemental_isotope_composition ; 
			this->mdbl_cc_mass = merc_distribution.mdbl_cc_mass ; 
			this->menm_ap_type = merc_distribution.menm_ap_type ; 
			this->mint_mercury_size = merc_distribution.mint_mercury_size ; 
			this->mint_points_per_amu = merc_distribution.mint_points_per_amu ; 
			return (*this) ; 
		}

	}
}