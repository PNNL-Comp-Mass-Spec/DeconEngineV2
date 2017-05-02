using System;
using System.Collections.Generic;
using System.IO;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;
using DeconToolsV2.Readers;
using Engine.DTAProcessing;
using Engine.Utilities;

namespace DeconToolsV2
{
    public enum enmProcessState
    {
        IDLE = 0,
        RUNNING,
        COMPLETE,
        ERROR
    };

    /// <summary>
    /// Used by DeconMSn
    /// </summary>
    public class clsProcRunner
    {
        private int mint_percent_done;
        private int mint_current_scan;
        private enmProcessState menm_state;
        private string mstr_file_name;
        private string mstr_output_path_for_dta_creation;
        private FileType menm_file_type;
        private clsPeakProcessorParameters mobj_peak_parameters;
        private clsHornTransformParameters mobj_transform_parameters;
        private clsRawDataPreprocessOptions mobj_fticr_preprocess_parameters;
        private Results.clsTransformResults mobj_results;
        private DTAGeneration.clsDTAGenerationParameters mobj_dta_generation_parameters;

        //public void OutfileNames(char* fileName, char* iso_file_name, char* scan_file_name, char* data_file_name);

        public void Reset()
        {
            mint_percent_done = 0;
            mint_current_scan = 0;
            menm_state = enmProcessState.IDLE;
        }

        public int CurrentScanNum
        {
            get
            {
                if (menm_state == enmProcessState.IDLE)
                    return 0;
                return mint_current_scan;
            }
        }

        public int PercentDone
        {
            get
            {
                if (menm_state == enmProcessState.IDLE)
                    return 0;

                if (menm_state == enmProcessState.RUNNING)
                    return mint_percent_done;

                return 100;
            }
        }

        public enmProcessState ProcessState => menm_state;

        public string FileName
        {
            get => mstr_file_name;

            set => mstr_file_name = value;
        }

        public FileType FileType
        {
            get => menm_file_type;

            set => menm_file_type = value;
        }

        public string OutputPathForDTACreation
        {
            get => mstr_output_path_for_dta_creation;

            set => mstr_output_path_for_dta_creation = value;
        }

        public int trial => 1;

        public clsPeakProcessorParameters PeakProcessorParameters
        {
            get => mobj_peak_parameters;

            set => mobj_peak_parameters = value;
        }

        public clsHornTransformParameters HornTransformParameters
        {
            get => mobj_transform_parameters;

            set => mobj_transform_parameters = value;
        }

        public Results.clsTransformResults HornTransformResults => mobj_results;

        public DTAGeneration.clsDTAGenerationParameters DTAGenerationParameters
        {
            get => mobj_dta_generation_parameters;

            set => mobj_dta_generation_parameters = value;
        }

        public clsRawDataPreprocessOptions FTICRPreprocessOptions
        {
            get => mobj_fticr_preprocess_parameters;

            set => mobj_fticr_preprocess_parameters = value;
        }

        public clsProcRunner()
        {
            mint_percent_done = 0;
            menm_state = enmProcessState.IDLE;
            mstr_file_name = null;
            mstr_output_path_for_dta_creation = null;
            mobj_peak_parameters = new clsPeakProcessorParameters();
            mobj_transform_parameters = new clsHornTransformParameters();
            mobj_dta_generation_parameters = new DTAGeneration.clsDTAGenerationParameters();
            mobj_results = null;
        }

        [Obsolete("Appears unused")]
        private Results.clsTransformResults CreateTransformResults(string file_name,
            FileType file_type, clsPeakProcessorParameters peak_parameters,
            clsHornTransformParameters transform_parameters,
            clsRawDataPreprocessOptions fticr_preprocess_parameters,
            bool save_peaks, bool transform)
        {
            if (menm_state == enmProcessState.RUNNING)
            {
                throw new Exception(
                    "Process already running in clsProcRunner. Cannot run two processes with same object");
            }

            if (string.IsNullOrEmpty(file_name))
            {
                throw new Exception("Please enter a file name to process");
            }

            if (peak_parameters == null)
            {
                throw new Exception("Please specify peak processing parameters.");
            }
            if (transform_parameters == null)
            {
                throw new Exception("Please specify mass transform parameters.");
            }

            Engine.HornTransform.MassTransform mass_transform = null;
            SavGolSmoother sgSmoother = null;

            //      Engine.ResultChecker.LCMSCheckResults *lcms_checker = null;
            var transform_results = new Results.clsTransformResults();

            try
            {
                var vect_transform_records = new List<clsHornTransformResults>();
                // while the thresholded parameter is already set in the clsPeakProcessParameters, we would
                // like to override that here if the data type is Finnigan because that data is threshold.
                bool thresholded;
                if (file_type == FileType.FINNIGAN ||
                    file_type == FileType.MZXMLRAWDATA)
                    thresholded = true;
                else
                    thresholded = peak_parameters.ThresholdedData;

                mint_percent_done = 0;
                menm_state = enmProcessState.RUNNING;

                // Create a RawData object and read through each scan and discover peaks.

                var file_name_ch = file_name;

                // enumerations of file type are the same in Readers namespace and
                // DeconWrapperManaged namespace.
                var raw_data = Engine.Readers.ReaderFactory.GetRawData(file_type, file_name_ch);
                if (file_type == FileType.ICR2LSRAWDATA && fticr_preprocess_parameters != null)
                {
                    var icr_raw_data = (Engine.Readers.Icr2lsRawData)raw_data;
                    icr_raw_data.SetApodizationZeroFillOptions(
                        fticr_preprocess_parameters.ApodizationType,
                        fticr_preprocess_parameters.ApodizationMinX, fticr_preprocess_parameters.ApodizationMaxX,
                        fticr_preprocess_parameters.ApodizationPercent, fticr_preprocess_parameters.NumZeroFills);
                    if (fticr_preprocess_parameters.ApplyCalibration)
                    {
                        icr_raw_data.OverrideDefaultCalibrator(
                            fticr_preprocess_parameters.CalibrationType,
                            fticr_preprocess_parameters.A, fticr_preprocess_parameters.B,
                            fticr_preprocess_parameters.C);
                    }
                }

                if (raw_data == null)
                {
                    throw new Exception(string.Concat("Could not open raw file: ", file_name));
                }

                var lcms_results = new Engine.Results.LCMSTransformResults();

                var peak_processor = new Engine.PeakProcessing.PeakProcessor();
                var original_peak_processor = new Engine.PeakProcessing.PeakProcessor();

                //lcms_checker = new Engine.ResultChecker.LCMSCheckResults();
                // Set parameters for discovering peaks. intensity threshold is set below.
                peak_processor.SetOptions(peak_parameters.SignalToNoiseThreshold, 0, thresholded,
                    (Engine.PeakProcessing.PeakFitType)peak_parameters.PeakFitType);
                original_peak_processor.SetOptions(peak_parameters.SignalToNoiseThreshold, 0, thresholded,
                    (Engine.PeakProcessing.PeakFitType)peak_parameters.PeakFitType);

                if (transform)
                {
                    mass_transform = new Engine.HornTransform.MassTransform
                    {
                        ElementalIsotopeComposition = transform_parameters.ElementIsotopeComposition
                    };

                    var tag_formula = "";

                    //mass_transform.SetOptions(transform_parameters.MaxCharge, transform_parameters.MaxMW, transform_parameters.MaxFit,
                    //  transform_parameters.MinS2N, transform_parameters.CCMass,transform_parameters.DeleteIntensityThreshold,
                    //  transform_parameters.MinIntensityForScore, transform_parameters.NumPeaksForShoulder,
                    //  transform_parameters.CheckAllPatternsAgainstCharge1, transform_parameters.UseMercuryCaching, transform_parameters.O16O18Media);

                    mass_transform.SetOptions(transform_parameters.MaxCharge, transform_parameters.MaxMW,
                                              transform_parameters.MaxFit,
                                              transform_parameters.MinS2N, transform_parameters.CCMass,
                                              transform_parameters.DeleteIntensityThreshold,
                                              transform_parameters.MinIntensityForScore, transform_parameters.NumPeaksForShoulder,
                                              transform_parameters.CheckAllPatternsAgainstCharge1, transform_parameters.UseMercuryCaching,
                                              transform_parameters.O16O18Media, transform_parameters.LeftFitStringencyFactor,
                                              transform_parameters.RightFitStringencyFactor, transform_parameters.IsActualMonoMZUsed);

                    var averagine_formula = transform_parameters.AveragineFormula;
                    if (transform_parameters.TagFormula != null)
                    {
                        tag_formula = transform_parameters.TagFormula;
                    }
                    mass_transform.SetIsotopeFitOptions(averagine_formula, tag_formula,
                                                        transform_parameters.ThrashOrNot, transform_parameters.CompleteFit);
                    mass_transform.IsotopeFitType = transform_parameters.IsotopeFitType;

                }

                var vect_mzs = new List<double>();
                var vect_intensities = new List<double>();
                var temp_vect_mzs = new List<double>();
                var temp_vect_intensities = temp_vect_mzs;

                var start_t = DateTime.Now;

                var min_scan = 1;
                if (transform_parameters.UseScanRange && transform_parameters.MinScan > 1)
                    min_scan = transform_parameters.MinScan;
                if (min_scan < raw_data.GetFirstScanNum())
                    min_scan = raw_data.GetFirstScanNum();
                var max_scan = raw_data.GetLastScanNum();
                if (transform_parameters.UseScanRange && transform_parameters.MaxScan < max_scan)
                    max_scan = transform_parameters.MaxScan;

               
                if (transform_parameters.UseSavitzkyGolaySmooth)
                {
                    sgSmoother = new SavGolSmoother(transform_parameters.SGNumLeft,
                        transform_parameters.SGNumRight, transform_parameters.SGOrder);
                }

                var transform_time = 0;

                //2009-04-03 [gord] will no longer use the SlidingWindow. It has litte speed benefit and there might be a bug
                /*if (transform_parameters.SumSpectraAcrossScanRange())
                {
                    if (file_type == PNNL_UIMF)
                        ((Engine.Readers.UIMFRawData *)raw_data).InitializeSlidingWindow(transform_parameters.NumScansToSumOver());
                    else if (file_type == PNNL_IMS)
                        ((Engine.Readers.IMSRawData *)raw_data).InitializeSlidingWindow(transform_parameters.NumScansToSumOver());
                }*/

                for (var scan_num = min_scan;
                    scan_num <= max_scan && scan_num != -1;
                    scan_num = raw_data.GetNextScanNum(scan_num))
                {
                    peak_processor.Clear();
                    original_peak_processor.Clear();
                    mint_current_scan = scan_num;
                    if (min_scan != max_scan)
                        mint_percent_done = ((scan_num - min_scan) * 100) / (max_scan - min_scan);
                    vect_mzs.Clear();
                    vect_intensities.Clear();
                    temp_vect_mzs.Clear();
                    temp_vect_intensities.Clear();

                    //Check if it needs to be processed
                    short scan_ms_level;
                    bool centroid;
                    if (raw_data.IsMSScan(scan_num))
                    {
                        scan_ms_level = 1;
                        centroid = false;
                    }
                    else
                    {
                        scan_ms_level = 2;
                        centroid = DTAGenerationParameters.CentroidMSn;
                    }
                    if (scan_ms_level != 1 && !transform_parameters.ProcessMSMS)
                        continue;

                    if (scan_ms_level != 1 && transform_parameters.ProcessMSMS &&
                        !raw_data.IsFTScan(scan_num))
                        continue;

                    //Get this scan first
                    raw_data.GetRawData(out vect_mzs, out vect_intensities, scan_num, centroid);
                    raw_data.GetRawData(out temp_vect_mzs, out temp_vect_intensities, scan_num, centroid);

                    // ------------------------------ Spectra summing ----------------------------------
                    if (transform_parameters.SumSpectra) // sum all spectra
                    {
                        double min_mz;
                        double max_mz;
                        if (transform_parameters.UseMZRange)
                        {
                            min_mz = transform_parameters.MinMZ;
                            max_mz = transform_parameters.MaxMZ;
                        }
                        else
                        {
                            var num_mzs_this_scan = vect_mzs.Count;
                            min_mz = vect_mzs[0];
                            max_mz = vect_mzs[num_mzs_this_scan - 1];
                        }
                        vect_mzs.Clear();
                        vect_intensities.Clear();
                        raw_data.GetSummedSpectra(out vect_mzs, out vect_intensities, min_scan, max_scan, min_mz, max_mz);
                        scan_num = max_scan;
                        mint_percent_done = 50;
                    }
                    else if (transform_parameters.SumSpectraAcrossScanRange) // sum across range
                    {
                        //// AM: Save original intensity of peaks prior to peaks
                        if (transform_parameters.ZeroFill)
                        {
                            Interpolation.ZeroFillMissing(ref vect_mzs, ref vect_intensities, transform_parameters.NumZerosToFill);
                        }
                        if (transform_parameters.UseSavitzkyGolaySmooth)
                        {
                            sgSmoother.Smooth(ref vect_mzs, ref vect_intensities);
                        }
                        var orig_thres = DeconEngine.Utils.GetAverage(vect_intensities, float.MaxValue);
                        var orig_background_intensity = DeconEngine.Utils.GetAverage(vect_intensities,
                            (float)(5 * orig_thres));
                        original_peak_processor.SetPeakIntensityThreshold(orig_background_intensity *
                                                                          peak_parameters.PeakBackgroundRatio);
                        if (mobj_transform_parameters.UseMZRange)
                            original_peak_processor.DiscoverPeaks(temp_vect_mzs,
                                                                  temp_vect_intensities, mobj_transform_parameters.MinMZ,
                                                                  mobj_transform_parameters.MaxMZ);
                        else
                            original_peak_processor.DiscoverPeaks(temp_vect_mzs,
                                                                  temp_vect_intensities);

                        // now sum
                        vect_mzs.Clear();
                        vect_intensities.Clear();
                        var scan_range = transform_parameters.NumScansToSumOver;

                        //2009-04-03 [gord] will no longer use the SlidingWindow. It has litte speed benefit and there might be a bug
                        //if (file_type == PNNL_UIMF)
                        //  ((Engine.Readers.UIMFRawData *)raw_data).GetSummedSpectraSlidingWindow(&vect_mzs, &vect_intensities, scan_num, scan_range);
                        //  //((Engine.Readers.UIMFRawData *)raw_data).GetSummedSpectra(&vect_mzs, &vect_intensities, scan_num, scan_range);  //Gord added this

                        //else if (file_type == PNNL_IMS)
                        //  ((Engine.Readers.IMSRawData *)raw_data).GetSummedSpectraSlidingWindow(&vect_mzs, &vect_intensities, scan_num, scan_range);
                        //else

                        raw_data.GetSummedSpectra(out vect_mzs, out vect_intensities, scan_num, scan_range);
                    }

                    var scan_time = raw_data.GetScanTime(scan_num);
                    //if (file_type == PNNL_UIMF)
                    //{
                    //  //drift_time =  ((Engine.Readers.UIMFRawData *)raw_data).GetDriftTime(scan_num);
                    //}
                    //else
                    //  drift_time = - 1;

                    if (vect_mzs.Count == 0)
                    {
                        if (save_peaks)
                        {
                            lcms_results.AddInfoForScan(scan_num, 0, 0, 0, 0, 0, 0, scan_time, scan_ms_level);

                            /*if (file_type == PNNL_UIMF)
                            lcms_results.AddInfoForUIMFScan(scan_num, 0, 0, 0, 0, 0, 0, scan_time, scan_ms_level, drift_time);
                        else
                            lcms_results.AddInfoForScan(scan_num, 0, 0, 0, 0, 0, 0, scan_time, scan_ms_level);*/
                        }
                        continue;
                    }

                    //------------------------------------- Zero fill --------------------------------------
                    if (transform_parameters.ZeroFill)
                    {
                        Interpolation.ZeroFillMissing(ref vect_mzs, ref vect_intensities, transform_parameters.NumZerosToFill);
                    }

                    // ------------------------------------ Smooth -----------------------------------------
                    if (transform_parameters.UseSavitzkyGolaySmooth)
                    {
                        sgSmoother.Smooth(ref vect_mzs, ref vect_intensities);
                    }

                    var minMZ = vect_mzs[0];
                    var maxMZ = vect_mzs[vect_mzs.Count - 1];

                    var thres = DeconEngine.Utils.GetAverage(vect_intensities, float.MaxValue);
                    var background_intensity = DeconEngine.Utils.GetAverage(vect_intensities, (float)(5 * thres));

                    double bpi = 0, bp_mz = 0;

                    double tic_intensity;
                    if (mobj_transform_parameters.UseMZRange)
                    {
                        tic_intensity = DeconEngine.Utils.GetTIC(mobj_transform_parameters.MinMZ,
                            mobj_transform_parameters.MaxMZ, ref vect_mzs, ref vect_intensities,
                            (float)(background_intensity * peak_parameters.PeakBackgroundRatio),
                            ref bpi, ref bp_mz);
                    }
                    else
                    {
                        tic_intensity = DeconEngine.Utils.GetTIC(400.0, 2000.0, ref vect_mzs, ref vect_intensities,
                            (float)(background_intensity * peak_parameters.PeakBackgroundRatio), ref bpi, ref bp_mz);
                    }

                    peak_processor.SetPeakIntensityThreshold(background_intensity * peak_parameters.PeakBackgroundRatio);
                    int numPeaks;
                    if (mobj_transform_parameters.UseMZRange)
                    {
                        numPeaks = peak_processor.DiscoverPeaks(vect_mzs, vect_intensities,
                            mobj_transform_parameters.MinMZ, mobj_transform_parameters.MaxMZ);

                    }
                    else
                    {
                        numPeaks = peak_processor.DiscoverPeaks(vect_mzs, vect_intensities);
                    }

                    if (save_peaks)
                    {
                        lcms_results.AddPeaksForScan(scan_num, peak_processor.PeakData.PeakTops);
                    }

                    var numDeisotoped = 0;

                    // ------------------------ Mass Transform -----------------------------------------
                    if (transform)
                    {
                        var min_peptide_intensity = background_intensity *
                                                       transform_parameters.PeptideMinBackgroundRatio;
                        if (transform_parameters.UseAbsolutePeptideIntensity)
                        {
                            if (min_peptide_intensity < transform_parameters.AbsolutePeptideIntensity)
                                min_peptide_intensity = transform_parameters.AbsolutePeptideIntensity;
                        }

                        clsPeak currentPeak;
                        var originalPeak = new clsPeak();

                        peak_processor.PeakData.InitializeUnprocessedPeakData();

                        var found = peak_processor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);

                        mass_transform.Reset();
                        vect_transform_records.Clear();
                        while (found)
                        {
                            if (currentPeak.Intensity < min_peptide_intensity)
                                break;

                            try
                            {
                                clsHornTransformResults transformRecord;
                                var found_transform = mass_transform.FindTransform(peak_processor.PeakData,
                                                                                   ref currentPeak, out transformRecord, background_intensity);

                                // AM (anoop?): if summing over a window, reinsert the original intensity     // [gord]  why?
                                if (found_transform && transformRecord.ChargeState <= transform_parameters.MaxCharge
                                    && transform_parameters.SumSpectraAcrossScanRange)
                                {
                                    original_peak_processor.PeakData.InitializeUnprocessedPeakData();
                                    originalPeak.Intensity = -1.0;
                                    originalPeak.Mz = -1.0;
                                    original_peak_processor.PeakData.FindPeak(
                                        transformRecord.Mz - 2 * transformRecord.FWHM,
                                        transformRecord.Mz + 2 * transformRecord.FWHM, out originalPeak);
                                    if (originalPeak.Intensity > 0)
                                    {
                                        transformRecord.Abundance = originalPeak.Intensity;
                                        transformRecord.Abundance = originalPeak.Intensity;
                                        transformRecord.FWHM = originalPeak.FWHM;
                                        // [gord] this might be the source of why FWHM is sometimes 0
                                    }
                                    else
                                    {
                                        found_transform = false; //AM : do not add to results
                                    }
                                }

                                if (found_transform && transformRecord.ChargeState <= transform_parameters.MaxCharge)
                                {
                                    numDeisotoped++;
                                    transformRecord.ScanNum = scan_num;

                                    if (transform_parameters.IsActualMonoMZUsed)
                                    {
                                        //retrieve experimental monoisotopic peak
                                        var monoPeakIndex = transformRecord.IsotopePeakIndices[0];
                                        clsPeak monoPeak;
                                        peak_processor.PeakData.GetPeak(monoPeakIndex, out monoPeak);

                                        //set threshold at 20% less than the expected 'distance' to the next peak
                                        var errorThreshold = 1.003 / transformRecord.ChargeState;
                                        errorThreshold = errorThreshold - errorThreshold * 0.2;

                                        var calc_monoMZ = transformRecord.MonoMw / transformRecord.ChargeState +
                                                             1.00727638;

                                        if (Math.Abs(calc_monoMZ - monoPeak.Mz) < errorThreshold)
                                        {
                                            transformRecord.MonoMw = monoPeak.Mz * transformRecord.ChargeState -
                                                                     1.00727638 * transformRecord.ChargeState;
                                        }
                                    }
                                    vect_transform_records.Add(transformRecord);
                                }
                                found = peak_processor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                            }
                            catch (Exception ex)
                            {
#if DEBUG
                                throw;
#endif
                                Console.WriteLine("Error in CreateTransformResults: " + ex.Message);
                                Console.WriteLine("Scan {0}, peak {1}", scan_num, currentPeak.PeakIndex);
                                Console.WriteLine(PRISM.clsStackTraceFormatter.GetExceptionStackTraceMultiLine(ex));
                            }
                        }

                        lcms_results.AddTransforms(vect_transform_records);
                        //  lcms_checker.AddTransformsToCheck(vect_transform_records);
                        if (file_type != FileType.PNNL_UIMF)
                        //if (file_type != PNNL_UIMF && scan_num % 20 == 0)
                        {
                            int iso_time = 0,
                                fit_time = 0,
                                cs_time = 0,
                                get_fit_score_time = 0,
                                remainder_time = 0,
                                find_peak_calc = 0,
                                find_peak_cached = 0;

                            var current_t = DateTime.Now;
                            var all = (int)(current_t - start_t).Ticks;

                            //mass_transform.GetProcessingTimes(out cs_time, out ac_time, out spline_time, out iso_time,
                            //    out fit_time,
                            //    out remainder_time, out get_fit_score_time, out find_peak_calc, out find_peak_cached);

                            Console.WriteLine(string.Concat("Scan # =", Convert.ToString(scan_num),
                                " CS= ", Convert.ToString(cs_time),
                                " Isotope= ", Convert.ToString(iso_time),
                                " FitScore= ", Convert.ToString(fit_time),
                                " GetFitScore= ", Convert.ToString(get_fit_score_time),
                                " GetFitScore-Isotope-FitScore-FindPeak= ",
                                Convert.ToString(get_fit_score_time - fit_time - find_peak_cached - find_peak_calc -
                                                 iso_time),
                                //                          " Raw Reading Time = ", Convert.ToString(raw_data_read_time),
                                //                          " PreProcessing Time = ", Convert.ToString(preprocessing_time),
                                " Transform= ", Convert.ToString(transform_time),
                                " Remaining= ", Convert.ToString(remainder_time),
                                " transform-cs-get_fit= ",
                                Convert.ToString(transform_time - cs_time - get_fit_score_time),
                                " All= ", Convert.ToString(all)
                            //                          " all-transform-preprocess-read= ", Convert.ToString(all-transform_time-preprocessing_time-raw_data_read_time)
                            ));
                        }
                    }
                    if (save_peaks)
                    {
                        var signal_range = raw_data.GetSignalRange(scan_num, centroid);
                        lcms_results.AddInfoForScan(scan_num, bp_mz, bpi, tic_intensity, signal_range, numPeaks,
                            numDeisotoped, scan_time, scan_ms_level);
                        /*if (file_type == PNNL_UIMF)
                        lcms_results.AddInfoForUIMFScan(scan_num, bp_mz, bpi, tic_intensity, signal_range, numPeaks, numDeisotoped, scan_time, scan_ms_level, drift_time);
                    else
                        lcms_results.AddInfoForScan(scan_num, bp_mz, bpi, tic_intensity, signal_range, numPeaks, numDeisotoped, scan_time, scan_ms_level); */
                    }
                }

                transform_results.SetLCMSTransformResults(lcms_results);

                // Anoop: write out those ones which are probably wrong
                /*string output_file_str = file_name.Remove(dotIndex, mstr_file_name.Length - dotIndex);
                lcms_checker.WriteOutTransformsToCheck(output_file_str); */

                mint_percent_done = 100;
            }
            catch (Exception)
            {
                menm_state = enmProcessState.ERROR;
                throw;
            }
            menm_state = enmProcessState.COMPLETE;
            return transform_results;
        }

        public void CreateDTAFile()
        {
            //Main function to create DTA files

            //check if we have everything
            if (mobj_peak_parameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            if (mobj_transform_parameters == null)
            {
                throw new Exception("Horn Transform parameters not set.");
            }
            if (mobj_dta_generation_parameters == null)
            {
                throw new Exception("DTA Generation parameters not set.");
            }

            if (menm_state == enmProcessState.RUNNING)
            {
                throw new Exception(
                    "Process already running in clsProcRunner. Cannot run two processes with same object");
            }

            mint_percent_done = 0;
            menm_state = enmProcessState.RUNNING;

            var dta_processor = new DTAProcessor();
            var dta_scanType = new DTAScanTypeGeneration();

            if (!File.Exists(mstr_file_name))
            {
                Console.WriteLine("Error: File \"{0}\" does not exist. Please check the command line arguments.", mstr_file_name);
                return;
            }

            //Read the rawfile in
            using (var fin = new FileStream(mstr_file_name, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                if (!fin.CanRead)
                {
                    Console.WriteLine("Unable to open input file");
                    return;
                }
            }

            // Check input format
            var dotIndex = mstr_file_name.IndexOf(".", StringComparison.Ordinal);
            var input_file_format = mstr_file_name.Remove(0, dotIndex + 1);
            if ((input_file_format.ToLower() == "raw"))
                menm_file_type = FileType.FINNIGAN;
            else if (input_file_format.ToLower() == "mzxml")
                menm_file_type = FileType.MZXMLRAWDATA;
            else
            {
                throw new Exception("Invalid input file format.");
            }

            // Set output path and filename
            string output_file;
            if (mstr_output_path_for_dta_creation != null)
            {
                var slashIndex = mstr_file_name.LastIndexOf("\\", StringComparison.Ordinal);
                var raw_name_plus_extension = mstr_file_name.Remove(dotIndex, mstr_file_name.Length - dotIndex);
                var raw_name = raw_name_plus_extension.Remove(0, slashIndex);
                output_file = Path.Combine(mstr_output_path_for_dta_creation, Path.GetFileName(raw_name));
            }
            else
            {
                output_file = mstr_file_name.Remove(dotIndex, mstr_file_name.Length - dotIndex);
            }

            bool thresholded;
            if (menm_file_type == FileType.FINNIGAN ||
                menm_file_type == FileType.MZXMLRAWDATA)
                thresholded = true;
            else
                thresholded = mobj_peak_parameters.ThresholdedData;

            //Raw Object
            dta_processor.mobj_raw_data_dta =
                Engine.Readers.ReaderFactory.GetRawData(menm_file_type, mstr_file_name);
            dta_processor.menm_dataset_type = menm_file_type;

            //File name base for all dtas
            dta_processor.mch_output_file = output_file;

            //Datasetname
            var lastSlashIndex = mstr_file_name.LastIndexOf("\\", StringComparison.Ordinal);
            var data_name_plus_extension = mstr_file_name.Remove(dotIndex, mstr_file_name.Length - dotIndex);
            var data_name = data_name_plus_extension.Remove(0, lastSlashIndex + 1);
            dta_processor.mch_dataset_name = data_name;

            // File name for log file
            var create_log_file_only = false;
            dta_processor.mch_log_filename = output_file + "_DeconMSn_log.txt";

            //File name for profile data
            dta_processor.mch_profile_filename = output_file + "_profile.txt";

            if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.LOG)
            {
                create_log_file_only = true;
            }

            dta_processor.mbln_write_progress_file = false;
            dta_processor.mch_progress_filename = output_file + "_DeconMSn_progress.txt";

            if (mobj_dta_generation_parameters.WriteProgressFile)
            {
                dta_processor.mbln_write_progress_file = true;
            }

            //file name for composite dta file
            var create_composite_dta = false;
            if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.CDTA)
            {
                dta_processor.mch_comb_dta_filename = output_file + "_dta.txt";
                dta_processor.mfile_comb_dta =
                    new StreamWriter(new FileStream(dta_processor.mch_comb_dta_filename, FileMode.Create,
                        FileAccess.ReadWrite, FileShare.Read));
                create_composite_dta = true;
                dta_scanType.DTAScanTypeFilename = output_file + "_ScanType.txt";
                dta_scanType.DTAScanTypeFileWriter = new StreamWriter(new FileStream(dta_scanType.DTAScanTypeFilename, FileMode.Create,
                    FileAccess.ReadWrite, FileShare.Read));
                dta_scanType.RawDataReader = dta_processor.mobj_raw_data_dta;
            }
            //file name for .mgf file
            if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
            {
                dta_processor.mch_mgf_filename = ".mgf";
                dta_processor.mfile_mgf =
                    new StreamWriter(new FileStream(dta_processor.mch_mgf_filename, FileMode.Create,
                        FileAccess.ReadWrite, FileShare.Read));
            }

            //Settings
            var tag_formula = "";
            var averagine_formula = mobj_transform_parameters.AveragineFormula;
            if (mobj_transform_parameters.TagFormula != null)
                tag_formula = mobj_transform_parameters.TagFormula;

            //Check if any dtas have to be ignored
            var vect_msn_ignore = new List<int>();
            if (mobj_dta_generation_parameters.IgnoreMSnScans)
            {
                var numLevels = mobj_dta_generation_parameters.NumMSnLevelsToIgnore;
                for (var levelNum = 0; levelNum < numLevels; levelNum++)
                {
                    var level = mobj_dta_generation_parameters.get_MSnLevelToIgnore(levelNum);
                    vect_msn_ignore.Add(level);
                }
                vect_msn_ignore.Sort();
            }

            dta_processor.SetDTAOptions(mobj_dta_generation_parameters.MinIonCount,
                mobj_dta_generation_parameters.MinScan, mobj_dta_generation_parameters.MaxScan,
                mobj_dta_generation_parameters.MinMass, mobj_dta_generation_parameters.MaxMass,
                create_log_file_only, create_composite_dta, mobj_dta_generation_parameters.ConsiderChargeValue,
                mobj_dta_generation_parameters.ConsiderMultiplePrecursors, mobj_dta_generation_parameters.CentroidMSn,
                mobj_dta_generation_parameters.IsolationWindowSize,
                mobj_dta_generation_parameters.IsProfileDataForMzXML);

            dta_processor.SetPeakProcessorOptions(mobj_peak_parameters.SignalToNoiseThreshold, 0, thresholded,
                (Engine.PeakProcessing.PeakFitType)mobj_peak_parameters.PeakFitType);

            dta_processor.SetMassTransformOptions(mobj_transform_parameters.MaxCharge,
                mobj_transform_parameters.MaxMW,
                mobj_transform_parameters.MaxFit, mobj_transform_parameters.MinS2N, mobj_transform_parameters.CCMass,
                mobj_transform_parameters.DeleteIntensityThreshold, mobj_transform_parameters.MinIntensityForScore,
                mobj_transform_parameters.NumPeaksForShoulder, mobj_transform_parameters.UseMercuryCaching,
                mobj_transform_parameters.O16O18Media, averagine_formula, tag_formula,
                mobj_transform_parameters.ThrashOrNot, mobj_transform_parameters.CompleteFit,
                mobj_transform_parameters.CheckAllPatternsAgainstCharge1,
                mobj_transform_parameters.IsotopeFitType,
                mobj_transform_parameters.ElementIsotopeComposition);

            var svm_file = mobj_dta_generation_parameters.SVMParamFile;
            dta_processor.InitializeSVM(svm_file);
            dta_processor.SetPeakParametersLowResolution(mobj_peak_parameters.PeakBackgroundRatio,
                mobj_transform_parameters.PeptideMinBackgroundRatio);

            //begin process
            //stick in range
            var scan_num = mobj_dta_generation_parameters.MinScan;
            var msNScanIndex = 0;
            int num_scans;
            var low_resolution = false;
            var scan_start = scan_num;
            var nextProgressScan = scan_start + 50;

            if (mobj_dta_generation_parameters.MaxScan <= dta_processor.mobj_raw_data_dta.GetNumScans())
                num_scans = mobj_dta_generation_parameters.MaxScan;
            else
                num_scans = dta_processor.mobj_raw_data_dta.GetNumScans();

            while (scan_num <= num_scans)
            {
                mint_percent_done = (scan_num * 100) / num_scans;
                int parent_scan;
                if (dta_processor.mobj_raw_data_dta.IsMSScan(scan_num))
                {
                    //Get MS spectra
                    dta_processor.GetParentScanSpectra(scan_num, mobj_peak_parameters.PeakBackgroundRatio,
                        mobj_transform_parameters.PeptideMinBackgroundRatio);

                    int msN_scan;
                    for (msN_scan = scan_num + 1;
                        msN_scan < num_scans && !dta_processor.mobj_raw_data_dta.IsMSScan(msN_scan);
                        msN_scan++)
                    {
                        //GetMS level and see if it is to be ignored
                        if (mobj_dta_generation_parameters.IgnoreMSnScans)
                        {
                            var msN_level = dta_processor.mobj_raw_data_dta.GetMSLevel(msN_scan);
                            var found_msN_level = false;
                            foreach (var msnLevel in vect_msn_ignore)
                            {
                                if (msN_level == msnLevel)
                                {
                                    found_msN_level = true;
                                    break;
                                }
                            }

                            if (found_msN_level)
                                continue;
                        }
                        //Get msN spectra
                        dta_processor.GetMsNSpectra(msN_scan, mobj_peak_parameters.PeakBackgroundRatio,
                            mobj_transform_parameters.PeptideMinBackgroundRatio);
                        //Identify which is parent_scan
                        parent_scan = dta_processor.mobj_raw_data_dta.GetParentScan(msN_scan);
                        // AM Modified to recieve new spectra everytime if (parent_scan != scan_num) //MSN data
                        dta_processor.GetParentScanSpectra(parent_scan, mobj_peak_parameters.PeakBackgroundRatio,
                            mobj_transform_parameters.PeptideMinBackgroundRatio);

                        if (mobj_dta_generation_parameters.SpectraType == 0 ||
                            dta_processor.GetSpectraType(msN_scan) == (int)mobj_dta_generation_parameters.SpectraType)
                        {
                            bool dta_success;
                            if (dta_processor.IsFTData(parent_scan))
                            {
                                //Get charge and mono
                                dta_success = dta_processor.GenerateDTA(msN_scan, parent_scan);
                            }
                            else if (dta_processor.IsZoomScan(parent_scan))
                            {
                                dta_success = dta_processor.GenerateDTAZoomScans(msN_scan, parent_scan, msNScanIndex);
                            }
                            else
                            {
                                //Low res data
                                low_resolution = true;
                                dta_success = dta_processor.GenerateDTALowRes(msN_scan, parent_scan, msNScanIndex);
                            }
                            if (dta_success)
                            {
                                //write out dta
                                if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                                    dta_processor.WriteToMGF(msN_scan, parent_scan);
                                else
                                    dta_processor.WriteDTAFile(msN_scan, parent_scan);
                            }
                        }
                    }
                    // reinitialize scan_count appropriately
                    scan_num = msN_scan - 1;
                }
                else
                {
                    //Get msN spectra
                    var msN_scan = scan_num;

                    //GetMS level and see if it is to be ignored
                    if (mobj_dta_generation_parameters.IgnoreMSnScans)
                    {
                        var msN_level = dta_processor.mobj_raw_data_dta.GetMSLevel(msN_scan);
                        var found_msN_level = false;
                        foreach (var msLevel in vect_msn_ignore)
                        {
                            if (msN_level == msLevel)
                            {
                                found_msN_level = true;
                                break;
                            }
                        }

                        if (found_msN_level)
                        {
                            scan_num++; // make sure an increment happens
                            continue;
                        }
                    }

                    dta_processor.GetMsNSpectra(msN_scan, mobj_peak_parameters.PeakBackgroundRatio,
                        mobj_transform_parameters.PeptideMinBackgroundRatio);

                    if (mobj_dta_generation_parameters.SpectraType == 0 ||
                        dta_processor.GetSpectraType(msN_scan) == (int)mobj_dta_generation_parameters.SpectraType)
                    {
                        //Identify which is parent_scan
                        parent_scan = dta_processor.mobj_raw_data_dta.GetParentScan(msN_scan);
                        // check to see if valid parent which wont be in MRM cases where MSn begins at 1.
                        if (parent_scan < 1)
                        {
                            scan_num++;
                            continue; //no dta is generated
                        }

                        // get parent data
                        dta_processor.GetParentScanSpectra(parent_scan, mobj_peak_parameters.PeakBackgroundRatio,
                            mobj_transform_parameters.PeptideMinBackgroundRatio);

                        if (dta_processor.IsFTData(parent_scan))
                        {
                            //Get charge and mono
                            var dta_success = dta_processor.GenerateDTA(msN_scan, parent_scan);
                            if (dta_success)
                            {
                                //write out dta
                                if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                                    dta_processor.WriteToMGF(msN_scan, parent_scan);
                                else
                                    dta_processor.WriteDTAFile(msN_scan, parent_scan);
                            }
                        }
                        else
                        {
                            //Low res data
                            low_resolution = true;
                            var dta_success = dta_processor.GenerateDTALowRes(msN_scan, parent_scan, msNScanIndex);
                            if (dta_success)
                            {
                                if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                                    dta_processor.WriteToMGF(msN_scan, parent_scan);
                                else
                                    dta_processor.WriteDTAFile(msN_scan, parent_scan);
                            }
                        }
                    }
                }
                scan_num++;

                if (dta_processor.mbln_write_progress_file)
                {
                    if (scan_num - scan_start >= nextProgressScan)
                    {
                        dta_processor.WriteProgressFile(scan_num - scan_start, num_scans - scan_start + 1, mint_percent_done);

                        nextProgressScan += 50;
                        while (nextProgressScan <= scan_num - scan_start)
                            nextProgressScan += 50;
                    }
                }
            }

            if (low_resolution && mobj_dta_generation_parameters.ConsiderChargeValue == 0)
            {
                Console.WriteLine("Determining charge");
                dta_processor.DetermineChargeForEachScan();
                Console.WriteLine("Generating DTAs for low-resolution data");
                if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                    dta_processor.WriteLowResolutionMGFFile();
                else
                    dta_processor.WriteLowResolutionDTAFile();
            }

            if (create_composite_dta)
            {
                try
                {
                    Console.WriteLine("Writing to file " + dta_scanType.DTAScanTypeFilename);
                    dta_scanType.GenerateScanTypeFile();
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error writing _ScanType.txt file: " + ex.Message);
                    Console.WriteLine(PRISM.clsStackTraceFormatter.GetExceptionStackTraceMultiLine(ex));
                }
                finally
                {
                    dta_scanType.DTAScanTypeFileWriter.Close();
                }
            }

            mint_percent_done = 100;
            dta_processor.WriteProgressFile(scan_num - scan_start, num_scans - scan_start + 1, mint_percent_done);

            // Write out log file
            dta_processor.WriteLogFile();

            // Write out profile
            dta_processor.WriteProfileFile();
            //Shutdown
            //dta_processor.mfile_log.close();
            if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.CDTA)
            {
                dta_processor.mfile_comb_dta.Close();
            }
            if (mobj_dta_generation_parameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                dta_processor.mfile_mgf.Close();

            //Done
            menm_state = enmProcessState.COMPLETE;
        }

        [Obsolete("Appears unused")]
        public void CreateTransformResultWithPeaksOnly()
        {
            if (mstr_file_name == null)
            {
                throw new Exception("File name is not set.");
            }
            if (mobj_peak_parameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            mobj_results = CreateTransformResults(mstr_file_name, menm_file_type, mobj_peak_parameters,
                mobj_transform_parameters, mobj_fticr_preprocess_parameters, true, false);
        }

        [Obsolete("Appears unused")]
        public void CreateTransformResultWithNoPeaks()
        {
            if (mstr_file_name == null)
            {
                throw new Exception("File name is not set.");
            }
            if (mobj_peak_parameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            if (mobj_transform_parameters == null)
            {
                throw new Exception("Horn Transform parameters not set.");
            }
            mobj_results = CreateTransformResults(mstr_file_name, menm_file_type, mobj_peak_parameters,
                mobj_transform_parameters, mobj_fticr_preprocess_parameters, false, true);
        }

        [Obsolete("Appears unused")]
        public void CreateTransformResults()
        {
            if (mstr_file_name == null)
            {
                throw new Exception("File name is not set.");
            }
            if (mobj_peak_parameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            if (mobj_transform_parameters == null)
            {
                throw new Exception("Horn Transform parameters not set.");
            }
            mobj_results = CreateTransformResults(mstr_file_name, menm_file_type, mobj_peak_parameters,
                mobj_transform_parameters, mobj_fticr_preprocess_parameters, true, true);
        }

        [Obsolete("Artifact from C++ version", true)]
        public void MemoryTest()
        {
            /*
            int how_many = 6000000;
            int sizeofPeak = sizeof (Engine.PeakProcessing.Peak);
            int sizeofLcmsPeak = sizeof (Peak);

            List<Peak> mvect_peaks;
            mvect_peaks.Capacity = how_many;

            Peak lc_peak;
            Engine.PeakProcessing.Peak pk;
            for (int i = 0; i < how_many; i++)
            {
                lc_peak.mobj_peak = pk;
                lc_peak.mint_scan_num = 0;
                mvect_peaks.Add(lc_peak);
            }
            char* temp = new char[how_many * 2 * sizeofLcmsPeak];
            char* temp1 = new char[how_many * 2 * sizeofLcmsPeak];
            char* temp2 = new char[how_many * 2 * sizeofLcmsPeak];
            if (temp2 == null)
            {
                std.cerr << "UnSuccessful" << std.flush;
            }
            delete[] temp;
            delete[] temp1;
            delete[] temp2;

            mvect_peaks.Capacity = 2 * how_many;
            std.cerr << "Blah" << std.endl;*/
        }
    }
}