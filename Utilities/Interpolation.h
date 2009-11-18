#pragma once
#include <vector> 
#include<fstream>

namespace Engine
{
	namespace Utilities
	{
		class  Interpolation
		{
	#pragma warning(disable : 4251)
			//! Temporary variable used in computation of spline coefficients.
			std::vector<double> mvect_temp_spline ; 
			//! std::vector to store the second derivatives at the knot points of the spline.
			std::vector<double> mvect_Y2 ; 

	#pragma warning(default : 4251)
		public:
			//! Default constructor
			Interpolation(void);
			//! destructor
			~Interpolation(void);
			//! Cubic Spline interpolation. This function generates the second derivatives at the knot points. 
			/*!
				\param x std::vector of x values.
				\param y std::vector of y values.
				\param yp1 second derivative at first point.
				\param ypn second derivative at the nth point.
				\remarks These alogrothims are from: Numerical Recipes in C by William H. Press, Brian P. Flannery, Saul A. Teukolsky, William T. Vetterling.
				\brief Given the arrays x[0..n-1] and y[0..n-1] containing the tabulated 
				function, i.e., yi = f(xi), with x0<x1<...<xn-1, and given values
				yp1 and ypn for the first derivative of the interpolating function
				at points 0 and n-1, respectively, this routine returns an array
				y2[1..n] that contains the second derivatives of the interpolating
				function at the tabulated points xi. If yp1 and/or ypn are equal to
				1x10^30 or larger, the routine is signaled to set the corresponding
				boundary condition for a natural spline, with zero second 
				derivative on that boundary.
			*/
			void Spline(std::vector<double> &x, std::vector<double> &y, double yp1, double ypn) ;
			//! Cubic Spline interpolation. This function does the actual interpolation at specified point, using provided second derivatives at the knot points. 
			/*!
				\param xa std::vector of x values.
				\param ya std::vector of y values.
				\param x is the value we want to find the interpolating y value at. 
				\return returns interpolated y at point x. 
			*/
			inline double Splint(std::vector<double> &xa, std::vector<double> &ya, double x)
			{
				int n = (int) xa.size() ; 
				int    klo,khi,k;
				double h,b,a;

				klo=0;
				khi=n-1;

				// binary search for khi, klo where xa[klo] <= x < xa[khi]
				while(khi-klo > 1)
				{
					k=(khi+klo) >> 1;
					if(xa[k] > x) 
						khi = k;
					else 
						klo = k;
				}
				h = xa[khi]-xa[klo];
				if(h==0.0) 
					return(-1);
				a = (xa[khi]-x)/h;
				b = (x-xa[klo])/h;
				// cubic interpolation at x.
				double yaKlo = ya[klo] ; 
				double yaKhi = ya[khi] ; 
				double y2Klo = mvect_Y2[klo] ;
				double y2Khi = mvect_Y2[khi] ; 
				double y = a*yaKlo+ b*yaKhi +((a*a*a-a)*y2Klo +(b*b*b-b)*y2Khi)*(h*h)/6.0 ;

				return y;
			}
			//! Cubic Spline interpolation. This function does the actual interpolation at specified point, using provided second derivatives at the knot points. 
			/*!
				\param xa std::vector of x values.
				\param ya std::vector of y values.
				\param x std::vector of x values that we want to find the interpolating y value at. 
				\param y std::vector of interpolated y at each point of x. 
			*/
			inline void Splint(std::vector<double> &xa, std::vector<double> &ya, std::vector<double> &x, std::vector<double> &y)
			{
				int num_x = x.size() ; 
				int n = (int) xa.size() ; 
				int    klo,khi,k;
				double h,b,a;
				double y1 ; 			

				if (num_x == 0)
				{
					y.insert(y.begin(), n, 0) ; 
					return ; 
				}

				double min_xa = xa[0] ; 
				double max_xa = xa[n-1] ; 

				for (int i = 0 ; i <num_x ; i ++)
				{
					double x1 = x[i] ; 
			
					if (x1 < min_xa || x1 > max_xa)
						y1 = 0 ; 
					else
					{			
						klo=0;
						khi=n-1;

						// binary search for khi, klo where xa[klo] <= x < xa[khi]
						while(khi-klo > 1)
						{
							k=(khi+klo) >> 1;
							if(xa[k] > x1) 
								khi = k;
							else 
								klo = k;
						}
						h = xa[khi]-xa[klo];
						if(h==0.0) 
						{
							y1 = -1; 
							break ; 
						}
							
						a = (xa[khi]-x1)/h;
						b = (x1-xa[klo])/h;
						// cubic interpolation at x.
						double yaKlo = ya[klo] ; 
						double yaKhi = ya[khi] ; 
						double y2Klo = mvect_Y2[klo] ;
						double y2Khi = mvect_Y2[khi] ; 
						y1 = a*yaKlo+ b*yaKhi +((a*a*a-a)*y2Klo +(b*b*b-b)*y2Khi)*(h*h)/6.0 ;
					}
					y.push_back(y1) ; 
				}		
					
				
			}
			//! Zero filling imputation. This function takes in data which has missing values and adds in zero values
			/*!
				\param xa std::vector of x values.
				\param ya std::vector of y values.
				\return number of points added. 
			*/
			int ZeroFillMissing(std::vector<double> &x, std::vector<double> &y, int num_pts) ; 
			//! Zero filling imputation. This function takes in data which has missing values and adds in zero values
			/*!
				\param xa std::vector of x values.
				\param ya std::vector of y values.
				\return number of points added. 
			*/
			int ZeroFillMissing(std::vector<float> &x, std::vector<float> &y, int num_pts) ; 
		};
	}
}