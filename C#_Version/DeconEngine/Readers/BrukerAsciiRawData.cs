#if Enable_Obsolete
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using DeconToolsV2.Readers;
using Engine.Utilities;

namespace Engine.Readers
{
    [Obsolete("Not used by DeconTools at all", false)]
    internal class BrukerAsciiRawData : RawData
    {
        private string marr_file_name;
        private int mint_num_spectra;
        private int mint_num_points_in_scan;
        private int mint_last_scan_num;
        private List<Int64> mvect_scan_start_position = new List<Int64>();
        private List<double> mvect_scan_time = new List<double>();
        private BinaryReader mint_file_handle;
        private double mdbl_signal_range;
        private char[] marr_buffer = new char[MAX_SCAN_SIZE];

        public override FileType GetFileType()
        {
            return FileType.BRUKER_ASCII;
        }

        public override int GetNumScansLoaded()
        {
            return mint_last_scan_num;
        }

        public override int GetFirstScanNum()
        {
            return 1;
        }

        public override int GetLastScanNum()
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

        ~BrukerAsciiRawData()
        {
            Clear();
        }

        public BrukerAsciiRawData()
        {
            mint_file_handle = null;
            Clear();
        }

        private void Clear()
        {
            mint_last_scan_num = -1;
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
                return mvect_scan_time[scan_num - 1];
            return 0;
        }

        public override void Load(string file_n)
        {
            Clear();

            marr_file_name = file_n;
            mint_file_handle =
                new BinaryReader(new FileStream(marr_file_name, FileMode.Open, FileAccess.Read, FileShare.Read),
                    Encoding.ASCII);
            mvect_scan_start_position.Add(0);
            mint_last_scan_num = 0;
        }

        public override bool IsZoomScan(int scan_num)
        {
            return false;
        }

        // Note that Centroid is ignored by this class
        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid)
        {
            int num_pts = mint_num_points_in_scan;
            return GetRawData(out mzs, out intensities, scan_num, centroid, num_pts);
        }

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
            // scan_nums are supposed to be 0 indexed for retrieval, but the function is expected to pass in 1 indexed.
            // hence substract 1 from scan_num.
            mzs = new List<double>();
            intensities = new List<double>();

            // Do not allow 0 indexed entry.
            if (scan_num == 0)
                return false;

            scan_num = scan_num - 1;

            long startOffset;
            long stopOffset;
            long pos;

            if (scan_num < mint_last_scan_num)
            {
                // we already know where to start reading because we tracked it before.
                startOffset = mvect_scan_start_position[scan_num];
                stopOffset = mvect_scan_start_position[scan_num + 1];
            }
            else
            {
                // otherwise, scan through the scans till we get there.
                // yes the file will be scanned two times but we'll have to
                // live with that for the simplicity of the read and to be able to
                // use low level read commands which allow 64 bit reading.

                startOffset = mvect_scan_start_position[mint_last_scan_num];
                pos = mint_file_handle.BaseStream.Seek(0, SeekOrigin.Current);
                pos = mint_file_handle.BaseStream.Seek(startOffset - pos, SeekOrigin.Current);

                while (mint_file_handle.BaseStream.Position < mint_file_handle.BaseStream.Length &&
                       mint_last_scan_num <= scan_num)
                {
                    int numRead = mint_file_handle.Read(marr_buffer, 0, MAX_SCAN_SIZE);

                    if (numRead < 0)
                        return false;
                    // now go through this loaded data and set up positions first.
                    // start at index 1 because the end of the previous last loaded scan will be at the
                    // return character.
                    for (int current_index = 1; current_index < numRead; current_index++)
                    {
                        if (marr_buffer[current_index] == '\n')
                        {
                            mint_last_scan_num++;
                            mvect_scan_start_position.Add(startOffset + current_index);
                        }
                    }
                }

                // let the number of spectra grow if it wasn't set by other means.
                if (mint_num_spectra < mint_last_scan_num)
                    mint_num_spectra = mint_last_scan_num;

                if (mint_last_scan_num <= scan_num)
                {
                    // means this really was the last scan.
                    return false;
                }
                stopOffset = mvect_scan_start_position[scan_num + 1];
            }

            /* Seek the beginning of the file: */
            pos = mint_file_handle.BaseStream.Seek(0, SeekOrigin.Current);
            pos = mint_file_handle.BaseStream.Seek(startOffset - pos, SeekOrigin.Current);

            if (pos == -1)
            {
                System.Console.Error.WriteLine("Could not read data for scan = " + scan_num + " at location " +
                                               startOffset);
                Environment.Exit(1);
            }

            // there's an extra carriage return in the end.
            int num_read = mint_file_handle.Read(marr_buffer, 0, (int) (stopOffset - startOffset));
            string buffer_string = new string(marr_buffer, 0, (int) (stopOffset - startOffset));
            double scan_time = Helpers.atof(buffer_string);

            int index = -1;
            int colNum = 0;
            while (index < num_read && colNum != 7)
            {
                index++;
                while (index < num_read && marr_buffer[index] != ',')
                    index++;
                colNum++;
            }
            if (colNum != 7)
                return false;

            int num_pts_in_scan = (int) Helpers.atol(buffer_string.Substring(index + 1));

            int pt_num = 0;
            mzs.Capacity = num_pts_in_scan;
            intensities.Capacity = num_pts_in_scan;

            index++;
            while (index < num_read && marr_buffer[index] != ',')
                index++;
            index++;
            int startIndex1 = index;

            char[] temp = new char[32];
            while (pt_num < num_pts_in_scan && index < num_read)
            {
                while (index < num_read && marr_buffer[index] != ' ')
                    index++;
                int startIndex2 = index;

                Buffer.BlockCopy(marr_buffer, startIndex1, temp, 0, startIndex2 - startIndex1);
                temp[startIndex2 - startIndex1] = '\0';
                double mz = Helpers.atof(new string(temp, 0, startIndex2 - startIndex1));

                while (index < num_read && marr_buffer[index] != ',')
                    index++;
                startIndex1 = index + 1;
                index++;

                Buffer.BlockCopy(marr_buffer, startIndex2, temp, 0, startIndex1 - startIndex2 - 1);
                temp[startIndex1 - startIndex2 - 1] = '\0';
                int intensity = (int) Helpers.atol(new string(temp, 0, startIndex1 - startIndex2 - 1));

                pt_num++;
                mzs.Add(mz);
                intensities.Add(intensity);
            }

            return true;
        }

        public override int GetNumScans()
        {
            return mint_num_spectra;
        }

        public override int GetScanSize()
        {
            return mint_num_points_in_scan;
        }

        private double GetBasePeakIntensity(List<double> mzs, List<double> intensities)
        {
            int num_pts = intensities.Count;
            if (num_pts == 0)
                return 0;
            double max_intensity = -1 * double.MaxValue;
            for (int pt_num = 0; pt_num < num_pts; pt_num++)
            {
                if (intensities[pt_num] > max_intensity && mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
                {
                    max_intensity = intensities[pt_num];
                }
            }
            return max_intensity;
        }

        private double GetTotalIonCount(List<double> mzs, List<double> intensities)
        {
            int num_pts = intensities.Count;
            if (num_pts == 0)
                return 0;

            double intensity_sum = 0;
            for (int pt_num = 0; pt_num < num_pts; pt_num++)
            {
                if (mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
                {
                    intensity_sum += intensities[pt_num];
                }
            }
            double bg_intensity = intensity_sum / num_pts;

            double min_intensity = bg_intensity * BACKGROUND_RATIO_FOR_TIC;

            intensity_sum = 0;
            for (int pt_num = 0; pt_num < num_pts; pt_num++)
            {
                if (intensities[pt_num] > min_intensity && mzs[pt_num] >= MIN_MZ && mzs[pt_num] <= MAX_MZ)
                {
                    intensity_sum += intensities[pt_num];
                }
            }
            return intensity_sum;
        }

        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();

            List<double> scan_mzs;
            List<double> scan_intensities;

            bool centroid = false;

            // remember that we are not going to know the number of spectra to begin with at it will update itself each time.
            bool got_data = true;
            int scan_num = 1;
            while (got_data)
            {
                // its time to read in that scan.
                got_data = GetRawData(out scan_mzs, out scan_intensities, scan_num, centroid);
                if (!got_data)
                    break;

                scan_times.Add(scan_num);
                if (base_peak_tic)
                {
                    double bp_intensity = GetBasePeakIntensity(scan_mzs, scan_intensities);

                    intensities.Add(bp_intensity);
                }
                else
                {
                    double tic_intensity = GetTotalIonCount(scan_mzs, scan_intensities);
                    intensities.Add(tic_intensity);
                }
                scan_num++;
            }
        }
    }
}
#endif