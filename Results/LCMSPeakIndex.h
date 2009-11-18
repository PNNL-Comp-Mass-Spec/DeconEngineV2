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
#include <vector>
#include "../Utilities/Helpers.h"

namespace Engine
{
	namespace Results
	{
		//! Used to search std::vector of LCMSPeak s for closest point. 
		/*!
			Can look for the closest point either
			\arg \c By looking in a binary search betwee specified indices
			\arg \c linear search around a given index. 
		*/
		template <class PeakClass> class LCMSPeakIndex
		{
		public:
			//! does a search for the given value by doing a linear scan to the left of the given index
			/*!
				\param vec is the std::vector<Engine::Results::LCMSPeak<Engine::PeakProcessing::Peak> > of the peaks. 
				\param mz_val is the value we are looking for.
				\param start_index index of the peak to the left of which we are scanning.
				\return returns the index of the point that is closest to the specified value. 
			*/
			inline int LookLeft(std::vector<Engine::Results::LCMSPeak<PeakClass> > &vec, double mz_val, int start_index)
			{
				// mv_val <= vec[start_index] so start moving index further left.
				int nearest_index = start_index ;
				int next_index = start_index ; 

				if (next_index == 0)
					return 0 ; 

				double next_val = vec[next_index].mobj_peak.mflt_mz ; 
				double best_distance = Helpers::absolute(mz_val - next_val) ; 

				while (next_val > mz_val)
				{
					next_index-- ; 
					next_val = vec[next_index].mobj_peak.mflt_mz ; 
					double dist = Helpers::absolute(next_val - mz_val) ; 
					if (dist < best_distance)
					{
						best_distance = dist ; 
						nearest_index = next_index ; 
					}
					if (next_index == 0)
						break ;
				}
				return nearest_index ; 
			}

			//! does a search for the given value by doing a linear scan to the right of the given index
			/*!
				\param vec is the std::vector of the LCMSPeak s. 
				\param mz_val is the value we are looking for.
				\param start_index index of the peak to the right of which we are scanning.
				\return returns the index of the point that is closest to the specified value. 
			*/
			inline int LookRight(std::vector<Engine::Results::LCMSPeak<PeakClass> > &vec, double mz_val, int start_index)
			{
				// mv_val >= vec[start_index] so start moving index further right.
				int nearest_index = start_index ;
				int next_index = start_index ; 
				int num_pts = (int) vec.size() ; 

				if (next_index >= num_pts -1)
					return num_pts - 1 ; 

				double next_val = vec[next_index].mobj_peak.mflt_mz ; 
				double best_distance = Helpers::absolute(mz_val - next_val) ; 

				// we've gone back too far, posibly. Move pas the mz_val and return that value. 
				while (next_val < mz_val)
				{
					next_index++ ; 

					next_val = vec[next_index].mobj_peak.mflt_mz ; 
					double dist = Helpers::absolute(next_val - mz_val) ; 
					if (dist < best_distance)
					{
						best_distance = dist ; 
						nearest_index = next_index ; 
					}

					if (next_index == num_pts-1)
						break ;
				}
				return nearest_index ; 
			}

			//! Gets the index of the point nearest to the specified point, between the specified indices.
			/*!
				\param vec is the std::vector of the points. 
				\param mz_val is the value we are looking for.
				\param start_index minimum index of the point.
				\param stop_index maximum index of the point.
				\return returns the index of the point that is closest to the specified value. 
			*/
			inline int GetNearestBinary(std::vector<Engine::Results::LCMSPeak<PeakClass> > &vec, 
				double mz_val, int start_index, int stop_index)
			{
				double min_val, max_val, mid_val, mid_next_val ; 
				if (vec[start_index].mobj_peak.mflt_mz > mz_val)
					return start_index ; 
				if (vec[stop_index].mobj_peak.mflt_mz < mz_val)
					return stop_index ; 

				int mid_index ; 
				while (true)
				{
					min_val = vec[start_index].mobj_peak.mflt_mz ; 
					max_val = vec[stop_index].mobj_peak.mflt_mz ;
					if (abs(stop_index - start_index) <= 1 && mz_val >= min_val && mz_val <= max_val)
					{
						//return closer value.
						if (Helpers::absolute(min_val - mz_val) < Helpers::absolute(max_val - mz_val))
							return start_index ; 
						return stop_index ; 
					}

					double ratio = ((max_val - mz_val) * 1.0) / (max_val - min_val) ; 
					mid_index = (int) (start_index * ratio + stop_index * (1-ratio) + 0.5) ; 

					if (mid_index == start_index)
						mid_index = start_index + 1 ; 
					else if (mid_index == stop_index)
						mid_index = stop_index - 1 ; 
					
					mid_val = vec[mid_index].mobj_peak.mflt_mz ; 
					if (mid_val >= mz_val)
					{
						stop_index = mid_index ; 
					}
					else if (mid_index +1 == stop_index)
					{
						if (Helpers::absolute(mid_val - mz_val) < Helpers::absolute(max_val - mz_val))
							return mid_index ; 
						return stop_index ; 			
					}
					else
					{
						mid_next_val = vec[mid_index+1].mobj_peak.mflt_mz ; 
						if (mz_val >= mid_val && mz_val <= mid_next_val)
						{
							if (mz_val - mid_val < mid_next_val - mid_val)
							return mid_index ; 
							return mid_index+1 ; 
						}
						start_index =  mid_index + 1 ; 
					}
				}
			}
		};
	}
}