using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using DeconToolsV2.Readers;
using Engine.Utilities;

namespace Engine.Readers
{
    [Obsolete("Only used by Decon2LS.UI", false)]
   internal class AsciiRawData : RawData
    {
        private string marr_file_name;
        private int mint_num_spectra;
        private int mint_num_points_in_scan;
        private List<Int64> mvect_scan_start_position = new List<long>();

        private List<double> mvect_scan_time = new List<double>();
        private List<double> mvect_scan_tic = new List<double>();
        private List<double> mvect_scan_bpi = new List<double>();
        private List<double> mvect_scan_bpi_mz = new List<double>();

        private BinaryReader mint_file_handle;
        private double mdbl_signal_range;
        private char mchar_delimiter;
        private int mint_percent_done;

       public int GetPercentDone()
       {
           return mint_percent_done;
       }

       public override FileType GetFileType()
       {
           return FileType.ASCII;
       }

       public override int GetLastScanNum()
       {
           return GetNumScans() - 1;
       }

       public override int GetNumScansLoaded()
       {
           return mint_num_spectra;
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
            //future work
            return 1;
        }

        ~AsciiRawData()
        {
            Clear();
        }

        public AsciiRawData()
        {
            mint_file_handle = null;
            Clear();
        }

        private void Clear()
        {
            mint_percent_done = 0;
            mint_num_spectra = 0;
            mint_num_points_in_scan = 0;
            mvect_scan_start_position.Clear();
            if (mint_file_handle != null)
            {
                mint_file_handle.Close();
                mint_file_handle = null;
            }
        }

        public override void Close()
        {
            if (mint_file_handle != null)
            {
                mint_file_handle.Close();
                mint_file_handle = null;
            }
        }

       public override string GetFileName()
        {
            return marr_file_name;
        }

        public override double GetScanTime(int scan_num)
        {
            if ((int) mvect_scan_time.Count <= scan_num)
                return mvect_scan_time[scan_num-1];
            return 0;
        }

       public override void Load(string file_n)
        {
            Clear();
            marr_file_name = file_n;
            mint_file_handle = new BinaryReader(new FileStream(marr_file_name, FileMode.Open, FileAccess.Read, FileShare.Read), Encoding.ASCII);

            var end_pos = mint_file_handle.BaseStream.Length;
            long start_pos = 0;

            long pos = 0;

            var mint_last_scan = -1;
            var leftover_length = 0;

            float scan_time = 0;
            var last_scan_time = -1 * float.MaxValue;

            double mz=0, intensity = 0;
            var temp_buffer = new char[MAX_SCAN_SIZE];
            var vect_mz = new List<double>();
            var vect_intensity = new List<double>();
            var line_start_index = 0;

            mchar_delimiter = ',';
            var num_read = 0;
            var first_line = true;

            var temp_copy_length = 511;
            var temp_copy = new char [temp_copy_length+1];

            while (mint_file_handle.BaseStream.Position < mint_file_handle.BaseStream.Length)
            {
                num_read = mint_file_handle.Read(temp_buffer, leftover_length, MAX_SCAN_SIZE - leftover_length) + leftover_length;
                for (var current_index = 0; current_index < num_read; current_index++)
                {
                    if (temp_buffer[current_index] == '\n')
                    {
                        if (first_line)
                        {
                            // look for mchar_delimiter..
                            first_line = false;
                            var pt_index = 0;
                            while(pt_index < current_index)
                            {
                                if (temp_buffer[pt_index] != '.' &&
                                        (temp_buffer[pt_index] < '0' || temp_buffer[pt_index] > '9'))
                                {
                                    mchar_delimiter = temp_buffer[pt_index];
                                    break;
                                }
                                pt_index++;
                            }
                        }
                        mint_percent_done = (int) ((100.0 * ((pos + current_index)*1.0)) / (end_pos+1.0));

                        var current_len = current_index - line_start_index;
                        if (current_len > temp_copy_length)
                        {
                            temp_copy_length = current_len;
                            temp_copy = new char [temp_copy_length+1];
                        }
                        Buffer.BlockCopy(temp_buffer, line_start_index, temp_copy, 0, current_len);
                        temp_copy[current_len] = '\0';

                        var temp_str = new string(temp_copy, 0, current_len);
                        mz = Helpers.atof(new string(temp_copy));
                        var next_val_index = 1;
                        while(next_val_index < current_len && temp_copy[next_val_index] != mchar_delimiter)
                            next_val_index++;
                        intensity = Helpers.atof(temp_str.Substring(next_val_index + 1));

                        next_val_index++;
                        while(next_val_index < current_len && temp_copy[next_val_index] != mchar_delimiter)
                            next_val_index++;
                        if (next_val_index < current_len)
                            scan_time = (float)Helpers.atof(temp_str.Substring(next_val_index + 1));
                        else
                            // no scan provided. Just set to 0.
                            scan_time = 0.0f;

                        if (last_scan_time != scan_time)
                        {
                            if (last_scan_time != float.MinValue)
                            {
                                double bpi_mz = 0;
                                var bpi = GetBasePeakIntensity(vect_mz, vect_intensity, out bpi_mz);
                                var tic = GetTotalIonCount(vect_mz, vect_intensity);

                                mvect_scan_time.Add(scan_time);
                                mvect_scan_tic.Add(tic);
                                mvect_scan_bpi.Add(bpi);
                                mvect_scan_bpi_mz.Add(bpi_mz);
                            }

                            mvect_scan_start_position.Add(line_start_index + pos - leftover_length);
                            last_scan_time = scan_time;
                            mint_num_spectra++;
                            vect_mz.Clear();
                            vect_intensity.Clear();
                        }
                        vect_mz.Add(mz);
                        vect_intensity.Add(intensity);
                        line_start_index = current_index + 1;
                    }
                }

                leftover_length = num_read - line_start_index;
                var num_copied = 0;
                while(line_start_index < num_read)
                {
                    temp_buffer[num_copied++] = temp_buffer[line_start_index++];
                }
                line_start_index = 0;
                if (num_copied != leftover_length)
                {
                    Console.Error.WriteLine("PROBLEMO");
                }
                pos+= (num_read-leftover_length);
            }

            // last line ?
            if (leftover_length != 0)
            {
                // there is one line left behind.
                var temp_str = new string(temp_buffer, 0, leftover_length);
                mz = Helpers.atof(temp_str.Substring(line_start_index));

                var next_val_index = line_start_index+1;

                while(next_val_index < num_read && temp_buffer[next_val_index] != mchar_delimiter)
                    next_val_index++;
                intensity = (float)Helpers.atof(temp_str.Substring(next_val_index + 1));

                next_val_index++;
                while(next_val_index < num_read && temp_buffer[next_val_index] != mchar_delimiter)
                    next_val_index++;
                scan_time = (float)Helpers.atof(temp_str.Substring(next_val_index + 1));

                if (last_scan_time != scan_time)
                {
                    double bpi_mz = 0;
                    var bpi = GetBasePeakIntensity(vect_mz, vect_intensity, out bpi_mz);
                    var tic = GetTotalIonCount(vect_mz, vect_intensity);

                    mvect_scan_time.Add(scan_time);
                    mvect_scan_tic.Add(tic);
                    mvect_scan_bpi.Add(bpi);
                    mvect_scan_bpi_mz.Add(bpi_mz);

                    mvect_scan_start_position.Add(line_start_index + pos - leftover_length);
                    last_scan_time = scan_time;
                    mint_num_spectra++;
                    vect_mz.Clear();
                    vect_intensity.Clear();

                    mvect_scan_start_position.Add(pos);
                    last_scan_time = scan_time;
                    mint_num_spectra++;
                }
                vect_mz.Add(mz);
                vect_intensity.Add(intensity);
            }
            mvect_scan_start_position.Add(pos+leftover_length);

            if (last_scan_time == scan_time)
            {
                double bpi_mz = 0;
                var bpi = GetBasePeakIntensity(vect_mz, vect_intensity, out bpi_mz);
                var tic = GetTotalIonCount(vect_mz, vect_intensity);

                mvect_scan_time.Add(scan_time);
                mvect_scan_tic.Add(tic);
                mvect_scan_bpi.Add(bpi);
                mvect_scan_bpi_mz.Add(bpi_mz);

                mvect_scan_start_position.Add(line_start_index + pos - leftover_length);
                last_scan_time = scan_time;
                mint_num_spectra++;
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

        // Note that Centroid is ignored by this class
        public override double GetSignalRange(int scan_num, bool centroid)
        {
            // only returns a value if the current scan is the one we are asking for.
            return 0;
        }

        // Note that Centroid is ignored by this class
        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            // scan_nums are supposed to be 0 indexed for retrieval, but the function is expected to pass in 1 indexed.
            // hence subtract 1 from scan_num.
            // Do not allow 0 indexed entry.
            if (scan_num == 0)
                return false;

            scan_num = scan_num - 1;

            long startOffset;
            long stopOffset;
            long pos;

            // we already know where to start reading because we tracked it before.
            startOffset = mvect_scan_start_position[scan_num];
            stopOffset = mvect_scan_start_position[scan_num+1];

            //pos = mint_file_handle.Seek(0, SeekOrigin.Current);
            //pos = mint_file_handle.Seek(startOffset-pos, SeekOrigin.Current);

            pos = mint_file_handle.BaseStream.Seek(startOffset, SeekOrigin.Begin);

            // there's an extra carriage return in the end.
            var temp_buffer = new char[stopOffset - startOffset+1];
            var num_read = mint_file_handle.Read(temp_buffer, 0, (int)(stopOffset - startOffset));
            temp_buffer[num_read] = '\0';

            var pt_num = 0;

            var read_scan_num = 0;
            double mz=0, intensity = 0;
            var line_start_index = 0;

            var temp_copy_length = 512;
            var temp_copy = new char [temp_copy_length+1];
            for (var current_index = 0; current_index < num_read; current_index++)
            {
                if (temp_buffer[current_index] == '\n')
                {
                    var current_len = current_index - line_start_index;
                    if (current_len > temp_copy_length)
                    {
                        temp_copy_length = current_len;
                        temp_copy = new char [temp_copy_length+1];
                    }
                    Buffer.BlockCopy(temp_buffer, line_start_index, temp_copy, 0, current_len);
                    temp_copy[current_len] = '\0';

                    var temp_str = new string(temp_copy, 0, current_len);
                    mz = Helpers.atof(temp_str);
                    var next_val_index = 1;
                    while(next_val_index < current_len && temp_copy[next_val_index] != mchar_delimiter)
                        next_val_index++;
                    intensity = Helpers.atof(temp_str.Substring(next_val_index + 1));

                    mzs.Add(mz);
                    intensities.Add(intensity);

                    line_start_index = current_index + 1;
                }
            }
            if (line_start_index < (uint) num_read)
            {
                var temp_str = new string(temp_buffer);
                mz = Helpers.atof(temp_str.Substring(line_start_index));

                var next_val_index = line_start_index+1;

                while(next_val_index < num_read && temp_buffer[next_val_index] != mchar_delimiter)
                    next_val_index++;
                intensity = Helpers.atof(temp_str.Substring(next_val_index + 1));
                mzs.Add(mz);
                intensities.Add(intensity);
            }
            return true;
        }

        public override int GetNumScans()
        {
            return mint_num_spectra;
        }

        public override int GetFirstScanNum()
        {
            return 1;
        }

       public override int GetScanSize()
        {
            return mint_num_points_in_scan;
        }

        private double GetBasePeakIntensity(List<double> mzs, List<double> intensities, out double bpi_mz)
        {
            bpi_mz = 0;
            var num_pts = intensities.Count;
            if (num_pts == 0)
                return 0;
            var max_intensity = -1 * double.MaxValue;
            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                if (intensities[pt_num] > max_intensity && mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
                {
                    max_intensity = intensities[pt_num];
                    bpi_mz = mzs[pt_num];
                }
            }
            return max_intensity;
        }

        private double GetTotalIonCount(List<double> mzs, List<double> intensities)
        {
            var num_pts = intensities.Count;
            if (num_pts == 0)
                return 0;

            double intensity_sum = 0;
            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                if (mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
                {
                    intensity_sum += intensities[pt_num];
                }
            }
            var bg_intensity = intensity_sum / num_pts;

            var min_intensity = bg_intensity * BACKGROUND_RATIO_FOR_TIC;

            intensity_sum = 0;
            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                if (intensities[pt_num] > min_intensity && mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
                {
                    intensity_sum += intensities[pt_num];
                }
            }
            return intensity_sum;
        }

        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times, bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();

            for (var scan_num = 0; scan_num < (int) mvect_scan_tic.Count; scan_num++)
            {
                scan_times.AddRange(mvect_scan_time);
                if (base_peak_tic)
                {
                    intensities.AddRange(mvect_scan_bpi);
                }
                else
                {
                    intensities.AddRange(mvect_scan_tic);
                }
            }
        }
    }
}
