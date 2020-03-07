using System;
using System.Collections.Generic;
using System.IO;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;
using Engine.HornTransform;
using Engine.PeakProcessing;
using DeconToolsV2.Readers;
using Engine.DTAProcessing;
using Engine.Utilities;

namespace DeconToolsV2
{

    [Obsolete("Only used by Decon2LS.UI", false)]
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
        private int _percentDone;
        private int _currentScan;

        //public void OutfileNames(char* fileName, char* iso_file_name, char* scan_file_name, char* data_file_name);

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void Reset()
        {
            _percentDone = 0;
            _currentScan = 0;
            ProcessState = enmProcessState.IDLE;
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public int CurrentScanNum
        {
            get
            {
                if (ProcessState == enmProcessState.IDLE)
                    return 0;
                return _currentScan;
            }
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public int PercentDone
        {
            get
            {
                if (ProcessState == enmProcessState.IDLE)
                    return 0;
                else if (ProcessState == enmProcessState.RUNNING)
                    return _percentDone;

                return 100;
            }
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public enmProcessState ProcessState { get; private set; }

        public string FileName { get; set; }

        public DeconToolsV2.Readers.FileType FileType { get; set; }

        public string OutputPathForDTACreation { get; set; }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public int trial => 1;

        public clsPeakProcessorParameters PeakProcessorParameters { get; set; }

        public clsHornTransformParameters HornTransformParameters { get; set; }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public Results.clsTransformResults HornTransformResults { get; private set; }

        public DTAGeneration.clsDTAGenerationParameters DTAGenerationParameters { get; set; }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public clsRawDataPreprocessOptions FTICRPreprocessOptions { get; set; }

        public clsProcRunner()
        {
            _percentDone = 0;

#pragma warning disable 618
            ProcessState = enmProcessState.IDLE;
            HornTransformResults = null;
#pragma warning restore 618
            FileName = null;
            OutputPathForDTACreation = null;
            PeakProcessorParameters = new DeconToolsV2.Peaks.clsPeakProcessorParameters();
            HornTransformParameters = new DeconToolsV2.HornTransform.clsHornTransformParameters();
            DTAGenerationParameters = new DeconToolsV2.DTAGeneration.clsDTAGenerationParameters();
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        private Results.clsTransformResults CreateTransformResults(string fileName, FileType fileType,
            clsPeakProcessorParameters peakParameters, clsHornTransformParameters transformParameters,
            clsRawDataPreprocessOptions fticrPreprocessParameters, bool savePeaks, bool transform)
        {
            if (ProcessState == enmProcessState.RUNNING)
            {
                throw new Exception(
                    "Process already running in clsProcRunner. Cannot run two processes with same object");
            }

            if (string.IsNullOrEmpty(fileName))
            {
                throw new Exception("Please enter a file name to process");
            }

            if (peakParameters == null)
            {
                throw new Exception("Please specify peak processing parameters.");
            }
            if (transformParameters == null)
            {
                throw new Exception("Please specify mass transform parameters.");
            }

            SavGolSmoother sgSmoother = null;

            //      Engine.ResultChecker.LCMSCheckResults *lcms_checker = null;
            var transformResults = new Results.clsTransformResults();

            try
            {
                var transformRecords = new List<clsHornTransformResults>();
                // while the thresholded parameter is already set in the clsPeakProcessParameters, we would
                // like to override that here if the data type is Finnigan because that data is threshold.
                bool thresholded;
                if (fileType == FileType.FINNIGAN ||
                    fileType == FileType.MZXMLRAWDATA)
                    thresholded = true;
                else
                    thresholded = peakParameters.ThresholdedData;

                _percentDone = 0;
                ProcessState = enmProcessState.RUNNING;

                // Create a RawData object and read through each scan and discover peaks.
                var file_name_ch = fileName;

                // enumerations of file type are the same in Readers namespace and
                // DeconWrapperManaged namespace.
                var rawData = Engine.Readers.ReaderFactory.GetRawData(fileType, fileName);
                if (fileType == FileType.ICR2LSRAWDATA && fticrPreprocessParameters != null)
                {
                    var icrRawData = (Engine.Readers.Icr2lsRawData) rawData;
                    icrRawData.SetApodizationZeroFillOptions(
                        fticrPreprocessParameters.ApodizationType,
                        fticrPreprocessParameters.ApodizationMinX, fticrPreprocessParameters.ApodizationMaxX,
                        fticrPreprocessParameters.ApodizationPercent, fticrPreprocessParameters.NumZeroFills);
                    if (fticrPreprocessParameters.ApplyCalibration)
                    {
                        icrRawData.OverrideDefaultCalibrator(
                            fticrPreprocessParameters.CalibrationType,
                            fticrPreprocessParameters.A, fticrPreprocessParameters.B,
                            fticrPreprocessParameters.C);
                    }
                }

                if (rawData == null)
                {
                    throw new Exception(string.Concat("Could not open raw file: ", fileName));
                }

                var lcmsResults = new Engine.Results.LCMSTransformResults();

                var peakProcessor = new PeakProcessor();
                var originalPeakProcessor = new PeakProcessor();

                //lcmsChecker = new Engine.ResultChecker.LCMSCheckResults();
                // Set parameters for discovering peaks. intensity threshold is set below.
                peakProcessor.SetOptions(peakParameters.SignalToNoiseThreshold, 0, thresholded,
                    peakParameters.PeakFitType);
                originalPeakProcessor.SetOptions(peakParameters.SignalToNoiseThreshold, 0, thresholded,
                    peakParameters.PeakFitType);

                MassTransform massTransform = null;
                if (transform)
                {
                    massTransform = new MassTransform();
                    massTransform.ElementalIsotopeComposition = transformParameters.ElementIsotopeComposition;
                    //massTransform.SetOptions(transformParameters.MaxCharge, transformParameters.MaxMW, transformParameters.MaxFit,
                    //  transformParameters.MinS2N, transformParameters.CCMass,transformParameters.DeleteIntensityThreshold,
                    //  transformParameters.MinIntensityForScore, transformParameters.NumPeaksForShoulder,
                    //  transformParameters.CheckAllPatternsAgainstCharge1, transformParameters.UseMercuryCaching, transformParameters.O16O18Media);

                    massTransform.SetOptions(transformParameters.MaxCharge, transformParameters.MaxMW,
                        transformParameters.MaxFit,
                        transformParameters.MinS2N, transformParameters.CCMass,
                        transformParameters.DeleteIntensityThreshold,
                        transformParameters.MinIntensityForScore, transformParameters.NumPeaksForShoulder,
                        transformParameters.CheckAllPatternsAgainstCharge1, transformParameters.UseMercuryCaching,
                        transformParameters.O16O18Media, transformParameters.LeftFitStringencyFactor,
                        transformParameters.RightFitStringencyFactor, transformParameters.IsActualMonoMZUsed);

                    var averagineFormula = transformParameters.AveragineFormula;
                    var tagFormula = "";
                    if (transformParameters.TagFormula != null)
                    {
                        tagFormula = transformParameters.TagFormula;
                    }
                    massTransform.SetIsotopeFitOptions(averagineFormula, tagFormula,
                        transformParameters.ThrashOrNot, transformParameters.CompleteFit);
                    massTransform.IsotopeFitType = transformParameters.IsotopeFitType;
                }

                var mzs = new List<double>();
                var intensities = new List<double>();
                var tempMzs = new List<double>();
                var tempIntensities = tempMzs;

                var startTime = DateTime.Now;

                var minScan = 1;
                if (transformParameters.UseScanRange && transformParameters.MinScan > 1)
                    minScan = transformParameters.MinScan;
                if (minScan < rawData.GetFirstScanNum())
                    minScan = rawData.GetFirstScanNum();
                var maxScan = rawData.GetLastScanNum();
                if (transformParameters.UseScanRange && transformParameters.MaxScan < maxScan)
                    maxScan = transformParameters.MaxScan;

                //Interpolation interpolator = null;
                //if (transformParameters.ZeroFill)
                //{
                //    interpolator = new Engine.Utilities.Interpolation();
                //}

                if (transformParameters.UseSavitzkyGolaySmooth)
                {
                    sgSmoother = new SavGolSmoother(transformParameters.SGNumLeft,
                        transformParameters.SGNumRight, transformParameters.SGOrder);
                }

                //DateTime rawDataReadTime;
                //DateTime preprocessingTime;
                //DateTime transformTime;
                //DateTime averageTime;
                //DateTime currentTime;
                //DateTime ticTime;
                //DateTime peakDiscoverTime;
                //2009-04-03 [gord] will no longer use the SlidingWindow. It has litte speed benefit and there might be a bug
                /*if (transformParameters.SumSpectraAcrossScanRange())
                {
                    if (fileType == DeconToolsV2.Readers.PNNL_UIMF)
                        ((Engine.Readers.UIMFRawData)rawData).InitializeSlidingWindow(transformParameters.NumScansToSumOver));
                    else if (fileType == DeconToolsV2.Readers.PNNL_IMS)
                        ((Engine.Readers.IMSRawData)rawData).InitializeSlidingWindow(transformParameters.NumScansToSumOver));
                }*/

                for (var scanNum = minScan;
                    scanNum <= maxScan && scanNum != -1;
                    scanNum = rawData.GetNextScanNum(scanNum))
                {

                    peakProcessor.Clear();
                    originalPeakProcessor.Clear();
                    _currentScan = scanNum;
                    if (minScan != maxScan)
                        _percentDone = ((scanNum - minScan) * 100) / (maxScan - minScan);
                    mzs.Clear();
                    intensities.Clear();
                    tempMzs.Clear();
                    tempIntensities.Clear();

                    short scanMsLevel = 1;
                    var centroid = false;
                    //Check if it needs to be processed
                    if (rawData.IsMSScan(scanNum))
                    {
                        scanMsLevel = 1;
                        centroid = false;
                    }
                    else
                    {
                        scanMsLevel = 2;
                        centroid = DTAGenerationParameters.CentroidMSn;
                    }
                    if (scanMsLevel != 1 && !transformParameters.ProcessMSMS)
                        continue;

                    if (scanMsLevel != 1 && transformParameters.ProcessMSMS &&
                        !rawData.IsFTScan(scanNum))
                        continue;

                    //Get this scan first
                    rawData.GetRawData(out mzs, out intensities, scanNum, centroid);
                    rawData.GetRawData(out tempMzs, out tempIntensities, scanNum, centroid);

                    // ------------------------------ Spectra summing ----------------------------------
                    if (transformParameters.SumSpectra) // sum all spectra
                    {
                        double minMz;
                        double maxMz;
                        if (transformParameters.UseMZRange)
                        {
                            minMz = transformParameters.MinMZ;
                            maxMz = transformParameters.MaxMZ;
                        }
                        else
                        {
                            var num_mzs_this_scan = mzs.Count;
                            minMz = mzs[0];
                            maxMz = mzs[mzs.Count - 1];
                        }
                        mzs.Clear();
                        intensities.Clear();
                        rawData.GetSummedSpectra(out mzs, out intensities, minScan, maxScan, minMz, maxMz);
                        scanNum = maxScan;
                        _percentDone = 50;
                    }
                    else if (transformParameters.SumSpectraAcrossScanRange) // sum across range
                    {
                        //// AM: Save original intensity of peaks prior to peaks
                        if (transformParameters.ZeroFill)
                        {
                            Interpolation.ZeroFillMissing(ref mzs, ref intensities, transformParameters.NumZerosToFill);
                        }
                        if (transformParameters.UseSavitzkyGolaySmooth)
                        {
                            sgSmoother.Smooth(ref mzs, ref intensities);
                        }
                        var origThres = DeconEngine.Utils.GetAverage(intensities, float.MaxValue);
                        var origBackgroundIntensity = DeconEngine.Utils.GetAverage(intensities,
                            (float) (5 * origThres));
                        originalPeakProcessor.SetPeakIntensityThreshold(origBackgroundIntensity *
                                                                          peakParameters.PeakBackgroundRatio);
                        var origNumPeaks = 0;
                        if (HornTransformParameters.UseMZRange)
                            origNumPeaks = originalPeakProcessor.DiscoverPeaks(tempMzs,
                                tempIntensities, HornTransformParameters.MinMZ,
                                HornTransformParameters.MaxMZ);
                        else
                            origNumPeaks = originalPeakProcessor.DiscoverPeaks(tempMzs,
                                tempIntensities);

                        // now sum
                        mzs.Clear();
                        intensities.Clear();
                        var scanRange = transformParameters.NumScansToSumOver;

                        //2009-04-03 [gord] will no longer use the SlidingWindow. It has litte speed benefit and there might be a bug
                        //if (fileType == DeconToolsV2.Readers.PNNL_UIMF)
                        //  ((Engine.Readers.UIMFRawData)rawData).GetSummedSpectraSlidingWindow(mzs, intensities, scanNum, scanRange);
                        //  //((Engine.Readers.UIMFRawData)rawData).GetSummedSpectra(mzs, intensities, scanNum, scanRange);  //Gord added this

                        //else if (fileType == DeconToolsV2.Readers.PNNL_IMS)
                        //  ((Engine.Readers.IMSRawData)rawData).GetSummedSpectraSlidingWindow(mzs, intensities, scanNum, scanRange);
                        //else

                        rawData.GetSummedSpectra(out mzs, out intensities, scanNum, scanRange);
                    }

                    var scanTime = rawData.GetScanTime(scanNum);
                    //var driftTime = 0d;
                    //if (fileType == DeconToolsV2.Readers.PNNL_UIMF)
                    //{
                    //  //driftTime =  ((Engine.Readers.UIMFRawData)rawData).GetDriftTime(scan_num);
                    //}
                    //else
                    //  driftTime = -1;

                    if (mzs.Count == 0)
                    {
                        if (savePeaks)
                        {
                            lcmsResults.AddInfoForScan(scanNum, 0, 0, 0, 0, 0, 0, scanTime, scanMsLevel);

                            /*if (fileType == DeconToolsV2.Readers.PNNL_UIMF)
                            lcmsResults.AddInfoForUIMFScan(scanNum, 0, 0, 0, 0, 0, 0, scanTime, scanMsLevel, driftTime);
                        else
                            lcmsResults.AddInfoForScan(scanNum, 0, 0, 0, 0, 0, 0, scanTime, scanMsLevel);*/
                        }
                        continue;
                    }

                    //------------------------------------- Zero fill --------------------------------------
                    if (transformParameters.ZeroFill)
                    {
                        Interpolation.ZeroFillMissing(ref mzs, ref intensities, transformParameters.NumZerosToFill);
                    }

                    // ------------------------------------ Smooth -----------------------------------------
                    if (transformParameters.UseSavitzkyGolaySmooth)
                    {
                        sgSmoother.Smooth(ref mzs, ref intensities);
                    }

                    var minMZ = mzs[0];
                    var maxMZ = mzs[mzs.Count - 1];

                    var thres = DeconEngine.Utils.GetAverage(intensities, float.MaxValue);
                    var backgroundIntensity = DeconEngine.Utils.GetAverage(intensities, (float) (5 * thres));

                    // currentTime = DateTime.Now;

                    var ticIntensity = 0d;
                    var bpi = 0d;
                    var bpMz = 0d;
                    if (HornTransformParameters.UseMZRange)
                    {
                        ticIntensity = DeconEngine.Utils.GetTIC(HornTransformParameters.MinMZ,
                            HornTransformParameters.MaxMZ, ref mzs, ref intensities,
                            (float) (backgroundIntensity * peakParameters.PeakBackgroundRatio),
                            ref bpi, ref bpMz);
                    }
                    else
                    {
                        ticIntensity = DeconEngine.Utils.GetTIC(400.0, 2000.0, ref mzs, ref intensities,
                            (float) (backgroundIntensity * peakParameters.PeakBackgroundRatio), ref bpi, ref bpMz);
                    }

                    peakProcessor.SetPeakIntensityThreshold(backgroundIntensity * peakParameters.PeakBackgroundRatio);
                    var numPeaks = 0;
                    if (HornTransformParameters.UseMZRange)
                    {
                        numPeaks = peakProcessor.DiscoverPeaks(mzs, intensities,
                            HornTransformParameters.MinMZ, HornTransformParameters.MaxMZ);
                    }
                    else
                    {
                        numPeaks = peakProcessor.DiscoverPeaks(mzs, intensities);
                    }

                    if (savePeaks)
                    {
                        lcmsResults.AddPeaksForScan(scanNum, peakProcessor.PeakData.PeakTops);
                    }

                    var numDeisotoped = 0;

                    // ------------------------ Mass Transform -----------------------------------------
                    if (transform)
                    {
                        var minPeptideIntensity = backgroundIntensity *
                                                       transformParameters.PeptideMinBackgroundRatio;
                        if (transformParameters.UseAbsolutePeptideIntensity)
                        {
                            if (minPeptideIntensity < transformParameters.AbsolutePeptideIntensity)
                                minPeptideIntensity = transformParameters.AbsolutePeptideIntensity;
                        }

                        clsPeak currentPeak;
                        var originalPeak = new clsPeak();

                        peakProcessor.PeakData.InitializeUnprocessedPeakData();

                        var found = peakProcessor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);

                        massTransform.Reset();
                        transformRecords.Clear();
                        while (found)
                        {
                            if (currentPeak.Intensity < minPeptideIntensity)
                                break;

                            try
                            {
                                var foundTransform = false;
                                clsHornTransformResults transformRecord;

                                foundTransform = massTransform.FindTransform(peakProcessor.PeakData,
                                    ref currentPeak, out transformRecord, backgroundIntensity);


                                // AM (anoop?): if summing over a window, reinsert the original intensity     // [gord]  why?
                                if (foundTransform && transformRecord.ChargeState <= transformParameters.MaxCharge
                                    && transformParameters.SumSpectraAcrossScanRange)
                                {
                                    originalPeakProcessor.PeakData.InitializeUnprocessedPeakData();
                                    originalPeak.Intensity = -1.0;
                                    originalPeak.Mz = -1.0;
                                    originalPeakProcessor.PeakData.FindPeak(
                                        transformRecord.Mz - 2 * transformRecord.FWHM,
                                        transformRecord.Mz + 2 * transformRecord.FWHM, out originalPeak);
                                    if (originalPeak.Intensity > 0)
                                    {
                                        transformRecord.Abundance = originalPeak.Intensity;
                                        transformRecord.FWHM = originalPeak.FWHM;
                                        // [gord] this might be the source of why FWHM is sometimes 0
                                    }
                                    else
                                    {
                                        foundTransform = false; //AM : do not add to results
                                    }
                                }

                                if (foundTransform && transformRecord.ChargeState <= transformParameters.MaxCharge)
                                {
                                    numDeisotoped++;
                                    transformRecord.ScanNum = scanNum;

                                    if (transformParameters.IsActualMonoMZUsed)
                                    {
                                        //retrieve experimental monoisotopic peak
                                        var monoPeakIndex = transformRecord.IsotopePeakIndices[0];
                                        clsPeak monoPeak;
                                        peakProcessor.PeakData.GetPeak(monoPeakIndex, out monoPeak);

                                        //set threshold at 20% less than the expected 'distance' to the next peak
                                        var errorThreshold = 1.003 / transformRecord.ChargeState;
                                        errorThreshold = errorThreshold - errorThreshold * 0.2;

                                        var calcMonoMZ = transformRecord.MonoMw / transformRecord.ChargeState +
                                                             1.00727638;

                                        if (Math.Abs(calcMonoMZ - monoPeak.Mz) < errorThreshold)
                                        {
                                            transformRecord.MonoMw = monoPeak.Mz * transformRecord.ChargeState -
                                                                     1.00727638 * transformRecord.ChargeState;
                                        }
                                    }
                                    transformRecords.Add(transformRecord);
                                }
                                found = peakProcessor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                            }
                            catch (Exception ex)
                            {
#if DEBUG
                                throw;
#else
                                Console.WriteLine("Error in CreateTransformResults: " + ex.Message);
                                Console.WriteLine("Scan {0}, peak {1}", scanNum, currentPeak.PeakIndex);
                                Console.WriteLine(PRISM.StackTraceFormatter.GetExceptionStackTraceMultiLine(ex));
#endif
                            }
                        }

                        lcmsResults.AddTransforms(transformRecords);
                        //  lcmsChecker.AddTransformsToCheck(transformRecords);
                        if (fileType != DeconToolsV2.Readers.FileType.PNNL_UIMF)
                            //if (fileType != DeconToolsV2.Readers.PNNL_UIMF && scan_num % 20 == 0)
                        {
                            //int iso_time = 0,
                            //    fit_time = 0,
                            //    cs_time = 0,
                            //    get_fit_score_time = 0,
                            //    remainder_time = 0,
                            //    find_peak_calc = 0,
                            //    find_peak_cached = 0;
                            //

                            //massTransform.GetProcessingTimes(out cs_time, out ac_time, out spline_time, out iso_time,
                            //    out fit_time,
                            //    out remainder_time, out get_fit_score_time, out find_peak_calc, out find_peak_cached);

                            Console.WriteLine(string.Concat("Scan # =", Convert.ToString(scanNum)//,
                                //" CS= ", Convert.ToString(cs_time),
                                //" Isotope= ", Convert.ToString(iso_time),
                                //" FitScore= ", Convert.ToString(fit_time),
                                //" GetFitScore= ", Convert.ToString(get_fit_score_time),
                                //" GetFitScore-Isotope-FitScore-FindPeak= ",
                                //Convert.ToString(get_fit_score_time - fit_time - find_peak_cached - find_peak_calc -
                                //                 iso_time),
                                //" Raw Reading Time = ", Convert.ToString(rawDataReadTime),
                                //" PreProcessing Time = ", Convert.ToString(preprocessingTime),
                                //" Transform= ", Convert.ToString(transformTime),
                                //" Remaining= ", Convert.ToString(remainder_time),
                                //" transform-cs-get_fit= ", Convert.ToString(transformTime - cs_time - get_fit_score_time),
                                //" All= ", Convert.ToString(all)
                                //" all-transform-preprocess-read= ", Convert.ToString(all-transformTime-preprocessingTime-rawDataReadTime)
                            ));
                        }
                    }
                    if (savePeaks)
                    {
                        var signalRange = rawData.GetSignalRange(scanNum, centroid);
                        lcmsResults.AddInfoForScan(scanNum, bpMz, bpi, ticIntensity, signalRange, numPeaks,
                            numDeisotoped, scanTime, scanMsLevel);
                        /*if (fileType == DeconToolsV2.Readers.PNNL_UIMF)
                        lcmsResults.AddInfoForUIMFScan(scanNum, bpMz, bpi, ticIntensity, signalRange, numPeaks, numDeisotoped, scanTime, scanMsLevel, driftTime);
                    else
                        lcmsResults.AddInfoForScan(scanNum, bpMz, bpi, ticIntensity, signalRange, numPeaks, numDeisotoped, scanTime, scanMsLevel); */
                    }
                }

                transformResults.SetLCMSTransformResults(lcmsResults);

                // Anoop: write out those ones which are probably wrong
                /*string output_file_str = fileName.Remove(dotIndex, mstr_file_name.Length - dotIndex);
                lcmsChecker.WriteOutTransformsToCheck(output_file_str); */

                _percentDone = 100;
            }
            catch (Exception)
            {
                ProcessState = enmProcessState.ERROR;
                throw;
            }
            ProcessState = enmProcessState.COMPLETE;
            return transformResults;
        }

        public void CreateDTAFile()
        {
            //Main function to create DTA files

            //check if we have everything
            if (PeakProcessorParameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            if (HornTransformParameters == null)
            {
                throw new Exception("Horn Transform parameters not set.");
            }
            if (DTAGenerationParameters == null)
            {
                throw new Exception("DTA Generation parameters not set.");
            }

#pragma warning disable 618
            if (ProcessState == enmProcessState.RUNNING)
            {
                throw new Exception(
                    "Process already running in clsProcRunner. Cannot run two processes with same object");
            }
            ProcessState = enmProcessState.RUNNING;
#pragma warning restore 618

            _percentDone = 0;

            var dtaProcessor = new Engine.DTAProcessing.DTAProcessor();
            var dtaScanType = new Engine.DTAProcessing.DTAScanTypeGeneration();

            if (!File.Exists(FileName))
            {
                Console.WriteLine("Error: File \"{0}\" does not exist. Please check the command line arguments.", FileName);
                return;
            }

            //Read the rawfile in
            using (var fin = new FileStream(FileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                if (!fin.CanRead)
                {
                    Console.WriteLine("Unable to open input file");
                    return;
                }
            }

            // Check input format
            var dotIndex = FileName.IndexOf('.');
            var inputFileFormat = FileName.Remove(0, dotIndex + 1);
            if ((inputFileFormat.ToLower() == "raw"))
                FileType = DeconToolsV2.Readers.FileType.FINNIGAN;
            else if (inputFileFormat.ToLower() == "mzxml")
                FileType = DeconToolsV2.Readers.FileType.MZXMLRAWDATA;
            else
            {
                throw new Exception("Invalid input file format.");
            }

            // Set output path and filename
            string outputFile;
            if (OutputPathForDTACreation != null)
            {
                var slashIndex = FileName.LastIndexOf("\\");
                var rawNamePlusExtension = FileName.Remove(dotIndex, FileName.Length - dotIndex);
                var rawName = rawNamePlusExtension.Remove(0, slashIndex);
                outputFile = Path.Combine(OutputPathForDTACreation, Path.GetFileName(rawName));
            }
            else
            {
                outputFile = FileName.Remove(dotIndex, FileName.Length - dotIndex);
            }

            var thresholded = true;
            /*
            if (FileType == DeconToolsV2.Readers.FileType.FINNIGAN ||
                FileType == DeconToolsV2.Readers.FileType.MZXMLRAWDATA)
                thresholded = true;
            else
                thresholded = _peakParameters.ThresholdedData;
            */

            //Raw Object
            dtaProcessor.RawDataDTA =
                Engine.Readers.ReaderFactory.GetRawData(FileType, FileName);
            dtaProcessor.DatasetType = FileType;

            //File name base for all dtas
            dtaProcessor.OutputFile = outputFile;

            //Datasetname
            var lastSlashIndex = FileName.LastIndexOf("\\");
            var dataNamePlusExtension = FileName.Remove(dotIndex, FileName.Length - dotIndex);
            var dataName = dataNamePlusExtension.Remove(0, lastSlashIndex + 1);
            dtaProcessor.DatasetName = dataName;

            // File name for log file
            DTAGenerationParameters.CreateLogFileOnly = false;
            dtaProcessor.LogFilename = outputFile + "_DeconMSn_log.txt";

            //File name for profile data
            dtaProcessor.ProfileFilename = outputFile + "_profile.txt";

            if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.LOG)
            {
                DTAGenerationParameters.CreateLogFileOnly = true;
            }

            dtaProcessor.DoWriteProgressFile = false;
            dtaProcessor.ProgressFilename = outputFile + "_DeconMSn_progress.txt";

            if (DTAGenerationParameters.WriteProgressFile)
            {
                dtaProcessor.DoWriteProgressFile = true;
            }

            //file name for composite dta file
            if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.CDTA)
            {
                dtaProcessor.CombinedDTAFilename = outputFile + "_dta.txt";
                dtaProcessor.CombinedDTAFileWriter =
                    new StreamWriter(new FileStream(dtaProcessor.CombinedDTAFilename, FileMode.Create,
                        FileAccess.ReadWrite, FileShare.Read));
                dtaScanType.DTAScanTypeFilename = outputFile + "_ScanType.txt";
                dtaScanType.DTAScanTypeFileWriter = new StreamWriter(new FileStream(dtaScanType.DTAScanTypeFilename, FileMode.Create,
                    FileAccess.ReadWrite, FileShare.Read));
                dtaScanType.RawDataReader = dtaProcessor.RawDataDTA;
            }
            //file name for .mgf file
            if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
            {
                dtaProcessor.MGFFilename = outputFile = ".mgf";
                dtaProcessor.MGFFileWriter =
                    new StreamWriter(new FileStream(dtaProcessor.MGFFilename, FileMode.Create,
                        FileAccess.ReadWrite, FileShare.Read));
            }

            //Settings
            //var tagFormula = "";
            //var averagineFormula = HornTransformParameters.AveragineFormula;
            //if (HornTransformParameters.TagFormula != null)
            //    tagFormula = HornTransformParameters.TagFormula;

            //Check if any dtas have to be ignored
            var msnIgnore = new List<int>();
            if (DTAGenerationParameters.IgnoreMSnScans)
            {
                var numLevels = DTAGenerationParameters.NumMSnLevelsToIgnore;
                for (var levelNum = 0; levelNum < numLevels; levelNum++)
                {
                    var level = DTAGenerationParameters.get_MSnLevelToIgnore(levelNum);
                    msnIgnore.Add(level);
                }
                msnIgnore.Sort();
            }

            dtaProcessor.DtaOptions = DTAGenerationParameters;
            dtaProcessor.SetPeakProcessorOptions(PeakProcessorParameters.SignalToNoiseThreshold, 0, thresholded,
                PeakProcessorParameters.PeakFitType);
            dtaProcessor.MassTransformOptions = HornTransformParameters;

            var svmFile = DTAGenerationParameters.SVMParamFile;
            dtaProcessor.InitializeSVM(svmFile);
            dtaProcessor.SetPeakParametersLowResolution(PeakProcessorParameters.PeakBackgroundRatio,
                HornTransformParameters.PeptideMinBackgroundRatio);

            //begin process
            //stick in range
            var scanNum = DTAGenerationParameters.MinScan;
            var msNScanIndex = 0;
            int numScans;
            //double parentMz = 0;
            var lowResolution = false;
            var scanStart = scanNum;
            var nextProgressScan = scanStart + 50;

            if (DTAGenerationParameters.MaxScan <= dtaProcessor.RawDataDTA.GetNumScans())
                numScans = DTAGenerationParameters.MaxScan;
            else
                numScans = dtaProcessor.RawDataDTA.GetNumScans();

            while (scanNum <= numScans)
            {
                _percentDone = (scanNum * 100) / numScans;
                if (dtaProcessor.RawDataDTA.IsMSScan(scanNum))
                {
                    //Get MS spectra
                    dtaProcessor.GetParentScanSpectra(scanNum, PeakProcessorParameters.PeakBackgroundRatio,
                        HornTransformParameters.PeptideMinBackgroundRatio);

                    var msNScan = scanNum + 1;
                    for (msNScan = scanNum + 1;
                        msNScan < numScans && !dtaProcessor.RawDataDTA.IsMSScan(msNScan);
                        msNScan++)
                    {
                        //GetMS level and see if it is to be ignored
                        if (DTAGenerationParameters.IgnoreMSnScans)
                        {
                            var msNLevel = dtaProcessor.RawDataDTA.GetMSLevel(msNScan);
                            /*
                            var foundMSnLevel = false;
                            for (var index = 0; index < msnIgnore.Count; index++)
                            {
                                if (msNLevel == msnIgnore[index])
                                {
                                    foundMSnLevel = true;
                                    break;
                                }
                            }

                            if (foundMSnLevel)
                                continue;
                            */
                            if (msnIgnore.Contains(msNLevel))
                            {
                                continue;
                            }
                        }
                        //Get msN spectra
                        dtaProcessor.GetMsNSpectra(msNScan, PeakProcessorParameters.PeakBackgroundRatio,
                            HornTransformParameters.PeptideMinBackgroundRatio);
                        //Identify which is parent_scan
                        var parentScan = dtaProcessor.RawDataDTA.GetParentScan(msNScan);
                        // AM Modified to recieve new spectra everytime if (parentScan != scanNum) //MSN data
                        dtaProcessor.GetParentScanSpectra(parentScan, PeakProcessorParameters.PeakBackgroundRatio,
                            HornTransformParameters.PeptideMinBackgroundRatio);

                        if (DTAGenerationParameters.SpectraType == 0 ||
                            dtaProcessor.GetSpectraType(msNScan) == (int) DTAGenerationParameters.SpectraType)
                        {
                            var dtaSuccess = false;
                            if (dtaProcessor.IsFTData(parentScan))
                            {
                                //Get charge and mono
                                dtaSuccess = dtaProcessor.GenerateDTA(msNScan, parentScan);
                            }
                            else if (dtaProcessor.IsZoomScan(parentScan))
                            {
                                dtaSuccess = dtaProcessor.GenerateDTAZoomScans(msNScan, parentScan, msNScanIndex);
                            }
                            else
                            {
                                //Low res data
                                lowResolution = true;
                                dtaSuccess = dtaProcessor.GenerateDTALowRes(msNScan, parentScan, msNScanIndex);
                            }
                            if (dtaSuccess)
                            {
                                //write out dta
                                if (DTAGenerationParameters.OutputType ==
                                    DTAGeneration.OUTPUT_TYPE.MGF)
                                    dtaProcessor.WriteToMGF(msNScan, parentScan);
                                else
                                    dtaProcessor.WriteDTAFile(msNScan, parentScan);
                            }
                        }
                    }
                    // reinitialize scan_count appropriately
                    scanNum = msNScan - 1;
                }
                else
                {
                    //Get msN spectra
                    var msNScan = scanNum;

                    //GetMS level and see if it is to be ignored
                    if (DTAGenerationParameters.IgnoreMSnScans)
                    {
                        var msNLevel = dtaProcessor.RawDataDTA.GetMSLevel(msNScan);
                        /*
                        var foundMSnLevel = false;
                        for (var index = 0; index < msnIgnore.Count; index++)
                        {
                            if (msNLevel == msnIgnore[index])
                            {
                                foundMSnLevel = true;
                                break;
                            }
                        }

                        if (foundMSnLevel)
                        {
                            scanNum++; // make sure an increment happens
                            continue;
                        }
                        */
                        if (msnIgnore.Contains(msNLevel))
                        {
                            scanNum++; // make sure an increment happens
                            continue;
                        }
                    }

                    dtaProcessor.GetMsNSpectra(msNScan, PeakProcessorParameters.PeakBackgroundRatio,
                        HornTransformParameters.PeptideMinBackgroundRatio);

                    if (DTAGenerationParameters.SpectraType == 0 ||
                        dtaProcessor.GetSpectraType(msNScan) == (int) DTAGenerationParameters.SpectraType)
                    {
                        //Identify which is parentScan
                        var parentScan = dtaProcessor.RawDataDTA.GetParentScan(msNScan);
                        // check to see if valid parent which wont be in MRM cases where MSn begins at 1.
                        if (parentScan < 1)
                        {
                            scanNum++;
                            continue; //no dta is generated
                        }

                        // get parent data
                        dtaProcessor.GetParentScanSpectra(parentScan, PeakProcessorParameters.PeakBackgroundRatio,
                            HornTransformParameters.PeptideMinBackgroundRatio);

                        if (dtaProcessor.IsFTData(parentScan))
                        {
                            //Get charge and mono
                            var dtaSuccess = dtaProcessor.GenerateDTA(msNScan, parentScan);
                            if (dtaSuccess)
                            {
                                //write out dta
                                if (DTAGenerationParameters.OutputType ==
                                    DTAGeneration.OUTPUT_TYPE.MGF)
                                    dtaProcessor.WriteToMGF(msNScan, parentScan);
                                else
                                    dtaProcessor.WriteDTAFile(msNScan, parentScan);
                            }
                        }
                        else
                        {
                            //Low res data
                            lowResolution = true;
                            var dtaSuccess = dtaProcessor.GenerateDTALowRes(msNScan, parentScan, msNScanIndex);
                            if (dtaSuccess)
                            {
                                if (DTAGenerationParameters.OutputType ==
                                    DTAGeneration.OUTPUT_TYPE.MGF)
                                    dtaProcessor.WriteToMGF(msNScan, parentScan);
                                else
                                    dtaProcessor.WriteDTAFile(msNScan, parentScan);
                            }
                        }
                    }
                }
                scanNum++;

                if (dtaProcessor.DoWriteProgressFile && scanNum - scanStart >= nextProgressScan)
                {
                    dtaProcessor.WriteProgressFile(scanNum - scanStart, numScans - scanStart + 1, _percentDone);

                    nextProgressScan += 50;
                    while (nextProgressScan <= scanNum - scanStart)
                        nextProgressScan += 50;
                }
            }

            if (lowResolution && DTAGenerationParameters.ConsiderChargeValue == 0)
            {
                System.Console.WriteLine("Determining charge");
                dtaProcessor.DetermineChargeForEachScan();
                System.Console.WriteLine("Generating DTAs for low-resolution data");
                if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                    dtaProcessor.WriteLowResolutionMGFFile();
                else
                    dtaProcessor.WriteLowResolutionDTAFile();
            }

            if (DTAGenerationParameters.CreateCompositeDTA)
            {
                try
                {
                    Console.WriteLine("Writing to file " + dtaScanType.DTAScanTypeFilename);
                    dtaScanType.GenerateScanTypeFile();
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error writing _ScanType.txt file: " + ex.Message);
                    Console.WriteLine(PRISM.StackTraceFormatter.GetExceptionStackTraceMultiLine(ex));
                }
                finally
                {
                    dtaScanType.DTAScanTypeFileWriter.Close();
                }
            }

            _percentDone = 100;
            dtaProcessor.WriteProgressFile(scanNum - scanStart, numScans - scanStart + 1, _percentDone);

            // Write out log file
            dtaProcessor.WriteLogFile();

            // Write out profile
            dtaProcessor.WriteProfileFile();
            //Shutdown
            //dta_processor.mfile_log.close();
            if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.CDTA)
            {
                dtaProcessor.CombinedDTAFileWriter.Close();
            }
            if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                dtaProcessor.MGFFileWriter.Close();

            //Done
#pragma warning disable 618
            ProcessState = enmProcessState.COMPLETE;
#pragma warning restore 618
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void CreateTransformResultWithPeaksOnly()
        {
            if (FileName == null)
            {
                throw new Exception("File name is not set.");
            }
            if (PeakProcessorParameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            HornTransformResults = CreateTransformResults(FileName, FileType, PeakProcessorParameters,
                HornTransformParameters, FTICRPreprocessOptions, true, false);
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void CreateTransformResultWithNoPeaks()
        {
            if (FileName == null)
            {
                throw new Exception("File name is not set.");
            }
            if (PeakProcessorParameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            if (HornTransformParameters == null)
            {
                throw new Exception("Horn Transform parameters not set.");
            }
            HornTransformResults = CreateTransformResults(FileName, FileType, PeakProcessorParameters,
                HornTransformParameters, FTICRPreprocessOptions, false, true);
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void CreateTransformResults()
        {
            if (FileName == null)
            {
                throw new Exception("File name is not set.");
            }
            if (PeakProcessorParameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            if (HornTransformParameters == null)
            {
                throw new Exception("Horn Transform parameters not set.");
            }
            HornTransformResults = CreateTransformResults(FileName, FileType, PeakProcessorParameters,
                HornTransformParameters, FTICRPreprocessOptions, true, true);
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