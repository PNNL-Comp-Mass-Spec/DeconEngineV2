// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include "MolecularFormula.h"
#include "../Utilities/FFt32.h" 
#include <iostream>
#include <time.h>
#include <float.h>
#include <math.h>
#include <fstream>
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
			double mdbl_min_mz ; 
			double mdbl_max_mz ; 
		public:
			double mdbl_mass_variance ; 
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

			/*[gord] 
			vect_x = mz values of isotopic profile that are above the threshold
			vect_y = intensity values of isotopic profile that are above the threshold
			vect_isotope_mzs = peak top mz's for the peaks of the isotopic profile
			vect_isotope_intensities = peak top intensities's for the peaks of the isotopic profile
			*/
			inline void CalculateDistribution(short charge, double resolution, MolecularFormula &formula, 
				std::vector<double> &vect_x, std::vector<double> &vect_y, double threshold, 
				std::vector<double> &vect_isotope_mzs, std::vector<double> &vect_isotope_intensities, 
				bool debug = false )
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
			
				/*
				[gord] 'OutputData' fills 'vect_x', 'vect_y'
				[gord] vect_x = mz values of isotopic profile that are above the threshold
				[gord] vect_y = intensity values of isotopic profile that are above the threshold
				[gord] vect_isotope_mzs = peak top mz's for the peaks of the isotopic profile
				[gord] vect_isotope_intensities = peak top intensities's for the peaks of the isotopic profile
				*/
				OutputData(num_points, charge, vect_x, vect_y, threshold, vect_isotope_mzs, vect_isotope_intensities);

				//NormalizeToPercentIons(vect_y) ; 
			}



			MercuryIsotopeDistribution& operator=(const MercuryIsotopeDistribution &merc_distribution) ; 
			inline double NormalizeToPercentIons(std::vector <double> &vect_y)
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


			inline void CalculateMasses(MolecularFormula &formula) 
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
			
			inline void CalcVariancesAndMassRange(short charge, MolecularFormula &formula)
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
			
			/*************************************************/
			/* FUNCTION Apodize - called by main()           */
			/*    ApType = 1 : Gaussian                      */
			/*           = 2 : Lorentzian                    */
			/*           = 3 : ??                            */
			/*           = -1 : Unapodize Gaussian           */
			/*************************************************/
			inline void Apodize(int num_points, double Resolution, double sub)
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

			inline void OutputData(int num_points, int charge, std::vector<double> &vect_x, 
				std::vector<double> &vect_y, double threshold, std::vector<double> &vect_isotope_mzs, 
				std::vector<double> &vect_isotope_intensities)
			{
				int i;
				double maxint=0;
				vect_isotope_mzs.clear() ; 
				vect_isotope_intensities.clear() ; 

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

				double X1_iso ; 
				double X2_iso ; 
				double X3_iso ; 

				double Y1_iso ; 
				double Y2_iso ; 
				double Y3_iso ; 

				int index = 0 ; 

				mvect_intensity.clear() ; 
				mvect_mz.clear() ; 

				if (charge == 0)
					charge = 1 ; 

			
				//[gord] fill mz and intensity arrays, ignoring minimum thresholds
				
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

				double highest_intensity = -1 * DBL_MAX ; 

				vect_x.clear();
				vect_y.clear() ; 
				int n1 = (int)mvect_intensity.size() ; 

				double last_intensity = DBL_MAX * -1 ; 
				for (i=0; i<num_points; i++)
				{
					double intensity = mvect_intensity[i] ; 
					double mz = mvect_mz[i] ; 
					if (intensity > threshold)
					{
						vect_y.push_back(intensity) ;            //fill intensity array if above threshold
						vect_x.push_back(mz) ;					//fill mz array if above threshold
						if (intensity >= last_intensity)     // intensities are increasing... (on the upslope of the theor peak)
						{
							if (i != num_points -1)
							{
								if (mvect_intensity[i+1] < intensity)   //  if true, 'intensity' marks the maximum
								{
									// now three points are defined (X1,Y1) (X2,Y2) and (X3,Y2), with the maximum being (X2,Y2)
									
									if (i > 0 )
									{
										X1_iso = mvect_mz[i-1] ; 
										Y1_iso = mvect_intensity[i-1] ; 
									}
									else
									{
										X1_iso = mz - 1.0/(charge * mint_points_per_amu) ;
										Y1_iso = 0 ; 
									}

									Y2_iso = intensity ;
									X2_iso = mz ; 

									X3_iso = mvect_mz[i+1] ; 
									Y3_iso = mvect_intensity[i + 1] ; 

									//[gord] it seems that the top three points are not necessarily symmetrical. 
									// so must first test the symmetry

									double symmetryRatioCalc = ((Y2_iso - Y1_iso) * (X3_iso - X2_iso) - (Y3_iso - Y2_iso) * (X2_iso - X1_iso)) ;						   
									if (symmetryRatioCalc == 0)    //symmetrical
										vect_isotope_mzs.push_back(X2_iso) ;
									else     //not symmetrical...   gord:  I'm not sure how the center point is calculated... perhaps a midpoint calc?
										vect_isotope_mzs.push_back(((X1_iso + X2_iso) - ((Y2_iso - Y1_iso) * (X3_iso - X2_iso) * (X1_iso - X3_iso)) / ((Y2_iso - Y1_iso) * (X3_iso - X2_iso) - (Y3_iso - Y2_iso) * (X2_iso - X1_iso))) / 2.0) ; 
									vect_isotope_intensities.push_back(intensity) ; 

								}
							}
							else    // if intensities are decreasing, fill the remaining points 
							{
								X1_iso = mvect_mz[i-1] ; 
								Y1_iso = mvect_intensity[i-1] ; 

								X2_iso = mz ; 
								Y2_iso = intensity ;

								X3_iso = mz + 1.0/(charge * mint_points_per_amu) ;
								Y3_iso = 0 ; 

								double d_iso = ((Y2_iso - Y1_iso) * (X3_iso - X2_iso) - (Y3_iso - Y2_iso) * (X2_iso - X1_iso)) ;						   
								if (d_iso == 0)
									vect_isotope_mzs.push_back(X2_iso) ;
								else
									vect_isotope_mzs.push_back(((X1_iso + X2_iso) - ((Y2_iso - Y1_iso) * (X3_iso - X2_iso) * (X1_iso - X3_iso)) / ((Y2_iso - Y1_iso) * (X3_iso - X2_iso) - (Y3_iso - Y2_iso) * (X2_iso - X1_iso))) / 2.0) ; 
								vect_isotope_intensities.push_back(intensity) ; 

							}
						}
					}
					last_intensity = intensity ; 
					if (intensity > highest_intensity)     //[gord] this is used in determining the max of the entire theor isotopic profile
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
				}     //end of loop over all points
				
				//[gord] determine if three points are symmetrical around the max point (X2,Y2)
				double symmetryRatioCalc = ((Y2 - Y1) * (X3 - X2) - (Y3 - Y2) * (X2 - X1)) ;
		   
				if (symmetryRatioCalc == 0)     //symmetrical
					mdbl_max_peak_mz = X2 ;
				else   //not symmetrical
					mdbl_max_peak_mz = ((X1 + X2) - ((Y2 - Y1) * (X3 - X2) * (X1 - X3)) / ((Y2 - Y1) * (X3 - X2) - (Y3 - Y2) * (X2 - X1))) / 2.0 ; 
				// remember that the mono isotopic mass is calculated using theoretical values rather than 
				// fit with the fourier transformed point. Hence even if the monoisotopic mass is the most intense
				// its value might not match exactly with mdbl_most_intense_mw, hence check if they are the same
				// and set it.
				mdbl_most_intense_mw = mdbl_max_peak_mz * charge - mdbl_cc_mass * charge + ELECTRON_MASS * charge ; 
				if (abs(mdbl_most_intense_mw - mdbl_mono_mw) < 0.5 * 1.003/charge)
					mdbl_most_intense_mw = mdbl_mono_mw ; 

			}  /* End of OutputData() */



			inline bool FindPeak(double min_mz, double max_mz, double &mz_value, double &intensity) 
			{
				int max_index = -1 ;
				mz_value = 0;
				intensity = 0;
				

				for (int i = 0 ; i < mint_mercury_size ; i++)
				{
					double mz_local = 0;
					mz_local = mdbl_min_mz + (i*1.0) / mint_points_per_amu ;
					if (mz_local > mdbl_max_mz || mz_local > max_mz)
						break ;
					if (mz_local > mdbl_min_mz && mz_local > min_mz)
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
				
				double X1 = mdbl_min_mz + ((max_index-1)* 1.0) / mint_points_per_amu ; 
				double X2 = X1 + 1.0 / mint_points_per_amu ; 
				double X3 = X2 + 1.0 / mint_points_per_amu ; 

				double Y1 = mvect_intensity[max_index - 1] ; 
				double Y2 = mvect_intensity[max_index] ; 
				double Y3 = mvect_intensity[max_index + 1] ; 

				double d ;
				d = (Y2 - Y1) * (X3 - X2) ; 
				d = d - (Y3 - Y2) * (X2 - X1) ;
		   
				if (d == 0)
					mz_value = X2 ; 
				else
					mz_value = ((X1 + X2) - ((Y2 - Y1) * (X3 - X2) * (X1 - X3)) / d) / 2.0 ; 
				return true ; 
			}
		
			inline void CalcFrequencies(short charge, int num_points, MolecularFormula &formula)
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


	};
	}
}