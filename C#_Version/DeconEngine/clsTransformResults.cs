#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using Engine.Results;

namespace DeconToolsV2.Results
{
    [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files)", false)]
    public class clsTransformResults
    {
        private int mint_percent_done;
        private DeconToolsV2.Readers.FileType menmFileType = DeconToolsV2.Readers.FileType.UNDEFINED;
        internal Engine.Results.LCMSTransformResults mobj_lcms_results;


        public string FileName
        {
            get { return mobj_lcms_results.GetFileName(); }
        }
        
        public DeconToolsV2.Readers.FileType FileType
        {
            get { return menmFileType; }
            set { menmFileType = value; }
        }

        public float[] MonoMasses
        {
            get
            {
                int numDeisotoped = mobj_lcms_results.GetNumTransforms();
                float[] mono_masses = new float[numDeisotoped];
                for (int index = 0; index < numDeisotoped; index++)
                {
                    Engine.HornTransform.IsotopeFitRecord fitRecord = mobj_lcms_results.GetIsoPattern(index);
                    mono_masses[index] = (float) fitRecord.MonoMw;
                }
                return mono_masses;
            }
        }

        public short[] Charges
        {
            get
            {
                int numDeisotoped = mobj_lcms_results.GetNumTransforms();
                short[] charges = new short[numDeisotoped];
                for (int index = 0; index < numDeisotoped; index++)
                {
                    Engine.HornTransform.IsotopeFitRecord fitRecord = mobj_lcms_results.GetIsoPattern(index);
                    charges[index] = (short)fitRecord.ChargeState;
                }
                return charges;
            }
        }

        public float[] Scans
        {
            get
            {
                int numDeisotoped = mobj_lcms_results.GetNumTransforms();
                float[] scans = new float[numDeisotoped];
                for (int index = 0; index < numDeisotoped; index++)
                {
                    Engine.HornTransform.IsotopeFitRecord fitRecord = mobj_lcms_results.GetIsoPattern(index);
                    scans[index] = (float) fitRecord.ScanNum;
                }
                return scans;
            }
        }

        public clsTransformResults()
        {
            mobj_lcms_results = null;
        }

        internal void SetLCMSTransformResults(Engine.Results.LCMSTransformResults results)
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
                throw new System.Exception("No results stored.");
            return mobj_lcms_results.IsDeisotoped();
        }

        public void GetRawDataSortedInIntensity(ref DeconToolsV2.Results.clsLCMSPeak[] lcms_peaks)
        {
            if (mobj_lcms_results == null)
            {
                lcms_peaks = null;
                return;
            }

            List<Engine.Results.LcmsPeak> vectPeaks = new List<LcmsPeak>();
            int num_peaks = mobj_lcms_results.GetNumPeaks();
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
                int result = y.Intensity.CompareTo(x.Intensity);
                if (result == 0)
                {
                    result = y.Mz.CompareTo(x.Mz);
                }
                return result;
            });

            lcms_peaks = new DeconToolsV2.Results.clsLCMSPeak[num_peaks];
            int min_scan = mobj_lcms_results.GetMinScan();
            int max_scan = mobj_lcms_results.GetMaxScan();
            Engine.Results.LcmsPeak pk;

            for (int pk_num = 0; pk_num < num_peaks; pk_num++)
            {
                mint_percent_done = (pk_num * 100) / num_peaks;
                pk = vectPeaks[pk_num];
                lcms_peaks[pk_num] = new clsLCMSPeak(pk.ScanNum, (float)pk.Mz, (float)pk.Intensity);
            }
            mint_percent_done = 100;
        }

        public void GetRawData(ref DeconToolsV2.Results.clsLCMSPeak[] lcms_peaks)
        {
            if (mobj_lcms_results == null)
            {
                lcms_peaks = null;
                return;
            }

            int num_peaks = mobj_lcms_results.GetNumPeaks();
            lcms_peaks = new DeconToolsV2.Results.clsLCMSPeak[num_peaks];
            int min_scan = mobj_lcms_results.GetMinScan();
            int max_scan = mobj_lcms_results.GetMaxScan();
            Engine.Results.LcmsPeak pk;

            for (int pk_num = 0; pk_num < num_peaks; pk_num++)
            {
                mint_percent_done = (pk_num * 100) / num_peaks;
                pk = mobj_lcms_results.GetPeak(pk_num);
                lcms_peaks[pk_num] = new clsLCMSPeak(pk.ScanNum, (float)pk.Mz, (float)pk.Intensity);
            }
            mint_percent_done = 100;
        }
        
        public void GetSIC(int min_scan, int max_scan, float mz, float mz_tolerance, ref float[] peak_intensities)
        {
            List<double> vect_intensities = new List<double>();
            mobj_lcms_results.GetSIC(min_scan, max_scan, mz - mz_tolerance, mz + mz_tolerance, out vect_intensities);
            int num_scans = max_scan - min_scan + 1;
            peak_intensities = new float[num_scans];

            for (int scan_num = min_scan; scan_num <= max_scan; scan_num++)
            {
                peak_intensities[scan_num - min_scan] = (float)vect_intensities[scan_num - min_scan];
            }
        }
        
        public void GetScanPeaks(int scan_num, ref float[] peak_mzs, ref float[] peak_intensities)
        {
            List<double> vect_mzs = new List<double>();
            List<double> vect_intensities = new List<double>();

            mobj_lcms_results.GetScanPeaks(scan_num, out vect_mzs, out vect_intensities);

            int num_pts = vect_intensities.Count;
            peak_mzs = new float[num_pts];
            peak_intensities = new float[num_pts];

            for (int pt_num = 0; pt_num < num_pts; pt_num++)
            {
                peak_mzs[pt_num] = (float)vect_mzs[pt_num];
                peak_intensities[pt_num] = (float)vect_intensities[pt_num];
            }
        }

        public int GetNumPeaks()
        {
            return mobj_lcms_results.GetNumPeaks();
        }

        public void ReadResults(string fileName)
        {
            if (mobj_lcms_results == null)
            {
                mobj_lcms_results = new Engine.Results.LCMSTransformResults();
            }

            mobj_lcms_results.LoadResults(fileName);
        }

        public void WriteResults(string fileName, bool save_signal_range)
        {
            try
            {
                mobj_lcms_results.SaveResults(fileName, save_signal_range);
            }
            catch (Exception e)
            {
                throw e;
            }
        }

        public void WriteScanResults(string fileName)
        {
            try
            {
                if (menmFileType != DeconToolsV2.Readers.FileType.ICR2LSRAWDATA)
                    mobj_lcms_results.SaveScanResults(fileName, false);
                else
                    mobj_lcms_results.SaveScanResults(fileName, true);
            }
            catch (Exception e)
            {
                throw e;
            }
        }
    }
}
#endif