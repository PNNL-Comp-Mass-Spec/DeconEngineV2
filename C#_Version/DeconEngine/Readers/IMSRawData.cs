using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using DeconToolsV2.Readers;
using Engine.Utilities;

namespace Engine.Readers
{
    [Obsolete("Only used by IMSRawData, which is no longer used", false)]
    internal class TOFRecord<T> where T : struct, IComparable
    {
        public int tof_bin;
        public T intensity;
    }

    [Obsolete("Used in DeconTools for IMF, which is obsolete.", false)]
    internal class IMSRawData : RawData
    {
        private string marr_filename;
        private double mdbl_k0;
        private double mdbl_t0;
        private double mdbl_min_mz;
        private double mdbl_max_mz;

        private int mint_num_scans;
        private int mint_max_scan_size;
        private double mdbl_scan_interval;
        private double mdbl_elution_time;
        private double mdbl_drift_time;
        private double mdbl_max_drift_time;
        private int mint_start_bin;
        private int mint_stop_bin;
        private int mint_time_offset;
        private int mint_last_scan_num;

        private double mdbl_avg_tof_length;
        private int mint_frame_num;

        private bool mbln_is_multiplexed_data;
        private bool mbln_is_adc_data;

        private List<int> mvect_scan_bpi_adc = new List<int>();
        private List<int> mvect_scan_start_index = new List<int>();
        private List<float> mvect_scan_bpi_mxed = new List<float>();
        private List<short> mvect_scan_bpi = new List<short>();

        private List<TOFRecord<short>> mvect_data = new List<TOFRecord<short>>();
        private List<TOFRecord<float>> mvect_mxed_data = new List<TOFRecord<float>>();
        private List<TOFRecord<int>> mvect_adc_data = new List<TOFRecord<int>>();

        private SortedDictionary<int, double> mmap_bin_intensity_map = new SortedDictionary<int, double>();


        private double GetMassFromBin(int bin)
        {
            var bin_val = bin * 0.1; //(bin/16 (for index) * 16/10(to convert to ns))
//          double bin_val = bin * 1.6; for better resolution
            var t = bin_val + mint_time_offset;
            return mdbl_k0 * mdbl_k0 * (t - mdbl_t0) * (t - mdbl_t0);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override FileType GetFileType()
        {
            return FileType.PNNL_IMS;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
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
            return true;
        }

        public override int GetMSLevel(int scan_num)
        {
            //future work
            return 1;
        }

        public IMSRawData()
        {
            mdbl_k0 = 0;
            mdbl_t0 = 0;
            mdbl_min_mz = 10;
            mdbl_scan_interval = 1;
            mdbl_max_mz = 2500;
            mdbl_avg_tof_length = 0;
            mint_frame_num = 0;
            mbln_is_multiplexed_data = false;
            mbln_is_adc_data = false;
        }

        ~IMSRawData()
        {
            if (mvect_data.Count != 0)
                mvect_data.Clear();
            if (mvect_mxed_data.Count != 0)
                mvect_mxed_data.Clear();
            if (mvect_scan_bpi.Count != 0)
                mvect_scan_bpi.Clear();
            if (mvect_scan_bpi_adc.Count != 0)
                mvect_scan_bpi_adc.Clear();
            if (mvect_scan_start_index.Count != 0)
                mvect_scan_start_index.Clear();
            if (mmap_bin_intensity_map.Count != 0)
                mmap_bin_intensity_map.Clear();
        }

        public override void Load(string file)
        {
            const int SIZE_BUF = 512;
            mint_max_scan_size = 0;
            marr_filename = file;
            var stringBuf = "";
            int pStr;

            double agilent_t0 = 0;
            double agilent_k0 = 0;

            if (!File.Exists(marr_filename))
            {
                throw new Exception(" Could not open " + marr_filename + " perhaps it does not exist.");
            }
            using (
                var pFile =
                    new StreamReader(new FileStream(marr_filename, FileMode.Open, FileAccess.Read, FileShare.Read), Encoding.ASCII))
            {
                var last_char = (char) 0;

                // Read the header in, and initialize values
                while (!pFile.EndOfStream)
                {
                    stringBuf = pFile.ReadLine();

                    if ((pStr = stringBuf.IndexOf("TimeOffset: ", StringComparison.InvariantCulture)) > -1)
                        mint_time_offset = (int) Helpers.atol(stringBuf.Substring(pStr + 11));
                    if ((pStr = stringBuf.IndexOf("CalibrationSlope: ", StringComparison.InvariantCulture)) > -1)
                        agilent_k0 = Helpers.atof(stringBuf.Substring(pStr + 17));
                    if ((pStr = stringBuf.IndexOf("CalibrationIntercept: ", StringComparison.InvariantCulture)) > -1)
                        agilent_t0 = Helpers.atof(stringBuf.Substring(pStr + 21));
                    if ((pStr = stringBuf.IndexOf("FrameNumber: ", StringComparison.InvariantCulture)) > -1)
                        mint_frame_num = (int) Helpers.atol(stringBuf.Substring(pStr + 12));
                    if ((pStr = stringBuf.IndexOf("AverageTOFLength: ", StringComparison.InvariantCulture)) > -1)
                        mdbl_avg_tof_length = Helpers.atof(stringBuf.Substring(pStr + 17));
                    if ((pStr = stringBuf.IndexOf("DataSubType: float", StringComparison.InvariantCulture)) > -1)
                    {
                        mbln_is_multiplexed_data = true;
                        mbln_is_adc_data = false;
                    }
                    if ((pStr = stringBuf.IndexOf("DataSubType: int", StringComparison.InvariantCulture)) > -1)
                    {
                        mbln_is_multiplexed_data = false;
                        mbln_is_adc_data = true;
                    }

                    var len = stringBuf.Length;
                    var foundEndBlock = false;
                    for (var charIndex = 0; charIndex < len; charIndex++)
                    {
                        if (stringBuf[charIndex] == 0x1B)
                        {
                            foundEndBlock = true;
                            break;
                        }
                    }
                    if (foundEndBlock)
                        break;
                }
                //delete pFile;  Anoop  - causes debug assertion in debug mode
            }

            mdbl_t0 = agilent_t0 * 1000; //convert to ns
            mdbl_k0 = agilent_k0 / 1000;

            //open file and skip through header
            using (
                var fh =
                    new BinaryReader(new FileStream(marr_filename, FileMode.Open, FileAccess.Read, FileShare.Read),
                        Encoding.ASCII))
            {
                var last_byte = Byte.MinValue;
                while (fh.BaseStream.Position < fh.BaseStream.Length && last_byte != 0x1B)
                {
                    last_byte = fh.ReadByte();
                }

                if (fh.BaseStream.Position >= fh.BaseStream.Length)
                {
                    fh.Close();
                    return;
                }

                const int INT_SIZE = sizeof (int);
                const int FLT_SIZE = sizeof (float);
                const int SHORT_SIZE = sizeof (short);

                //Get TOF size based on data yype
                // Standard IMS - short
                // Multiplexed IMS - float
                // AGC IMS - int

                var tof_rec_size = 0;
                if (mbln_is_multiplexed_data)
                    tof_rec_size = FLT_SIZE + INT_SIZE;
                else if (mbln_is_adc_data)
                    tof_rec_size = 2 * INT_SIZE;
                else
                    tof_rec_size = SHORT_SIZE + INT_SIZE;

                //Read in number of IMS scans
                //int num_read = _read(fh, &mint_num_scans, INT_SIZE);
                mint_num_scans = fh.ReadInt32();
                var num_read = 0;

                // Read in the lengths and the tic values.
                var int_vals = new int[2 * mint_num_scans];
                var buffer = new byte[4 * 2 * mint_num_scans];
                // TODO: These all really do the exact same thing.... 8 * num_scans
                if (mbln_is_multiplexed_data)
                {
                    num_read = fh.Read(buffer, 0, (INT_SIZE + FLT_SIZE) * mint_num_scans);
                    Buffer.BlockCopy(buffer, 0, int_vals, 0, (INT_SIZE + FLT_SIZE) * mint_num_scans);
                }
                else if (mbln_is_adc_data)
                {
                    num_read = fh.Read(buffer, 0, (2 * INT_SIZE) * mint_num_scans);
                    Buffer.BlockCopy(buffer, 0, int_vals, 0, (2 * INT_SIZE) * mint_num_scans);
                }
                else
                {
                    num_read = fh.Read(buffer, 0, INT_SIZE * 2 * mint_num_scans);
                    Buffer.BlockCopy(buffer, 0, int_vals, 0, INT_SIZE * 2 * mint_num_scans);
                }

                // now the values are fetched, lets save them.
                var num_pts_so_far = 0;
                for (var scan_num = 0; scan_num < mint_num_scans; scan_num++)
                {
                    mvect_scan_bpi.Add((short) int_vals[2 * scan_num]);
                    mvect_scan_bpi_adc.Add(int_vals[2 * scan_num]);
                    mvect_scan_start_index.Add(num_pts_so_far);
                    num_pts_so_far += int_vals[scan_num * 2 + 1] / tof_rec_size;
                }
                mvect_scan_start_index.Add(num_pts_so_far);

                var data_size = tof_rec_size * num_pts_so_far;

                //byte[] temp = new byte[(int) data_size];
                //num_read = fh.Read(temp, 0, data_size);
                // TODO: Or switch entirely to use the BinaryReader to read values one by one...

                mvect_data.Capacity = num_pts_so_far;
                var current_scan = 0;
                //std.cerr.precision(6);
                //std.cerr.setf(std.ios.fixed, std.ios.floatfield);
                //Console.Error.WriteLine("Loading " + file + " # of points = " + num_pts_so_far + " Read points = " + (num_read/TOFREC_SIZE));

                // Now copy into TOFRecords and stuff them into the vector
                for (var pt_num = 0; pt_num < num_pts_so_far; pt_num++)
                {
                    try
                    {
                        while (mvect_scan_start_index[current_scan] < pt_num)
                        {
                            current_scan++;
                        }

                        // ok Byte alignment issues is a pain. To avoid pain,
                        //TOFRecord *ptr = (TOFRecord *) &temp[pt_num*TOFREC_SIZE];
                        if (mbln_is_multiplexed_data)
                        {
                            var mxed_rec = new TOFRecord<float>();
                            //Buffer.BlockCopy(temp, pt_num * tof_rec_size, mxed_rec, 0, tof_rec_size);
                            mxed_rec.tof_bin = fh.ReadInt32();
                            mxed_rec.intensity = fh.ReadSingle();
                            mvect_mxed_data.Add(mxed_rec);

                            if (mxed_rec.tof_bin > mint_max_scan_size)
                                mint_max_scan_size = mxed_rec.tof_bin;
                        }
                        else if (mbln_is_adc_data)
                        {
                            var adc_rec = new TOFRecord<int>();
                            //Buffer.BlockCopy(temp, pt_num * tof_rec_size, adc_rec, 0, tof_rec_size);
                            adc_rec.tof_bin = fh.ReadInt32();
                            adc_rec.intensity = fh.ReadInt32();
                            mvect_adc_data.Add(adc_rec);

                            if (adc_rec.tof_bin > mint_max_scan_size)
                                mint_max_scan_size = adc_rec.tof_bin;
                        }
                        else
                        {
                            var rec = new TOFRecord<short>();
                            //Buffer.BlockCopy(temp, pt_num * tof_rec_size, rec, 0, tof_rec_size);
                            rec.tof_bin = fh.ReadInt32();
                            rec.intensity = fh.ReadInt16();
                            mvect_data.Add(rec);

                            if (rec.tof_bin > mint_max_scan_size)
                                mint_max_scan_size = rec.tof_bin;
                        }
                    }
                    catch (Exception e)
                    {
#if DEBUG
                        throw;
#endif
                        Console.Error.WriteLine(e.Message);
                    }
                }

                mint_max_scan_size++;
            }

            mint_start_bin = int.MaxValue;
            mint_stop_bin = 0;

            for (var i = 0; i < mint_max_scan_size; i++)
            {
                //ignoring /16 to increase resolution
                var index_shifted = i / 16;
                var mz_val = GetMassFromBin(i);

                if (mz_val < mdbl_max_mz)
                {
                    mint_stop_bin = index_shifted;
                }
                else
                {
                    break;
                }
                if (mz_val > mdbl_min_mz && index_shifted < mint_start_bin)
                {
                    mint_start_bin = index_shifted;
                }
            }
            return;
        }

        public override int GetFirstScanNum()
        {
            return 1;
        }

        public override int GetLastScanNum()
        {
            return mint_num_scans;
        }

        public override int GetFrameNumber()
        {
            return mint_frame_num;
        }

        public double GetDriftTime(int scan_num)
        {
            if (scan_num == mint_last_scan_num)
                return mdbl_drift_time / 1000000; //in musecs

            return ((scan_num * mdbl_avg_tof_length) / 1000000);
        }

        public override double GetScanTime(int scan_num)
        {
            /*if (scan_num == mint_last_scan_num)
                return mdbl_elution_time/1000000000; // returns elution time in secs

            double lc_elution_time;
            double max_drift_time;
            max_drift_time = mint_num_scans * mdbl_avg_tof_length;
            lc_elution_time = (mint_frame_num * max_drift_time) + (scan_num * mdbl_avg_tof_length);
            return lc_elution_time/1000000000; */

            return scan_num * mdbl_scan_interval;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override string GetFileName()
        {
            return marr_filename;
        }

        public override void Close()
        {
            mvect_data.Clear();
            mvect_scan_bpi.Clear();
            mvect_scan_bpi_adc.Clear();
            mvect_scan_start_index.Clear();
            mvect_adc_data.Clear();
            mvect_mxed_data.Clear();
        }

        public override bool IsZoomScan(int scan_num)
        {
            return false;
        }

        // Note that Centroid is ignored by this class
        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid)
        {
            var found_data = GetRawData(out mzs, out intensities, scan_num, centroid, -1);
            if (!found_data)
                return found_data;
            //mobj_savgol.Smooth(mzs, intensities);
            return true;
        }

        // Note that Centroid is ignored by this class
        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts)
        {
            var bin_intensity_map = new SortedDictionary<int, double>();
            mzs = new List<double>();
            intensities = new List<double>();
            // get all the points for now.
            if (scan_num > mint_num_scans || scan_num <= 0)
                return false;

            var startIndex = mvect_scan_start_index[scan_num - 1];
            int stopIndex;
            if (mbln_is_multiplexed_data)
                stopIndex = mvect_mxed_data.Count;
            else if (mbln_is_adc_data)
                stopIndex = mvect_adc_data.Count;
            else
                stopIndex = mvect_data.Count;

            if (scan_num < mint_num_scans)
                stopIndex = mvect_scan_start_index[scan_num];

            if (stopIndex == 0)
                return false;

            TOFRecord<short> rec;
            TOFRecord<float> mxed_rec;
            TOFRecord<int> adc_rec;

            mint_last_scan_num = scan_num;
            mdbl_drift_time = scan_num * mdbl_avg_tof_length;
            mdbl_max_drift_time = mint_num_scans * mdbl_avg_tof_length;
            mdbl_elution_time = (mint_frame_num * mdbl_max_drift_time) + mdbl_drift_time;

            for (var index = startIndex; index < stopIndex; index++)
            {
                var bin = 0;
                double intensity_val = 0;
                if (mbln_is_multiplexed_data)
                {
                    mxed_rec = mvect_mxed_data[index];
                    bin = mxed_rec.tof_bin;
                    intensity_val = mxed_rec.intensity;
                }
                else if (mbln_is_adc_data)
                {
                    adc_rec = mvect_adc_data[index];
                    bin = adc_rec.tof_bin;
                    intensity_val = adc_rec.intensity;
                }
                else
                {
                    rec = mvect_data[index];
                    bin = rec.tof_bin;
                    intensity_val = rec.intensity;
                }

                if (bin_intensity_map.ContainsKey(bin))
                {
                    bin_intensity_map[bin] += intensity_val;
                }
                else
                {
                    bin_intensity_map[bin] = intensity_val;
                }
            }

            // now copy all the m/z values into vector.
            var last_bin = 0;
            var last_intensity = 0;
            double mz_val = 0;
            var first = true;
            foreach (var item in bin_intensity_map)
            {
                if (item.Key > last_bin + 1 && !first) //need to revisit this
                {
                    if (last_intensity != 0)
                    {
                        mz_val = this.GetMassFromBin(last_bin + 1);
                        //mzs.Add(mz_val);
                        //intensities.Add(0);
                        last_bin++;
                    }
                    while (last_bin < item.Key - 1)
                    {
                        mz_val = this.GetMassFromBin(last_bin + 1);
                        //mzs.Add(mz_val);
                        //intensities.Add(0);
                        last_bin++;
                    }
                }
                mz_val = this.GetMassFromBin(item.Key);
                mzs.Add(mz_val);

                intensities.Add(item.Value);

                last_bin = (int) item.Key;
                last_intensity = (int) item.Value;
                first = false;
            }

            return true;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override int GetScanSize()
        {
            return mint_max_scan_size;
        }

        public override int GetNumScans()
        {
            return mint_num_scans;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override double GetSignalRange(int scan_num, bool centroid)
        {
            return 0;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            var num_pts = mvect_scan_bpi.Count;
            intensities = new List<double>();
            scan_times = new List<double>();
            intensities.Capacity = num_pts;
            scan_times.Capacity = num_pts;
            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                double intensity;
                if (mbln_is_adc_data)
                {
                    intensity = (double) mvect_scan_bpi_adc[pt_num];
                }
                else if (mbln_is_multiplexed_data)
                {
                    intensity = (double) mvect_scan_bpi_mxed[pt_num];
                }
                else
                {
                    intensity = mvect_scan_bpi[pt_num];
                }
                intensities.Add(intensity);
                scan_times.Add(pt_num);
            }
        }

        public override void GetSummedSpectra(out List<double> mzs, out List<double> intensities, int start_scan,
            int stop_scan, double min_mz, double max_mz)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            var bin_intensity_map = new SortedDictionary<int, double>();
            double mz;

            TOFRecord<short> rec;
            TOFRecord<float> mxed_rec;
            TOFRecord<int> adc_rec;

            // Start
            for (var scan_num = start_scan; scan_num <= stop_scan; scan_num++)
            {
                if (scan_num > mint_num_scans || scan_num <= 0)
                    return;

                if (mvect_scan_start_index[scan_num - 1] < 0)
                    break;

                var startIndex = mvect_scan_start_index[scan_num - 1];
                int stopIndex;
                if (mbln_is_multiplexed_data)
                    stopIndex = mvect_mxed_data.Count;
                else if (mbln_is_adc_data)
                    stopIndex = mvect_adc_data.Count;
                else
                    stopIndex = mvect_data.Count;

                if (scan_num < mint_num_scans)
                    stopIndex = mvect_scan_start_index[scan_num];

                for (var index = startIndex; index < stopIndex; index++)
                {
                    var bin = 0;
                    double intensity_val = 0;
                    if (mbln_is_multiplexed_data)
                    {
                        mxed_rec = mvect_mxed_data[index];
                        bin = mxed_rec.tof_bin;
                        intensity_val = mxed_rec.intensity;
                    }
                    else if (mbln_is_adc_data)
                    {
                        adc_rec = mvect_adc_data[index];
                        bin = adc_rec.tof_bin;
                        intensity_val = adc_rec.intensity;
                    }
                    else
                    {
                        rec = mvect_data[index];
                        bin = rec.tof_bin;
                        intensity_val = rec.intensity;
                    }

                    mz = GetMassFromBin(bin);

                    if (mz >= min_mz && mz <= max_mz)
                    {
                        if (bin_intensity_map.ContainsKey(bin))
                        {
                            bin_intensity_map[bin] += intensity_val;
                        }
                        else
                        {
                            bin_intensity_map[bin] = intensity_val;
                        }
                    }
                }
            }

            foreach (var item in bin_intensity_map)
            {
                mz = this.GetMassFromBin(item.Key);
                if (mz >= min_mz && mz <= max_mz)
                {
                    mzs.Add(mz);
                    intensities.Add(item.Value);
                }
            }
        }

        public void GetSummedSpectraSlidingWindow(out List<double> mzs, out List<double> intensities, int current_scan,
            int scan_range)
        {
            double mz;

            TOFRecord<short> rec;
            TOFRecord<float> mxed_rec;
            TOFRecord<int> adc_rec;

            var min_mz = double.MaxValue;
            var max_mz = double.MinValue;
            var min_mz_bin = 0;
            var max_mz_bin = 0;
            int bin;

            // get mz_range to sum over
            var startIndex = mvect_scan_start_index[current_scan - 1];
            int stopIndex;
            if (mbln_is_multiplexed_data)
                stopIndex = mvect_mxed_data.Count;
            else if (mbln_is_adc_data)
                stopIndex = mvect_adc_data.Count;
            else
                stopIndex = mvect_data.Count;
            if (current_scan < mint_num_scans)
                stopIndex = mvect_scan_start_index[current_scan];

            //get min and max values for m/z and bin
            for (var index = startIndex; index < stopIndex; index++)
            {
                if (mbln_is_multiplexed_data)
                {
                    mxed_rec = mvect_mxed_data[index];
                    bin = mxed_rec.tof_bin;
                    mz = GetMassFromBin(bin);
                    if (mz < min_mz)
                    {
                        min_mz = mz;
                        min_mz_bin = bin;
                    }
                    if (mz > max_mz)
                    {
                        max_mz = mz;
                        max_mz_bin = bin;
                    }
                }
                else if (mbln_is_adc_data)
                {
                    adc_rec = mvect_adc_data[index];
                    bin = adc_rec.tof_bin;
                    mz = GetMassFromBin(bin);
                    if (mz < min_mz)
                    {
                        min_mz = mz;
                        min_mz_bin = bin;
                    }
                    if (mz > max_mz)
                    {
                        max_mz = mz;
                        max_mz_bin = bin;
                    }
                }
                else
                {
                    rec = mvect_data[index];
                    bin = rec.tof_bin;
                    mz = GetMassFromBin(bin);
                    if (mz < min_mz)
                    {
                        min_mz = mz;
                        min_mz_bin = bin;
                    }
                    if (mz > max_mz)
                    {
                        max_mz = mz;
                        max_mz_bin = bin;
                    }
                }
            }

            // now start the summing
            var start_scan = current_scan - scan_range - 1;
            var stop_scan = current_scan + scan_range;

            // Subtract start_scan from bin_intensity_map
            if (start_scan > 1)
            {
                startIndex = mvect_scan_start_index[start_scan - 1];
                if (mbln_is_multiplexed_data)
                    stopIndex = mvect_mxed_data.Count;
                else if (mbln_is_adc_data)
                    stopIndex = mvect_adc_data.Count;
                else
                    stopIndex = mvect_data.Count;

                if (start_scan < mint_num_scans)
                    stopIndex = mvect_scan_start_index[start_scan];

                for (var index = startIndex; index < stopIndex; index++)
                {
                    bin = 0;
                    double intensity_val = 0;
                    if (mbln_is_multiplexed_data)
                    {
                        mxed_rec = mvect_mxed_data[index];
                        bin = mxed_rec.tof_bin;
                        intensity_val = mxed_rec.intensity;
                    }
                    else if (mbln_is_adc_data)
                    {
                        adc_rec = mvect_adc_data[index];
                        bin = adc_rec.tof_bin;
                        intensity_val = adc_rec.intensity;
                    }
                    else
                    {
                        rec = mvect_data[index];
                        bin = rec.tof_bin;
                        intensity_val = rec.intensity;
                    }
                    if (mmap_bin_intensity_map[bin] > 0)
                        mmap_bin_intensity_map[bin] -= intensity_val;
                    else
                        mmap_bin_intensity_map[bin] = 0;
                }
            }

            // now add the second scan to the bin
            if (stop_scan < mint_num_scans)
            {
                startIndex = mvect_scan_start_index[stop_scan - 1];
                stopIndex = mvect_scan_start_index[stop_scan];
                for (var index = startIndex; index < stopIndex; index++)
                {
                    bin = 0;
                    double intensity_val = 0;
                    if (mbln_is_multiplexed_data)
                    {
                        mxed_rec = mvect_mxed_data[index];
                        bin = mxed_rec.tof_bin;
                        intensity_val = mxed_rec.intensity;
                    }
                    else if (mbln_is_adc_data)
                    {
                        adc_rec = mvect_adc_data[index];
                        bin = adc_rec.tof_bin;
                        intensity_val = adc_rec.intensity;
                    }
                    else
                    {
                        rec = mvect_data[index];
                        bin = rec.tof_bin;
                        intensity_val = rec.intensity;
                    }

                    if (mmap_bin_intensity_map.ContainsKey(bin))
                    {
                        mmap_bin_intensity_map[bin] += intensity_val;
                    }
                    else
                    {
                        mmap_bin_intensity_map[bin] = intensity_val;
                    }
                }
            }

            mzs = new List<double>();
            intensities = new List<double>();

            // now choose the mz range we care about
            foreach (
                var item in
                    mmap_bin_intensity_map.Where(x => x.Key >= min_mz_bin && x.Key <= max_mz_bin))
            {
                mz = this.GetMassFromBin(item.Key);
                if (mz >= min_mz && mz <= max_mz)
                {
                    mzs.Add(mz);
                    intensities.Add(item.Value);
                }
            }
        }

        public void InitializeSlidingWindow(int scan_range)
        {
            // Function that initializes the mmap_bin_intensity_map for summing using a sliding window
            // Sum from scans 1 to scan_range and set it to mmap_bin_intensity_map
            TOFRecord<short> rec;
            TOFRecord<float> mxed_rec;
            TOFRecord<int> adc_rec;

            // Start
            for (var scan_num = 1; scan_num <= scan_range; scan_num++)
            {
                if (scan_num > mint_num_scans || scan_num <= 0)
                    return;

                if (mvect_scan_start_index[scan_num - 1] < 0)
                    break;

                var startIndex = mvect_scan_start_index[scan_num - 1];
                int stopIndex;
                if (mbln_is_multiplexed_data)
                    stopIndex = mvect_mxed_data.Count;
                else if (mbln_is_adc_data)
                    stopIndex = mvect_adc_data.Count;
                else
                    stopIndex = mvect_data.Count;

                if (scan_num < mint_num_scans)
                    stopIndex = mvect_scan_start_index[scan_num];

                for (var index = startIndex; index < stopIndex; index++)
                {
                    var bin = 0;
                    double intensity_val = 0;
                    if (mbln_is_multiplexed_data)
                    {
                        mxed_rec = mvect_mxed_data[index];
                        bin = mxed_rec.tof_bin;
                        intensity_val = mxed_rec.intensity;
                    }
                    else if (mbln_is_adc_data)
                    {
                        adc_rec = mvect_adc_data[index];
                        bin = adc_rec.tof_bin;
                        intensity_val = adc_rec.intensity;
                    }
                    else
                    {
                        rec = mvect_data[index];
                        bin = rec.tof_bin;
                        intensity_val = rec.intensity;
                    }
                    // sum over all mzs do not bother about mz range
                    if (mmap_bin_intensity_map.ContainsKey(bin))
                    {
                        mmap_bin_intensity_map[bin] += intensity_val;
                    }
                    else
                    {
                        // Add the value
                        mmap_bin_intensity_map[bin] = intensity_val;
                    }
                }
            }
        }

        public override void GetSummedSpectra(out List<double> mzs, out List<double> intensities, int current_scan,
            int scan_range)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            var bin_intensity_map = new SortedDictionary<int, double>();
            double mz;

            TOFRecord<short> rec;
            TOFRecord<float> mxed_rec;
            TOFRecord<int> adc_rec;

            var min_mz = double.MaxValue;
            var max_mz = double.MinValue;
            int bin;

            var start_scan = current_scan - scan_range;
            var stop_scan = current_scan + scan_range;

            //check
            if (start_scan < 1)
                start_scan = 1;
            if (stop_scan > mint_num_scans)
                stop_scan = mint_num_scans;

            // Setting m/z range to view
            var startIndex = mvect_scan_start_index[current_scan - 1];
            int stopIndex;
            if (mbln_is_multiplexed_data)
                stopIndex = mvect_mxed_data.Count;
            else if (mbln_is_adc_data)
                stopIndex = mvect_adc_data.Count;
            else
                stopIndex = mvect_data.Count;

            stopIndex = mvect_scan_start_index[current_scan];
            for (var index = startIndex; index < stopIndex; index++)
            {
                if (mbln_is_multiplexed_data)
                {
                    mxed_rec = mvect_mxed_data[index];
                    bin = mxed_rec.tof_bin;
                    mz = GetMassFromBin(bin);
                    if (mz < min_mz)
                        min_mz = mz;
                    if (mz > max_mz)
                        max_mz = mz;
                }
                else if (mbln_is_adc_data)
                {
                    adc_rec = mvect_adc_data[index];
                    bin = adc_rec.tof_bin;
                    mz = GetMassFromBin(bin);
                    if (mz < min_mz)
                        min_mz = mz;
                    if (mz > max_mz)
                        max_mz = mz;
                }
                else
                {
                    rec = mvect_data[index];
                    bin = rec.tof_bin;
                    mz = GetMassFromBin(bin);
                    if (mz < min_mz)
                        min_mz = mz;
                    if (mz > max_mz)
                        max_mz = mz;
                }
            }

            for (var scan_num = start_scan; scan_num <= stop_scan; scan_num++)
            {
                //if it goes out of bounds, then return
                if (scan_num > mint_num_scans || scan_num <= 0)
                    return;

                startIndex = mvect_scan_start_index[scan_num - 1];
                stopIndex = mvect_data.Count;

                if (scan_num < mint_num_scans)
                    stopIndex = mvect_scan_start_index[scan_num];

                for (var index = startIndex; index < stopIndex; index++)
                {
                    bin = 0;
                    double intensity_val = 0;
                    if (mbln_is_multiplexed_data)
                    {
                        mxed_rec = mvect_mxed_data[index];
                        bin = mxed_rec.tof_bin;
                        intensity_val = mxed_rec.intensity;
                    }
                    else if (mbln_is_adc_data)
                    {
                        adc_rec = mvect_adc_data[index];
                        bin = adc_rec.tof_bin;
                        intensity_val = adc_rec.intensity;
                    }
                    else
                    {
                        rec = mvect_data[index];
                        bin = rec.tof_bin;
                        intensity_val = rec.intensity;
                    }

                    mz = GetMassFromBin(bin);

                    if (mz >= min_mz && mz <= max_mz)
                    {
                        if (bin_intensity_map.ContainsKey(bin))
                        {
                            bin_intensity_map[bin] += intensity_val;
                        }
                        else
                        {
                            // Add the value
                            bin_intensity_map[bin] = intensity_val;
                        }
                    }
                }
            }

            foreach (var item in bin_intensity_map)
            {
                mz = this.GetMassFromBin(item.Key);
                if (mz >= min_mz && mz <= max_mz)
                {
                    mzs.Add(mz);
                    intensities.Add(item.Value);
                }
            }
        }
    }
}
