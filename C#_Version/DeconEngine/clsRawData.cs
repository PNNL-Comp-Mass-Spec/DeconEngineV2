using System;
using System.Collections.Generic;

namespace DeconToolsV2.Readers
{
    public enum FileType
    {
        [Obsolete("Only used by Decon2LS.UI", false)]
        BRUKER = 0,
        THERMORAW = 1,
        FINNIGAN = THERMORAW,
        [Obsolete("Only used by Decon2LS.UI", false)]
        MICROMASSRAWDATA,
        [Obsolete("Only used by Decon2LS.UI", false)]
        AGILENT_TOF,
        [Obsolete("Only used by Decon2LS.UI", false)]
        SUNEXTREL,
        [Obsolete("Only used by Decon2LS.UI", false)]
        ICR2LSRAWDATA,
        MZXMLRAWDATA,
        [Obsolete("Only used by Decon2LS.UI", false)]
        PNNL_IMS,
        [Obsolete("Only used by Decon2LS.UI", false)]
        BRUKER_ASCII,
        [Obsolete("Only used by Decon2LS.UI", false)]
        ASCII,
        [Obsolete("Only used by Decon2LS.UI", false)]
        PNNL_UIMF,
        [Obsolete("Only used by Decon2LS.UI", false)]
        YAFMS,
        UNDEFINED
    }

    [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
    public class clsRawData
    {
        // Ignore Spelling: namespace

        private Engine.Readers.RawData mobj_raw_data;
        private clsRawDataPreprocessOptions mobj_preprocess_options;

        public FileType FileType
        {
            get
            {
                if (mobj_raw_data == null)
                    return FileType.UNDEFINED;
                return (FileType) mobj_raw_data.GetFileType();
            }
        }

        public clsRawDataPreprocessOptions FTICRRawPreprocessOptions
        {
            get => mobj_preprocess_options;
            set
            {
                mobj_preprocess_options = value;
                if (mobj_raw_data != null && mobj_raw_data.GetFileType() == FileType.ICR2LSRAWDATA)
                {
                    var icr_raw_data = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
                    icr_raw_data.SetApodizationZeroFillOptions(
                        mobj_preprocess_options.ApodizationType, mobj_preprocess_options.ApodizationMinX,
                        mobj_preprocess_options.ApodizationMaxX, mobj_preprocess_options.ApodizationPercent,
                        mobj_preprocess_options.NumZeroFills);
                    if (mobj_preprocess_options.ApplyCalibration)
                    {
                        icr_raw_data.OverrideDefaultCalibrator(
                            mobj_preprocess_options.CalibrationType, mobj_preprocess_options.A,
                            mobj_preprocess_options.B, mobj_preprocess_options.C);
                    }
                }
            }
        }

        public string FileName
        {
            get
            {
                if (mobj_raw_data == null)
                    return null;
                return mobj_raw_data.GetFileName();
            }
        }

        public int PercentDone
        {
            get
            {
                if (mobj_raw_data == null)
                    return 0;
                if (mobj_raw_data.GetFileType() == FileType.ASCII)
                {
                    return ((Engine.Readers.AsciiRawData) mobj_raw_data).GetPercentDone();
                }
                var num_scans = mobj_raw_data.GetNumScans();
                if (num_scans != 0)
                {
                    var percent_done = (100 * mobj_raw_data.GetNumScansLoaded()) / num_scans;
                    if (percent_done < 0)
                        return 0;
                    if (percent_done > 100)
                        return 100;
                    return percent_done;
                }
                return 0;
            }
        }

        public string StatusMessage
        {
            get
            {
                if (mobj_raw_data == null)
                    return "";
                var current_scan = mobj_raw_data.GetNumScansLoaded();
                var num_scans = mobj_raw_data.GetNumScans();
                return string.Concat("Processed :", Convert.ToString(current_scan), " of ",
                    Convert.ToString(num_scans), " scans");
            }
        }

        public bool IsFTScan(int scanNum)
        {
            return mobj_raw_data.IsFTScan(scanNum);
        }

        public clsRawData()
        {
            mobj_raw_data = null;
            mobj_preprocess_options = new clsRawDataPreprocessOptions();
        }

        public clsRawData(string file_name, FileType file_type)
        {
            LoadFile(file_name, file_type);
        }

        public int GetFirstScanNum()
        {
            if (mobj_raw_data == null)
                return 0;
            return mobj_raw_data.GetFirstScanNum();
        }

        public void SetFFTCalibrationValues(CalibrationSettings calSettings)
        {
            var calib =
                new Engine.Calibrations.Calibrator(CalibrationType.A_OVER_F_PLUS_B);
            calib.NumPointsInScan = calSettings.TD;
            calib.LowMassFrequency = calSettings.FRLow;
            calib.SampleRate = calSettings.SW_h;
            calib.SetCalibrationEquationParams(calSettings.ML1, calSettings.ML2, 0.0);

            mobj_raw_data.SetCalibrator(calib);
            return;
        }

        public void LoadFile(string file_name, FileType file_type)
        {
            // enumerations of file type are the same in Readers namespace and
            // DeconWrapperManaged namespace.
            mobj_raw_data = Engine.Readers.ReaderFactory.GetRawData(file_type);
            if (file_type == FileType.ICR2LSRAWDATA && mobj_preprocess_options != null &&
                mobj_preprocess_options.ApodizationType != ApodizationType.NOAPODIZATION)
            {
                var icr_raw_data = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
                icr_raw_data.SetApodizationZeroFillOptions(mobj_preprocess_options.ApodizationType,
                    mobj_preprocess_options.ApodizationMinX, mobj_preprocess_options.ApodizationMaxX,
                    mobj_preprocess_options.ApodizationPercent, mobj_preprocess_options.NumZeroFills);
            }
            mobj_raw_data.Load(file_name);
            if (file_type == FileType.ICR2LSRAWDATA && mobj_preprocess_options != null &&
                mobj_preprocess_options.ApplyCalibration)
            {
                var icr_raw_data = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
                icr_raw_data.OverrideDefaultCalibrator(mobj_preprocess_options.CalibrationType,
                    mobj_preprocess_options.A, mobj_preprocess_options.B, mobj_preprocess_options.C);
            }
        }

        /*public void LoadFile(char[] file_name, DeconToolsV2.Readers.FileType file_type)
        {
            LoadFile(new string(file_name), file_type);
        }*/

        public void Close()
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("Cannot close file because no file has been opened.");
            }
            mobj_raw_data.Close();
        }

        public int GetNumScans()
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("Cannot get number of scans because no file has been opened");
            }
            return mobj_raw_data.GetNumScans();
        }

        public int GetMSLevel(int scan_num)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("Cannot get MS level because no file has been opened");
            }
            return mobj_raw_data.GetMSLevel(scan_num);
        }

        public double GetScanTime(int scan_num)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("Cannot get scan time because no file has been opened");
            }
            return mobj_raw_data.GetScanTime(scan_num);
        }

        public int GetScanSize()
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("No file has been opened");
            }
            return mobj_raw_data.GetScanSize();
        }

        public short GetSpectrumType(int scan_num)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("No file has been opened");
            }
            return mobj_raw_data.GetSpectrumType(scan_num);
        }

        public void GetTicFromFile(ref float[] intensities, ref float[] scan_times, bool base_peak_tic)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("No file has been opened");
            }

            mobj_raw_data.GetTicFromFile(out var vect_intensities, out var vect_scan_times, base_peak_tic);

            var num_pts = (int) vect_intensities.Count;
            intensities = new float[num_pts];
            scan_times = new float[num_pts];

            for (var i = 0; i < num_pts; i++)
            {
                scan_times[i] = (float) vect_scan_times[i];
                intensities[i] = (float) vect_intensities[i];
            }
            return;
        }

        public int GetTIC(int scan_num)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("No file has been opened");
            }
            return (int) (mobj_raw_data.GetTICForScan(scan_num));
        }

        public void GetSummedSpectra(int start_scan, int stop_scan, double min_mz, double max_mz, ref double[] mzs,
            ref double[] intensities)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("No file has been opened");
            }

            mobj_raw_data.GetSummedSpectra(out var vect_mzs, out var vect_intensities, start_scan, stop_scan, min_mz, max_mz);
            var num_pts = (int) vect_intensities.Count;
            intensities = new double[num_pts];
            mzs = new double[num_pts];

            for (var i = 0; i < num_pts; i++)
            {
                mzs[i] = vect_mzs[i];
                intensities[i] = vect_intensities[i];
            }
        }

        public void GetSummedSpectra(int current_scan, int scan_range, ref double[] mzs, ref double[] intensities)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("No file has been opened");
            }

            var vect_mzs = new List<double>();
            var vect_intensities = new List<double>();
            mobj_raw_data.GetSummedSpectra(out vect_mzs, out vect_intensities, current_scan, scan_range);
            var num_pts = (int) vect_intensities.Count;
            intensities = new double[num_pts];
            mzs = new double[num_pts];

            for (var i = 0; i < num_pts; i++)
            {
                mzs[i] = vect_mzs[i];
                intensities[i] = vect_intensities[i];
            }
        }

        public void GetSpectrum(int scan_num, ref double[] mzs, ref double[] intensities, bool centroid)
        {
            if (mobj_raw_data == null)
            {
                throw new ApplicationException("No file has been opened");
            }
            var vect_mzs = new List<double>();
            var vect_intensities = new List<double>();
            mobj_raw_data.GetRawData(out vect_mzs, out vect_intensities, scan_num, centroid);
            var num_pts = (int) vect_intensities.Count;
            intensities = new double[num_pts];
            mzs = new double[num_pts];

            for (var i = 0; i < num_pts; i++)
            {
                mzs[i] = vect_mzs[i];
                intensities[i] = vect_intensities[i];
            }
        }

        public int GetParentScan(int scan_num)
        {
            //Given a scan number of an MS2 scan this identifies the parent scan
            if (mobj_raw_data == null)
                return 0;
            return mobj_raw_data.GetParentScan(scan_num);
        }

        public void GetMzsInRange(ref float[] in_mzs, ref float[] in_intensities, ref float[] out_mzs,
            ref float[] out_intensities, float central_value, float range)
        {
            var index = 0;
            //get count first
            for (var i = 0; i < in_mzs.Length; i++)
            {
                if (in_mzs[i] >= (central_value - range) && in_mzs[i] <= (central_value + range))
                {
                    index++;
                }
            }
            out_intensities = new float[index];
            out_mzs = new float[index];
            index = 0;
            for (var i = 0; i < in_mzs.Length; i++)
            {
                if (in_mzs[i] >= (central_value - range) && in_mzs[i] <= (central_value + range))
                {
                    out_mzs[index] = in_mzs[i];
                    out_intensities[index] = in_intensities[i];
                    index++;
                }
            }
        }

        public string GetScanDescription(int scan_num)
        {
            if (mobj_raw_data == null)
                return "";

            mobj_raw_data.GetScanDescription(scan_num, out var description);
            return description;
        }

        public double GetFTICRSamplingRate()
        {
            if (FileType != FileType.ICR2LSRAWDATA)
                throw new Exception("FileType is not FTICR or Transient information is not available");
            if (mobj_raw_data == null)
                throw new Exception("RawData not instantiated");

            var icrRawData = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
            return icrRawData.GetSampleRate();
        }

        public void GetFTICRTransient(ref float[] intensities)
        {
            if (FileType != FileType.ICR2LSRAWDATA)
                throw new Exception("FileType is not FTICR or Transient information is not available");
            if (mobj_raw_data == null)
                throw new Exception("RawData not instantiated");

            var icrRawData = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
            var vect_intensities = new List<float>();
            icrRawData.GetFTICRTransient(ref vect_intensities);

            var num_pts = (int) vect_intensities.Count;
            intensities = new float[num_pts];

            for (var i = 0; i < num_pts; i++)
            {
                intensities[i] = (float) vect_intensities[i];
            }
        }

        /*  public double GetDriftTime(int scanNum)
        {
        if (get_FileType() != FileType.PNNL_UIMF)
            throw new Exception("FileType is not UIMF");
        if (mobj_raw_data == null)
            throw new Exception("RawData not instantiated");

        Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;
        return uimfRawData.GetDriftTime(scanNum);
                }*/

        /*  public double GetFramePressure(int frameNum)
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new Exception("RawData not instantiated");

            Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;
            return uimfRawData.GetFramePressure(frameNum);
        }*/

        /*public int GetNumOfFrames()
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new Exception("RawData not instantiated");

            Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;
            return uimfRawData.GetNumOfFrames();
        }*/

        /*public void GetSummedFrameSpectra(double (&mzs) __gc[], double (&intensities) __gc[],
            int startFrame, int endFrame, double min_mz, double max_mz, int imsScanNum)
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new Exception("RawData not instantiated");

            Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;

            DataReader *uimfDataReader = new DataReader();
            List<double> vect_mzs;
            List<double> vect_intensities;
            try
            {
                bool chk = uimfRawData.YanGetSummedFrameSpectra(uimfDataReader, &vect_mzs, &vect_intensities, startFrame, endFrame,
                    imsScanNum, imsScanNum, min_mz, max_mz);
                int num_pts = (int) vect_intensities.Count;
                intensities = new double [num_pts];
                mzs = new double [num_pts];

                for (int i = 0; i < num_pts; i++)
                {
                    mzs[i] = vect_mzs[i];
                    intensities[i] = vect_intensities[i];
                }
                vect_mzs.Clear();
                vect_intensities.Clear();
            }
            catch (char *mesg)
            {
                string exception_msg = new string (mesg);
                throw new Exception(exception_msg);
            }
        }*/

        /*public void GetSummedFrameAndScanSpectra(DataReader *uimfDataReader, double (&mzs) __gc[],
            double (&intensities) __gc[], int start_frame, int end_frame, int ims_start_scan,
            int ims_end_scan, double min_mz, double max_mz, int numBins )
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new Exception("RawData not instantiated");

            string data_type;

            Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;
            UIMFLibrary.GlobalParameters *gp = uimfDataReader.GetGlobalParameters();
            data_type = gp.TOFIntensityType;

            List<double> vect_mzs;
            List<double> vect_intensities;
            try
            {
                bool chk = uimfRawData.YanGetSummedFrameAndScanSpectra(uimfDataReader, &vect_mzs,
                    &vect_intensities, start_frame, end_frame, ims_start_scan,ims_end_scan, min_mz, max_mz, numBins, data_type);
                int num_pts = (int) vect_intensities.Count;
                intensities = new double [num_pts];
                mzs = new double [num_pts];

                for (int i = 0; i < num_pts; i++)
                {
                    mzs[i] = vect_mzs[i];
                    intensities[i] = vect_intensities[i];
                }
                vect_mzs.Clear();
                vect_intensities.Clear();
            }
            catch (char *mesg)
            {
                string exception_msg = new string (mesg);
                throw new Exception(exception_msg);
            }
        }*/
    }
}
