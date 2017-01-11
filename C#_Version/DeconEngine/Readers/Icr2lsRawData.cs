#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using DeconToolsV2.Readers;

namespace Engine.Readers
{
    [Obsolete("Only used by Icr2lsRawData, which has fallen out of use", false)]
    internal enum Icr2lsFileType
    {
        ICR2LS_TIME = 1,
        ICR2LS_FREQ,
        ICR2LS_MASS
    };

    [Obsolete("Only used by Icr2lsRawData, which has fallen out of use", false)]
    internal enum Icr2lsDataType
    {
        ICR2LS_INT_NO_HEADER = 1,
        ICR2LS_FLT_SUN_EXTREL,
        ICR2LS_HEADER
    };

    [Obsolete("Used only rarely by DeconTools for ICR2LS data files, which are rarely used.", false)]
    internal class Icr2lsRawData : RawData
    {
        private Icr2lsFileType menm_file_type;
        private Icr2lsDataType menm_data_type;
        private double mdbl_sample_rate;
        private double mdbl_low_mass_freq;
        private double mdbl_freq_shift;
        private int mint_num_segments;
        private double mdbl_max_point;
        private ApodizationType menmApodizationType;
        private int mint_apodization_apex_percent;
        private double mdbl_apodization_min_x;
        private double mdbl_apodization_max_x;
        private short mshort_num_zeros;
        private short mshort_num_zeros_in_read_data;

        private CalibrationType menm_calibration_type;
        //      ' FTMS cal types:
        //     '   1  m/z = A/f + |Vt|B/f^2
        //     '   3  m/z = A/f + |Vt|B/f^2 + C
        //     '   2  m/z = A/f + |Vt|B/f^2 + I|Vt|C/f^2
        private double mdbl_calib_const_a;
        private double mdbl_calib_const_b;
        private double mdbl_calib_const_c;
        private double mdbl_intensity_calibration;

        private string marr_file_name;
        private float[] mptr_data;
        private float[] mptr_data_copy;
        private int mint_last_scan_num;
        private int mint_num_points_in_scan;

        private int mint_allocated_size;
        private double mdbl_signal_range;
        private bool mbln_tic_file;

        private bool mbln_use_specified_calibration;

        SortedDictionary<int, string> mmap_files_names = new SortedDictionary<int, string>();

        public override FileType GetFileType()
        {
            return FileType.ICR2LSRAWDATA;
        }

        public double GetSampleRate()
        {
            return mdbl_sample_rate;
        }

        public override int GetNumScansLoaded()
        {
            return GetNumScans();
        }

        public override int GetParentScan(int scan_num)
        {
            //future work
            return 0;
        }

        public override bool IsMSScan(int scan_num)
        {
            //future work
            return true;
        }

        public override double GetParentMz(int scan_num)
        {
            //future work
            return 0;
        }

        public override bool IsProfileScan(int scan_num)
        {
            //future work
            return true;
        }

        public override int GetMSLevel(int scan_num)
        {
            return 1;
        }

        public override bool IsFTScan(int scanNum)
        {
            return true;
        }

        public void SetApodizationZeroFillOptions(ApodizationType type, double min_x, double max_x,
            int apodization_percent,
            short num_zeros)
        {
            menmApodizationType = type;
            mdbl_apodization_min_x = min_x;
            mdbl_apodization_max_x = max_x;
            mint_apodization_apex_percent = apodization_percent;
            mshort_num_zeros = num_zeros;
        }

        public void GetFTICRTransient(ref List<float> vect_intensities)
        {
            // first reload current data.
            string file_name;
            const int flt_size = sizeof (float);
            GetFileName(mint_last_scan_num, out file_name);
            if (string.IsNullOrWhiteSpace(file_name))
                return;
            bool loaded = LoadFile(file_name, mint_last_scan_num);
            if (!loaded)
                return;
            Buffer.BlockCopy(mptr_data, 0, mptr_data_copy, 0, mint_allocated_size * flt_size);
            for (int pt_num = 0; pt_num < mint_allocated_size; pt_num++)
            {
                vect_intensities.Add(mptr_data_copy[pt_num]);
            }
        }

        private bool LoadFile(string f_name, int scan_num)
        {
            const int flt_size = sizeof (float);
            using (FileStream fin = new FileStream(f_name, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                //std.ifstream fin(f_name, std.ios.binary);
                bool found_data = ReadHeader(fin);
                if (!found_data)
                {
                    fin.Close();
                    return false;
                }
                long pos = fin.Position;
                if (mint_allocated_size < mint_num_points_in_scan)
                {
                    mint_allocated_size = mint_num_points_in_scan;
                    if (mptr_data != null)
                    {
                        mptr_data = null;
                        mptr_data_copy = null;
                    }
                    mptr_data = new float[mint_num_points_in_scan];
                    mptr_data_copy = new float[mint_num_points_in_scan];
                }
                fin.ReadByte();
                pos = fin.Position;
                fin.Flush();
                fin.Seek(pos, SeekOrigin.Begin);
                byte[] buffer = new byte[mint_num_points_in_scan * flt_size];
                int num_read = fin.Read(buffer, 0, mint_num_points_in_scan * flt_size);
                //Buffer.BlockCopy(buffer, 0, mptr_data, 0, mint_num_points_in_scan * flt_size);
                Buffer.BlockCopy(buffer, 0, mptr_data, 0, num_read);
            }
            double max_intensity = -1 * double.MaxValue;
            double min_intensity = double.MaxValue;
            const int interval_size = 2000;
            int skip = (mint_num_points_in_scan - 1) / interval_size + 1;
            bool max_side = true; // to take reading from max size.
            for (int i = 0; i < mint_num_points_in_scan; i += skip)
            {
                double current_max_intensity = -1 * double.MaxValue;
                double current_min_intensity = double.MaxValue;

                for (int j = i; j < mint_num_points_in_scan && j < i + skip; j++)
                {
                    if (current_max_intensity < mptr_data[j])
                        current_max_intensity = mptr_data[j];
                    if (current_min_intensity > mptr_data[j])
                        current_min_intensity = mptr_data[j];
                }
                double current_intensity;

                if (max_side)
                {
                    current_intensity = current_max_intensity;
                    max_side = false;
                }
                else
                {
                    current_intensity = current_min_intensity;
                    max_side = true;
                }

                if (max_intensity < current_intensity)
                    max_intensity = current_intensity;
                if (min_intensity > current_intensity)
                    min_intensity = current_intensity;
            }
            mdbl_signal_range = (max_intensity - min_intensity);

            if (mbln_tic_file)
                menm_calibration_type = (CalibrationType) 5;

            if (menmApodizationType != ApodizationType.NOAPODIZATION)
                Engine.Utilities.Apodization.Apodize(mdbl_apodization_min_x, mdbl_apodization_max_x,
                    mdbl_sample_rate, false, menmApodizationType, mptr_data, mint_num_points_in_scan,
                    mint_apodization_apex_percent);

            if (mshort_num_zeros != 0)
            {
                mint_allocated_size = mint_num_points_in_scan * (1 << mshort_num_zeros);
                float[] temp = new float[mint_allocated_size];
                Buffer.BlockCopy(mptr_data, 0, temp, 0, mint_num_points_in_scan * sizeof (float));
                for (int zeroIndex = mint_num_points_in_scan;
                    zeroIndex < mint_allocated_size;
                    zeroIndex++)
                {
                    temp[zeroIndex] = 0;
                }
                if (mptr_data != null)
                {
                    mptr_data = null;
                    mptr_data_copy = null;
                }
                mptr_data = temp;
                mptr_data_copy = new float[mint_allocated_size];
            }

            if (!mbln_use_specified_calibration || mobj_calibrator == null)
            {
                Calibrations.Calibrator calib = new Calibrations.Calibrator(menm_calibration_type);
                calib.NumPointsInScan = mint_num_points_in_scan;
                calib.LowMassFrequency = mdbl_low_mass_freq;
                calib.SampleRate = mdbl_sample_rate;

                //[gord] this hack is meant to reverse the sign of calibrationConstantB, resulting in the correct
                //m/z calculation for CalibrationType 9
                if (menm_calibration_type == (CalibrationType) 9)
                {
                    mdbl_calib_const_b = -1 * mdbl_calib_const_b;
                }

                calib.SetCalibrationEquationParams(mdbl_calib_const_a, mdbl_calib_const_b, mdbl_calib_const_c);
                SetCalibrator(calib);
            }
            else
            {
                mobj_calibrator.NumPointsInScan = mint_num_points_in_scan;
                mobj_calibrator.LowMassFrequency = mdbl_low_mass_freq;
                mobj_calibrator.SampleRate = mdbl_sample_rate;
            }

            return true;
        }

        // once this calibration function is called, the default calibration is
        // overridden.
        public virtual void OverrideDefaultCalibrator(CalibrationType calibType, double A, double B, double C)
        {
            mbln_use_specified_calibration = true;
            menm_calibration_type = calibType;
            Calibrations.Calibrator calib = new Calibrations.Calibrator(menm_calibration_type);
            calib.NumPointsInScan = mint_num_points_in_scan;
            calib.LowMassFrequency = mdbl_low_mass_freq;
            calib.SampleRate = mdbl_sample_rate;
            mdbl_calib_const_a = A;
            mdbl_calib_const_b = B;
            mdbl_calib_const_c = C;
            calib.SetCalibrationEquationParams(mdbl_calib_const_a, mdbl_calib_const_b, mdbl_calib_const_c);
            SetCalibrator(calib);
        }

        public void SetIsTic(bool is_tic)
        {
            mbln_tic_file = is_tic;
        }

        /*void GetLine(Stream fin, ref char[] buffer, int max_size)
        {
            int copy_pt = 0;
            char copy_char;
            bool start_copy = false;
            while (copy_pt < max_size - 1 && (copy_char = (char) fin.ReadByte()) != '\n')
            {
                if (Char.IsLetter(copy_char))
                    start_copy = true;
                if (start_copy)
                    buffer[copy_pt++] = copy_char;
            }
            buffer[copy_pt] = '\0';
            return;
        }*/

        private bool ReadHeader(Stream fin)
        {
            string buffer = "";

            const string comment_tag = "Comment:";
            const string commentend_tag = "CommentEnd";
            const string end_tag = "End";
            const string file_type_tag = "FileType:";
            const string data_type_tag = "DataType:";
            const string num_samples_tag = "NumberOfSamples:";
            const string sample_rate_tag = "SampleRate:";
            const string low_mass_freq_tag = "LowMassFreq:";
            const string freq_shift_tag = "FreqShift:";
            const string number_of_segments_tag = "NumberSegments:";
            const string max_point_tag = "MaxPoint:";
            const string cal_type_tag = "CalType:";
            const string cal_a_tag = "CalA:";
            const string cal_b_tag = "CalB:";
            const string cal_c_tag = "CalC:";
            const string intensity_tag = "Intensity:";
            const string zero_fills_tag = "ZeroFills:";
            const string xmin_tag = "CurrentXmin:";
            const string xmax_tag = "CurrentXmax:";

            using (StreamReader afin = new StreamReader(fin, Encoding.ASCII, true, 128, true))
            {
                while (!afin.EndOfStream)
                {
                    buffer = afin.ReadLine();
                    if (string.IsNullOrWhiteSpace(buffer) || buffer.ToLower().StartsWith(end_tag.ToLower()))
                    {
                        break;
                    }
                    if (buffer.ToLower().StartsWith(comment_tag.ToLower()))
                    {
                        while (!afin.EndOfStream)
                        {
                            buffer = afin.ReadLine();
                            if (string.IsNullOrWhiteSpace(buffer) ||
                                buffer.ToLower().StartsWith(commentend_tag.ToLower()))
                            {
                                break;
                            }
                        }
                        if (string.IsNullOrWhiteSpace(buffer))
                        {
                            continue;
                        }
                    }
                    int ptr = -1;
                    if ((ptr = buffer.IndexOf(file_type_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += file_type_tag.Length;
                        menm_file_type = (Icr2lsFileType) int.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(data_type_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += data_type_tag.Length;
                        menm_data_type = (Icr2lsDataType) int.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(num_samples_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += num_samples_tag.Length;
                        mint_num_points_in_scan = int.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(sample_rate_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += sample_rate_tag.Length;
                        mdbl_sample_rate = double.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(low_mass_freq_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += low_mass_freq_tag.Length;
                        mdbl_low_mass_freq = double.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(freq_shift_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += freq_shift_tag.Length;
                        mdbl_freq_shift = double.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(number_of_segments_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        //forgeddd abouddd idd!!
                    }
                    if ((ptr = buffer.IndexOf(max_point_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        //forgeddd abouddd idd!!
                    }
                    if ((ptr = buffer.IndexOf(cal_type_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += cal_type_tag.Length;
                        int cal_num = int.Parse(buffer.Substring(ptr));
                        menm_calibration_type = (CalibrationType) cal_num;
                    }
                    if ((ptr = buffer.IndexOf(cal_a_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += cal_a_tag.Length;
                        mdbl_calib_const_a = double.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(cal_b_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += cal_b_tag.Length;
                        mdbl_calib_const_b = double.Parse(buffer.Substring(ptr));
                        // somehow, because of a nice quirk in the s/w, this is stored negatively.

                        mdbl_calib_const_b = -1 * mdbl_calib_const_b;
                            //Gord says:  this might be the source of the discrepancy
                        // for CalibrationType 9; between ICR2LS and DeconTools!
                        // See the LoadFile method and the adjustment made downstream of this method call
                    }
                    if ((ptr = buffer.IndexOf(cal_c_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += cal_c_tag.Length;
                        mdbl_calib_const_c = double.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(intensity_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += intensity_tag.Length;
                        mdbl_intensity_calibration = double.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(zero_fills_tag, StringComparison.InvariantCulture)) > -1)
                    {
                        ptr += zero_fills_tag.Length;
                        mshort_num_zeros_in_read_data = (short) int.Parse(buffer.Substring(ptr));
                    }
                    if ((ptr = buffer.IndexOf(xmin_tag, StringComparison.InvariantCulture)) > -1)
                    {
                    }
                    if ((ptr = buffer.IndexOf(xmax_tag, StringComparison.InvariantCulture)) > -1)
                    {
                    }
                }
            }
            return true;
        }

        private bool LoadFile(int scan_num)
        {
            string file_name;
            GetFileName(scan_num, out file_name);
            LoadFile(file_name, scan_num);
            return true;
        }

        private void GetFileName(int scan_num, out string temp_path)
        {
            temp_path = "";
            if (mmap_files_names.ContainsKey(scan_num))
            {
                temp_path = mmap_files_names[scan_num];
            }
        }

        public override bool IsZoomScan(int scan_num)
        {
            return false;
        }

        // Note that Centroid is ignored by this class
        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid)
        {
            int num_pts = mint_num_points_in_scan * (1 << mshort_num_zeros);
            return GetRawData(out mzs, out intensities, scan_num, centroid, num_pts);
        }

        public override double GetScanTime(int scan_num)
        {
            return scan_num;
        }

        public override double GetSignalRange(int scan_num, bool centroid)
        {
            // only returns a value if the current scan is the one we are asking for.
            if (mint_last_scan_num == scan_num)
                return mdbl_signal_range;
            return 0;
        }

        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();
            SetIsTic(true); // don't ask why, in fact..forgedd abouddd idddd!!
            string file_name;
            const int flt_size = sizeof (float);
            int scan_num = 0; //as the tic is atored as scan 0

            GetFileName(scan_num, out file_name);
            if (string.IsNullOrWhiteSpace(file_name))
                return;
            mint_last_scan_num = scan_num;
            bool loaded = LoadFile(file_name, scan_num);
            int num_pts = mint_num_points_in_scan * (1 << mshort_num_zeros);
            Buffer.BlockCopy(mptr_data, 0, mptr_data_copy, 0, num_pts * flt_size);
            mobj_calibrator.GetRawPointsApplyFFT(ref mptr_data, out scan_times, out intensities, num_pts);
        }

        // Note that Centroid is ignored by this class
        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts)
        {
            string file_name;
            mzs = new List<double>();
            intensities = new List<double>();
            const int flt_size = sizeof (float);

            GetFileName(scan_num, out file_name);
            if (string.IsNullOrWhiteSpace(file_name))
                return false;

            if (scan_num != mint_last_scan_num)
            {
                mint_last_scan_num = scan_num;
                bool loaded = LoadFile(file_name, scan_num);
                if (num_pts <= 0 || num_pts > mint_num_points_in_scan * (1 << mshort_num_zeros))
                    num_pts = mint_num_points_in_scan * (1 << mshort_num_zeros);
                if (!loaded)
                    return false;
                Buffer.BlockCopy(mptr_data, 0, mptr_data_copy, 0,
                    mint_num_points_in_scan * (1 << mshort_num_zeros) * flt_size);
            }
            else
            {
                if (num_pts <= 0 || num_pts > mint_num_points_in_scan * Math.Pow((float) 2, (float) mshort_num_zeros))
                    num_pts = mint_num_points_in_scan * (1 << mshort_num_zeros);
                Buffer.BlockCopy(mptr_data_copy, 0, mptr_data, 0, num_pts);
            }

            mobj_calibrator.GetRawPointsApplyFFT(ref mptr_data, out mzs, out intensities, num_pts);
            return true;
        }

        private void AddFilesInDir(string directory_path, string folder_name)
        {
            string file_name;
            string directory_name;
            // found a directory. If its of the form sxxx then get the files from it.
            if ((folder_name[0] == 'S' || folder_name[0] == 's') &&
                (folder_name[1] >= '0' && folder_name[1] <= '9') &&
                (folder_name[2] >= '0' && folder_name[2] <= '9') &&
                (folder_name[3] >= '0' && folder_name[3] <= '9'))
            {
                directory_name = directory_path;
                int len = directory_name.Length;
                if (directory_name[len - 1] != '\\' && directory_name[len - 1] != '/')
                {
                    directory_name += "\\";
                }
                directory_name += folder_name;
                len = directory_name.Length;
                if (directory_name[len - 1] != '\\' && directory_name[len - 1] != '/')
                {
                    directory_name += "\\";
                }

                // Skip subdirectories
                foreach (string path in Directory.EnumerateFiles(directory_name))
                {
                    file_name = Path.GetFileName(path);
                    AddScanFile(directory_name, file_name);
                }
            }
            return;
        }

        private void AddScanFile(string path, string file_name)
        {
            // add file to list of files. Extract scan number by starting from last letter
            // of file_name.
            int len_filen = file_name.Length;
            int len_pathn = path.Length;
            string full_path;
            int start_index = len_filen;
            for (int i = len_filen - 1; i >= 0 && file_name[i] >= '0' && file_name[i] <= '9'; i--)
            {
                start_index = i;
            }
            if (start_index != len_filen && file_name[start_index - 1] == '.')
            {
                // have the right format.
                int scan_num = int.Parse(file_name.Substring(start_index));
                full_path = path;

                if (full_path[len_pathn - 1] != '\\' && full_path[len_pathn] != '/')
                    full_path += "\\";
                full_path += file_name;
                AddScanFile(full_path, scan_num);
            }
            else
                AddScanFile(file_name, 0); //for the TIC
        }

        private void AddScanFile(string file_name)
        {
            // add file to list of files. Extract scan number by starting from last letter
            // of file_name.
            int len_filen = file_name.Length;
            int start_index = len_filen;
            for (int i = len_filen - 1; i >= 0 && file_name[i] >= '0' && file_name[i] <= '9'; i--)
            {
                start_index = i;
            }
            if (start_index != len_filen && file_name[start_index - 1] == '.')
            {
                // have the right format.
                int scan_num = int.Parse(file_name.Substring(start_index));
                AddScanFile(file_name, scan_num);
            }
            else
                AddScanFile(file_name, 0);
        }

        public void AddScanFile(string file_name, int scan_num)
        {
            mmap_files_names.Add(scan_num, file_name);
        }

        public override void Load(string directory_path)
        {
            string sfile_name;
            string file_name;

            if (!IsDir(directory_path))
            {
                AddScanFile(directory_path);
                return;
            }

            sfile_name = directory_path;
            int len = directory_path.Length;

            if (directory_path[len - 1] != '\\' && directory_path[len - 1] != '/')
                sfile_name += "\\*";
            else
                sfile_name += "*";

            foreach (string path in Directory.EnumerateFileSystemEntries(directory_path))
            {
                file_name = path;
                if (file_name == "." || file_name == "..")
                {
                    // current or above directory. Do nothing.
                }
                else if (Directory.Exists(path))
                {
                    // Add files in directory to the list of files to process.
                    AddFilesInDir(directory_path, file_name);
                }
                else
                {
                    AddScanFile(directory_path, file_name);
                }
            }
            // get first file and call load on it.
            return;
        }

        public Icr2lsRawData()
        {
            mptr_data = null;
            mptr_data_copy = null;
            mint_allocated_size = 0;
            marr_file_name = "";
            mint_last_scan_num = -1;
            mint_num_points_in_scan = 0;
            mint_allocated_size = 0;
            mbln_tic_file = false;
            menmApodizationType = ApodizationType.NOAPODIZATION;
            mshort_num_zeros = 0;
            mbln_use_specified_calibration = false;
        }

        public override void GetScanDescription(int scan, out string description)
        {
            description = "Scan #" + scan;
        }

        ~Icr2lsRawData()
        {
        }

        public override string GetFileName()
        {
            return marr_file_name;
        }

        public override int GetScanSize()
        {
            return this.mint_num_points_in_scan;
        }

        public override int GetNumScans()
        {
            if (mmap_files_names.Count == 0)
                return 0;
            return mmap_files_names.Last().Key;
        }

        public override int GetNextScanNum(int current_scan_num)
        {
            return mmap_files_names.Select(x => x.Key).Where(x => x > current_scan_num).DefaultIfEmpty(-1).First();
        }

        public override int GetFirstScanNum()
        {
            if (mmap_files_names.Count == 0)
            {
                throw new System.Exception("No Scans loaded.");
            }
            return mmap_files_names.First().Key;
        }

        public override int GetLastScanNum()
        {
            if (mmap_files_names.Count == 0)
            {
                throw new System.Exception("No Scans loaded.");
            }
            return mmap_files_names.Last().Key;
        }
    }
}
#endif