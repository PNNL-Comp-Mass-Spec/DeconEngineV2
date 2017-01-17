using System;
using System.Collections.Generic;

namespace DeconToolsV2.Readers
{
    public enum FileType
    {
        [Obsolete("Only used by Decon2LS.UI", true)]
        BRUKER = 0,
        FINNIGAN,
        [Obsolete("Only used by Decon2LS.UI", true)]
        MICROMASSRAWDATA,
        [Obsolete("Only used by Decon2LS.UI", true)]
        AGILENT_TOF,
        [Obsolete("Only used by Decon2LS.UI", true)]
        SUNEXTREL,
        [Obsolete("Only used by Decon2LS.UI", true)]
        ICR2LSRAWDATA,
        MZXMLRAWDATA,
        [Obsolete("Only used by Decon2LS.UI", true)]
        PNNL_IMS,
        [Obsolete("Only used by Decon2LS.UI", true)]
        BRUKER_ASCII,
        [Obsolete("Only used by Decon2LS.UI", true)]
        ASCII,
        [Obsolete("Only used by Decon2LS.UI", true)]
        PNNL_UIMF,
        [Obsolete("Only used by Decon2LS.UI", true)]
        YAFMS,
        UNDEFINED
    }

#if Enable_Obsolete
    [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
    public class clsRawData
    {
        private Engine.Readers.RawData mobj_raw_data;
        private clsRawDataPreprocessOptions mobj_preprocess_options;

        public DeconToolsV2.Readers.FileType FileType
        {
            get
            {
                if (mobj_raw_data == null)
                    return DeconToolsV2.Readers.FileType.UNDEFINED;
                return (DeconToolsV2.Readers.FileType) mobj_raw_data.GetFileType();
            }
        }

        public DeconToolsV2.Readers.clsRawDataPreprocessOptions FTICRRawPreprocessOptions
        {
            get { return mobj_preprocess_options; }
            set
            {
                mobj_preprocess_options = value;
                if (mobj_raw_data != null && mobj_raw_data.GetFileType() == FileType.ICR2LSRAWDATA)
                {
                    Engine.Readers.Icr2lsRawData icr_raw_data = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
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
                int num_scans = mobj_raw_data.GetNumScans();
                if (num_scans != 0)
                {
                    int percent_done = (100 * mobj_raw_data.GetNumScansLoaded()) / num_scans;
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
                int current_scan = mobj_raw_data.GetNumScansLoaded();
                int num_scans = mobj_raw_data.GetNumScans();
                return string.Concat("Processed :", System.Convert.ToString(current_scan), " of ",
                    System.Convert.ToString(num_scans), " scans");
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

        public clsRawData(string file_name, DeconToolsV2.Readers.FileType file_type)
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
            Engine.Calibrations.Calibrator calib =
                new Engine.Calibrations.Calibrator(CalibrationType.A_OVER_F_PLUS_B);
            calib.NumPointsInScan = calSettings.TD;
            calib.LowMassFrequency = calSettings.FRLow;
            calib.SampleRate = calSettings.SW_h;
            calib.SetCalibrationEquationParams(calSettings.ML1, calSettings.ML2, 0.0);

            mobj_raw_data.SetCalibrator(calib);
            return;
        }

        public void LoadFile(string file_name, DeconToolsV2.Readers.FileType file_type)
        {
            // enumerations of file type are the same in Readers namespace and
            // DeconWrapperManaged namespace.
            mobj_raw_data = Engine.Readers.ReaderFactory.GetRawData(file_type);
            if (file_type == FileType.ICR2LSRAWDATA && mobj_preprocess_options != null &&
                mobj_preprocess_options.ApodizationType != ApodizationType.NOAPODIZATION)
            {
                Engine.Readers.Icr2lsRawData icr_raw_data = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
                icr_raw_data.SetApodizationZeroFillOptions(mobj_preprocess_options.ApodizationType,
                    mobj_preprocess_options.ApodizationMinX, mobj_preprocess_options.ApodizationMaxX,
                    mobj_preprocess_options.ApodizationPercent, mobj_preprocess_options.NumZeroFills);
            }
            mobj_raw_data.Load(file_name);
            if (file_type == FileType.ICR2LSRAWDATA && mobj_preprocess_options != null &&
                mobj_preprocess_options.ApplyCalibration)
            {
                Engine.Readers.Icr2lsRawData icr_raw_data = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
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
                throw new System.ApplicationException("Cannot close file because no file has been opened.");
            }
            mobj_raw_data.Close();
        }

        public int GetNumScans()
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("Cannot get number of scans because no file has been opened");
            }
            return mobj_raw_data.GetNumScans();
        }

        public int GetMSLevel(int scan_num)
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("Cannot get MS level because no file has been opened");
            }
            return mobj_raw_data.GetMSLevel(scan_num);
        }

        public double GetScanTime(int scan_num)
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("Cannot get scan time because no file has been opened");
            }
            return mobj_raw_data.GetScanTime(scan_num);
        }

        public int GetScanSize()
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("No file has been opened");
            }
            return mobj_raw_data.GetScanSize();
        }

        public short GetSpectrumType(int scan_num)
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("No file has been opened");
            }
            return mobj_raw_data.GetSpectrumType(scan_num);
        }

        public void GetTicFromFile(ref float[] intensities, ref float[] scan_times, bool base_peak_tic)
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("No file has been opened");
            }

            List<double> vect_intensities;
            List<double> vect_scan_times;
            mobj_raw_data.GetTicFromFile(out vect_intensities, out vect_scan_times, base_peak_tic);

            int num_pts = (int) vect_intensities.Count;
            intensities = new float[num_pts];
            scan_times = new float[num_pts];

            for (int i = 0; i < num_pts; i++)
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
                throw new System.ApplicationException("No file has been opened");
            }
            return (int) (mobj_raw_data.GetTICForScan(scan_num));
        }

        public void GetSummedSpectra(int start_scan, int stop_scan, double min_mz, double max_mz, ref double[] mzs,
            ref double[] intensities)
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("No file has been opened");
            }

            List<double> vect_mzs;
            List<double> vect_intensities;
            mobj_raw_data.GetSummedSpectra(out vect_mzs, out vect_intensities, start_scan, stop_scan, min_mz, max_mz);
            int num_pts = (int) vect_intensities.Count;
            intensities = new double[num_pts];
            mzs = new double[num_pts];

            for (int i = 0; i < num_pts; i++)
            {
                mzs[i] = vect_mzs[i];
                intensities[i] = vect_intensities[i];
            }
        }

        public void GetSummedSpectra(int current_scan, int scan_range, ref double[] mzs, ref double[] intensities)
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("No file has been opened");
            }

            List<double> vect_mzs = new List<double>();
            List<double> vect_intensities = new List<double>();
            mobj_raw_data.GetSummedSpectra(out vect_mzs, out vect_intensities, current_scan, scan_range);
            int num_pts = (int) vect_intensities.Count;
            intensities = new double[num_pts];
            mzs = new double[num_pts];

            for (int i = 0; i < num_pts; i++)
            {
                mzs[i] = vect_mzs[i];
                intensities[i] = vect_intensities[i];
            }
        }

        public void GetSpectrum(int scan_num, ref double[] mzs, ref double[] intensities, bool centroid)
        {
            if (mobj_raw_data == null)
            {
                throw new System.ApplicationException("No file has been opened");
            }
            List<double> vect_mzs = new List<double>();
            List<double> vect_intensities = new List<double>();
            mobj_raw_data.GetRawData(out vect_mzs, out vect_intensities, scan_num, centroid);
            int num_pts = (int) vect_intensities.Count;
            intensities = new double[num_pts];
            mzs = new double[num_pts];

            for (int i = 0; i < num_pts; i++)
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
            int index = 0;
            //get count first
            for (int i = 0; i < in_mzs.Length; i++)
            {
                if (in_mzs[i] >= (central_value - range) && in_mzs[i] <= (central_value + range))
                {
                    index++;
                }
            }
            out_intensities = new float[index];
            out_mzs = new float[index];
            index = 0;
            for (int i = 0; i < in_mzs.Length; i++)
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
            string description;
            mobj_raw_data.GetScanDescription(scan_num, out description);
            return description;
        }

        public double GetFTICRSamplingRate()
        {
            if (FileType != FileType.ICR2LSRAWDATA)
                throw new System.Exception("FileType is not FTICR or Transient information is not available");
            if (mobj_raw_data == null)
                throw new System.Exception("RawData not instantiated");

            Engine.Readers.Icr2lsRawData icrRawData = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
            return icrRawData.GetSampleRate();
        }

        public void GetFTICRTransient(ref float[] intensities)
        {
            if (FileType != FileType.ICR2LSRAWDATA)
                throw new System.Exception("FileType is not FTICR or Transient information is not available");
            if (mobj_raw_data == null)
                throw new System.Exception("RawData not instantiated");

            Engine.Readers.Icr2lsRawData icrRawData = (Engine.Readers.Icr2lsRawData) mobj_raw_data;
            List<float> vect_intensities = new List<float>();
            icrRawData.GetFTICRTransient(ref vect_intensities);

            int num_pts = (int) vect_intensities.Count;
            intensities = new float[num_pts];

            for (int i = 0; i < num_pts; i++)
            {
                intensities[i] = (float) vect_intensities[i];
            }
        }

        /*  public double GetDriftTime(int scanNum)
        {
        if (get_FileType() != FileType.PNNL_UIMF)
            throw new System.Exception("FileType is not UIMF");
        if (mobj_raw_data == null)
            throw new System.Exception("RawData not instantiated");

        Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;
        return uimfRawData.GetDriftTime(scanNum);
                }*/

        /*  public double GetFramePressure(int frameNum)
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new System.Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new System.Exception("RawData not instantiated");

            Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;
            return uimfRawData.GetFramePressure(frameNum);
        }*/

        /*public int GetNumOfFrames()
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new System.Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new System.Exception("RawData not instantiated");

            Engine.Readers.UIMFRawData *uimfRawData = (Engine.Readers.UIMFRawData *) mobj_raw_data;
            return uimfRawData.GetNumOfFrames();
        }*/

        /*public void GetSummedFrameSpectra(double (&mzs) __gc[], double (&intensities) __gc[],
            int startFrame, int endFrame, double min_mz, double max_mz, int imsScanNum)
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new System.Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new System.Exception("RawData not instantiated");

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
                throw new System.Exception(exception_msg);
            }
        }*/

        /*public void GetSummedFrameAndScanSpectra(DataReader *uimfDataReader, double (&mzs) __gc[],
            double (&intensities) __gc[], int start_frame, int end_frame, int ims_start_scan,
            int ims_end_scan, double min_mz, double max_mz, int numBins )
        {
            if (get_FileType() != FileType.PNNL_UIMF)
                throw new System.Exception("FileType is not UIMF");
            if (mobj_raw_data == null)
                throw new System.Exception("RawData not instantiated");

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
                throw new System.Exception(exception_msg);
            }
        }*/
    }
#endif
}
