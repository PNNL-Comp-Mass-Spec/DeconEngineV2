#if !Disable_Obsolete
using System;
using System.Collections.Generic;

namespace Engine.Results
{
    /// <summary>
    /// Used to search List of LCMSPeak s for closest point.
    /// </summary>
    /// <remarks>
    /// Can look for the closest point either
    /// - By looking in a binary search between specified indices
    /// - linear search around a given index.
    /// </remarks>
    [Obsolete("All uses are apparently obsolete", true)]
    internal class LCMSPeakIndex
    {
        /// <summary>
        /// does a search for the given value by doing a linear scan to the left of the given index
        /// </summary>
        /// <param name="vec">is the List<Peak > of the peaks.
        /// <param name="mz_val">is the value we are looking for.
        /// <param name="start_index">index of the peak to the left of which we are scanning.
        /// <returns>returns the index of the point that is closest to the specified value.
        public int LookLeft(List<Engine.Results.LcmsPeak> vec, double mz_val, int start_index)
        {
            // mv_val <= vec[start_index] so start moving index further left.
            int nearest_index = start_index;
            int next_index = start_index;

            if (next_index == 0)
                return 0;

            double next_val = vec[next_index].Mz;
            double best_distance = Math.Abs(mz_val - next_val);

            while (next_val > mz_val)
            {
                next_index--;
                next_val = vec[next_index].Mz;
                double dist = Math.Abs(next_val - mz_val);
                if (dist < best_distance)
                {
                    best_distance = dist;
                    nearest_index = next_index;
                }
                if (next_index == 0)
                    break;
            }
            return nearest_index;
        }

        /// <summary>
        /// does a search for the given value by doing a linear scan to the right of the given index
        /// </summary>
        /// <param name="vec">is the List of the LCMSPeak s.
        /// <param name="mz_val">is the value we are looking for.
        /// <param name="start_index">index of the peak to the right of which we are scanning.
        /// <returns>returns the index of the point that is closest to the specified value.
        public int LookRight(List<Engine.Results.LcmsPeak> vec, double mz_val, int start_index)
        {
            // mv_val >= vec[start_index] so start moving index further right.
            int nearest_index = start_index;
            int next_index = start_index;
            int num_pts = (int) vec.Count;

            if (next_index >= num_pts - 1)
                return num_pts - 1;

            double next_val = vec[next_index].Mz;
            double best_distance = Math.Abs(mz_val - next_val);

            // we've gone back too far, posibly. Move pas the mz_val and return that value.
            while (next_val < mz_val)
            {
                next_index++;

                next_val = vec[next_index].Mz;
                double dist = Math.Abs(next_val - mz_val);
                if (dist < best_distance)
                {
                    best_distance = dist;
                    nearest_index = next_index;
                }

                if (next_index == num_pts - 1)
                    break;
            }
            return nearest_index;
        }

        /// <summary>
        /// Gets the index of the point nearest to the specified point, between the specified indices.
        /// </summary>
        /// <param name="vec">is the List of the points.
        /// <param name="mz_val">is the value we are looking for.
        /// <param name="start_index">minimum index of the point.
        /// <param name="stop_index">maximum index of the point.
        /// <returns>returns the index of the point that is closest to the specified value.
        public int GetNearestBinary(List<Engine.Results.LcmsPeak> vec,
            double mz_val, int start_index, int stop_index)
        {
            double min_val, max_val, mid_val, mid_next_val;
            if (vec[start_index].Mz > mz_val)
                return start_index;
            if (vec[stop_index].Mz < mz_val)
                return stop_index;

            int mid_index;
            while (true)
            {
                min_val = vec[start_index].Mz;
                max_val = vec[stop_index].Mz;
                if (Math.Abs(stop_index - start_index) <= 1 && mz_val >= min_val && mz_val <= max_val)
                {
                    //return closer value.
                    if (Math.Abs(min_val - mz_val) < Math.Abs(max_val - mz_val))
                        return start_index;
                    return stop_index;
                }

                double ratio = ((max_val - mz_val) * 1.0) / (max_val - min_val);
                mid_index = (int) (start_index * ratio + stop_index * (1 - ratio) + 0.5);

                if (mid_index == start_index)
                    mid_index = start_index + 1;
                else if (mid_index == stop_index)
                    mid_index = stop_index - 1;

                mid_val = vec[mid_index].Mz;
                if (mid_val >= mz_val)
                {
                    stop_index = mid_index;
                }
                else if (mid_index + 1 == stop_index)
                {
                    if (Math.Abs(mid_val - mz_val) < Math.Abs(max_val - mz_val))
                        return mid_index;
                    return stop_index;
                }
                else
                {
                    mid_next_val = vec[mid_index + 1].Mz;
                    if (mz_val >= mid_val && mz_val <= mid_next_val)
                    {
                        if (mz_val - mid_val < mid_next_val - mid_val)
                            return mid_index;
                        return mid_index + 1;
                    }
                    start_index = mid_index + 1;
                }
            }
        }
    }
}
#endif