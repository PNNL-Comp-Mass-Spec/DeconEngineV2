using System;
using System.Collections.Generic;
using System.IO;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;
using Engine.HornTransform;
using Engine.PeakProcessing;
using DeconToolsV2.Readers;
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
        // Ignore Spelling: namespace, interpolator

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

                if (ProcessState == enmProcessState.RUNNING)
                    return _percentDone;

                return 100;
            }
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public enmProcessState ProcessState { get; private set; }

        /// <summary>
        /// Path to the input file (full or partial path)
        /// </summary>
        public string InputFilePath { get; set; }

        /// <summary>
        /// Path to the input file (full or partial path)
        /// </summary>
        [Obsolete("Use InputFilePath")]
        public string FileName
        {
            get => InputFilePath;
            set => InputFilePath = value;
        }

        public FileType FileType { get; set; }

        /// <summary>
        /// Directory where output files should be created
        /// </summary>
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
            InputFilePath = null;
            OutputPathForDTACreation = null;
            PeakProcessorParameters = new clsPeakProcessorParameters();
            HornTransformParameters = new clsHornTransformParameters();
            DTAGenerationParameters = new DTAGeneration.clsDTAGenerationParameters();
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
                // Although the thresholded parameter is already set in the clsPeakProcessParameters, we would
                // like to override that here if the data type is Thermo because that data is thresholded.
                bool thresholded;
                if (fileType == FileType.THERMORAW ||
                    fileType == FileType.MZXMLRAWDATA)
                    thresholded = true;
                else
                    thresholded = peakParameters.ThresholdedData;

                _percentDone = 0;
                ProcessState = enmProcessState.RUNNING;

                // Create a RawData object and read through each scan and discover peaks.

                // enumerations of file type are the same in Readers namespace and
                // DeconWrapperManaged namespace.
                var rawData = Engine.Readers.ReaderFactory.GetRawData(fileType, fileName);
                if (fileType == FileType.ICR2LSRAWDATA && fticrPreprocessParameters != null)
                {
                    var icrRawData = (Engine.Readers.Icr2lsRawData)rawData;
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
                    massTransform = new MassTransform
                    {
                        ElementalIsotopeComposition = transformParameters.ElementIsotopeComposition
                    };

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

                // var startTime = DateTime.Now;

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
                //2009-04-03 [gord] will no longer use the SlidingWindow. It has little speed benefit and there might be a bug
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

                    short scanMsLevel;
                    bool centroid;

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
                        var origAverageIntensity = DeconEngine.Utils.GetAverage(intensities, float.MaxValue);
                        var origBackgroundIntensity = DeconEngine.Utils.GetAverage(intensities, (float)(5 * origAverageIntensity));
                        originalPeakProcessor.SetPeakIntensityThreshold(origBackgroundIntensity * peakParameters.PeakBackgroundRatio);

                        if (HornTransformParameters.UseMZRange)
                        {
                            originalPeakProcessor.DiscoverPeaks(tempMzs, tempIntensities,
                                HornTransformParameters.MinMZ,
                                HornTransformParameters.MaxMZ);
                        }
                        else
                        {
                            originalPeakProcessor.DiscoverPeaks(tempMzs, tempIntensities);
                        }

                        // now sum
                        mzs.Clear();
                        intensities.Clear();
                        var scanRange = transformParameters.NumScansToSumOver;

                        //2009-04-03 [gord] will no longer use the SlidingWindow. It has little speed benefit and there might be a bug
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

                    var averageIntensity = DeconEngine.Utils.GetAverage(intensities, float.MaxValue);
                    var backgroundIntensity = DeconEngine.Utils.GetAverage(intensities, (float)(5 * averageIntensity));

                    // currentTime = DateTime.Now;

                    double ticIntensity;
                    var bpi = 0d;
                    var bpMz = 0d;
                    if (HornTransformParameters.UseMZRange)
                    {
                        ticIntensity = DeconEngine.Utils.GetTIC(HornTransformParameters.MinMZ,
                            HornTransformParameters.MaxMZ, ref mzs, ref intensities,
                            (float)(backgroundIntensity * peakParameters.PeakBackgroundRatio),
                            ref bpi, ref bpMz);
                    }
                    else
                    {
                        ticIntensity = DeconEngine.Utils.GetTIC(400.0, 2000.0, ref mzs, ref intensities,
                            (float)(backgroundIntensity * peakParameters.PeakBackgroundRatio), ref bpi, ref bpMz);
                    }

                    peakProcessor.SetPeakIntensityThreshold(backgroundIntensity * peakParameters.PeakBackgroundRatio);

                    int numPeaks;
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

                        var originalPeak = new clsPeak();

                        peakProcessor.PeakData.InitializeUnprocessedPeakData();

                        var found = peakProcessor.PeakData.GetNextPeak(minMZ, maxMZ, out var currentPeak);

                        massTransform.Reset();
                        transformRecords.Clear();
                        while (found)
                        {
                            if (currentPeak.Intensity < minPeptideIntensity)
                                break;

                            try
                            {
                                var foundTransform = massTransform.FindTransform(peakProcessor.PeakData,
                                    ref currentPeak, out var transformRecord, backgroundIntensity);

                                // AM (anoop?): if summing over a window, re-insert the original intensity     // [gord]  why?
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
                                        peakProcessor.PeakData.GetPeak(monoPeakIndex, out var monoPeak);

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
                        if (fileType != FileType.PNNL_UIMF)
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

                            Console.WriteLine(string.Concat("Scan # =", Convert.ToString(scanNum)
                                 //,
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

        public void CreateDTAFile(string deconMSnVersion)
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

            var inputFile = new FileInfo(InputFilePath);

            if (!inputFile.Exists)
            {
                Console.WriteLine("Error: File \"{0}\" does not exist. Please check the command line arguments.", inputFile.FullName);
                return;
            }

            // Open the input file
            using (var fin = new FileStream(inputFile.FullName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                if (!fin.CanRead)
                {
                    Console.WriteLine("Unable to open input file");
                    return;
                }
            }

            // Check input format

            if (string.Equals(inputFile.Extension, ".raw", StringComparison.OrdinalIgnoreCase))
            {
                FileType = FileType.THERMORAW;
            }
            else if (string.Equals(inputFile.Extension, ".mzXml", StringComparison.OrdinalIgnoreCase))
            {
                FileType = FileType.MZXMLRAWDATA;
            }
            else
            {
                throw new Exception("Invalid input file format; must be .raw or .mzXML");
            }

            // Define the base output file path

            var baseName = Path.GetFileNameWithoutExtension(inputFile.Name);

            var baseOutputFilePath = string.IsNullOrWhiteSpace(OutputPathForDTACreation)
                ? Path.Combine(inputFile.Directory?.FullName ?? ".", baseName)
                : Path.Combine(OutputPathForDTACreation, baseName);

            var thresholded = true;

            /*
            if (FileType == DeconToolsV2.Readers.FileType.THERMORAW ||
                FileType == DeconToolsV2.Readers.FileType.MZXMLRAWDATA)
                thresholded = true;
            else
                thresholded = _peakParameters.ThresholdedData;
            */

            //Raw Object
            dtaProcessor.RawDataDTA = Engine.Readers.ReaderFactory.GetRawData(FileType, inputFile.FullName);
            dtaProcessor.DatasetType = FileType;

            // Base path for all output files
            dtaProcessor.OutputFile = baseOutputFilePath;

            //Dataset name

            dtaProcessor.DatasetName = Path.GetFileNameWithoutExtension(inputFile.Name);

            // File name for log file
            DTAGenerationParameters.CreateLogFileOnly = false;
            dtaProcessor.LogFilename = baseOutputFilePath + "_DeconMSn_log.txt";

            //File name for profile data
            dtaProcessor.ProfileFilename = baseOutputFilePath + "_profile.txt";

            if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.LOG)
            {
                DTAGenerationParameters.CreateLogFileOnly = true;
            }

            dtaProcessor.DoWriteProgressFile = false;
            dtaProcessor.ProgressFilename = baseOutputFilePath + "_DeconMSn_progress.txt";

            if (DTAGenerationParameters.WriteProgressFile)
            {
                dtaProcessor.DoWriteProgressFile = true;
            }

            string outputFilePath;

            if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.CDTA)
            {
                //file name for composite DTA file
                outputFilePath = baseOutputFilePath + "_dta.txt";
                dtaProcessor.CombinedDTAFilename = outputFilePath;
                dtaProcessor.CombinedDTAFileWriter = new StreamWriter(new FileStream(outputFilePath, FileMode.Create, FileAccess.ReadWrite, FileShare.Read));

                dtaScanType.DTAScanTypeFilename = baseOutputFilePath + "_ScanType.txt";
                dtaScanType.DTAScanTypeFileWriter = new StreamWriter(new FileStream(dtaScanType.DTAScanTypeFilename, FileMode.Create, FileAccess.ReadWrite, FileShare.Read));
                dtaScanType.RawDataReader = dtaProcessor.RawDataDTA;
            }
            else if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
            {
                //file name for .mgf file
                outputFilePath = baseOutputFilePath + ".mgf";
                dtaProcessor.MGFFilename = outputFilePath;
                dtaProcessor.MGFFileWriter =
                    new StreamWriter(new FileStream(outputFilePath, FileMode.Create,
                        FileAccess.ReadWrite, FileShare.Read));
            }
            else
            {
                outputFilePath = string.Empty;
            }

            //Settings
            //var tagFormula = "";
            //var averagineFormula = HornTransformParameters.AveragineFormula;
            //if (HornTransformParameters.TagFormula != null)
            //    tagFormula = HornTransformParameters.TagFormula;

            //Check if any DTAs have to be ignored
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
            var scansWritten = 0;

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

                    int msNScan;

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

                        //Get MSn spectra
                        dtaProcessor.GetMsNSpectra(msNScan, PeakProcessorParameters.PeakBackgroundRatio,
                            HornTransformParameters.PeptideMinBackgroundRatio);

                        //Identify which is parent_scan
                        var parentScan = dtaProcessor.RawDataDTA.GetParentScan(msNScan);

                        // AM Modified to receive new spectra every time if (parentScan != scanNum) //MSN data
                        dtaProcessor.GetParentScanSpectra(parentScan, PeakProcessorParameters.PeakBackgroundRatio,
                            HornTransformParameters.PeptideMinBackgroundRatio);

                        if (DTAGenerationParameters.SpectraType == 0 ||
                            dtaProcessor.GetSpectraType(msNScan) == (int)DTAGenerationParameters.SpectraType)
                        {
                            bool dtaSuccess;
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
                                // Write out spectrum
                                if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                                {
                                    dtaProcessor.WriteToMGF(msNScan, parentScan);
                                }
                                else
                                {
                                    dtaProcessor.WriteDTAFile(msNScan, parentScan);
                                }

                                scansWritten++;
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
                        dtaProcessor.GetSpectraType(msNScan) == (int)DTAGenerationParameters.SpectraType)
                    {
                        //Identify which is parentScan
                        var parentScan = dtaProcessor.RawDataDTA.GetParentScan(msNScan);
                        // check to see if valid parent which wont be in MRM cases where MSn begins at 1.
                        if (parentScan < 1)
                        {
                            scanNum++;
                            continue; //no DTA is generated
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
                                // Write out spectrum
                                if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                                {
                                    dtaProcessor.WriteToMGF(msNScan, parentScan);
                                }
                                else
                                {
                                    dtaProcessor.WriteDTAFile(msNScan, parentScan);
                                }

                                scansWritten++;
                            }
                        }
                        else
                        {
                            //Low res data
                            lowResolution = true;
                            var dtaSuccess = dtaProcessor.GenerateDTALowRes(msNScan, parentScan, msNScanIndex);
                            if (dtaSuccess)
                            {
                                if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                                {
                                    dtaProcessor.WriteToMGF(msNScan, parentScan);
                                }
                                else
                                {
                                    dtaProcessor.WriteDTAFile(msNScan, parentScan);
                                }

                                scansWritten++;
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
                    {
                        nextProgressScan += 50;
                    }
                }
            }

            if (!string.IsNullOrWhiteSpace(outputFilePath))
            {
                Console.WriteLine();
                Console.WriteLine("Wrote {0} spectra to file {1}", scansWritten, PRISM.PathUtils.CompactPathString(outputFilePath, 100));
            }

            if (lowResolution && DTAGenerationParameters.ConsiderChargeValue == 0)
            {
                Console.WriteLine("Determining charge");
                dtaProcessor.DetermineChargeForEachScan();
                Console.WriteLine("Generating DTAs for low-resolution data");
                if (DTAGenerationParameters.OutputType == DTAGeneration.OUTPUT_TYPE.MGF)
                    dtaProcessor.WriteLowResolutionMGFFile();
                else
                    dtaProcessor.WriteLowResolutionDTAFile();
            }

            if (DTAGenerationParameters.CreateCompositeDTA)
            {
                Console.WriteLine();
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
            dtaProcessor.WriteLogFile(deconMSnVersion);

            // Write out profile
            dtaProcessor.WriteProfileFile();

            //Shutdown
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
            if (string.IsNullOrWhiteSpace(InputFilePath))
            {
                throw new Exception("File name is not set.");
            }
            if (PeakProcessorParameters == null)
            {
                throw new Exception("Peak parameters not set.");
            }
            HornTransformResults = CreateTransformResults(InputFilePath, FileType, PeakProcessorParameters,
                HornTransformParameters, FTICRPreprocessOptions, true, false);
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void CreateTransformResultWithNoPeaks()
        {
            if (string.IsNullOrWhiteSpace(InputFilePath))
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
            HornTransformResults = CreateTransformResults(InputFilePath, FileType, PeakProcessorParameters,
                HornTransformParameters, FTICRPreprocessOptions, false, true);
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void CreateTransformResults()
        {
            if (string.IsNullOrWhiteSpace(InputFilePath))
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
            HornTransformResults = CreateTransformResults(InputFilePath, FileType, PeakProcessorParameters,
                HornTransformParameters, FTICRPreprocessOptions, true, true);
        }
    }
}