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
#include <math.h>
#include <string> 
#include <vector>

namespace Engine
{
	namespace Helpers
	{
		template <class T> T absolute(T x)
		{
			if (x >= 0)
				return x ; 
			return -1 * x ; 
		}

		double	to_double(std::string doubleStr) ; 
		int	to_int(std::string intStr) ; 

		bool GetInt16(char *option_str, char *search_str, int start, short &val) ;
		bool GetInt32(char *option_str, char *search_str, int start, int &val) ;
		bool GetDouble(char *option_str, char *search_str, int start, double &val) ; 

		char ReadByte(char *f_name, long pos);
		short ReadInt16(char *f_name, long pos);
		int ReadInt32(char *f_name, long pos);
		char* ReadFileString(char *f_name, long pos, long size) ; 

		double sgn(double a) ;
		void SolveQuadraticEquation(double b, double c, double *roots) ;
		int CubeRoots(double a, double b, double c, double *roots) ; 

		//! Gets the average intensity for points in a scan.
	/*!
		\param mzs is std::vector of m/z values
		\param intensities is std::vector of intensity values.
		\param max_mz is maximum m\z value to look at while calculating the average.
		\param max_intensity is the maximum intensity to used in the calculation.
		\return returns the average intensity.
	*/
		double GetAverage(std::vector<double> &mzs, std::vector<double> &intensities, double max_mz, double max_intensity);

	}
}