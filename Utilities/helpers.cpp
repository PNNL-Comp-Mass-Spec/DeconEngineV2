// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 
// Licensed under the Apache License, Version 2.0; you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at 
// http://www.apache.org/licenses/LICENSE-2.0

#include "helpers.h" 
#include <fstream> 
#include "DeconException.h" 

namespace Engine
{
	namespace Helpers
	{
		double	to_double(std::string doubleStr)
		{
			char** stopStr = 0;
			return strtod(doubleStr.c_str(),stopStr);
		}

		int	to_int(std::string intStr)
		{
			char** stopStr = 0;
			return strtol(intStr.c_str(),stopStr,10);
		}

		bool GetInt16(char *option_str, char *search_str, int start, short &val)
		{
			char *found_ptr = strstr(&option_str[start], search_str) ; 
			if (found_ptr == NULL)
				return false ; 
			found_ptr += strlen(search_str) ; 
			int ival = atoi(found_ptr) ; 
			val = (short)ival ; 
			return true ; 
		}

		bool GetInt32(char *option_str, char *search_str, int start, int &val)
		{
			char *found_ptr = strstr(&option_str[start], search_str) ; 
			if (found_ptr == NULL)
				return false ; 
			found_ptr += strlen(search_str) ; 
			val = atoi(found_ptr) ; 
			return true ; 
		}

		bool GetDouble(char *option_str, char *search_str, int start, double &val)
		{
			char *found_ptr = strstr(&option_str[start], search_str) ; 
			if (found_ptr == NULL)
				return false ; 
			found_ptr += strlen(search_str) ; 
			val = atof(found_ptr) ; 
			return true ; 
		}

		char ReadByte(char *f_name, long pos)
		{
			std::ifstream fin(f_name, std::ios::binary) ; 
			fin.seekg(pos) ; 
			bool fail = fin.fail() ; 

			char data ; 

			fin.read(&data, 1) ;
			fin.close() ; 
			return data ; 
		}

		short ReadInt16(char *f_name, long pos)
		{

			char data[2] ; 
			short ival ; 

			std::ifstream fin(f_name, std::ios::binary) ;
			fin.seekg(pos) ; 
			bool fail = fin.fail() ; 

			fin.read(data, 2) ;
			fin.close() ; 
			*((char *)&ival) =  data[1] ; 
			*((char *)&ival+1) =  data[0] ; 

			return ival ;
		}

		int ReadInt32(char *f_name, long pos)
		{
			char data[4] ; 
			int ival ;

			std::ifstream fin(f_name, std::ios::binary || std::ios::in) ; 
			fin.seekg(pos) ; 
			bool fail = fin.fail() ; 

			fin.read(data, 4) ;
			fin.close() ; 
			*((char *)&ival) =  data[3] ; 
			*((char *)&ival+1) =  data[2] ; 
			*((char *)&ival+2) =  data[1] ; 
			*((char *)&ival+3) =  data[0] ; 

			return ival ;
		}

		char* ReadFileString(char *f_name, long pos, long size)
		{
			std::ifstream fin(f_name, std::ios::binary || std::ios::in)  ; 

			fin.seekg(pos) ; 
			char *data = new char [size+1] ; 
			fin.read(data, size) ;
			data[size] = '\0' ; 
			fin.close() ; 
			return data ; 
		}

		double sgn(double a)
		{
			if(a<0.0) return(-1.0);
			return(1.0);
		}
		void SolveQuadraticEquation(double b, double c, double *roots)
		{
			// x^2 + b x + c = 0. return in roots.
			double discriminant = b*b - 4*c ; 
			if (discriminant < 0)
			{
				roots[0] = -1 ; 
				roots[1] = -1 ; 
				return ; 
			}
			discriminant = sqrt(discriminant) ; 
			roots[0] = (-b - discriminant)/2 ; 
			roots[1] = (-b + discriminant)/2 ; 
		}

		int CubeRoots(double a, double b, double c, double *roots)
		{
			const double PI=3.1415926 ; 
			double  Q,R;
			double  A,B;
			double  theta;

			Q = (a*a - 3.0*b)/9.0;
			R = (2.0*a*a*a - 9.0*a*b + 27.0*c)/54.0;
			if((R*R)<(Q*Q*Q))
			{
				// here with three real roots.
				theta = acos(R/sqrt(Q*Q*Q));
				roots[0] = -2.0*sqrt(Q)*cos(theta/3.0) - a/3.0;
				roots[1] = -2.0*sqrt(Q)*cos((theta+2.0*PI)/3.0) - a/3.0;
				roots[2] = -2.0*sqrt(Q)*cos((theta-2.0*PI)/3.0) - a/3.0;
				return(3);
			}
			// here with one real root...
			A = -1.0*sgn(R)*pow((fabs(R)+sqrt(fabs(R*R-Q*Q*Q))),1.0/3.0);
			if(A != 0.0) B=Q/A;
			else B = 0.0;
			roots[0] = (A+B)-a/3.0;
			return(1);
		}

		double GetAverage(std::vector<double> &mzs, std::vector<double> &intensities, double max_mz, double max_intensity)
		{
			unsigned int num_pts = (unsigned int) intensities.size() ; 
			if (num_pts == 0)
				return 0 ; 

			double background_intensity = 0 ; 
			double current_intensity ; 

			unsigned int num_pts_used = 0 ;
			
			for (unsigned int i = 0 ; i < num_pts ; i++)
			{
				if (mzs[i] > max_mz)
				{
					break ; 
				}
				if (intensities[i] < max_intensity)
				{
					current_intensity = intensities[i] ; 
					background_intensity += current_intensity ; 
					num_pts_used++ ; 
				}
			}

			return background_intensity / num_pts_used  ; 
		}
	}

}