#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
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

        private IXRawfile3 m_xraw2_class;

        // This RegEx matches Full ms2, Full ms3, ..., Full ms10, Full ms11, ...
        // It also matches p ms2
        // It also matches SRM ms2
        // It also matches CRM ms3
        // It also matches Full msx ms2 (multiplexed parent ion selection, introduced with the Q-Exactive)
        private const string MS2_REGEX = "(?<ScanMode> p|Full|SRM|CRM|Full msx) ms(?<MSLevel>[2-9]|[1-9][0-9]) ";

        // This RegEx is used to extract parent ion m/z from a filter string that does not contain msx
        // ${ParentMZ} will hold the last parent ion m/z found
        // For example, 756.71 in FTMS + p NSI d Full ms3 850.70@cid35.00 756.71@cid35.00 [195.00-2000.00]
        private const string PARENTION_ONLY_NONMSX_REGEX = @"[Mm][Ss]\d*[^\[\r\n]* (?<ParentMZ>[0-9.]+)@?(?<CollisionMode>[A-Za-z]*)\d*\.?\d*(\[[^\]\r\n]\])?";

        // This RegEx is used to extract parent ion m/z from a filter string that does contain msx
        // ${ParentMZ} will hold the first parent ion m/z found (the first parent ion m/z corresponds to the highest peak)
        // For example, 636.04 in FTMS + p NSI Full msx ms2 636.04@hcd28.00 641.04@hcd28.00 654.05@hcd28.00 [88.00-1355.00]
        private const string PARENTION_ONLY_MSX_REGEX = @"[Mm][Ss]\d* (?<ParentMZ>[0-9.]+)@?(?<CollisionMode>[A-Za-z]*)\d*\.?\d*[^\[\r\n]*(\[[^\]\r\n]+\])?";

        private static readonly Regex mFindMS = new Regex(MS2_REGEX, RegexOptions.IgnoreCase | RegexOptions.Compiled);
        private static readonly Regex mFindParentIonOnlyNonMsx = new Regex(PARENTION_ONLY_NONMSX_REGEX, RegexOptions.IgnoreCase | RegexOptions.Compiled);
        private static readonly Regex mFindParentIonOnlyMsx = new Regex(PARENTION_ONLY_MSX_REGEX, RegexOptions.IgnoreCase | RegexOptions.Compiled);

        public override FileType GetFileType()
        {
            return FileType.FINNIGAN;
        }

        public override int GetNumScansLoaded()
        {
            return GetNumScans();
        }

        public string GetScanFilterString(int scanNum)
        {
            string filterString = null;
            m_xraw2_class.GetFilterForScanNum(scanNum, ref filterString);
            return filterString;
        }

        public override short GetSpectrumType(int scan_num)
        {
            var filterString = GetScanFilterString(scan_num);

            double parentIon;
            var collisionMode = "";
            if (!ExtractParentIonMZFromFilterText(filterString, out parentIon, out collisionMode) || string.IsNullOrWhiteSpace(collisionMode))
            {
                return 0;
            }
            var primaryCollisionMode = collisionMode;

            switch (primaryCollisionMode[0])
            {
                case 'c':
                    return 1;
                case 'e':
                    return 2;
                case 'h':
                    return 3;
                default:
                    return 0;
            }

            var ch_num = filterString.IndexOf('@');
            if (ch_num == -1)
            {
                // Not found - usually MS1 scans
                return 0;
            }
            ch_num++;

            //now we've reached the value after @, this is either cid, hcd or etd
            if (filterString[ch_num + 1] == 'c')
            {
                return 1;
            }
            else if (filterString[ch_num + 1] == 'e')
            {
                return 2;
            }
            else if (filterString[ch_num + 1] == 'h')
            {
                return 3;
            }

            return 0;
            //return 1;
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
            description = GetScanFilterString(scan);
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

        public int FirstSpectraNumber()
        {
            return (int) mlong_spectra_num_first;
        }

        public int LastSpectraNumber()
        {
            return (int)mlong_spectra_num_last;
        }

        public int Open(string raw_file_name)
        {
            marr_rawfileName = raw_file_name;
            MSFileReader_XRawfile temp = new MSFileReader_XRawfileClass();
            m_xraw2_class = (IXRawfile3)temp;

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

        public override double GetSignalRange(int scan_num, bool centroid)
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

            int centroidFlag = 0;
            if (centroid)
                centroidFlag = 1;

            // Warning: the masses reported by GetMassListFromScanNum when centroiding are not properly calibrated and thus could be off by 0.3 m/z or more
            //          For example, in scan 8101 of dataset RAW_Franc_Salm_IMAC_0h_R1A_18Jul13_Frodo_13-04-15, we see these values:
            //          Profile m/z         Centroid m/z	Delta_PPM
            //			112.051 			112.077			232
            //			652.3752			652.4645		137
            //			1032.56495			1032.6863		118
            //			1513.7252			1513.9168		127

            m_xraw2_class.SetCurrentController(0, 1);
            if (centroid && IsFTScan(scanN))
            {
                // Centroiding is enabled, and the dataset was acquired on an Orbitrap, Exactive, or FTMS instrument
                object massIntensityLabelsObject = null;
                object labelFlags = null;
                m_xraw2_class.GetLabelData(ref massIntensityLabelsObject, ref labelFlags, scanN);

                var massIntensityLabels = (double[,]) massIntensityLabelsObject;

                nArraySize = massIntensityLabels.GetLength(1);

                double[,] massList;

                if (nArraySize > 0)
                {
                    massList = new double[2, nArraySize];

                    for (var i = 0; i <= nArraySize - 1; i++)
                    {
                        massList[0, i] = massIntensityLabels[0, i];
                        // m/z
                        massList[1, i] = massIntensityLabels[1, i];
                        // Intensity
                    }
                }
                else
                {
                    massList = new double[0, 0];
                }
                varMassList = massList;
            }
            else
            {
                m_xraw2_class.GetMassListFromScanNum(ref scanN,
                    bstr, // no filter
                    0, // no cutoff
                    0, // no cutoff
                    0, // all peaks returned
                    centroidFlag, // do not centroid - 0 (False)
                    ref peak_width,
                    ref varMassList, // mass list data
                    ref varPeakFlags, // peak flags data
                    ref nArraySize); // size of mass list array
            }

            if (nArraySize > 0)
            {
                double[,] peaks = (double[,]) varMassList;
                double min_intensity = double.MaxValue;
                double max_intensity = double.MinValue;

                for (long j = 0; j < nArraySize; j++)
                {
                    double intensity = peaks[1, j];
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

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_points)
        {
            // Finnigan data is already truncated. Dont mess with it.
            return GetRawData(out mzs, out intensities, scan_num, centroid);
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
            int is_profile_scan = 0;
            m_xraw2_class.IsProfileScanForScanNum(scan_num, ref is_profile_scan);
            if (is_profile_scan == 1)
                return true;
            else
                return false;
        }

        public override bool IsZoomScan(int scan_num)
        {
            var filterString = GetScanFilterString(scan_num);
            if (filterString.Contains("Z ms"))
            {
                return true;
            }
            return false;
        }

        public override bool IsFTScan(int scan_num)
        {
            var bstr_filter = GetScanFilterString(scan_num);
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
            if (var_value is double)
                time = (double) var_value;
            else if (var_value is float)
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
            var filterString = GetScanFilterString(scan_num);

            var intMatchTextLength = 0;

            ms_level = 1;
            var charIndex = 0;

            var reMatchMS = mFindMS.Match(filterString);

            if (reMatchMS.Success)
            {
                ms_level = Convert.ToInt32(reMatchMS.Groups["MSLevel"].Value);
            }

            return ms_level;

            /*var pos = filterString.IndexOf("ms");
            var trunc = filterString.Substring(pos + 2);
            pos = trunc.IndexOf(' ');
            var trunc2 = trunc.Substring(0, pos);

            //search for 'ms'
            for (int chNum = 0; chNum < filterString.Length; chNum++)
            {
                if (filterString[chNum] == 'm')
                {
                    if (filterString[chNum + 1] == 's')
                    {
                        chNum = chNum + 2;
                        char ch = filterString[chNum];
                        char ch1 = filterString[chNum + 1];
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
                            //case '10': ms_level  = 10;
                            //           break;
                            //case '11': ms_level = 11;
                            //           break;
                            //case '12': ms_level = 12;
                            //           break;
                            //case '13': ms_level  = 13;
                            //           break;
                            //case '14': ms_level  = 14;
                            //           break;
                            //case '15': ms_level  = 15;
                            //           break;
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
                        return int.Parse(trunc2);
                        return ms_level;
                    }

                }
            }
            return int.Parse(trunc2);
            return ms_level;*/
        }

        public override bool IsMSScan(int scan_num)
        {
            //Returns true if the scan is a MS-level scan

            int ms_level = GetMSLevel(scan_num);
            if (ms_level == 1)
            {
                var filterString = GetScanFilterString(scan_num);

                if (filterString.Contains("SIM"))
                {
                    return false;
                }

                return true;
            }
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
            var bstr_filter = GetScanFilterString(scan_num);
            string collisionMode;

            if (ExtractParentIonMZFromFilterText(bstr_filter, out parent_mz, out collisionMode))
            {
                return parent_mz;
            }

            int ms_level = GetMSLevel(scan_num);

            int parent_count = 0;

            string ch_mz = "";
            if (ms_level == 2)
            {
                for (int chNum = 0; chNum < bstr_filter.Length; chNum++)
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

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid)
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

            int centroidFlag = 0;
            if (centroid)
                centroidFlag = 1;

            m_xraw2_class.SetCurrentController(0, 1);
            if (centroid && IsFTScan(scanN))
            {
                // Centroiding is enabled, and the dataset was acquired on an Orbitrap, Exactive, or FTMS instrument
                object massIntensityLabelsObject = null;
                object labelFlags = null;
                m_xraw2_class.GetLabelData(ref massIntensityLabelsObject, ref labelFlags, scanN);

                var massIntensityLabels = (double[,])massIntensityLabelsObject;

                nArraySize = massIntensityLabels.GetLength(1);

                double[,] massList;

                if (nArraySize > 0)
                {
                    massList = new double[2, nArraySize];

                    for (var i = 0; i <= nArraySize - 1; i++)
                    {
                        massList[0, i] = massIntensityLabels[0, i];
                        // m/z
                        massList[1, i] = massIntensityLabels[1, i];
                        // Intensity
                    }
                }
                else
                {
                    massList = new double[0, 0];
                }
                varMassList = massList;
            }
            else
            {
                m_xraw2_class.GetMassListFromScanNum(ref scanN,
                    bstr, // no filter
                    0, // no cutoff
                    0, // no cutoff
                    0, // all peaks returned
                    centroidFlag, // do not centroid - 0 (false)
                    ref peak_width,
                    ref varMassList, // mass list data
                    ref varPeakFlags, // peak flags data
                    ref nArraySize); // size of mass list array
            }

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
                ref high_mass, ref tic, ref base_peak, ref base_intensity, ref num_channels, ref unif_time, ref frequency);
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
                    if (peaks[0, j] > high_mass)
                    {
                        break;
                    }
                    double intensity = peaks[1, j];
                    if (intensity > max_intensity)
                        max_intensity = intensity;
                    if (intensity < min_intensity)
                        min_intensity = intensity;

                    mzs.Add(peaks[0, j]);
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

        /// <summary>
        /// Parse out the parent ion from filterText
        /// </summary>
        /// <param name="filterText"></param>
        /// <param name="parentIonMz">Parent ion m/z (output)</param>
        /// <param name="collisionMode">Collision mode</param>
        /// <returns>True if success</returns>
        ///  <remarks>If multiple parent ion m/z values are listed then parentIonMz will have the last one.  However, if the filter text contains "Full msx" then parentIonMz will have the first parent ion listed</remarks>
        ///  <remarks>
        ///  This was created for use in other programs that only need the parent ion m/z, and no other functions from ThermoRawFileReader.
        ///  Other projects that use this:
        ///       PHRPReader
        /// 
        ///  To copy this, take the code from this function, plus the regex strings <see cref="PARENTION_ONLY_NONMSX_REGEX"/> and <see cref="PARENTION_ONLY_MSX_REGEX"/>,
        ///  with their uses in <see cref="mFindParentIonOnlyNonMsx"/> and <see cref="mFindParentIonOnlyMsx"/>
        ///  </remarks>
        public static bool ExtractParentIonMZFromFilterText(string filterText, out double parentIonMz, out string collisionMode)
        {
            Regex matcher;
            if (filterText.ToLower().Contains("msx"))
            {
                matcher = mFindParentIonOnlyMsx;
            }
            else
            {
                matcher = mFindParentIonOnlyNonMsx;
            }

            var match = matcher.Match(filterText);
            if (match.Success)
            {
                collisionMode = "";
                var collisionMatch = match.Groups["CollisionMode"];
                if (collisionMatch != null && !string.IsNullOrWhiteSpace(collisionMatch.Value))
                {
                    collisionMode = collisionMatch.Value;
                }

                var parentIonMzText = match.Groups["ParentMZ"].Value;

                var success = double.TryParse(parentIonMzText, out parentIonMz);
                return success;
            }

            parentIonMz = 0;
            collisionMode = "";
            return false;
        }
    }
}

#endif
#endif