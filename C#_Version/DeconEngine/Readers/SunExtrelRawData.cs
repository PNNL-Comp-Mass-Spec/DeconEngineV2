using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using DeconToolsV2.Readers;
using Engine.Utilities;

namespace Engine.Readers
{
    [Obsolete("Only used by SunExtrelRawData, which is obsolete", false)]
    internal class FinniganHeader
    {
        public char ByteFormat;
        public char Processor;
        public char MagicNum;
        public char FileType;
        public short RevLevel;
        public short Descriptor;
        public int SeqSize;
        public int reserved;
        public int DataSize;
        public int TrailorRecordsSize;
        public int TrailorRecordsNamesSize;
        public int ExtendedDataSize;
    }

    [Obsolete("Not used by DeconTools at all", false)]
    internal class SunExtrelRawData : RawData
    {
        private string marr_file_name;
        private float[] mptr_data;
        private float[] mptr_data_copy;
        private int mint_num_points_in_scan;
        private readonly double mdbl_calib_const_c;

        private int mint_data_section_start;
        private int mint_allocated_size;

        private short mshort_options_size;

        private short mshort_ftype;

        private bool mbln_zero_fill;
        private short mshort_num_zero_fill;
        private int mint_last_scan_num;
        private double mdbl_signal_range;

        private readonly SortedDictionary<int, string> mmap_files_names = new SortedDictionary<int, string>();

        private CalibrationType menm_calibration_type;
        //      ' FTMS cal types:
        //     '   1  m/z = A/f + |Vt|B/f^2
        //     '   3  m/z = A/f + |Vt|B/f^2 + C
        //     '   2  m/z = A/f + |Vt|B/f^2 + I|Vt|C/f^2

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override FileType GetFileType()
        {
            return FileType.SUNEXTREL;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override int GetNumScansLoaded()
        {
            return mint_last_scan_num;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();
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

        public override bool IsFTScan(int scanNum)
        {
            return true;
        }

        public override int GetMSLevel(int scan_num)
        {
            //future work
            return 1;
        }

        private long ReadHeader(BinaryReader fStream)
        {
            // Gets the number of points in scan, the size of the header which gives the offset to the actual data.

            var tag = FindDetectSlice(fStream);

            if (!string.IsNullOrWhiteSpace(tag))
            {
                tag += " {";
            }
            else
            {
                tag = "Type=detect";
            }

            //' Type of data
            //'      1 = Time
            //'      2 = Freq
            //'      3 = Mass
            //'      5 = X,Y floating point pairs...

            mshort_ftype = (short) Helpers.ReadByte(fStream, 3);
            mshort_options_size = Helpers.ReadInt16(fStream.BaseStream, 10);
            mint_data_section_start = mshort_options_size + 64;

            var options_str = Helpers.ReadFileString(fStream.BaseStream, 64, mshort_options_size);

            var start_ptr = options_str.IndexOf(tag);
            var end_ptr = -1;
            if (start_ptr >= 0)
                end_ptr = options_str.IndexOf("}", start_ptr);
            //char *start_ptr = strstr(options_str, tag);
            //char *end_ptr = null;
            //if (start_ptr != null)
            //    end_ptr = strstr(start_ptr, "}");

            string slice_options = null;
            if (end_ptr >= 0 && start_ptr >= 0)
                slice_options = "";
            if (slice_options != null)
            {
                slice_options = options_str.Substring(start_ptr, end_ptr - start_ptr + 1);
            }

            ExtractSettings(0, slice_options);

            if (mshort_ftype == 2 && mbln_zero_fill)
            {
                mint_num_points_in_scan = mint_num_points_in_scan * (2 ^ mshort_num_zero_fill);
            }

            //Determine the sequence total time...
            long pos = 0;
            double total_time = 0;
            while (true)
            {
                if (!Helpers.GetDouble(options_str, "time:", (int) pos, out var this_time))
                    break;
                var pos2 = options_str.IndexOf("time:", (int) pos);
                pos = pos2 + 5;
                total_time += this_time;
            }
            //     Call LogMess(ps, "Total Sequence Time = " & Format(TotalTime))

            return mint_data_section_start;
        }

        private void ExtractSettings(int start_p, string option_str)
        {
            Helpers.GetInt32(option_str, "dataPoints:", start_p, out var temp_int);
            mint_num_points_in_scan = temp_int;

            Helpers.GetDouble(option_str, "dwell:", start_p, out var sample_rate);

            if (sample_rate != 0)
                sample_rate = 1 / sample_rate;

            if (Helpers.GetInt16(option_str, "Zerofill=true", start_p, out var short_bluff))
            {
                mbln_zero_fill = true;
            }

            Helpers.GetInt16(option_str, "ZerofillNumber=", start_p, out var temp);
            mshort_num_zero_fill = temp;
            Helpers.GetDouble(option_str, "analyzerTrapVoltage:", start_p, out var anal_trap_voltage);
            Helpers.GetDouble(option_str, "conductanceLimitVoltage:", start_p, out var conductance_lim_voltage);
            Helpers.GetDouble(option_str, "sourceTrapVoltage:", start_p, out var source_trap_voltage);
            Helpers.GetInt16(option_str, "calType:", start_p, out var calibration_type);
            Helpers.GetDouble(option_str, "calReferenceFrequency:", start_p, out var low_mass_frequency);

            double calib_a = 0;
            bool found_a;
            found_a = Helpers.GetDouble(option_str, "calC0:", start_p, out calib_a);
            double calib_b = 0;
            bool found_b;
            found_b = Helpers.GetDouble(option_str, "calC1:", start_p, out calib_b);
            double calib_c = 0;
            bool found_c;
            found_c = Helpers.GetDouble(option_str, "calC2:", start_p, out calib_c);

            Helpers.GetDouble(option_str, "chirpStartFrequency:", start_p, out low_mass_frequency);

            if (Helpers.GetInt16(option_str, "detectType:analyzer", start_p, out short_bluff))
            {
                calib_b = calib_b * Math.Abs(anal_trap_voltage);
            }
            else
            {
                calib_b = calib_b * Math.Abs(source_trap_voltage);
            }

            if (calibration_type == 2)
            {
                //  Call ErrorMess.LogError("Warning, type 2 calibration, make sure intensity option is defined in calibration generation dialog!", ps.FileName)
                calib_c = calib_c * Math.Abs(anal_trap_voltage);
            }

            if (calib_a == 0 && calib_b == 0 && calib_c == 0)
            {
                //no header cal data found, use defaults!
                //Call ErrorMess.LogError("No calibration data in file, using defaults...", ps.FileName)
                calib_a = 108205311.2284;
                calib_b = -1767155067.018;
                calib_c = 29669467490280;
            }
            menm_calibration_type = (CalibrationType) calibration_type;

            var calib = new Calibrations.Calibrator(menm_calibration_type);

            calib.NumPointsInScan = mint_num_points_in_scan;
            calib.LowMassFrequency = low_mass_frequency;
            calib.SampleRate = sample_rate;
            calib.SetCalibrationEquationParams(calib_a, calib_b, calib_c);
            SetCalibrator(calib);
        }

        private int ReadFinniganHeader(BinaryReader fStream, out FinniganHeader FH)
        {
            FH = new FinniganHeader();
            FH.ByteFormat = Helpers.ReadByte(fStream, 0);
            FH.Processor = Helpers.ReadByte(fStream, 1);
            FH.MagicNum = Helpers.ReadByte(fStream, 2);
            FH.FileType = Helpers.ReadByte(fStream, 3);
            FH.RevLevel = Helpers.ReadInt16(fStream.BaseStream, 4);
            FH.Descriptor = Helpers.ReadInt16(fStream.BaseStream, 6);
            FH.SeqSize = Helpers.ReadInt32(fStream.BaseStream, 8);
            FH.DataSize = Helpers.ReadInt32(fStream.BaseStream, 16);
            FH.TrailorRecordsSize = Helpers.ReadInt32(fStream.BaseStream, 20);
            FH.TrailorRecordsNamesSize = Helpers.ReadInt32(fStream.BaseStream, 24);
            FH.ExtendedDataSize = Helpers.ReadInt32(fStream.BaseStream, 28);
            if (FH.MagicNum != 62) return -1;
            if (FH.FileType != 1 && FH.FileType != 2 && FH.FileType != 3 && FH.FileType != 37)
                return -1;
            return 0;
        }

        private long FindTrailorIndex(BinaryReader fStream)
        {
            const string var_name = "Detect_Slice";
            // Need to stop reading everything one field at a time for
            // these header files.

            long pos;
            long length;
            var start = 0;
            var count = 0;
            string labels;

            if (ReadFinniganHeader(fStream, out var FH) != 0)
                return -1;

            pos = FH.SeqSize + FH.DataSize + FH.TrailorRecordsSize + 64;
            length = FH.TrailorRecordsNamesSize;
            // TODO: This should probably be split on newlines...
            labels = Helpers.ReadFileString(fStream.BaseStream, pos, length);

            var index = -1;

            //while (start < length)
            //{
            //    int len = (int) strlen(labels + start);
            //    if (strncmp(var_name, labels + start, len) == 0)
            //        index = count;
            //    count = count + 1;
            //    start += (len + 1);
            //}
            index = labels.IndexOf(var_name, StringComparison.InvariantCulture);

            return index;
        }

        private string FindDetectSlice(BinaryReader fStream)
        {
            long pos;
            long trailor;
            char Rtype;
            char Dtype;
            long cnt;
            string tag;
            int ln;

            if (ReadFinniganHeader(fStream, out var FH) == -1)
                return null;

            cnt = FindTrailorIndex(fStream);

            if (cnt == -1)
                return null;

            trailor = 64 + FH.SeqSize + FH.DataSize + 16 * cnt;

            Rtype = Helpers.ReadByte(fStream, trailor);
            Dtype = Helpers.ReadByte(fStream, trailor + 1);
            ln = Helpers.ReadInt16(fStream.BaseStream, trailor + 2);

            if (Rtype == 1 && Dtype == 8)
            {
                // Now read the detect slice number!
                tag = Helpers.ReadFileString(fStream.BaseStream, trailor + 8, ln - 1);
                return tag;
            }
            else if (Rtype == 17 && Dtype == 8)
            {
                // If here, then the slice name is saved in misc area
                pos = 64 + FH.SeqSize + FH.DataSize + FH.TrailorRecordsSize + FH.TrailorRecordsNamesSize;
                trailor = pos + Helpers.ReadInt32(fStream.BaseStream, trailor + 8);
                tag = Helpers.ReadFileString(fStream.BaseStream, trailor, ln - 1);
                return tag;
            }
            return null;
        }

        private bool LoadFile(string f_name, int scan_num)
        {
            // for SunExtrelRawData scan_num is always 0.
            scan_num = 0;
            marr_file_name = f_name;
            if (!File.Exists(marr_file_name))
            {
                return false; // File does not exist!
            }
            using (
                var sun_extrel_file =
                    new BinaryReader(new FileStream(marr_file_name, FileMode.Open, FileAccess.Read,
                        FileShare.Read), Encoding.ASCII))
            {
                var data_section_start = ReadHeader(sun_extrel_file);
                long file_pointer = mint_data_section_start + scan_num * mint_num_points_in_scan * 4;

                if (mint_allocated_size < mint_num_points_in_scan || mptr_data == null)
                {
                    mptr_data = new float[mint_num_points_in_scan];
                    mptr_data_copy = new float[mint_num_points_in_scan];
                    mint_allocated_size = mint_num_points_in_scan;
                }
                // seek to location and read.
                sun_extrel_file.BaseStream.Seek(file_pointer, SeekOrigin.Begin);
                var bytes = new byte[mint_num_points_in_scan * sizeof (float)];
                var bytesRead = 0;
                bytesRead = sun_extrel_file.Read(bytes, 0, mint_num_points_in_scan * sizeof (float));
                var found = bytesRead < mint_num_points_in_scan * sizeof (float);

                var min_intensity = double.MaxValue;
                var max_intensity = double.MinValue;
                for (var i = 0; i < mint_num_points_in_scan; i++)
                {
                    if (BitConverter.IsLittleEndian)
                    {
                        Array.Reverse(bytes, i * sizeof (float), sizeof (float));
                    }
                    mptr_data[i] = BitConverter.ToSingle(bytes, i * sizeof (float));
                    if (mptr_data[i] < min_intensity)
                        min_intensity = mptr_data[i];
                    if (mptr_data[i] > max_intensity)
                        max_intensity = mptr_data[i];
                }
                mdbl_signal_range = max_intensity - min_intensity;
                return true;
            }
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
            var num_pts = mint_num_points_in_scan;
            return GetRawData(out mzs, out intensities, scan_num, centroid, num_pts);
        }

        public override double GetScanTime(int scan_num)
        {
            return scan_num;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override double GetSignalRange(int scan_num, bool centroid)
        {
            // only returns a value if the current scan is the one we are asking for.
            if (mint_last_scan_num == scan_num)
                return mdbl_signal_range;
            return 0;
        }

        // Note that Centroid is ignored by this class
        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts)
        {
            const int flt_size = sizeof (float);
            mzs = new List<double>();
            intensities = new List<double>();

            GetFileName(scan_num, out var file_name);
            if (string.IsNullOrWhiteSpace(file_name))
                return false;

            if (num_pts > mint_num_points_in_scan || scan_num != mint_last_scan_num)
            {
                mint_last_scan_num = scan_num;
                var loaded = LoadFile(file_name, scan_num);
                if (!loaded)
                    return false;
                Buffer.BlockCopy(mptr_data, 0, mptr_data_copy, 0, mint_num_points_in_scan * flt_size);
            }
            else
            {
                Buffer.BlockCopy(mptr_data_copy, 0, mptr_data, 0, mint_num_points_in_scan * flt_size);
            }
            if (num_pts <= 0)
                num_pts = mint_num_points_in_scan;

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
                var len = directory_name.Length;
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
                foreach (var path in Directory.EnumerateFiles(directory_name))
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
            var len_filen = file_name.Length;
            var len_pathn = path.Length;
            string full_path;
            var start_index = len_filen;
            for (var i = len_filen - 1; i >= 0 && file_name[i] >= '0' && file_name[i] <= '9'; i--)
            {
                start_index = i;
            }
            if (start_index != len_filen && file_name[start_index - 1] == '.')
            {
                // have the right format.
                var scanNum = file_name.Substring(start_index);
                var scan_num = int.Parse(scanNum);
                full_path = path;

                if (full_path[len_pathn - 1] != '\\' && full_path[len_pathn] != '/')
                    full_path += "\\";
                full_path += file_name;
                mmap_files_names.Add(scan_num, full_path);
            }
        }

        private void AddScanFile(string file_name)
        {
            // add file to list of files. Extract scan number by starting from last letter
            // of file_name.
            var len_filen = file_name.Length;
            var start_index = len_filen;
            for (var i = len_filen - 1; i >= 0 && file_name[i] >= '0' && file_name[i] <= '9'; i--)
            {
                start_index = i;
            }
            if (start_index != len_filen && file_name[start_index - 1] == '.')
            {
                // have the right format.
                var scanNum = file_name.Substring(start_index);
                var scan_num = int.Parse(scanNum);
                AddScanFile(file_name, scan_num);
            }
        }

        private void AddScanFile(string file_name, int scan_num)
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
            var len = directory_path.Length;

            if (directory_path[len - 1] != '\\' && directory_path[len - 1] != '/')
                sfile_name += "\\*";
            else
                sfile_name += "*";

            foreach (var path in Directory.EnumerateFileSystemEntries(directory_path))
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

        public SunExtrelRawData()
        {
            mptr_data = null;
            mptr_data_copy = null;
            mint_allocated_size = 0;
            marr_file_name = "";
            mint_last_scan_num = -1;
            mint_num_points_in_scan = 0;
        }

        ~SunExtrelRawData()
        {
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override string GetFileName()
        {
            return marr_file_name;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override int GetScanSize()
        {
            return mint_num_points_in_scan;
        }

        public override int GetNumScans()
        {
            return mmap_files_names.Count;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override int GetNextScanNum(int current_scan_num)
        {
            return mmap_files_names.Select(x => x.Key).Where(x => x > current_scan_num).DefaultIfEmpty(-1).First();
        }

        public override int GetFirstScanNum()
        {
            if (mmap_files_names.Count == 0)
            {
                throw new Exception("No Scans loaded.");
            }
            return mmap_files_names.First().Key;
        }

        public override int GetLastScanNum()
        {
            if (mmap_files_names.Count == 0)
            {
                throw new Exception("No Scans loaded.");
            }
            return mmap_files_names.Last().Key;
        }
    }
}
