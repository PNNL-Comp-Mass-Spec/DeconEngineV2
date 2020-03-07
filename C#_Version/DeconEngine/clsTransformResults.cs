using System;
using System.Collections.Generic;
using Engine.Results;

namespace DeconToolsV2.Results
{

    /// <summary>
    /// Transform results
    /// </summary>
    /// <remarks>Used by Decon2LS.UI and by PeakImporter (from peak.dat files)</remarks>
    public class clsTransformResults
    {
        private Readers.FileType menmFileType = Readers.FileType.UNDEFINED;
        internal LCMSTransformResults mobj_lcms_results;


        public string FileName => mobj_lcms_results.GetFileName();

        public Readers.FileType FileType
        {
            get => menmFileType;
            set => menmFileType = value;
        }

        public float[] MonoMasses
        {
            get
            {
                var numDeisotoped = mobj_lcms_results.GetNumTransforms();
                var mono_masses = new float[numDeisotoped];
                for (var index = 0; index < numDeisotoped; index++)
                {
                    var fitRecord = mobj_lcms_results.GetIsoPattern(index);
                    mono_masses[index] = (float) fitRecord.MonoMw;
                }
                return mono_masses;
            }
        }

        public short[] Charges
        {
            get
            {
                var numDeisotoped = mobj_lcms_results.GetNumTransforms();
                var charges = new short[numDeisotoped];
                for (var index = 0; index < numDeisotoped; index++)
                {
                    var fitRecord = mobj_lcms_results.GetIsoPattern(index);
                    charges[index] = (short)fitRecord.ChargeState;
                }
                return charges;
            }
        }

        public float[] Scans
        {
            get
            {
                var numDeisotoped = mobj_lcms_results.GetNumTransforms();
                var scans = new float[numDeisotoped];
                for (var index = 0; index < numDeisotoped; index++)
                {
                    var fitRecord = mobj_lcms_results.GetIsoPattern(index);
                    scans[index] = fitRecord.ScanNum;
                }
                return scans;
            }
        }

        public clsTransformResults()
        {
            mobj_lcms_results = null;
        }

        internal void SetLCMSTransformResults(LCMSTransformResults results)
        {
            mobj_lcms_results = results;
        }

        public int GetMinScan()
        {
            if (mobj_lcms_results == null)
                return -1;
            return mobj_lcms_results.GetMinScan();
        }

        public int GetMaxScan()
        {
            if (mobj_lcms_results == null)
                return -1;
            return mobj_lcms_results.GetMaxScan();
        }

        public bool IsDeisotoped()
        {
            if (mobj_lcms_results == null)
                throw new Exception("No results stored.");
            return mobj_lcms_results.IsDeisotoped();
        }

#if Enable_Obsolete
        [Obsolete("Use GetRawDataSortedInIntensity that works on Engine.Results.LcmsPeak")]
        public void GetRawDataSortedInIntensity(out clsLCMSPeakOld[] lcms_peaks)
        {
            LcmsPeak[] lcmsPeaks;

            GetRawDataSortedInIntensity(out lcmsPeaks);

            lcms_peaks = ConvertLcmsPeaksToLCMSPeaks(lcmsPeaks);
        }
#endif

        public void GetRawDataSortedInIntensity(out LcmsPeak[] lcmsPeaks)
        {
            if (mobj_lcms_results == null)
            {
                lcmsPeaks = null;
                return;
            }

            var vectPeaks = new List<LcmsPeak>();
            var num_peaks = mobj_lcms_results.GetNumPeaks();
            vectPeaks.Capacity = num_peaks;
            mobj_lcms_results.GetAllPeaks(out vectPeaks);

            /*bool RawPeaksIntensityComparison(Engine.Results.LCMSPeak pk1,
                Engine.Results.LCMSPeak pk2)
            {
                if (pk1.mdbl_intensity < pk2.mdbl_intensity)
                    return true;
                if (pk1.mdbl_intensity > pk2.mdbl_intensity)
                    return false;
                return pk1.mdbl_mz < pk2.mdbl_mz;
            }*/
            // Used by the sort algorithm to sort List of peaks in descending order of mdbl_intensity.
            vectPeaks.Sort((x, y) =>
            {
                var result = y.Intensity.CompareTo(x.Intensity);
                if (result == 0)
                {
                    result = y.Mz.CompareTo(x.Mz);
                }
                return result;
            });

            lcmsPeaks = new LcmsPeak[num_peaks];

            for (var pk_num = 0; pk_num < num_peaks; pk_num++)
            {
                var pk = vectPeaks[pk_num];
                lcmsPeaks[pk_num] = new LcmsPeak(pk.ScanNum, pk.Mz, pk.Intensity);
            }
        }

#if Enable_Obsolete
        [Obsolete("Use GetRawDataSortedInIntensity that works on Engine.Results.LcmsPeak")]
        public void GetRawData(out clsLCMSPeakOld[] lcms_peaks)
        {
            LcmsPeak[] lcmsPeaks;

            GetRawDataSortedInIntensity(out lcmsPeaks);

            lcms_peaks = ConvertLcmsPeaksToLCMSPeaks(lcmsPeaks);
        }
#endif

        public void GetRawData(out LcmsPeak[] lcmsPeaks)
        {
            if (mobj_lcms_results == null)
            {
                lcmsPeaks = null;
                return;
            }

            var num_peaks = mobj_lcms_results.GetNumPeaks();
            lcmsPeaks = new LcmsPeak[num_peaks];

            for (var pk_num = 0; pk_num < num_peaks; pk_num++)
            {
                var pk = mobj_lcms_results.GetPeak(pk_num);
                lcmsPeaks[pk_num] = new LcmsPeak(pk.ScanNum, pk.Mz, pk.Intensity);
            }
        }

        public void GetSIC(int min_scan, int max_scan, float mz, float mz_tolerance, out float[] peak_intensities)
        {
            List<double> vect_intensities;
            mobj_lcms_results.GetSIC(min_scan, max_scan, mz - mz_tolerance, mz + mz_tolerance, out vect_intensities);
            var num_scans = max_scan - min_scan + 1;
            peak_intensities = new float[num_scans];

            for (var scan_num = min_scan; scan_num <= max_scan; scan_num++)
            {
                peak_intensities[scan_num - min_scan] = (float)vect_intensities[scan_num - min_scan];
            }
        }

        public void GetScanPeaks(int scan_num, out float[] peak_mzs, out float[] peak_intensities)
        {
            List<double> vect_mzs;
            List<double> vect_intensities;

            mobj_lcms_results.GetScanPeaks(scan_num, out vect_mzs, out vect_intensities);

            var num_pts = vect_intensities.Count;
            peak_mzs = new float[num_pts];
            peak_intensities = new float[num_pts];

            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                peak_mzs[pt_num] = (float)vect_mzs[pt_num];
                peak_intensities[pt_num] = (float)vect_intensities[pt_num];
            }
        }

        public int GetNumPeaks()
        {
            return mobj_lcms_results.GetNumPeaks();
        }

        [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files)", false)]
        public void ReadResults(string fileName)
        {
            if (mobj_lcms_results == null)
            {
                mobj_lcms_results = new LCMSTransformResults();
            }

            mobj_lcms_results.LoadResults(fileName);
        }

        [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files)", false)]
        public void WriteResults(string fileName, bool save_signal_range)
        {
            mobj_lcms_results.SaveResults(fileName, save_signal_range);
        }

        public void WriteScanResults(string fileName)
        {
#pragma warning disable 618
            if (menmFileType != Readers.FileType.ICR2LSRAWDATA)
#pragma warning restore 618
                mobj_lcms_results.SaveScanResults(fileName, false);
            else
                mobj_lcms_results.SaveScanResults(fileName, true);
        }

#if Enable_Obsolete
        [Obsolete("Conversion class; only used by obsolete methods")]
        private clsLCMSPeakOld[] ConvertLcmsPeaksToLCMSPeaks(IReadOnlyList<LcmsPeak> lcmsPeaks)
        {
            var lcms_peaks = new clsLCMSPeakOld[lcmsPeaks.Count];

            for (var i = 0; i < lcmsPeaks.Count; i++)
            {
                lcms_peaks[i] = new clsLCMSPeakOld(lcmsPeaks[i].ScanNum, lcmsPeaks[i].Mz, lcmsPeaks[i].Intensity);
            }
            return lcms_peaks;
        }
#endif

    }
}
