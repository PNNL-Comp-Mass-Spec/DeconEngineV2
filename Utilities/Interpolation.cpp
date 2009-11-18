#include "interpolation.h"
#include <math.h>

//#include "DeconException.h"

#include <iostream>
namespace Engine
{
	namespace Utilities
	{
		Interpolation::Interpolation(void)
		{

		}

		Interpolation::~Interpolation(void)
		{
		}

		void Interpolation::Spline(std::vector<double> &x, std::vector<double> &y, double yp1, double ypn)
		{
			int n = (int) x.size() ; 				
						
			if (n <= 0 )
			{
				return ;			
			}

			int i, k;
			double p,qn,sig,un ;			
				
			if (mvect_temp_spline.size() > 0)
				mvect_temp_spline.clear() ; 
			if (mvect_Y2.size() > 0)
				mvect_Y2.clear() ;
			
			try
			{
				mvect_Y2.resize(n) ; 
				if(yp1 > 0.99e30) 
				{
					mvect_Y2[0]=0.0;
					mvect_temp_spline.push_back(0) ; 
				}
				else
				{
					mvect_Y2[0] = -0.5;
					mvect_temp_spline.push_back((3.0f/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1));
				}
				// generate second derivatives at internal points using recursive spline equations.
				for (i=1;i<=n-2;i++)
				{
					sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
					p = sig*mvect_Y2[i-1]+2.0;
					mvect_Y2[i]=(sig-1.0)/p;
					double last_spline_val = mvect_temp_spline[i-1] ; 
					double spline_val = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]) ; 
					spline_val = (6.0*spline_val/(x[i+1]-x[i-1])-sig*last_spline_val)/p ; 
					mvect_temp_spline.push_back(spline_val);
				}
				if (ypn > 0.99e30) 
					qn=un=0.0;
				else
				{
					qn = 0.5;
					un = (3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
				}

				double y2_last_last = mvect_Y2[n-2]; 
				double temp_last_last = mvect_temp_spline[n-2] ; 
				double y2_last = (un-qn*temp_last_last)/(qn*y2_last_last + 1.0) ; 
				mvect_Y2[n-1]= y2_last ;
				for(k=n-2;k>=0;k--)                                       //[gord] this loop takes forever when summing spectra
				{
					double temp = mvect_temp_spline[k] ;     
					double y2_next_val = mvect_Y2[k+1] ; 
					double y2_val = mvect_Y2[k] ; 
					mvect_Y2[k]=y2_val* y2_next_val + temp;
				}
			}
			catch (System::NullReferenceException *err)
			{
				return ; 
			}
		}

		int Interpolation::ZeroFillMissing(std::vector<float> &x, std::vector<float> &y, int max_pts_toAddForZero)
		{
			std::vector<float> tempX ; 
			std::vector<float> tempY ; 
			if (x.size() != y.size())
			{
				char *mesg = "x and y need to be of the same size in ZeroFillMissing" ; 
				throw mesg ; 				
			}

			int num_pts = x.size() ; 
			if (num_pts <= 1)
			{
				char *mesg=  "x is empty in ZeroFillMissing" ; 
				return 0 ; 
			}
			tempX.reserve(num_pts*2) ; 
			tempY.reserve(num_pts*2) ; 

			float min_distance = x[1]-x[0] ; 
			for (int index = 2 ; index < num_pts-1 ; index++)
			{
				float current_diff = x[index] - x[index - 1] ; 
				if (min_distance > current_diff && current_diff > 0)
				{
					min_distance = current_diff ; 
				}
			}

			tempX.push_back(x[0]) ; 
			tempY.push_back(y[0]) ; 

			float last_diff = min_distance ; 
			int lastDiffIndex = 0;
			float lastX = x[0] ; 
			for (int index = 1 ; index < num_pts-1 ; index++)
			{
				float current_diff = x[index] - lastX ; 
				double diffBetweenCurrentAndBase = x[index] - x[lastDiffIndex];
				double differenceFactor = 1.5;


				if (sqrt(diffBetweenCurrentAndBase)>differenceFactor)
				{
					differenceFactor=sqrt(diffBetweenCurrentAndBase);
				}

				if (current_diff > differenceFactor * last_diff)
				{
					// insert points. 
					int num_pts_to_add = ((int) (current_diff / last_diff + 0.5)) -1  ; 
					if (num_pts_to_add > 2 * max_pts_toAddForZero)
					{
						for (int pt_num = 0 ; pt_num < max_pts_toAddForZero ; pt_num++)
						{
							if (lastX >= x[index])
								break ; 
							lastX += last_diff ; 
							tempX.push_back(lastX) ; 
							tempY.push_back(0) ; 
						}
						float nextLastX =x[index] - max_pts_toAddForZero * last_diff ; 
						if (nextLastX > lastX + last_diff)
						{
							lastX = nextLastX; 
						}
						for (int pt_num = 0 ; pt_num < max_pts_toAddForZero ; pt_num++)
						{
							lastX += last_diff ; 
							if (lastX >= x[index])
								break ; 
							tempX.push_back(lastX) ; 
							tempY.push_back(0) ; 
						}
					}
					else
					{
						for (int pt_num = 0 ; pt_num < num_pts_to_add ; pt_num++)
						{
							lastX += last_diff ; 
							if (lastX >= x[index])
								break ; 
							tempX.push_back(lastX) ; 
							tempY.push_back(0) ; 
						}
					}
					tempX.push_back(x[index]); 
					tempY.push_back(y[index]); 
					lastX = x[index] ; 
				}
				else
				{
					tempX.push_back(x[index]); 
					tempY.push_back(y[index]); 
					last_diff = current_diff ; 
					lastDiffIndex = index;
					lastX = x[index] ; 
				}
			}
			int num_pts_total = tempX.size() ; 
			int num_pts_added = tempX.size() - x.size() ; 

			x.clear() ; 
			y.clear() ; 

			x.insert(x.begin(), tempX.begin(), tempX.end()) ; 
			y.insert(y.begin(), tempY.begin(), tempY.end()) ; 

			return num_pts_added ; 
		}

		
		int Interpolation::ZeroFillMissing(std::vector<double> &x, std::vector<double> &y, int max_pts_toAddForZero)
		{				
			std::vector<double> tempX ; 
			std::vector<double> tempY ; 
			if (x.size() != y.size())
			{
				char *mesg = "x and y need to be of the same size in ZeroFillMissing" ; 
				//throw new Engine::Exception::InterpolationException(mesg) ; 
				return 0 ; 
			}

			int num_pts = x.size() ; 
			if (num_pts <= 1)
			{
				char *mesg =  "x size is 1 or less in ZeroFillMissing Overloaded" ; 
				return 0 ; 
			}

			try
			{
				tempX.reserve(num_pts*2) ; 
				tempY.reserve(num_pts*2) ; 

				double min_distance = x[1]-x[0] ; 
				for (int index = 2 ; index < num_pts-1 ; index++)
				{
					float current_diff = (float)(x[index] - x[index - 1]) ; 
					if (min_distance > current_diff && current_diff > 0)
					{
						min_distance = current_diff ; 
					}
				}

				tempX.push_back(x[0]) ; 
				tempY.push_back(y[0]) ; 

				double last_diff = min_distance ; 
				int lastDiffIndex = 0;

				double lastX = x[0] ; 

				for (int index = 1 ; index < num_pts-1 ; index++)
				{
					double current_diff = x[index] - lastX ; 
					double diffBetweenCurrentAndBase = x[index] - x[lastDiffIndex];
					double differenceFactor = 1.5;


					if (sqrt(diffBetweenCurrentAndBase)>differenceFactor)
					{
						differenceFactor=sqrt(diffBetweenCurrentAndBase);
					}


					if (current_diff > differenceFactor * last_diff)
					{
						// insert points. 
						int num_pts_to_add = ((int) (current_diff / last_diff + 0.5)) -1  ; 
						if (num_pts_to_add > 2 * max_pts_toAddForZero)
						{
							for (int pt_num = 0 ; pt_num < max_pts_toAddForZero ; pt_num++)
							{
								if (lastX >= x[index])
									break ; 
								lastX += last_diff ; 
								tempX.push_back(lastX) ; 
								tempY.push_back(0) ; 
							}
							double nextLastX =x[index] - max_pts_toAddForZero * last_diff ; 
							if (nextLastX > lastX + last_diff)
							{
								lastX = nextLastX; 
							}

							for (int pt_num = 0 ; pt_num < max_pts_toAddForZero ; pt_num++)
							{
								if (lastX >= x[index])
									break ; 
								tempX.push_back(lastX) ; 
								tempY.push_back(0) ; 
								lastX += last_diff ; 
							}
						}
						else
						{
							for (int pt_num = 0 ; pt_num < num_pts_to_add ; pt_num++)
							{
								lastX += last_diff ; 
								if (lastX >= x[index])
									break ; 
								tempX.push_back(lastX) ; 
								tempY.push_back(0) ; 
							}
						}
						tempX.push_back(x[index]); 
						tempY.push_back(y[index]); 
						lastX = x[index] ; 
					}
					else
					{
						tempX.push_back(x[index]); 
						tempY.push_back(y[index]); 
						last_diff = current_diff ; 
						lastDiffIndex = index;
						lastX = x[index] ; 
					}
				}
				int num_pts_total = tempX.size() ; 
				int num_pts_added = tempX.size() - x.size() ; 

				x.clear() ; 
				y.clear() ; 

				x.insert(x.begin(), tempX.begin(), tempX.end()) ; 
				y.insert(y.begin(), tempY.begin(), tempY.end()) ;
				return num_pts_added ; 

			}
			catch (System::NullReferenceException *err)
			{
				return 0 ; 
				x.clear() ; 
				y.clear() ; 				
			}			
		}

	}
}