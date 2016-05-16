#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using DeconToolsV2.Readers;
using MSDataFileReader;

namespace Engine.Readers
{
    [Obsolete("Only use in Decon Tools is obsolete, use not allowed", false)]
    internal class MZXmlRawData : RawData
    {
        private string mstr_file_name;
        private int mint_num_scans;
        private int mint_current_scan;
        private double mdbl_signal_level;
        private clsSpectrumInfo mobj_CurrentScanData;
        private clsMzXMLFileAccessor mobj_MzXmlFile;

        public override short GetSpectrumType(int scan_num)
        {
            return 0;
        }

        public override void GetScanDescription(int scan, out string description)
        {
            description = "Scan #" + scan;
        }

        public override int GetNextScanNum(int current_scan_num)
        {
            return current_scan_num + 1;
        }

        public override int GetFirstScanNum()
        {
            return 1;
        }

        public override int GetLastScanNum()
        {
            return mint_num_scans;
        }

        public override int GetNumScansLoaded()
        {
            return mint_current_scan;
        }

        public MZXmlRawData()
        {
            mint_num_scans = 0;
            mint_current_scan = -1;
            mobj_MzXmlFile = null;
        }

        ~MZXmlRawData()
        {
            if (mobj_MzXmlFile != null)
            {
                Close();
                mobj_MzXmlFile = null;
            }
        }

        public override string GetFileName()
        {
            return mstr_file_name;
        }

        public override FileType GetFileType()
        {
            return FileType.MZXMLRAWDATA;
        }

        public override void Close()
        {
            mint_num_scans = 0;
            if (mobj_MzXmlFile != null)
            {
                mobj_MzXmlFile.CloseFile();
                mobj_MzXmlFile = null;
            }
        }

        public override int GetNumScans()
        {
            return mint_num_scans;
        }

        public override double GetScanTime(int scan_num)
        {
            if (mint_current_scan == scan_num)
            {
                return mobj_CurrentScanData.RetentionTimeMin;
            }
            else
            {
                clsSpectrumInfo scanData = new clsSpectrumInfo();

                // its time to read in that scan.
                mint_current_scan = scan_num;
                mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan_num, out scanData);
                return scanData.RetentionTimeMin;
            }
        }

        public override int GetScanSize()
        {
            return 0;
        }

        public override void Load(string file_name)
        {
            mstr_file_name = file_name;
            Close();
            mobj_MzXmlFile = new clsMzXMLFileAccessor();
            mobj_MzXmlFile.OpenFile(mstr_file_name);
            mobj_MzXmlFile.ReadAndCacheEntireFile();
        }

        public override int GetParentScan(int scan_num)
        {
            int scan = scan_num;
            clsSpectrumInfo scanData = new clsSpectrumInfo();
            mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan, out scanData);

            for (scan = scan_num - 1;; scan--)
            {
                mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan, out scanData);
                if (scanData.MSLevel == 1)
                    break;
            }

            return scan;
        }

        public override bool IsFTScan(int scan_num)
        {
            int scan = scan_num;
            clsSpectrumInfo scanData = new clsSpectrumInfo();
            mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan, out scanData);
            string xmlText;
            mobj_MzXmlFile.GetSourceXMLByScanNumber(scan_num, out xmlText);
            // TODO: Validate this solution
            if (xmlText.Contains("FTMS"))
            {
                return true;
            }
            else if (xmlText.Contains("ITMS"))
            {
                return false;
            }

            //default
            return false;
        }

        public override bool IsProfileScan(int scan_num)
        {
            clsSpectrumInfo scanData = new clsSpectrumInfo();
            mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan_num, out scanData);
            // TODO: Validate this solution
            //if (scanData.profileType == 'p')
            //    return true;
            //else if (scanData.profileType == 'c')
            //    return false;
            return !scanData.Centroided;

            //default
            return true;
        }

        public override int GetMSLevel(int scan_num)
        {
            clsSpectrumInfo scanData;
            mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan_num, out scanData);
            return scanData.MSLevel;
        }

        public override double GetParentMz(int scan_num)
        {
            clsSpectrumInfo scanData;
            mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan_num, out scanData);
            return scanData.ParentIonMZ;
        }

        public override bool IsMSScan(int scan_num)
        {
            int ms_level = GetMSLevel(scan_num);
            if (ms_level == 1)
                return true;
            else
                return false;
        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num)
        {
            return GetRawData(out mzs, out intensities, scan_num, -1);
        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, int num_pts)
        {
            if (scan_num > mint_num_scans)
            {
                string mesg = "File only has " + mint_num_scans + " scans. Cannot read to scan number: " + scan_num;
                throw new System.Exception(mesg);
            }

            mint_current_scan = scan_num;
            mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan_num, out mobj_CurrentScanData);

            mzs = new List<double>();
            intensities = new List<double>();

            clsSpectrumInfo scanData = new clsSpectrumInfo();
            mobj_MzXmlFile.GetSpectrumByScanNumber(scan_num, out scanData);

            int num_points = scanData.DataCount;
            if (num_pts > 0 && num_pts < num_points)
            {
                num_points = num_pts;
            }

            mzs.Capacity = num_points;
            intensities.Capacity = num_points;
            double fMass;
            double fInten;
            double max_intensity = -1 * double.MaxValue;
            double min_intensity = double.MaxValue;
            for (int i = 0; i < scanData.DataCount; i++)
            {
                fMass = scanData.MZList[i];
                fInten = scanData.IntensityList[i];
                if (fInten > max_intensity)
                    max_intensity = fInten;
                if (fInten < min_intensity) //Anoop Jan 08: was > before
                    min_intensity = fInten;
                mzs.Add(fMass);
                intensities.Add(fInten);
            }

            mdbl_signal_level = max_intensity - min_intensity;

            return true;
        }

        public override double GetSignalRange(int scan_num)
        {
            return 0;
        }

        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();

            if (mobj_MzXmlFile == null)
                return;

            bool nonzero_time_reported = false;
            for (int scan_num = 0; scan_num <= mint_num_scans; scan_num++)
            {
                // its time to read in that scan.
                mint_current_scan = scan_num;
                clsSpectrumInfo scanData = new clsSpectrumInfo();
                mobj_MzXmlFile.GetSpectrumHeaderInfoByScanNumber(scan_num, out scanData);
                scan_times.Add(scanData.RetentionTimeMin);
                if (scanData.RetentionTimeMin != 0.0)
                    nonzero_time_reported = true;
                if (base_peak_tic)
                {
                    intensities.Add(scanData.BasePeakIntensity);
                }
                else
                {
                    intensities.Add(scanData.TotalIonCurrent);
                }
            }

            // for mzxml files in which no retention times were recorded.
            if (!nonzero_time_reported)
            {
                for (int scan_num = 0; scan_num <= mint_num_scans; scan_num++)
                {
                    scan_times[scan_num] = scan_num;
                }
            }
        }
    }
}
#endif