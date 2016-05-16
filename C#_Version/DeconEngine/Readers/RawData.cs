#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.IO;
using DeconToolsV2.Readers;
using Engine.PeakProcessing;
using Engine.Utilities;

namespace Engine.Readers
{
    [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
    internal abstract class RawData
    {
        public const int MAX_FNAME_LEN = 512;
        public const int MAX_ERR_LEN = 512;
        public static int BACKGROUND_RATIO_FOR_TIC = 3;
        public static double MIN_MZ = 400;
        public static double MAX_MZ = 2000;
        private FileType menm_file_type;

        protected Calibrations.Calibrator mobj_calibrator;

        public const int MAX_SCAN_SIZE = 4 * 1024 * 1024;
        public abstract string GetFileName();
        public abstract FileType GetFileType();

        public virtual Calibrations.Calibrator GetCalibrator()
        {
            return mobj_calibrator;
        }

        public abstract bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num);
        public abstract bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, int num_pts);
        public abstract void Load(string file_n);

        public virtual void Close()
        {
        }

        public abstract int GetNumScans();
        public abstract double GetScanTime(int scan_num);
        public abstract int GetScanSize();

        public virtual int GetNumScansLoaded()
        {
            return GetNumScans();
        }

        public virtual short GetSpectrumType(int scan_num)
        {
            return 0;
        }

        public virtual void GetScanDescription(int scan, out string description)
        {
            description = "Scan #" + scan;
        }

        public abstract double GetSignalRange(int scan_num);

        public abstract void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic);

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

        public virtual double GetMonoMZFromHeader(int scan_num)
        {
            return 0;
        }

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

        public RawData()
        {
            mobj_calibrator = null;
        }

        public virtual void SetCalibrator(Calibrations.Calibrator calib)
        {
            mobj_calibrator = calib;
        }

        public virtual int GetMassIndex(double mz)
        {
            return mobj_calibrator.FindIndexByMass(mz);
        }

        protected virtual void GetRawData(out List<double> vectMZs, out List<double> vectIntensities, int scan,
            double min_mz,
            double max_mz)
        {
            if (max_mz <= min_mz)
                throw new System.Exception("max_mz should be greater than min_mz");

            vectMZs = new List<double>();
            vectIntensities = new List<double>();
            List<double> allMZs;
            List<double> allIntensities;
            GetRawData(out allMZs, out allIntensities, scan);
            int numPts = allMZs.Count;
            if (numPts <= 1)
            {
                System.Console.Error.WriteLine(scan);
                throw new System.Exception("mz_vector empty in GetRawData in RawData.cpp");
                return;
            }
            int startIndex = PeakIndex.GetNearestBinary(allMZs, min_mz, 0, numPts - 1);
            int stopIndex = PeakIndex.GetNearestBinary(allMZs, max_mz, 0, numPts - 1);
            if ((stopIndex - startIndex) <= 1) //nothing in this m/z range
                return;

            vectMZs.AddRange(allMZs.GetRange(startIndex, stopIndex - startIndex));
            vectIntensities.AddRange(allIntensities.GetRange(startIndex, stopIndex - startIndex));
        }

        public virtual void GetSummedSpectra(out List<double> mzs, out List<double> intensities, int scan_num,
            int scan_range)
        {
            Engine.Utilities.Interpolation interpolator = new Interpolation();
            // Get scan info
            GetRawData(out mzs, out intensities, scan_num);
            int num_mzs = mzs.Count;
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
                        int demarcationPoint = (int) ((mzs.Count * 1.0) / 1.414);
                        double mzBin = mzs[demarcationPoint + 1] - mzs[demarcationPoint];
                        double minMZBin = mzs[1] - mzs[0];
                        // need to make sure this bins size never goes below 0.1.
                        // we need a better way than this stupid idea of mine.
                        if (mzBin > 5 * minMZBin)
                            mzBin = minMZBin;
                        else if (mzBin > 0.1)
                            mzBin = 0.1;

                        //checking range
                        double minMZ = mzs[0];
                        double maxMZ = mzs[num_mzs - 1];
                        GetSummedSpectra(out mzs, out intensities, scan_num, scan_range, minMZ, maxMZ, mzBin);
                    }
                }
                catch (System.NullReferenceException e)
                {
                    mzs.Clear();
                    intensities.Clear();
#if DEBUG
                    throw e;
#endif
                    return;
                }
            }
        }

        public virtual void GetSummedSpectra(out List<double> mzs, out List<double> intensities,
            int scan, int scan_range, double min_mz, double max_mz)
        {
            Engine.Utilities.Interpolation interpolator = new Interpolation();

            // Get scan info
            GetRawData(out mzs, out intensities, scan, min_mz, max_mz);
            int num_mzs = mzs.Count;
            const int numZeroFills = 3;
            if (num_mzs > numZeroFills)
            {
                try
                {
                    Interpolation.ZeroFillMissing(ref mzs, ref intensities, numZeroFills);
                    //recheck
                    num_mzs = mzs.Count;
                    if (num_mzs > numZeroFills)
                    {
                        int demarcationPoint = (int) ((mzs.Count * 1.0) / 1.414);
                        double mzBin = mzs[demarcationPoint + 1] - mzs[demarcationPoint];
                        double minMZBin = mzs[1] - mzs[0];

                        // need to make sure this bins size never goes below 0.1.
                        // we need a better way than this stupid idea of mine.
                        if (mzBin > 5 * minMZBin)
                            mzBin = minMZBin;
                        else if (mzBin > 0.1)
                            mzBin = 0.1;

                        GetSummedSpectra(out mzs, out intensities, scan, scan_range, min_mz, max_mz, mzBin);
                    }
                }
                catch (System.NullReferenceException e)
                {
                    mzs.Clear();
                    intensities.Clear();
#if DEBUG
                    throw e;
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
            int minDatasetScan = GetFirstScanNum();
            int maxDatasetScan = GetLastScanNum();

            List<double> scan_mzs = new List<double>();
            List<double> scan_intensities = new List<double>();
            List<double> interpolatedIntensities;

            if (max_mz <= min_mz)
            {
                return;
            }

            int num_bins = (int) ((max_mz - min_mz) / mz_bin);
            double mz = min_mz;
            try
            {
                for (int bin_num = 0; bin_num < num_bins; bin_num++)
                {
                    mzs.Add(mz);
                    intensities.Add(0);
                    mz += mz_bin;
                }
            }
            catch (System.NullReferenceException e)
            {
                mzs.Clear();
                intensities.Clear();
#if DEBUG
                throw e;
#endif
                return;
            }

            // first read current scan and move to the left
            int currentScan = scan;
            int numScansSummed = 0;

            try
            {
                // numScansSummed needs to be 1 + the scan range because we are summing the first
                // scan here.
                while (currentScan >= minDatasetScan && numScansSummed < scan_range + 1)
                {
                    if (!IsMSScan(currentScan))
                    {
                        currentScan--;
                        continue;
                    }

                    GetRawData(out scan_mzs, out scan_intensities, currentScan, min_mz, max_mz);

                    if (scan_intensities.Count <= 3) //Keeping the min number of data points same as number of Zero Fill
                    {
                        currentScan--;
                        numScansSummed++;
                        continue;
                    }

                    Engine.Utilities.Interpolation scan_interpolator = new Interpolation();
                    Interpolation.ZeroFillMissing(ref scan_mzs, ref scan_intensities, 3);
                    if (scan_intensities.Count <= 3) //re-check
                    {
                        currentScan--;
                        numScansSummed++;
                        continue;
                    }
                    scan_interpolator.Spline(scan_mzs, scan_intensities, 0, 0);
                    scan_interpolator.Splint(scan_mzs, scan_intensities, mzs, out interpolatedIntensities);

                    double maxScanMz = scan_mzs[(int) scan_mzs.Count - 1];
                    for (int bin_num = 0; bin_num < num_bins; bin_num++)
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

                    GetRawData(out scan_mzs, out scan_intensities, currentScan, min_mz, max_mz);

                    if (scan_intensities.Count <= 3) //Keeping the min number of data points same as number of Zero Fill
                    {
                        currentScan++;
                        numScansSummed++;
                        continue;
                    }

                    Engine.Utilities.Interpolation scan_interpolator = new Interpolation();
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

                    double maxScanMz = scan_mzs[(int) scan_mzs.Count - 1];
                    for (int bin_num = 0; bin_num < num_bins; bin_num++)
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
            catch (System.NullReferenceException e)
            {
                mzs.Clear();
                intensities.Clear();
#if DEBUG
                throw e;
#endif
                return;
            }
        }
    }
}
#endif