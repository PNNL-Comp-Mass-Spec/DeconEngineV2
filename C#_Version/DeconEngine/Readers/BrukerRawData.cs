using System;
using System.Collections.Generic;
using System.IO;
using DeconToolsV2.Readers;

namespace Engine.Readers
{
    [Obsolete("Use exists in DeconTools, but no path to that code exists", false)]
    internal class BrukerRawData : RawData
    {
        private string marr_headerName;
        private string marr_serName;

        private int mint_num_spectra;
        private int mint_num_points_in_scan;
        private int[] marr_data_block;
        private int[] marr_data_block_copy;
        private float[] marr_temp_data_block;

        private int mint_last_scan_num;
        private double mdbl_signal_range;

        public override FileType GetFileType()
        {
            return FileType.BRUKER;
        }

        public override int GetNumScansLoaded()
        {
            return mint_last_scan_num;
        }

        public override int GetLastScanNum()
        {
            return mint_num_spectra;
        }

        public override int GetFirstScanNum()
        {
            return 1;
        }

        ~BrukerRawData()
        {
            if (marr_data_block != null)
            {
                marr_data_block = null;
                marr_temp_data_block = null;
                marr_data_block_copy = null;
            }

            mint_last_scan_num = -1;
        }

        public BrukerRawData()
        {
            marr_data_block = null;
            marr_data_block_copy = null;
            marr_temp_data_block = null;
            marr_headerName = "";
            marr_serName = "";
            mint_last_scan_num = -1;
        }

        public override string GetFileName()
        {
            return marr_serName;
        }

        public override void SetCalibrator(Engine.Calibrations.Calibrator calib)
        {
            mobj_calibrator = calib;
            SetDataSize(mobj_calibrator.NumPointsInScan);
            mint_num_spectra = GetNumSpectraFromFileSizeInfo();
        }

        public void Open(string header_n, string ser_file_name)
        {
            if (IsDir(ser_file_name))
            {
                marr_headerName = ser_file_name + "\\acqus";
                marr_serName = ser_file_name + "\\ser";
            }
            else
            {
                // assuming an acqu file was selected.. go back to the name of the folder.
                var dirName = Path.GetDirectoryName(ser_file_name);
                marr_headerName = dirName + "\\acqus";
                marr_serName = dirName + "\\ser";
            }

            FindHeaderParams();
            mint_num_spectra = GetNumSpectraFromFileSizeInfo();
        }

        public override double GetScanTime(int scan_num)
        {
            return scan_num;
        }

        public override void Load(string file_n)
        {
            Open("acqus", file_n);
        }

        private void SetDataSize(int sz)
        {
            mint_num_points_in_scan = sz;
            marr_data_block = new int[mint_num_points_in_scan];
            marr_data_block_copy = new int[mint_num_points_in_scan];
            marr_temp_data_block = new float[mint_num_points_in_scan];
        }

        private int FindHeaderParams()
        {
            string line;
            string sub;
            var pos = 0;
            double ML1 = 0;
            double ML2 = 0;
            double SW_h = 0;
            double FR_low = 0;
            var byte_order = 0;
            var TD = 0;
            var NF = 0;

            if (!File.Exists(marr_headerName))
            {
                return -1; // File does not exist!
            }
            // Open header
            using (
                var acqusHeader =
                    new StreamReader(new FileStream(marr_headerName, FileMode.Open, FileAccess.Read, FileShare.Read)))
            {
                // Read first line
                while (!acqusHeader.EndOfStream)
                {
                    line = acqusHeader.ReadLine();
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }
                    // Find the name,value pair within the acqus header file.
                    if ((pos = line.IndexOf("##$ML1= ", StringComparison.InvariantCulture)) != -1)
                    {
                        pos += 7;
                        ML1 = double.Parse(line.Substring(pos, line.Length - pos - 1));
                    }
                    if ((pos = line.IndexOf("##$ML2= ", StringComparison.InvariantCulture)) != -1)
                    {
                        pos += 7;
                        ML2 = double.Parse(line.Substring(pos, line.Length - pos - 1));
                    }
                    if ((pos = line.IndexOf("##$SW_h= ", StringComparison.InvariantCulture)) != -1)
                    {
                        pos += 8;
                        SW_h = double.Parse(line.Substring(pos, line.Length - pos - 1));
                    }
                    if ((pos = line.IndexOf("##$BYTORDA= ", StringComparison.InvariantCulture)) != -1)
                    {
                        pos += 12;
                        byte_order = int.Parse(line.Substring(pos, line.Length - pos - 1));
                    }
                    if ((pos = line.IndexOf("##$TD= ", StringComparison.InvariantCulture)) != -1)
                    {
                        pos += 6;
                        TD = int.Parse(line.Substring(pos, line.Length - pos - 1));
                    }
                    if ((pos = line.IndexOf("##$FR_low= ", StringComparison.InvariantCulture)) != -1)
                    {
                        pos += 9;
                        FR_low = double.Parse(line.Substring(pos, line.Length - pos - 1));
                    }
                    if ((pos = line.IndexOf("##$NF= ", StringComparison.InvariantCulture)) != -1)
                    {
                        pos += 6;
                        NF = int.Parse(line.Substring(pos, line.Length - pos - 1));
                    }
                }

                mint_num_spectra = NF;
                // Done.
            }

            // It's in ICR-2LS...
            SW_h *= 2.0;
            if (SW_h > FR_low)
                FR_low = 0.0;

            var calib = new Calibrations.Calibrator(CalibrationType.A_OVER_F_PLUS_B);

            SetDataSize(TD);
            calib.NumPointsInScan = TD;
            calib.LowMassFrequency = FR_low;
            calib.SampleRate = SW_h;
            calib.SetCalibrationEquationParams(ML1, ML2, 0.0);
            SetCalibrator(calib);

            return 0; // success

            //[gord] delete later - this has been moved to method: GetNumSpectraFromFileSizeInfo, called elsewhere
            //// Now we need to get the size of the data so we can find out the number of scans.
            //int fh;
            //fh = _open(marr_serName, _O_RDONLY | _O_BINARY );
            //if (fh ==  ENOENT || fh == -1 )
            //{
            //  // try using the fid extension instead of the .ser business.
            //  int len = strlen(marr_serName);
            //  marr_serName[len-3] = 'f';
            //  marr_serName[len-2] = 'i';
            //  marr_serName[len-1] = 'd';
            //  fh = _open(marr_serName, _O_RDONLY | _O_BINARY );
            //}
            ///* Seek the beginning of the file: */
            //__int64 pos64 = 0;
            //pos64 = _lseeki64(fh, 0, SEEK_END );
            //__int64       blockSizeInBytes = (__int64) (sizeof(int) * mint_num_points_in_scan);
            //mint_num_spectra = (int)((pos64+2) / blockSizeInBytes); // add 2 just in case we have an exact multiple - 1.

            //_close( fh );
        }

        private int ReadSpectraFloats(int spectra_num)
        {
            FileStream fh = null;
            long startOffset;
            long pos;
            int blockSizeInBytes;
            var num_read = 0;
            // Prepare for data read
            blockSizeInBytes = sizeof (int) * mint_num_points_in_scan;
            startOffset = ((long) spectra_num) * ((long) blockSizeInBytes);
            // Read the data from the SER file into "intBlock"
            try
            {
                fh = new FileStream(marr_serName, FileMode.Open, FileAccess.Read, FileShare.Read);
            }
            catch (Exception)
            {
                Console.Error.WriteLine(" Could not open " + marr_serName + " perhaps it does not exist. Exiting");
#if DEBUG
                throw;
#endif
                System.Environment.Exit(1);
            }
            using (fh)
            {
                /* Seek the beginning of the file: */
                pos = fh.Seek(startOffset, SeekOrigin.Begin);

                if (pos == -1)
                {
                    Console.Error.WriteLine("Could not read data for scan = " + spectra_num + " at location " +
                                                   startOffset);
                    System.Environment.Exit(1);
                }
                var buffer = new byte[blockSizeInBytes];
                num_read = fh.Read(buffer, 0, blockSizeInBytes);
                Buffer.BlockCopy(buffer, 0, marr_data_block, 0, blockSizeInBytes);
            }
            return num_read;
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

        public override int GetMSLevel(int scan_num)
        {
            //future work
            return 1;
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
            // hence subtract 1 from scan_num.
            mzs = new List<double>();
            intensities = new List<double>();

            // Do not allow 0 indexed entry.
            if (scan_num == 0)
                return false;

            scan_num = scan_num - 1;
            const int int_size = sizeof (int);
            if (mint_last_scan_num != scan_num)
            {
                var num_read = ReadSpectraFloats(scan_num);
                mint_last_scan_num = scan_num;
                if (num_read <= 0)
                    return false;
                Buffer.BlockCopy(marr_data_block, 0, marr_data_block_copy, 0, mint_num_points_in_scan * int_size);
            }
            else
            {
                Buffer.BlockCopy(marr_data_block_copy, 0, marr_data_block, 0, num_pts * int_size);
            }

            var min_intensity = double.MaxValue;
            var max_intensity = double.MinValue;
            for (var i = 0; i < num_pts; i++)
            {
                marr_temp_data_block[i] = (float) marr_data_block[i];
                if (marr_data_block[i] < min_intensity)
                    min_intensity = marr_data_block[i];
                if (marr_data_block[i] > max_intensity)
                    max_intensity = marr_data_block[i];
            }
            mdbl_signal_range = max_intensity - min_intensity;
            var n = mobj_calibrator.GetRawPointsApplyFFT(ref marr_temp_data_block, out mzs, out intensities, num_pts);
            // lets only take points whose mz is less than MAX_MZ.
            // these are going to be sorted so just start at the right end.

            int pt_num;
            for (pt_num = 0; pt_num < num_pts; pt_num++)
            {
                if (mzs[pt_num] > MAX_MZ)
                    break;
            }
            // TODO: Safely reduce the count first.
            mzs.Capacity = pt_num;
            intensities.Capacity = pt_num;
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
            var num_pts = intensities.Count;
            if (num_pts == 0)
                return 0;
            var max_intensity = -1 * double.MaxValue;
            for (var pt_num = 0; pt_num < num_pts; pt_num++)
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

        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();

            List<double> scan_mzs;
            List<double> scan_intensities;

            var centroid = false;

            for (var scan_num = 1; scan_num < mint_num_spectra; scan_num++)
            {
                // its time to read in that scan.
                var got_data = GetRawData(out scan_mzs, out scan_intensities, scan_num, centroid);
                if (!got_data)
                    continue;

                scan_times.Add(scan_num);
                if (base_peak_tic)
                {
                    var bp_intensity = GetBasePeakIntensity(scan_mzs, scan_intensities);

                    intensities.Add(bp_intensity);
                }
                else
                {
                    var tic_intensity = GetTotalIonCount(scan_mzs, scan_intensities);
                    intensities.Add(tic_intensity);
                }
            }
        }

        private int GetNumSpectraFromFileSizeInfo()
        {
            var numSpectra = 0;

            if (mint_num_points_in_scan == 0) return 0; // prevent divide by 0

            var fName = marr_serName;
            if (!File.Exists(fName))
            {
                // try using the fid extension instead of the .ser business.
                fName = Path.ChangeExtension(fName, "fid");
            }
            using (var fh = new FileStream(fName, FileMode.Open, FileAccess.Read, FileShare.Read))
            {
                /* Get the total number of bytes in the file. */
                var pos64 = fh.Length;
                var blockSizeInBytes = (long) (sizeof (int) * mint_num_points_in_scan);
                numSpectra = (int) ((pos64 + 2) / blockSizeInBytes);
                    // add 2 just in case we have an exact multiple - 1.
            }
            return numSpectra;
        }
    }
}
