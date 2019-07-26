using System;
using System.Collections.Generic;
using System.IO;
using DeconToolsV2.Readers;
using Engine.PeakProcessing;
using Engine.Utilities;

namespace Engine.Readers
{
    /// <summary>
    /// RawData
    /// </summary>
    /// <remarks>Used by DeconTools for ICR2LSRun and DeconMSn</remarks>
    internal abstract class RawData
    {
        public const int MAX_FNAME_LEN = 512;
        public const int MAX_ERR_LEN = 512;
        public static int BACKGROUND_RATIO_FOR_TIC = 3;
        public static double MIN_MZ = 400;
        public static double MAX_MZ = 2000;

#if Enable_Obsolete
        protected Calibrations.Calibrator mobj_calibrator;

        public virtual Calibrations.Calibrator GetCalibrator()
        {
            return mobj_calibrator;
        }

#endif

        public const int MAX_SCAN_SIZE = 4 * 1024 * 1024;

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public abstract string GetFileName();

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public abstract FileType GetFileType();

        public abstract bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid);
        public abstract bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts);
        public abstract void Load(string file_n);

        public virtual void Close()
        {
        }

        public abstract int GetNumScans();
        public abstract double GetScanTime(int scan_num);

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public abstract int GetScanSize();

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public virtual int GetNumScansLoaded()
        {
            return GetNumScans();
        }

        public virtual short GetSpectrumType(int scan_num)
        {
            return 0;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public virtual void GetScanDescription(int scan, out string description)
        {
            description = "Scan #" + scan;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public abstract double GetSignalRange(int scan_num, bool centroid);

        public abstract bool IsZoomScan(int scan_num);

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public abstract void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic);

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public virtual int GetNextScanNum(int current_scan_num)
        {
            return current_scan_num + 1;
        }

        public virtual int GetFirstScanNum()
        {
            return 1;
        }

        public virtual int GetLastScanNum()
        {
            return 1;
        }

        public abstract int GetParentScan(int scan_num);
        public abstract bool IsMSScan(int scan_num);

        public abstract int GetMSLevel(int scan_num);
        public abstract bool IsProfileScan(int scan_num);
        public abstract double GetParentMz(int scan_num);

        public virtual bool IsFTScan(int scanNum)
        {
            return false;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public virtual double GetMonoMZFromHeader(int scan_num)
        {
            return 0;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public virtual short GetMonoChargeFromHeader(int scan_num)
        {
            return 0;
        }

        public virtual int GetFrameNumber()
        {
            return 0;
        }

        public virtual double GetAGCAccumulationTime(int scan_num)
        {
            return 0;
        }

        public virtual double GetTICForScan(int scan_num)
        {
            return 0;
        }

        public bool IsDir(string path)
        {
            return Directory.Exists(path);
        }

        protected RawData()
        {
#if Enable_Obsolete
            mobj_calibrator = null;
#endif

        }
#if Enable_Obsolete
        public virtual void SetCalibrator(Calibrations.Calibrator calib)
        {
            mobj_calibrator = calib;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public virtual int GetMassIndex(double mz)
        {
            return mobj_calibrator.FindIndexByMass(mz);
        }
#endif

        protected virtual void GetRawData(out List<double> vectMZs, out List<double> vectIntensities, int scan,
            double min_mz, double max_mz, bool centroid)
        {
            if (max_mz <= min_mz)
                throw new Exception("max_mz should be greater than min_mz");

            vectMZs = new List<double>();
            vectIntensities = new List<double>();
            GetRawData(out var allMZs, out var allIntensities, scan, centroid);
            var numPts = allMZs.Count;
            if (numPts <= 1)
            {
                Console.Error.WriteLine(scan);
                throw new Exception("mz_vector empty in GetRawData in RawData.cpp");
            }
            var startIndex = PeakIndex.GetNearestBinary(allMZs, min_mz, 0, numPts - 1);
            var stopIndex = PeakIndex.GetNearestBinary(allMZs, max_mz, 0, numPts - 1);
            if ((stopIndex - startIndex) <= 1) //nothing in this m/z range
                return;

            vectMZs.AddRange(allMZs.GetRange(startIndex, stopIndex - startIndex));
            vectIntensities.AddRange(allIntensities.GetRange(startIndex, stopIndex - startIndex));
        }

        /*public double GetSummedSpectraFast(out List<double> mzs, out List<double> intensities, int scan_num, int scan_range, double minMz, double maxMz )
        {
            // AM : new faster version of getting a summed spectra using maps as opposed to interpolation
            long temp_mz, minMzLong, maxMzLong;
            double set_precision = 1000000 ;
            double temp_intensity ;
            //typedef std::pair <long, double> Mz_Int_Pair;
            std::map<long,double>::iterator iter_map ;
            std::map <long, double> mz_intensity_map ;
            int  current_scan = scan_num ;
            int scan_count = 0 ;
            int total_scans = (scan_range*2) + 1;
            double threshold, background_intensity = 0.0;
            int direction = 0; //0 - means move left while 1 means move right

            bool centroid = false;

            while (scan_count < total_scans)
            {

                if (current_scan < GetFirstScanNum() || current_scan > GetLastScanNum())
                {
                    break;
                }

                //start and check if current_scan == MS scan

                if (IsMSScan(current_scan))
                {

                    //that means we've found an msScan, add it up to our existing map
                    //if it's the center scan then there won't be any data in the map
                    //nevertheless this is common to all operations and so to keep the
                    //lines of code to a minimum we do this
                    GetRawData(out mzs, out intensities, current_scan, centroid);


                    threshold = Utils.GetAverage(intensities, float.MaxValue);
                    background_intensity += Utils.GetBackgroundLevel(intensities, (float) (threshold));

                    int num_points = mzs.Count;
                    if (minMz == 0)
                    {
                        minMz = mzs[0];
                    }
                    if (maxMz == 0)
                    {
                        maxMz = mzs[num_points - 1];
                    }

                    minMzLong = (long) Math.Floor(minMz * set_precision + 0.5);
                    maxMzLong = (long) Math.Floor(maxMz * set_precision + 0.5);

                    for (int j = 0; j < num_points; j++)
                    {
                        //if ( 0.0 == intensities->at(j)){
                        //	continue;
                        //}

                        temp_mz = Math.Floor(mzs[j] * set_precision + 0.5);
                        if (temp_mz >= minMzLong && temp_mz <= maxMzLong)
                        {
                            iter_map = mz_intensity_map.find(temp_mz);
                            if (iter_map == mz_intensity_map.end())
                            {
                                // absent, create new
                                mz_intensity_map[temp_mz] = intensities->at(j);
                            }
                            else
                            {
                                //present, sum it up
                                temp_intensity = mz_intensity_map[iter_map->first] + intensities->at(j);
                                mz_intensity_map[iter_map->first] = temp_intensity;
                            }
                        }
                    }

                    if (scan_count == scan_range)
                    {
                        direction = 1;

                        //since we have to start moving right, we need to start from the scan that is
                        //immediately following our original scan number
                        current_scan = scan_num + 1;
                    }
                    scan_count++;
                }

                if (0 == direction)
                {
                    current_scan--;
                }
                else
                {
                    current_scan++;
                }

            }

            //clear the original arrays and now populate them from the map
            //Also make sure that we've got to get our original mz value back from the
            //long int value that's in the map.
            mzs.Clear() ;
            intensities.Clear() ;
            for (iter_map = mz_intensity_map.begin(); iter_map != mz_intensity_map.end(); iter_map++)
            {
                if (iter_map->first >= minMzLong && iter_map->first <= maxMzLong)
                {
                    mzs.Add((iter_map->first) / set_precision);
                    intensities.Add(iter_map->second);
                }
            }
            mz_intensity_map.Clear() ;
            background_intensity = background_intensity/scan_count;

            return background_intensity;

#if false
            try
            {
                // Old method
                //
                //---------- Init ----------//
                // AM : Sticking with this way cuz the above way in release threw results off from debug
                GetRawData(mzs, intensities, scan_num, centroid) ;
                int num_points = mzs->size() ;
                if (minMz == 0)
                {
                    minMz = mzs->at(0) ;
                }
                if(maxMz == 0)
                {
                    maxMz = mzs->at(num_points-1) ;
                }


                minMz = floor(minMz*set_precision+0.5);
                maxMz = floor(maxMz*set_precision+0.5);



                // ---------- Map --------- //
                for(int i =0 ; i< num_points ; i++)
                {
                    temp_mz1 = (*mzs)[i] ;
                    temp_mz = floor(temp_mz1*set_precision+0.5);
                    if (temp_mz >= minMz && temp_mz <= maxMz)
                    {
                        mz_intensity_map.insert(Mz_Int_Pair(temp_mz, (*intensities)[i])) ;
                    }
                }

                // ---------- Process 'before' scan ---------- //
                int current_scan = scan_num - 1 ;
                int num_scans_processed = 1 ;
                while (num_scans_processed <= scan_range && current_scan >= GetFirstScanNum())
                {
                    if (IsMSScan(current_scan))
                    {
                        num_scans_processed++;
                        scanMzs.clear() ;
                        scanIntensities.clear() ;
                        GetRawData(&scanMzs, &scanIntensities, current_scan, centroid) ;
                        for(int j = 0 ; j < (int) scanMzs.size() ; j++)
                        {
                            temp_mz1 = scanMzs[j] ;
                            temp_mz = floor(temp_mz1*set_precision+0.5);
                            if (temp_mz >= minMz && temp_mz <= maxMz)
                            {
                                iter_map = mz_intensity_map.find(temp_mz) ;
                                if (iter_map == mz_intensity_map.end())
                                {
                                    // absent, create new
                                    mz_intensity_map.insert(Mz_Int_Pair(temp_mz, scanIntensities[j])) ;
                                    temp_intensity = scanIntensities[j] ;
                                }
                                else
                                {
                                    //present, sum it up
                                    temp_intensity = scanIntensities[j] ;
                                    temp_mz = iter_map->first ;
                                    temp_intensity = mz_intensity_map[iter_map->first];
                                    mz_intensity_map[iter_map->first] = mz_intensity_map[iter_map->first] + scanIntensities[j] ;
                                    temp_intensity = mz_intensity_map[iter_map->first];
                                }
                            }
                        }
                    }
                    current_scan--;
                }

                // ---------- Process 'after' scan ----------//
                current_scan = scan_num+1 ;
                num_scans_processed = 1 ;
                while(num_scans_processed <= scan_range && current_scan <= GetLastScanNum())
                {
                    if (IsMSScan(current_scan))
                    {
                        num_scans_processed++;
                        scanMzs.clear() ;
                        scanIntensities.clear() ;
                        GetRawData(&scanMzs, &scanIntensities, current_scan, centroid) ;
                        for(int j = 0 ; j < (int) scanMzs.size() ; j++)
                        {
                            temp_mz1 = scanMzs[j] ;
                            temp_mz = floor(temp_mz1*set_precision+0.5);
                            if (temp_mz >= minMz && temp_mz <= maxMz)
                            {
                                iter_map = mz_intensity_map.find(temp_mz) ;
                                if (iter_map == mz_intensity_map.end())
                                {
                                    //absent, create new
                                    mz_intensity_map.insert(Mz_Int_Pair(temp_mz, scanIntensities[j])) ;
                                    temp_intensity = scanIntensities[j] ;
                                }
                                else
                                {
                                    //present, so sum
                                    temp_intensity = scanIntensities[j] ;
                                    temp_mz = iter_map->first ;
                                    temp_intensity = mz_intensity_map[iter_map->first];
                                    mz_intensity_map[iter_map->first] = mz_intensity_map[iter_map->first] + scanIntensities[j] ;
                                    temp_intensity = mz_intensity_map[iter_map->first];
                                }
                            }
                        }
                    }
                    current_scan++ ;
                }

                // ---------- Store and return ----------//
                mzs->clear() ;
                intensities->clear() ;
                for (iter_map = mz_intensity_map.begin();iter_map != mz_intensity_map.end(); iter_map++)
                {
                    if (iter_map->first >= minMz && iter_map->first <= maxMz)
                    {
                        mzs->push_back((iter_map->first)/set_precision) ;
                        intensities->push_back(iter_map->second) ;
                    }
                }
                mz_intensity_map.clear() ;
            }
            catch (System::NullReferenceException *err)
            {
                mzs->clear() ;
                intensities->clear() ;
                return ;
            }
#endif
        }*/

        public virtual void GetSummedSpectra(out List<double> mzs, out List<double> intensities, int scan_num,
            int scan_range)
        {
            var interpolator = new Interpolation();
            var centroid = false;
            // Get scan info
            GetRawData(out mzs, out intensities, scan_num, centroid);
            var num_mzs = mzs.Count;
            const int numZeroFills = 3;
            if (num_mzs > numZeroFills)
            {
                try
                {
                    Interpolation.ZeroFillMissing(ref mzs, ref intensities, numZeroFills);
                    num_mzs = mzs.Count;
                    //recheck
                    if (num_mzs > numZeroFills)
                    {
                        var demarcationPoint = (int) ((mzs.Count * 1.0) / 1.414);
                        var mzBin = mzs[demarcationPoint + 1] - mzs[demarcationPoint];
                        var minMZBin = mzs[1] - mzs[0];

                        // Do not allow mzBin to be too small
                        if (mzBin < 0.00001)
                            mzBin = 0.00001;

                        // Do not allow minMZBin to be too small
                        if (minMZBin < 0.00001)
                            minMZBin = 0.00001;

                        // need to make sure this bins size never goes below 0.1.
                        // we need a better way than this stupid idea of mine.
                        if (mzBin > 5 * minMZBin)
                            mzBin = minMZBin;
                        else if (mzBin > 0.1)
                            mzBin = 0.1;

                        //checking range
                        var minMZ = mzs[0];
                        var maxMZ = mzs[num_mzs - 1];
                        GetSummedSpectra(out mzs, out intensities, scan_num, scan_range, minMZ, maxMZ, mzBin);
                    }
                }
                catch (NullReferenceException)
                {
                    mzs.Clear();
                    intensities.Clear();
#if DEBUG
                    throw;
#endif
                    return;
                }
            }
        }

        public virtual void GetSummedSpectra(out List<double> mzs, out List<double> intensities,
            int scan, int scan_range, double min_mz, double max_mz)
        {
            var interpolator = new Interpolation();
            var centroid = false;

            // Get scan info
            GetRawData(out mzs, out intensities, scan, min_mz, max_mz, centroid);
            var num_mzs = mzs.Count;
            const int numZeroFills = 3;
            if (num_mzs > numZeroFills)
            {
                try
                {
                    Interpolation.ZeroFillMissing(ref mzs, ref intensities, numZeroFills);
                    //recheck
                    num_mzs = mzs.Count;
                    // recheck
                    if (num_mzs > numZeroFills)
                    {
                        var demarcationPoint = (int) ((mzs.Count * 1.0) / 1.414);
                        var mzBin = mzs[demarcationPoint + 1] - mzs[demarcationPoint];
                        var minMZBin = mzs[1] - mzs[0];

                        // Do not allow mzBin to be too small
                        if (mzBin < 0.00001)
                            mzBin = 0.00001;

                        // Do not allow minMZBin to be too small
                        if (minMZBin < 0.00001)
                            minMZBin = 0.00001;

                        // need to make sure this bins size never goes below 0.1.
                        // we need a better way than this stupid idea of mine.
                        if (mzBin > 5 * minMZBin)
                            mzBin = minMZBin;
                        else if (mzBin > 0.1)
                            mzBin = 0.1;

                        GetSummedSpectra(out mzs, out intensities, scan, scan_range, min_mz, max_mz, mzBin);
                    }
                }
                catch (NullReferenceException)
                {
                    mzs.Clear();
                    intensities.Clear();
#if DEBUG
                    throw;
#endif
                    return;
                }
            }
        }

        protected virtual void GetSummedSpectra(out List<double> mzs, out List<double> intensities, int scan,
            int scan_range,
            double min_mz, double max_mz, double mz_bin)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            var minDatasetScan = GetFirstScanNum();
            var maxDatasetScan = GetLastScanNum();

            if (max_mz <= min_mz)
            {
                return;
            }

            var num_bins = (int) ((max_mz - min_mz) / mz_bin);
            var mz = min_mz;
            try
            {
                for (var bin_num = 0; bin_num < num_bins; bin_num++)
                {
                    mzs.Add(mz);
                    intensities.Add(0);
                    mz += mz_bin;
                }
            }
            catch (NullReferenceException)
            {
                mzs.Clear();
                intensities.Clear();
#if DEBUG
                throw;
#endif
                return;
            }

            // first read current scan and move to the left
            var currentScan = scan;
            var numScansSummed = 0;
            var centroid = false;

            try
            {
                // numScansSummed needs to be 1 + the scan range because we are summing the first
                // scan here.
                List<double> interpolatedIntensities;
                List<double> scan_mzs;
                List<double> scan_intensities;
                while (currentScan >= minDatasetScan && numScansSummed < scan_range + 1)
                {
                    if (!IsMSScan(currentScan))
                    {
                        currentScan--;
                        continue;
                    }

                    GetRawData(out scan_mzs, out scan_intensities, currentScan, min_mz, max_mz, centroid);

                    if (scan_intensities.Count <= 3) //Keeping the min number of data points same as number of Zero Fill
                    {
                        currentScan--;
                        numScansSummed++;
                        continue;
                    }

                    var scan_interpolator = new Interpolation();
                    Interpolation.ZeroFillMissing(ref scan_mzs, ref scan_intensities, 3);
                    if (scan_intensities.Count <= 3) //re-check
                    {
                        currentScan--;
                        numScansSummed++;
                        continue;
                    }
                    scan_interpolator.Spline(scan_mzs, scan_intensities, 0, 0);
                    scan_interpolator.Splint(scan_mzs, scan_intensities, mzs, out interpolatedIntensities);
                    // TODO: Should be able to replace the internal CubicSpline with the following code:
                    //var interpolator = CubicSpline.InterpolateNaturalSorted(scan_mzs.ToArray(), scan_intensities.ToArray());
                    //interpolatedIntensities = new List<double>();
                    //foreach (var thisMz in mzs)
                    //{
                    //    interpolatedIntensities.Add(interpolator.Interpolate(thisMz));
                    //}

                    var maxScanMz = scan_mzs[(int) scan_mzs.Count - 1];
                    for (var bin_num = 0; bin_num < num_bins; bin_num++)
                    {
                        if (bin_num * mz_bin + min_mz >= maxScanMz)
                            break;
                        if (interpolatedIntensities[bin_num] > 0)
                            intensities[bin_num] = intensities[bin_num] + interpolatedIntensities[bin_num];
                    }
                    currentScan--;
                    numScansSummed++;
                }
                currentScan = scan + 1;
                numScansSummed = 0;
                while (currentScan <= maxDatasetScan && numScansSummed < scan_range)
                {
                    if (!IsMSScan(currentScan))
                    {
                        currentScan ++;
                        continue;
                    }

                    GetRawData(out scan_mzs, out scan_intensities, currentScan, min_mz, max_mz, centroid);

                    if (scan_intensities.Count <= 3) //Keeping the min number of data points same as number of Zero Fill
                    {
                        currentScan++;
                        numScansSummed++;
                        continue;
                    }

                    var scan_interpolator = new Interpolation();
                    Interpolation.ZeroFillMissing(ref scan_mzs, ref scan_intensities, 3);
                    if (scan_intensities.Count <= 3) //re-check
                    {
                        currentScan++;
                        numScansSummed++;
                        continue;
                    }

                    scan_interpolator.Spline(scan_mzs, scan_intensities, 0, 0);
                        //[gord] this might be the chokepoint for why summing takes so long
                    scan_interpolator.Splint(scan_mzs, scan_intensities, mzs, out interpolatedIntensities);
                        //[gord] this might be the chokepoint for why summing takes so long
                    // TODO: Should be able to replace the internal CubicSpline with the following code:
                    //var interpolator = CubicSpline.InterpolateNaturalSorted(scan_mzs.ToArray(), scan_intensities.ToArray());
                    //interpolatedIntensities = new List<double>();
                    //foreach (var thisMz in mzs)
                    //{
                    //    interpolatedIntensities.Add(interpolator.Interpolate(thisMz));
                    //}

                    var maxScanMz = scan_mzs[(int) scan_mzs.Count - 1];
                    for (var bin_num = 0; bin_num < num_bins; bin_num++)
                    {
                        if (bin_num * mz_bin + min_mz >= maxScanMz)
                            break;
                        if (interpolatedIntensities[bin_num] > 0)
                            intensities[bin_num] = intensities[bin_num] + interpolatedIntensities[bin_num];
                    }
                    currentScan++;
                    numScansSummed++;
                }
            }
            catch (NullReferenceException)
            {
                mzs.Clear();
                intensities.Clear();
#if DEBUG
                throw;
#else
                return;
#endif
            }
        }
    }
}