#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using DeconToolsV2.Readers;
using Engine.Utilities;
//////////////////////////////////////////////////////////////////////////////
//
// FinniganRawData class
//      This class is a wrapper for the XRawFile.ocx COM control that is used
//      to open LCQ and LTQ-FT data.
//
//////////////////////////////////////////////////////////////////////////////

#if XCALIBUR_INSTALLED
using MSFileReaderLib;
//set preprocessor declaration XCALIBUR_INSTALLED if Xcalibur is installed on computer
// also copy the following files into the bin folder in DeconEngine folder :
//CFRDBResources.dll
//CFRUtil.dll
//FControl2.dll
//Fglobal.dll
//Fileio.dll
//finDB.dll
//finSSClientLib.dll
//Fregistry.dll
//xrawfile.ocx
//XRawfile2.dll

namespace Engine.Readers
{
    [Obsolete("Not used by DeconTools anymore - still exists in XCaliburRun, but that is Obsolete - do not use", false)]
    internal class FinniganRawData : RawData
    {
        private struct DataPeak
        {
            double dMass;
            double dIntensity;
        }

        private string marr_rawfileName;

        private int mlong_num_spectra;
        private int mlong_spectra_num_first;
        private int mlong_spectra_num_last;
        private int mint_last_scan_size;
        private int mint_last_scan_num;
        private double mdbl_last_scan_time;
        private double mdbl_signal_range;

        private int mint_num_points_in_scan;
        private int[] marr_data_block;
        private float[] marr_temp_data_block;

        private IXRawfile m_xraw2_class;

        public override FileType GetFileType()
        {
            return FileType.FINNIGAN;
        }

        public override int GetNumScansLoaded()
        {
            return GetNumScans();
        }

        public override short GetSpectrumType(int scan_num)
        {
            return 1;
            //      char filter_str [512];
            ////        BSTR *bstr_filter = new BSTR();
            //      _bstr_t bstr;
            //      m_xraw2_class.GetFilterForScanNum((long)scan_num, &bstr.GetBSTR());

            //      strcpy(filter_str,(char*)bstr);

            //      if (strstr(filter_str, "ms2") != null)
            //          return 2;
            //      return 1;
        }

        ~FinniganRawData()
        {
            // Let the COM object go
            if (m_xraw2_class != null)
                m_xraw2_class.Close();
        }

        public FinniganRawData()
        {
            m_xraw2_class = null;
            marr_data_block = null;
            marr_temp_data_block = null;
            mint_last_scan_size = 0;
            marr_rawfileName = "";
        }

        public override void GetScanDescription(int scan, out string description)
        {
            string bstr_filter = string.Empty;
            m_xraw2_class.GetFilterForScanNum(scan, ref bstr_filter);
            description = string.Copy(bstr_filter);
        }

        public override string GetFileName()
        {
            return marr_rawfileName;
        }

        public override int GetScanSize()
        {
            return mint_last_scan_size;
        }

        public override int GetNumScans()
        {
            return (int) mlong_num_spectra;
        }

        public override int GetFirstScanNum()
        {
            return (int) mlong_spectra_num_first;
        }

        public override int GetLastScanNum()
        {
            return (int) mlong_spectra_num_last;
        }

        public int Open(string raw_file_name)
        {
            marr_rawfileName = raw_file_name;
            MSFileReader_XRawfile temp = new MSFileReader_XRawfileClass();
            m_xraw2_class = temp;

            int err = 0;
            try
            {
                m_xraw2_class.Open(marr_rawfileName);
                m_xraw2_class.GetErrorCode(ref err);
                if (err != 0)
                {
                    throw new System.Exception("Unable to open XCalibur file: " + marr_rawfileName);
                }
            }
            catch (System.Exception e)
            {
#if DEBUG
                throw e;
#endif
                throw new System.Exception("Unable to open XCalibur file: " + marr_rawfileName);
            }

            // Get the number of spectra
            m_xraw2_class.SetCurrentController(0, 1);
            m_xraw2_class.GetNumSpectra(ref mlong_num_spectra);
            m_xraw2_class.GetFirstSpectrumNumber(ref mlong_spectra_num_first);
            m_xraw2_class.GetLastSpectrumNumber(ref mlong_spectra_num_last);

            m_xraw2_class.GetErrorCode(ref err);

            if (err != 0)
            {
                throw new System.Exception("Unable to get number of spectra from " + marr_rawfileName);
                return 1;
            }

            return 0;
        }

        public override void Close()
        {
            m_xraw2_class.Close();
        }

        public override void Load(string file_n)
        {
            Open(file_n);
        }

        public override double GetSignalRange(int scan_num)
        {
            if (scan_num == mint_last_scan_num)
                return mdbl_signal_range;

            int lastWholeNumber = 0;
            double signal_range = 0;
            object varMassList = null;
            object varPeakFlags = null;
            int nArraySize = 0;

            string bstr = "";
            double peak_width = 0;

            int scanN = scan_num;

            m_xraw2_class.SetCurrentController(0, 1);
            m_xraw2_class.GetMassListFromScanNum(ref scanN,
                bstr, // no filter
                0, // no cutoff
                0, // no cutoff
                0, // all peaks returned
                0, // do not centroid - 0 (False)
                ref peak_width,
                ref varMassList, // mass list data
                ref varPeakFlags, // peak flags data
                ref nArraySize); // size of mass list array

            if (nArraySize > 0)
            {
                double[,] peaks = (double[,]) varMassList;
                double min_intensity = double.MaxValue;
                double max_intensity = double.MinValue;

                for (long j = 0; j < nArraySize; j++)
                {
                    double intensity = peaks[j, 1];
                    if (intensity > max_intensity)
                        max_intensity = intensity;
                    if (intensity < min_intensity)
                        min_intensity = intensity;
                }

                signal_range = (max_intensity - min_intensity);
            }

            return signal_range;
        }

        public override double GetScanTime(int scan_num)
        {
            if (scan_num == mint_last_scan_num)
                return mdbl_last_scan_time;
            double start_time = 0;

            m_xraw2_class.RTFromScanNum(scan_num, ref start_time);
            return start_time;
        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, int num_points)
        {
            // Finnigan data is already truncated. Dont mess with it.
            return GetRawData(out mzs, out intensities, scan_num);
        }

        public override int GetParentScan(int scan_num)
        {
            int msN_level = GetMSLevel(scan_num);
            int level = msN_level;

            int i = 0;

            while (level >= msN_level)
            {
                level = GetMSLevel(scan_num - i);
                i++;
            }
            i--;
            return (scan_num - i);
        }

        public override bool IsProfileScan(int scan_num)
        {
            int ms_level = 0;
            m_xraw2_class.IsProfileScanForScanNum(scan_num, ref ms_level);
            if (ms_level == 1)
                return true;
            else
                return false;
        }

        public override bool IsFTScan(int scan_num)
        {
            string bstr_filter = "";
            m_xraw2_class.GetFilterForScanNum(scan_num, ref bstr_filter);
            if (bstr_filter.ToLower().IndexOf("ft", StringComparison.InvariantCulture) == 0)
            {
                return true;
            }
            return false;
        }

        public override double GetAGCAccumulationTime(int scan_num)
        {
            object var_value = null;
            m_xraw2_class.GetTrailerExtraValueForScanNum(scan_num, "Ion Injection Time (ms):", ref var_value);
            double time = 0.0;
            time = (float) var_value;
            return time;
        }

        public override double GetTICForScan(int scan_num)
        {
            int num_packets = 0;
            double start_time = 0;
            double low_mass = 0;
            double high_mass = 0;
            double tic = 0;
            double base_peak = 0;
            double base_intensity = 0;
            double frequency = 0;
            int unif_time = 0;
            int num_channels = 0;

            m_xraw2_class.GetScanHeaderInfoForScanNum(scan_num, ref num_packets, ref start_time, ref low_mass,
                ref high_mass, ref tic, ref base_peak,
                ref base_intensity, ref num_channels, ref unif_time, ref frequency);

            return tic;
        }

        public override int GetMSLevel(int scan_num)
        {
            int ms_level = 1;

            //gets the filter string
            string bstr_filter = "";
            m_xraw2_class.GetFilterForScanNum(scan_num, ref bstr_filter);

            //search for 'ms'
            for (int chNum = 0; chNum < 512; chNum++)
            {
                if (bstr_filter[chNum] == 'm')
                {
                    if (bstr_filter[chNum + 1] == 's')
                    {
                        chNum = chNum + 2;
                        char ch = bstr_filter[chNum];
                        char ch1 = bstr_filter[chNum + 1];
                        int ms = (int) ch;
                        switch (ch)
                        {
                            case '2':
                                ms_level = 2;
                                break;
                            case '3':
                                ms_level = 3;
                                break;
                            case '4':
                                ms_level = 4;
                                break;
                            case '5':
                                ms_level = 5;
                                break;
                            case '6':
                                ms_level = 6;
                                break;
                            case '7':
                                ms_level = 7;
                                break;
                            case '8':
                                ms_level = 8;
                                break;
                            case '9':
                                ms_level = 9;
                                break;
                            case '1':
                                ms_level = 0;
                                break;
                            /*  case '10': ms_level  = 10;
                                       break;
                            case '11': ms_level = 11;
                                       break;
                            case '12': ms_level = 12;
                                       break;
                            case '13': ms_level  = 13;
                                       break;
                            case '14': ms_level  = 14;
                                       break;
                            case '15': ms_level  = 15;
                                       break; */
                            case ' ':
                                ms_level = 1;
                                break;
                            default:
                                ms_level = 1;
                                break;
                        }

                        if (ms_level == 0)
                        {
                            switch (ch1)
                            {
                                case '0':
                                    ms_level = 10;
                                    break;
                                case '1':
                                    ms_level = 11;
                                    break;
                                case '2':
                                    ms_level = 12;
                                    break;
                                case '3':
                                    ms_level = 13;
                                    break;
                                case '4':
                                    ms_level = 14;
                                    break;
                                case '5':
                                    ms_level = 15;
                                    break;
                                case '6':
                                    ms_level = 16;
                                    break;
                                case '7':
                                    ms_level = 17;
                                    break;
                                case ' ':
                                    ms_level = 1;
                                    break;
                                default:
                                    ms_level = 1;
                                    break;

                            }
                        }
                        return ms_level;
                    }

                }
            }
            return ms_level;
        }

        public override bool IsMSScan(int scan_num)
        {
            //Returns true if the scan is a MS-level scan

            int ms_level = GetMSLevel(scan_num);
            if (ms_level == 1)
                return true;
            else
                return false;
        }

        public override double GetMonoMZFromHeader(int scan_num)
        {
            double mono_mz = 0;
            object var_value = null;
            m_xraw2_class.GetTrailerExtraValueForScanNum(scan_num, "Monoisotopic M/Z:", ref var_value);
            mono_mz = (double) var_value;
            return mono_mz;
        }

        public override short GetMonoChargeFromHeader(int scan_num)
        {
            short cs = 0;
            object var_value = null;
            m_xraw2_class.GetTrailerExtraValueForScanNum(scan_num, "Charge State:", ref var_value);
            cs = (short) var_value;
            return cs;
        }

        public override double GetParentMz(int scan_num)
        {
            //Return the parent m/z of the particular msN scan
            double parent_mz = 0;

            //gets the filter string
            string ch_mz = "";
            string bstr_filter = "";
            m_xraw2_class.GetFilterForScanNum(scan_num, ref bstr_filter);

            int ms_level = GetMSLevel(scan_num);

            int parent_count = 0;

            if (ms_level == 2)
            {
                for (int chNum = 0; chNum < 512; chNum++)
                {
                    if (bstr_filter[chNum] == '2')
                    {
                        chNum++;
                        int mzIndex = 0;
                        while (bstr_filter[chNum] != '@')
                        {
                            ch_mz += bstr_filter[chNum];
                            chNum++;
                            mzIndex++;
                        }
                        break;
                    }
                }

            }
            else
            {
                int chNum;
                for (chNum = 0; chNum < 512; chNum++)
                {
                    if (bstr_filter[chNum] == '@')
                    {
                        parent_count++;
                        if (parent_count <= (ms_level - 1))
                            break;
                    }
                }

                while (bstr_filter[chNum] != ' ')
                    chNum ++;

                int mzIndex = 0;
                while (bstr_filter[chNum] != '@')
                {
                    ch_mz += bstr_filter[chNum];
                    chNum++;
                    mzIndex++;
                }

            }

            parent_mz = Helpers.atof(ch_mz);

            return parent_mz;

        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num)
        {
            intensities = new List<double>();
            mzs = new List<double>();
            mint_last_scan_num = scan_num;
            int lastWholeNumber = 0;

            object varMassList = null;
            object varPeakFlags = null;
            int nArraySize = 0;

            string bstr = "";
            double peak_width = 0;

            int scanN = scan_num;

            m_xraw2_class.SetCurrentController(0, 1);
            m_xraw2_class.GetMassListFromScanNum(ref scanN,
                bstr, // no filter
                0, // no cutoff
                0, // no cutoff
                0, // all peaks returned
                0, // do not centroid - 0 (false)
                ref peak_width,
                ref varMassList, // mass list data
                ref varPeakFlags, // peak flags data
                ref nArraySize); // size of mass list array

            int num_packets = 0;
            double start_time = 0;
            double low_mass = 0;
            double high_mass = 0;
            double tic = 0;
            double base_peak = 0;
            double base_intensity = 0;
            double frequency = 0;
            int unif_time = 0;
            int num_channels = 0;

            m_xraw2_class.GetScanHeaderInfoForScanNum(scanN, ref num_packets, ref start_time, ref low_mass,
                ref high_mass, ref tic, ref base_peak,
                ref base_intensity, ref num_channels, ref unif_time, ref frequency);
            mdbl_last_scan_time = start_time;
            if (nArraySize > 0)
            {
                double[,] peaks = (double[,]) varMassList;

                if (nArraySize < (int) intensities.Capacity)
                {
                    intensities.Capacity = nArraySize;
                    mzs.Capacity = nArraySize;
                }

                double min_intensity = double.MaxValue;
                double max_intensity = double.MinValue;

                for (long j = 0; j < nArraySize; j++)
                {
                    if (peaks[j, 0] > high_mass)
                    {
                        break;
                    }
                    double intensity = peaks[j, 1];
                    if (intensity > max_intensity)
                        max_intensity = intensity;
                    if (intensity < min_intensity)
                        min_intensity = intensity;

                    mzs.Add(peaks[j, 0]);
                    intensities.Add(intensity);
                }

                mdbl_signal_range = (max_intensity - min_intensity);
            }

            mint_last_scan_size = (int) mzs.Count;
            if (nArraySize == 0)
                return false;
            return true;
        }

        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            int num_packets = 0;
            double start_time = 0;
            double low_mass = 0;
            double high_mass = 0;
            double tic = 0;
            double base_peak = 0;
            double base_intensity = 0;
            double frequency = 0;
            int unif_time = 0;
            int num_channels = 0;
            intensities = new List<double>();
            scan_times = new List<double>();

            for (int scan_num = mlong_spectra_num_first; scan_num <= mlong_spectra_num_last; scan_num++)
            {
                m_xraw2_class.GetScanHeaderInfoForScanNum(scan_num, ref num_packets, ref start_time, ref low_mass,
                    ref high_mass, ref tic, ref base_peak,
                    ref base_intensity, ref num_channels, ref unif_time, ref frequency);
                if (base_peak_tic)
                    intensities.Add(base_intensity);
                else
                    intensities.Add(tic);
                scan_times.Add(start_time);
            }
        }
    }
}

#endif
#endif