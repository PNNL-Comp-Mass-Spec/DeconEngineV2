using System;
using System.Collections.Generic;
using System.Linq;
using DeconToolsV2.Readers;
using ThermoRawFileReader;

//////////////////////////////////////////////////////////////////////////////
//
// ThermoRawData class
//      This class is a wrapper for ThermoRawFileReader DLL
//
//////////////////////////////////////////////////////////////////////////////

namespace Engine.Readers
{
    internal class ThermoRawData : RawData
    {
        private string mRawFilePath;

        private int mScanCount;
        private int mScanStart;

        /// <summary>
        /// When the .raw file is opened, this is set to the maximum scan number in the file
        /// However, each time GetScanData is called, this is updated to be the scan number of the retrieved data
        /// </summary>
        private int mScanEnd;

        private double mSignalRange;

        private XRawFileIO mThermoRawFileReader;

        private int mLastScanMzCount;

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override FileType GetFileType()
        {
            return FileType.THERMORAW;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override int GetNumScansLoaded()
        {
            return GetNumScans();
        }

        public string GetScanFilterString(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            return scanInfo.FilterText;
        }

        private clsScanInfo GetScanInfo(int scanNum)
        {
            mThermoRawFileReader.GetScanInfo(scanNum, out clsScanInfo scanInfo);
            return scanInfo;
        }

        public override short GetSpectrumType(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);

            var collisionMode = XRawFileIO.GetScanTypeNameFromThermoScanFilterText(scanInfo.FilterText);

            if (collisionMode.StartsWith("CID", StringComparison.OrdinalIgnoreCase))
                return 1;

            if (collisionMode.StartsWith("ET", StringComparison.OrdinalIgnoreCase))
                return 2;

            if (collisionMode.StartsWith("HCD", StringComparison.OrdinalIgnoreCase))
                return 3;

            return 0;

        }

        /// <summary>
        /// Constructor
        /// </summary>
        public ThermoRawData()
        {
            mThermoRawFileReader = null;
            mLastScanMzCount = 0;
            mRawFilePath = string.Empty;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override void GetScanDescription(int scan, out string description)
        {
            description = GetScanFilterString(scan);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override string GetFileName()
        {
            return mRawFilePath;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override int GetScanSize()
        {
            return mLastScanMzCount;
        }

        public override int GetNumScans()
        {
            return mScanCount;
        }

        public override int GetFirstScanNum()
        {
            return mScanStart;
        }

        /// <summary>
        /// Most recently retrieved scan number
        /// </summary>
        /// <returns></returns>
        public override int GetLastScanNum()
        {
            return mScanEnd;
        }

        public int FirstSpectraNumber()
        {
            return mThermoRawFileReader?.ScanStart ?? 0;
        }

        public int LastSpectraNumber()
        {
            return mThermoRawFileReader?.ScanEnd ?? 0;
        }

        public int Open(string thermoRawFilePath)
        {
            mRawFilePath = thermoRawFilePath;

            try
            {
                mThermoRawFileReader = new XRawFileIO(thermoRawFilePath);
            }
            catch (Exception)
            {
#if DEBUG
                throw;
#endif
                throw new Exception("Unable to open Thermo .raw file: " + mRawFilePath);
            }

            // Get the number of spectra
            mScanCount = mThermoRawFileReader.GetNumScans();
            mScanStart = mThermoRawFileReader.ScanStart;
            mScanEnd = mThermoRawFileReader.ScanEnd;

            return 0;
        }

        public override void Close()
        {
            mThermoRawFileReader.CloseRawFile();
        }

        public override void Load(string file_n)
        {
            Open(file_n);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override double GetSignalRange(int scanNum, bool centroid)
        {
            // When the .raw file is opened, mScanEnd is set to the maximum scan number in the file
            // However, each time GetScanData is called, this is updated to be the scan number of the retrieved data

            if (scanNum == mScanEnd)
                return mSignalRange;

            var maxNumberOfPeaks = 0;
            mThermoRawFileReader.GetScanData(scanNum, out _, out var intensityList, maxNumberOfPeaks, centroid);

            if (intensityList.Length <= 0)
                return 0;

            var minIntensity = intensityList.Min();
            var maxIntensity = intensityList.Max();

            var signalRange = maxIntensity - minIntensity;
            return signalRange;

        }

        public override double GetScanTime(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            return scanInfo.RetentionTime;
        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scanNum, bool centroid, int num_points)
        {
            return GetRawData(out mzs, out intensities, scanNum, centroid);
        }

        public override int GetParentScan(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            if (scanInfo.MSLevel == 1)
                return scanInfo.ScanNumber;

            var parentScanNum = scanNum - 1;

            while (parentScanNum > 0)
            {
                var parentScanLevel = GetMSLevel(parentScanNum);
                if (parentScanLevel < scanInfo.MSLevel)
                    return parentScanNum;

                parentScanNum--;
            }

            return parentScanNum;
        }

        public override bool IsProfileScan(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            return !scanInfo.IsCentroided;
        }

        public override bool IsZoomScan(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            return scanInfo.FilterText.ToLower().Contains("z ms");
        }

        public override bool IsFTScan(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            return scanInfo.FilterText.StartsWith("FT", StringComparison.OrdinalIgnoreCase);
        }

        public override double GetAGCAccumulationTime(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            if (scanInfo.TryGetScanEvent("Ion Injection Time (ms):", out var eventValue, true))
            {
                if (double.TryParse(eventValue, out var injectionTime))
                    return injectionTime;
            }

            return 0;
        }

        public override double GetTICForScan(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            return scanInfo.TotalIonCurrent;
        }

        public override int GetMSLevel(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);
            return scanInfo.MSLevel;
        }

        /// <summary>
        /// Returns true if the scan is a MS1 scan
        /// However, if it is MS1, but the scan filter contains SIM, returns false
        /// </summary>
        /// <param name="scanNum"></param>
        /// <returns></returns>
        public override bool IsMSScan(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);

            if (scanInfo.MSLevel != 1)
                return false;

            return !scanInfo.FilterText.Contains("SIM");
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override double GetMonoMZFromHeader(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);

            if (scanInfo.TryGetScanEvent("Monoisotopic M/Z:", out var eventValue, true))
            {
                if (double.TryParse(eventValue, out var monoisotopicMz))
                    return monoisotopicMz;
            }

            return 0;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override short GetMonoChargeFromHeader(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);

            if (scanInfo.TryGetScanEvent("Charge State:", out var eventValue, true))
            {
                if (short.TryParse(eventValue, out var chargeState))
                    return chargeState;
            }

            return 0;
        }

        /// <summary>
        /// Return the parent m/z of the particular msN scan
        /// </summary>
        /// <param name="scanNum"></param>
        /// <returns></returns>
        public override double GetParentMz(int scanNum)
        {
            var scanInfo = GetScanInfo(scanNum);

            if (ExtractParentIonMZFromFilterText(scanInfo.FilterText, out var parent_mz, out _))
            {
                return parent_mz;
            }

            return 0;
        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scanNum, bool centroid)
        {
            intensities = new List<double>();
            mzs = new List<double>();

            // Update mScanEnd to hold the scan number of data being retrieved
            mScanEnd = scanNum;

            var maxNumberOfPeaks = 0;
            mThermoRawFileReader.GetScanData(scanNum, out var mzList, out var intensityList, maxNumberOfPeaks, centroid);

            var dataCount = mzList.Length;
            if (dataCount > 0)
            {
                if (dataCount < intensities.Capacity)
                {
                    intensities.Capacity = dataCount;
                    mzs.Capacity = dataCount;
                }

                var minIntensity = double.MaxValue;
                var maxIntensity = double.MinValue;
                double lastMz = 0;

                for (var i = 0; i < dataCount; i++)
                {
                    var intensity = intensityList[i];
                    if (intensity > maxIntensity)
                        maxIntensity = intensity;
                    if (intensity < minIntensity)
                        minIntensity = intensity;

                    if (Math.Abs(mzList[i] - lastMz) < double.Epsilon)
                    {
                        // Duplicate m/z value; this is very rare
                        // Occurs in scan 3535 in QC_Shew_16_01-500ng-start-1_28Apr17_Merry_16-11-25.raw, mass 669.4859
                        // Use the higher intensity
                        intensities[intensities.Count - 1] = Math.Max(intensities[intensities.Count - 1], intensity);
                    }
                    else
                    {
                        mzs.Add(mzList[i]);
                        intensities.Add(intensity);
                        lastMz = mzList[i];
                    }
                }

                mSignalRange = (maxIntensity - minIntensity);
            }

            mLastScanMzCount = mzs.Count;

            return mLastScanMzCount > 0;
        }


        /// <summary>
        /// Get TIC or BPI
        /// </summary>
        /// <param name="intensities"></param>
        /// <param name="scanTimes"></param>
        /// <param name="getBPI">When true get the base peak intensity data instead of the TIC</param>
        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override void GetTicFromFile(out List<double> intensities, out List<double> scanTimes, bool getBPI)
        {
            intensities = new List<double>();
            scanTimes = new List<double>();

            var scanStart = FirstSpectraNumber();
            var scanEnd = LastSpectraNumber();

            for (var scanNum = scanStart; scanNum <= scanEnd; scanNum++)
            {
                var scanInfo = GetScanInfo(scanNum);

                if (getBPI)
                    intensities.Add(scanInfo.BasePeakIntensity);
                else
                    intensities.Add(scanInfo.TotalIonCurrent);

                scanTimes.Add(scanInfo.RetentionTime);
            }
        }

        /// <summary>
        /// Parse out the parent ion from filterText
        /// </summary>
        /// <param name="filterText"></param>
        /// <param name="parentIonMz">Parent ion m/z (output)</param>
        /// <param name="collisionMode">Collision mode</param>
        /// <returns>True if success</returns>
        public static bool ExtractParentIonMZFromFilterText(string filterText, out double parentIonMz, out string collisionMode)
        {
            var success = XRawFileIO.ExtractParentIonMZFromFilterText(filterText, out parentIonMz, out _, out collisionMode);

            return success;
        }

    }
}
