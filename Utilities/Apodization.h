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

namespace Engine
{
	namespace Utilities
	{
		enum ApodizationType {SQUARE =0, PARZEN, HANNING, WELCH, TRIANGLE, NOAPODIZATION } ; 
		class  Apodization
		{
				static void PerformApodization(int startIndex, int stopIndex, ApodizationType type, 
					float *vectIntensities) ;
				static void PerformTriangleApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformWelchApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformHanningApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformParzenApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformSquareApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformInvertedTriangleApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformInvertedWelchApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformInvertedHanningApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformInvertedParzenApodization(int startIndex, int stopIndex, float *vectIntensities) ;
				static void PerformInvertedSquareApodization(int startIndex, int stopIndex, float *vectIntensities) ;
			public:
				static void Apodize(double minX, double maxX, double sampleRate, bool invert, ApodizationType type,
					float *vectIntensities, int numPts, int apexPosition) ;
		};
	}
}