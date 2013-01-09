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
#include "../Utilities/Matrix.h"
#include "PeakIndex.h"
#include <vector>

namespace Engine
{
	namespace PeakProcessing
	{
		//! class used to compute FWHM and signal to noise for peaks.
		template <typename X, typename Y> class PeakStatistician
		{
	#pragma warning(disable: 4251)
			//! internal variable to store temporary m/z values. It also guarantees a workspace that doesn't need to be reallocated all the time.
			std::vector<X> mvect_mzs ;
			//! internal variable to store temporary intensity values. It also guarantees a workspace that doesn't need to be reallocated all the time.
			std::vector<Y> mvect_intensities ; 
	#pragma warning(default: 4251)

		public:
			//! default constructor.
			PeakStatistician<X, Y> () {};
			//! destructor
			~PeakStatistician(void){}
			//! Find signal to noise value at position specified. 
			/*!
				\param yValue is intenstiy at specified index.
				\param vect_intensities is std::vector of intensities.
				\param index is position of point at which we want to calculate signal to noise.
				\return returns computed signal to nosie value. 
				\remarks Looks for local minima on the left and the right hand sides and calculates signal to noise of peak relative to the minimum of these shoulders.
			*/
			double FindSignalToNoise(Y yValue, std::vector<Y> &vect_intensities,
				int index) // The place in arrDerivative the derivative crossed 0
			{
				Y minIntensityLeft=0, minIntensityRight=0;
				int num_data_pts = (int) vect_intensities.size() ;
				if (yValue == 0)
					return 0 ; 

				if ( index <= 0 || index >= num_data_pts - 1)
					return 0 ; 

				// Find the first local minimum as we go down the m/z range.
				bool found = false ; 
				for(int i=index; i > 0; i--)
				{
					if(vect_intensities[i+1] >= vect_intensities[i] && vect_intensities[i-1] > vect_intensities[i]) // Local minima here \/
					{
						minIntensityLeft = vect_intensities[i] ;
						found = true ; 
						break ; 
					}
				}
				if (!found)
					minIntensityLeft = vect_intensities[0] ; 

				found = false ; 
				//// Find the first local minimum as we go up the m/z range.
				for(int i=index; i < num_data_pts-1 ; i++)
				{
					if(vect_intensities[i+1] >= vect_intensities[i] && vect_intensities[i-1] > vect_intensities[i]) // Local minima here \/
					{
						minIntensityRight = vect_intensities[i] ;
						found = true ; 
						break ; 
					}
				}	
				if (!found)
					minIntensityRight = vect_intensities[num_data_pts-1] ; 
				if (minIntensityLeft == 0)
				{
					if (minIntensityRight == 0)
						return 100 ; 
					return (1.0*yValue) / minIntensityRight ;					
				}
				if (minIntensityRight < minIntensityLeft && minIntensityRight != 0)
					return (1.0*yValue) / minIntensityRight ;					
					
				return (1.0*yValue) / minIntensityLeft ;			
				
			}
			
			//! Find full width at half maximum value at position specified. 
			/*!
				\param vect_mzs is std::vector of mzs.
				\param vect_intensities is std::vector of intensities.
				\param data_index is position of point at which we want to calculate FWHM.
				\param signalToNoise is option parameter that specifies minimum signal to noise ratio to use. 
				\return returns computed FWHM. 
				\remarks Looks for half height locations at left and right side, and uses twice of that value as the FWHM value. If half height 
				locations cannot be found (because of say an overlapping neighbouring peak), we perform interpolations.
			*/
			double FindFWHM(std::vector<X> &vect_mzs, std::vector<Y> &vect_intensities, int data_index, double signalToNoise=0.0) 
			{
				X upper, lower, mass;
				X X1, X2 ; 
				Y Y1, Y2, peak, peakHalf;
				int iStat;
				unsigned int points ; 
				double coe[2], mse; // coe is coefficients found by curve regression.

				points = 0;
				peak = vect_intensities[data_index] ; 
				peakHalf = Y ((double)peak / 2.0);
				mass = vect_mzs[data_index] ;

				if(peak == 0.0)
					return 0.0;

				int num_input_pts = (int) vect_mzs.size() ; 

				if (data_index <= 0 || data_index >= num_input_pts -1)
					return 0 ; 

				upper = vect_mzs[0] ; 
				for(int index = data_index ; index >= 0; index--)
				{
					X current_mass = vect_mzs[index] ; 
					Y1 = vect_intensities[index] ; 
					if((Y1 < peakHalf) || (Helpers::absolute(mass - current_mass) > 5.0) || ((index < 1 || vect_intensities[index-1] > Y1) && (index < 2 || vect_intensities[index-2] > Y1) && (signalToNoise < 4.0)))
					{
						Y2 = vect_intensities[index+1];
						X1 = vect_mzs[index] ;
						X2 = vect_mzs[index+1] ;
						if((Y2 - Y1 != 0) && (Y1 < peakHalf))
						{
							upper = X1 - (X1 - X2) * (peakHalf - Y1) / (Y2 - Y1);
						}
						else
						{
							upper = X1;
							points = data_index - index + 1 ;
							if(points >= 3) 
							{
								mvect_mzs.clear() ; 
								mvect_intensities.clear() ; 
								if (mvect_mzs.capacity() < (unsigned int)points)
								{
									mvect_mzs.reserve(points); 
									mvect_intensities.reserve(points); 
								}

								int j = points-1 ;
								for( ; j>= 0 ; j--)
								{
									mvect_mzs.push_back(vect_mzs[data_index-j]);
									mvect_intensities.push_back(vect_intensities[data_index-j]);
								}
								for(j=0; j < (int)points && mvect_intensities[0] == mvect_intensities[j] ; j++);
								if(j==points) 
									return 0.0;
								iStat = CurvReg(mvect_intensities, mvect_mzs, points, coe, 1, &mse);
								// only if successful calculation of peak was done, should we change upper. 
								if (iStat != -1)
									upper = coe[1] * peakHalf + coe[0];
							}
						}
						break ; 
					}		
				}

				lower = vect_mzs[num_input_pts-1] ; 
				for(int index = data_index ; index < num_input_pts ; index++ )
				{				
					X current_mass = vect_mzs[index] ; 
					Y1 = vect_intensities[index] ; 
					if((Y1 < peakHalf) || (Helpers::absolute(mass - current_mass) > 5.0) || ((index > num_input_pts - 2 || vect_intensities[index+1] > Y1) && (index > num_input_pts - 3 || vect_intensities[index+2] > Y1) && signalToNoise < 4.0))
					{
						Y2 = vect_intensities[index-1];
						X1 = vect_mzs[index] ;
						X2 = vect_mzs[index-1] ;

						if((Y2 - Y1 != 0) && (Y1 < peakHalf))
						{
							lower = X1 - (X1 - X2) * (peakHalf - Y1) / (Y2 - Y1);
						}
						else
						{
							lower = X1;
							points = index - data_index + 1 ;
							if(points >= 3) 
							{
								mvect_mzs.clear() ; 
								mvect_intensities.clear() ; 
								if (mvect_mzs.capacity() < points)
								{
									mvect_mzs.reserve(points); 
									mvect_intensities.reserve(points); 
								}
								for(int k=points-1; k>= 0; k--)
								{
									mvect_mzs.push_back(vect_mzs[index-k]);
									mvect_intensities.push_back(vect_intensities[index-k]);
								}
								int j = 0 ;
								for(j=0; j < (int)points && mvect_intensities[0] == mvect_intensities[j] ; j++);
								if(j==points) 
									return 0.0;
								
								iStat = CurvReg(mvect_intensities, mvect_mzs, points, coe, 1, &mse);
								// only if successful calculation of peak was done, should we change lower. 
								if (iStat != -1)
									lower = coe[1] * peakHalf + coe[0];
							}
						}
						break ; 
					}
				}


				if(upper == 0.0) 
					return 2 * Helpers::absolute(mass-lower);
				if(lower == 0.0)
					return 2 * Helpers::absolute(mass-upper);
				return Helpers::absolute(upper-lower);
			}


			//! Calculate Least Square error mapping y = f(x).  [GORD] This is linear regression - that's it! 
			/*!
				\param x std::vector of x values. 
				\param y std::vector of y values. 
				\param n number of points in std::vector.
				\param terms output coefficients of Least Square Error parameters. Coefficients are slope and intercept! 
				\param nterms order of the function y = f(x).
				\param mse minimum square error value.
				\return returns 0 if successful an -1 if not. 
			*/
			template <typename X, typename Y> int CurvReg(std::vector<X> x, std::vector<Y> y, int n, double *terms, int nterms, double *mse)
			{
				MATRIX *At, *B, *At_Ai_At, *Z, *OUTa;
				MATRIX *I_At_At_T, *At_At_T, *At_T;
				int i,j;
				float *w;
				double **at, **b, **z, **out;
				double yfit, xpow;

				// weights
				w = (float *)new double[n];
				if(!w) return(-1);
				for(i = 0 ; i < n ; i++) w[i] = 1.0;

				// weighted powers of x matrix transpose = At 
				At = matrix_allocate(nterms+1,n,sizeof(double));
				at = (double **) At->ptr;
				for(i = 0 ; i < n ; i++) {
					at[0][i] = w[i];
					for(j = 1 ; j < (nterms + 1) ; j++)
						at[j][i] = at[j-1][i] * x[i];
				}

				// Z = weighted y std::vector 
				Z = matrix_allocate(n,1,sizeof(double));
				z = (double **) Z->ptr;

				for(i = 0 ; i < n ; i++) 
					z[i][0] = w[i] * y[i];

				At_T = matrix_transpose(At);
				At_At_T = matrix_mult(At,At_T);
				I_At_At_T = matrix_invert(At_At_T);
				if (I_At_At_T == NULL)
				{
					matrix_free(At_At_T);
					matrix_free(At_T);
					matrix_free(At);
					matrix_free(Z);
					// Deep Jaitly. Was using free for some reason.
					delete [] w ;
					return -1 ; 
				}

				At_Ai_At = matrix_mult(I_At_At_T,At);
				matrix_free(I_At_At_T);
				matrix_free(At_At_T);
				matrix_free(At_T);

				B = matrix_mult(At_Ai_At,Z);
				b = (double **)B->ptr;

				// make a matrix with the fit y and the difference 
				OUTa = matrix_allocate(2,n,sizeof(double));
				out = (double **) OUTa->ptr;

				// calculate the least squares y values 
				*mse=0.0;
				for(i = 0 ; i < n ; i++) {
					terms[0] = b[0][0];
					yfit = b[0][0];
					xpow = x[i];
					for(j = 1 ; j <= nterms ; j++) {
						terms[j] = b[j][0];
						yfit += b[j][0]*xpow;
						xpow = xpow * x[i];
					}
					out[0][i] = yfit;
					out[1][i] = y[i]-yfit;
					*mse += y[i]-yfit;
				}
				matrix_free(At);
				matrix_free(At_Ai_At);
				matrix_free(B);
				matrix_free(Z);
				matrix_free(OUTa);
				delete [] w ;
				
				return 0;
			}


		};
	}
}
