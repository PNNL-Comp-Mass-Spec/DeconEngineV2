using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using DeconToolsV2;
using DeconToolsV2.DTAGeneration;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;
using DeconToolsV2.Readers;
using Engine.ChargeDetermination;
using Engine.HornTransform;
using Engine.PeakProcessing;
using Engine.Utilities;

namespace Engine.DTAProcessing
{
    /// <summary>
    /// This class is used by DeconMSn
    /// </summary>
    internal class DTAProcessor
    {
        public int MinIonCount { get; set; }
        public int MinScan { get; set; }
        public int MaxScan { get; set; }
        public int ConsiderCharge { get; set; }
        public int IsolationWindowSize { get; set; }

        public double MinMass { get; set; }
        public double MaxMass { get; set; }
        public double PeakBackgroundRatio { get; set; }
        public double PeptideMinBackgroundRatio { get; set; }
        public double MinPeptideIntensity { get; set; }
        private double _parentIntensity;
        private double _parentMz;

        private const int WindowSize = 5;
        private const double CCMass = 1.00727638;
        private const double MinFitForSingleSpectra = 0.1;

        public bool CreateLogFileOnly { get; set; }
        public bool CreateCompositeDTA { get; set; }
        public bool ConsiderMultiplePrecursors { get; set; }

        public bool DoCentroidMSn { get; set; }

        public bool IsProfileDataForMzXML { get; set; }
        private bool _firstScanWritten;

        private List<double> _parentScanMzs = new List<double>();
        private List<double> _parentScanIntensities = new List<double>();
        private List<double> _msNScanMzs = new List<double>();
        private List<double> _msNScanIntensities = new List<double>();
        private readonly List<int> _chargeStateList = new List<int>();

        private clsPeak _parentPeak;
        private readonly PeakProcessor _parentPeakProcessor = new PeakProcessor();
        private readonly PeakProcessor _msNPeakProcessor = new PeakProcessor();
        private readonly PeakProcessor _summedPeakProcessor = new PeakProcessor();
        private readonly clsHornTransform _massTransform = new clsHornTransform();
        private readonly SVMChargeDetermine _svmChargeDetermination = new SVMChargeDetermine();
        private readonly List<clsHornTransformResults> _transformRecords = new List<clsHornTransformResults>();
        private readonly List<MSnInformationRecord> _msnRecords = new List<MSnInformationRecord>();
        private readonly List<ProfileRecord> _profileRecords = new List<ProfileRecord>();

        private clsDTAGenerationParameters _dtaOptions = new clsDTAGenerationParameters();

        public int MSnScanIndex { get; set; }
        public int NumMSnScansProcessed { get; set; }
        public int NumDTARecords { get; set; }

        public bool DoWriteProgressFile { get; set; }

        public Readers.RawData RawDataDTA { get; set; }
        public FileType DatasetType { get; set; }

        private readonly SortedDictionary<int, int> _msNScanToParentMapping = new SortedDictionary<int, int>();

        public string LogFilename { get; set; }
        public string ProgressFilename { get; set; }
        public string CombinedDTAFilename { get; set; }
        public string ProfileFilename { get; set; }
        public string MGFFilename { get; set; }
        public string OutputFile { get; set; }
        public string DatasetName { get; set; }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public StreamWriter LogFileWriter { get; set; }
        public StreamWriter CombinedDTAFileWriter { get; set; }
        public StreamWriter MGFFileWriter { get; set; }

        /// <summary>
        /// default constructor.
        /// </summary>
        public DTAProcessor()
        {
            RawDataDTA = null;
            MaxMass = 0.0;
            MaxScan = 0;
            MinIonCount = 0;
            MinMass = 0.0;
            _parentMz = 0.0;
            _parentIntensity = 0.0;
            MinScan = 0;
            MSnScanIndex = 0;
            ConsiderCharge = 0;
            NumDTARecords = 0;
            NumMSnScansProcessed = 0;
            IsolationWindowSize = 3;
            ConsiderMultiplePrecursors = false;
            DoCentroidMSn = false;
            DatasetType = FileType.FINNIGAN;
            IsProfileDataForMzXML = false;
            _firstScanWritten = false;
        }

        // destructor.
        [Obsolete("Only used by Decon2LS.UI", false)]
        ~DTAProcessor()
        {
            RawDataDTA?.Close();
        }

#if Enable_Obsolete
        /// <summary>
        /// Set options
        /// </summary>
        /// <param name="minIonCount"></param>
        /// <param name="minScan"></param>
        /// <param name="maxScan"></param>
        /// <param name="minMass"></param>
        /// <param name="maxMass"></param>
        /// <param name="createLogFileOnly"></param>
        /// <param name="createCompositeDTA"></param>
        /// <param name="considerCharge"></param>
        /// <param name="considerMultiplePrecursors"></param>
        /// <param name="isolationWindowSize"></param>
        /// <param name="isProfileDataForMzXML"></param>
        [Obsolete("Only used by Decon2LS.UI", false)]
        public void SetDTAOptions(int minIonCount, int minScan, int maxScan, double minMass, double maxMass,
            bool createLogFileOnly, bool createCompositeDTA, int considerCharge, bool considerMultiplePrecursors,
            int isolationWindowSize, bool isProfileDataForMzXML)
        {
            MinIonCount = minIonCount;
            MaxMass = maxMass;
            MinMass = minMass;
            MaxScan = maxScan;
            MinScan = minScan;
            CreateCompositeDTA = createCompositeDTA;
            CreateLogFileOnly = createLogFileOnly;
            ConsiderCharge = considerCharge;
            IsolationWindowSize = isolationWindowSize;
            ConsiderMultiplePrecursors = considerMultiplePrecursors;
            IsProfileDataForMzXML = isProfileDataForMzXML;
        }

        /// <summary>
        /// Set the deconMSn options
        /// </summary>
        /// <param name="minIonCount"></param>
        /// <param name="minScan"></param>
        /// <param name="maxScan"></param>
        /// <param name="minMass"></param>
        /// <param name="maxMass"></param>
        /// <param name="createLogFileOnly"></param>
        /// <param name="createCompositeDTA"></param>
        /// <param name="considerCharge"></param>
        /// <param name="considerMultiplePrecursors"></param>
        /// <param name="centroid"></param>
        /// <param name="isolationWindowSize"></param>
        /// <param name="isProfileDataForMzXML"></param>
        [Obsolete("Only used by Decon2LS.UI", false)]
        public void SetDTAOptions(int minIonCount, int minScan, int maxScan, double minMass, double maxMass,
            bool createLogFileOnly, bool createCompositeDTA, int considerCharge, bool considerMultiplePrecursors,
            bool centroid, int isolationWindowSize, bool isProfileDataForMzXML)
        {
            MinIonCount = minIonCount;
            MaxMass = maxMass;
            MinMass = minMass;
            MaxScan = maxScan;
            MinScan = minScan;
            CreateCompositeDTA = createCompositeDTA;
            CreateLogFileOnly = createLogFileOnly;
            ConsiderCharge = considerCharge;
            IsolationWindowSize = isolationWindowSize;
            ConsiderMultiplePrecursors = considerMultiplePrecursors;
            DoCentroidMSn = centroid;
            IsProfileDataForMzXML = isProfileDataForMzXML;
            _firstScanWritten = false;
        }
#endif

        public clsDTAGenerationParameters DtaOptions
        {
            get => _dtaOptions;
            set
            {
                _dtaOptions = value;
                MinIonCount = _dtaOptions.MinIonCount;
                MaxMass = _dtaOptions.MaxMass;
                MinMass = _dtaOptions.MinMass;
                MaxScan = _dtaOptions.MaxScan;
                MinScan = _dtaOptions.MinScan;
                CreateCompositeDTA = _dtaOptions.CreateCompositeDTA;
                CreateLogFileOnly = _dtaOptions.CreateLogFileOnly;
                ConsiderCharge = _dtaOptions.ConsiderChargeValue;
                IsolationWindowSize = _dtaOptions.IsolationWindowSize;
                ConsiderMultiplePrecursors = _dtaOptions.ConsiderMultiplePrecursors;
                DoCentroidMSn = _dtaOptions.CentroidMSn;
                IsProfileDataForMzXML = _dtaOptions.IsProfileDataForMzXML;
                _firstScanWritten = false;
            }
        }

#if Enable_Obsolete
        [Obsolete("Only used by Decon2LS.UI", false)]
        public void SetPeakParameters(double pkBkgRatio, double peptideMinBkgRatio)
        {
            SetPeakParametersLowResolution(pkBkgRatio, peptideMinBkgRatio);
        }
#endif

        public void SetPeakParametersLowResolution(double pkBkgRatio, double peptideMinBkgRatio)
        {
            PeakBackgroundRatio = pkBkgRatio;
            PeptideMinBackgroundRatio = peptideMinBkgRatio;
        }

        /// <summary>
        /// Sets the Peak Processor Options for peak data
        /// </summary>
        /// <param name="s2n">Signal-To-Noise Ratio</param>
        /// <param name="thresh">Peak-Intensity Threshold</param>
        /// <param name="thresholded"></param>
        /// <param name="fitType">Peak-Fit Type</param>
        public void SetPeakProcessorOptions(double s2n, double thresh, bool thresholded,
            PEAK_FIT_TYPE fitType)
        {
            _parentPeakProcessor.SetOptions(s2n, thresh, thresholded, fitType);
            _msNPeakProcessor.SetOptions(s2n, thresh, thresholded, fitType);
            _summedPeakProcessor.SetOptions(s2n, thresh, thresholded, fitType);
        }

        public clsHornTransformParameters MassTransformOptions
        {
            get => _massTransform.TransformParameters;
            set => _massTransform.TransformParameters = value;
        }

#if Enable_Obsolete
        [Obsolete("Use MassTransformOptions instead")]
        public void SetMassTransformOptions(short maxCharge, double maxMw, double maxFit, double minS2n,
            double ccMass, double deleteThresholdIntensity, double minTheoreticalIntensityForScore,
            short numPeaksForShoulder, bool useMercuryCaching, bool o16O18Media, string averagineMf,
            string tagMf, bool thrashOrNot, bool completeFit, bool chkAgainstCh1, enmIsotopeFitType fitType,
            clsElementIsotopes atomicInfo)
        {
            var mtParams = _massTransform.TransformParameters;

            mtParams.MaxCharge = maxCharge;
            mtParams.MaxMW = maxMw;
            mtParams.MaxFit = maxFit;
            mtParams.MinS2N = minS2n;
            mtParams.CCMass = ccMass;
            mtParams.DeleteIntensityThreshold = deleteThresholdIntensity;
            mtParams.MinIntensityForScore = minTheoreticalIntensityForScore;
            mtParams.NumPeaksForShoulder = numPeaksForShoulder;
            mtParams.CheckAllPatternsAgainstCharge1 = chkAgainstCh1;
            mtParams.UseMercuryCaching = useMercuryCaching;
            mtParams.O16O18Media = o16O18Media;

            mtParams.AveragineFormula = averagineMf;
            mtParams.TagFormula = tagMf;
            mtParams.ThrashOrNot = thrashOrNot;
            mtParams.CompleteFit = completeFit;

            mtParams.IsotopeFitType = fitType;
            mtParams.ElementIsotopeComposition = atomicInfo;

            _massTransform.TransformParameters = mtParams;
        }
#endif

        public bool FindPrecursorForChargeStates()
        {
            //simple check for charge state
            var fwhm = _parentPeak.FWHM;

            // This has known to happen
            if (fwhm > 1)
                return false;

            //preference given to higher charge states
            try
            {
                var monoMz = 0d;
                var monoIntensity = 0d;
                var maxIntensity = _parentIntensity;
                var peakIndex = 0;
                for (var cs = 4; cs > 0; cs--)
                {
                    //Look back first
                    var prevMz = _parentMz - CCMass / cs;
                    clsPeak prevPeak;
                    _parentPeakProcessor.PeakData.FindPeakAbsolute(prevMz - fwhm / cs, prevMz + fwhm / cs,
                        out prevPeak);
                    if (prevPeak.Mz > 0 && prevPeak.Intensity > maxIntensity / 8)
                    {
                        while (prevPeak.Mz > 0 && prevPeak.Intensity > maxIntensity / 8)
                        {
                            monoMz = prevPeak.Mz;
                            monoIntensity = prevPeak.Intensity;
                            prevMz = prevPeak.Mz - 1.003 / cs;
                            peakIndex = prevPeak.PeakIndex;
                            if (prevPeak.Intensity > maxIntensity)
                                maxIntensity = prevPeak.Intensity;
                            _parentPeakProcessor.PeakData.FindPeakAbsolute(prevMz - fwhm / cs,
                                prevMz + fwhm / cs, out prevPeak);
                        }

                        var parentMono = (_parentMz - CCMass) * cs;
                        var monoMw = (monoMz - CCMass) * cs;
                        if (Math.Abs(parentMono - monoMw) < 4) // to route out co-eluting peptides
                        {
                            _transformRecords.Add(new clsHornTransformResults
                            {
                                Mz = monoMz,
                                ChargeState = cs,
                                MonoMw = (monoMz - CCMass) * cs,
                                Fit = 1,
                                FitCountBasis = 0,
                                PeakIndex = peakIndex,
                                //MonoIntensity = (int) prevPeak.Intensity,
                                MonoIntensity = (int) monoIntensity,
                            });
                            return true;
                        }
                    }
                    // Look ahead
                    var nextMz = _parentMz + 1.003 / cs;
                    clsPeak nextPeak;
                    _parentPeakProcessor.PeakData.FindPeakAbsolute(nextMz - fwhm / cs, nextMz + fwhm / cs,
                        out nextPeak);
                    if (nextPeak.Mz > 0 && nextPeak.Intensity > maxIntensity / 8)
                    {
                        //this is the first isotope
                        _transformRecords.Add(new clsHornTransformResults
                        {
                            Mz = _parentMz,
                            ChargeState = cs,
                            MonoMw = (_parentMz - CCMass) * cs,
                            Fit = 1,
                            FitCountBasis = 0,
                            MonoIntensity = (int) _parentIntensity,
                            PeakIndex = _parentPeak.PeakIndex,
                        });
                        return true;
                    }
                }
            }
            catch (Exception)
            {
#if DEBUG
                throw;
#else
                return false;
#endif
            }

            //Not found any peaks
            return false;
        }

#if DEBUG
        public void WriteSpectrumToFile(int scanNum, List<double> mzs, List<double> intensities)
        {
            var fileName = OutputFile + "_testFile.spectra";
            using (var stream = new StreamWriter(new FileStream(fileName, FileMode.Create, FileAccess.Write, FileShare.None)))
            {
                for (var i = 0; i < mzs.Count; i++)
                {
                    stream.WriteLine("{0:F10}\t{1:F10}", mzs[i], intensities[i]);
                }
            }
        }
#endif

        public bool DeisotopeSummedSpectra(int parentScanNumber, double parentMz)
        {
            var minMZ = parentMz - WindowSize;
            var maxMZ = parentMz + WindowSize;

            List<double> fullMzList;
            List<double> fullIntensityList;

            //Settting to see if found_precursor worked
            //bool foundPrecursor = false;
            //if (_transformRecords.Count > 0)
            //    foundPrecursor = true;

            //check to see if it is a MS scan i.e. to say perform summing only on MS scans
            if (!RawDataDTA.IsMSScan(parentScanNumber))
                return false;

            //get raw data first
            try
            {
                RawDataDTA.GetSummedSpectra(out fullMzList, out fullIntensityList, parentScanNumber, 2, minMZ, maxMZ);
            }
            catch (Exception)
            {
                return false;
            }
            //write output spectra to file
            //WriteSpectrumToFile (parent_scan_number, vect_mzs_full, vect_intensities_full);

            if (fullIntensityList.Count <= 1)
                return false;

            // Condense regions where adjacent ions all have an intensity of 0
            var mzs = new List<double>();
            var intensities = new List<double>();

            var numPoints = fullIntensityList.Count;
            const int condenseDataThreshold = 10000;

            if (numPoints < condenseDataThreshold)
            {
                // Keep all of the data
                mzs = fullMzList;
                intensities = fullIntensityList;
            }
            else
            {
                const double epsilon = 1E-10;
                var mzMergeTolerancePPM = 0.25;
                var previousMZ = -1d;
                var previousIntensity = -1d;

                for (var i = 0; i < numPoints; i++)
                {
                    bool addPeak = true;

                    if (i > 0 && i < numPoints - 1)
                    {
                        if (fullIntensityList[i] > epsilon ||
                            (fullIntensityList[i] < epsilon && fullIntensityList[i - 1] > epsilon) ||
                            (fullIntensityList[i] < epsilon && fullIntensityList[i + 1] > epsilon))
                        {

                            if (previousMZ > -1)
                            {
                                var deltaMZ = fullMzList[i] - previousMZ;
                                var deltaPPM = deltaMZ / (previousMZ / 1E6);
                                if (deltaPPM < mzMergeTolerancePPM)
                                {
                                    if (fullIntensityList[i] > previousIntensity)
                                    {
                                        // Replace the previous data point with this data point
                                        mzs.RemoveAt(mzs.Count - 1);
                                        intensities.RemoveAt(intensities.Count - 1);
                                    }
                                    else
                                    {
                                        addPeak = false;
                                    }
                                }
                            }
                        }
                        else
                        {
                            addPeak = false;
                        }
                    }

                    if (addPeak)
                    {
                        mzs.Add(fullMzList[i]);
                        intensities.Add(fullIntensityList[i]);

                        previousMZ = mzs[mzs.Count - 1];
                        previousIntensity = intensities[intensities.Count - 1];
                    }

                }
            }

            //discover peaks
            //computes the average of all points in the spectrum (all points below FLT_MAX)
            var thres = DeconEngine.Utils.GetAverage(intensities, float.MaxValue);

            //compute the average of all points below that are below 5 times the average of all points in the spectrum
            var backgroundIntensity = DeconEngine.Utils.GetAverage(intensities, (float) (5 * thres));

            //ARS added the correct calculation for noise floor level from a DeconToolsV2 presentation
            //double calcBackgroundIntensity = DeconEngine.Utils.GetBackgroundLevel(intensities, float.MaxValue);

            _summedPeakProcessor.SetPeakIntensityThreshold(backgroundIntensity * 5);
            //hard-coded now, need to change it later
            _summedPeakProcessor.SetPeaksProfileType(RawDataDTA.IsProfileScan(parentScanNumber));
            var numPeaks = _summedPeakProcessor.DiscoverPeaks(mzs, intensities);
            _summedPeakProcessor.PeakData.InitializeUnprocessedPeakData();

            //System.Console.WriteLine("{0}\t{1}\t{2}", parentScanNumber, calcBackgroundIntensity, numPeaks);

            //Get ParentPeak and index in this peak processor
            var parentMzMatch = _summedPeakProcessor.GetClosestPeakMz(parentMz, out _parentPeak);
            if (_parentPeak.Intensity < MinPeptideIntensity)
            {
                return false;
            }

            var parentIndex = _parentPeak.PeakIndex;

            try
            {
                // Make sure that checking against charge 1 is false
                var mtParams = _massTransform.TransformParameters;
                var chkcharge1 = mtParams.CheckAllPatternsAgainstCharge1;
                mtParams.CheckAllPatternsAgainstCharge1 = false;

                // now start THRASH all over again
                clsPeak currentPeak;
                clsHornTransformResults transformRecord;
                var transformRecords = new List<clsHornTransformResults>();

                var foundPeak = _summedPeakProcessor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                var numDeisotoped = 0;

                while (foundPeak)
                {
                    var foundTransform = _massTransform.FindTransform(_summedPeakProcessor.PeakData,
                        ref currentPeak, out transformRecord);
                    if (foundTransform)
                    {
                        numDeisotoped++;
                        transformRecords.Add(transformRecord);
                    }
                    foundPeak = _summedPeakProcessor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                }

                //reset chk_charge1
                mtParams.CheckAllPatternsAgainstCharge1 = chkcharge1;

                // not ejecting precursor record here as the indices are going to be off
                // also algo is changed to use only THRASH if both cs are equal
                var foundTransformRecord = false;
                var transformRecord2 = new clsHornTransformResults();
                for (var recordNum = 0; recordNum < numDeisotoped; recordNum++)
                {
                    transformRecord = transformRecords[recordNum];
                    var numIsotopes = transformRecord.NumIsotopesObserved;
                    for (var isotopeNum = 0; isotopeNum < numIsotopes && !foundTransformRecord; isotopeNum++)
                    {
                        var isotopeIndex = transformRecord.IsotopePeakIndices[isotopeNum];
                        if (isotopeIndex == parentIndex)
                        {
                            foundTransformRecord = true;
                            transformRecord2 = transformRecord;
                            break;
                        }
                    }
                }
                if (foundTransformRecord)
                {
                    // set mono intensity to replace the one that summes spectra would return
                    int monoOrigIntensity;
                    clsPeak tempPeak;
                    var monoMatch = _parentPeakProcessor.GetClosestPeakMz(transformRecord2.Mz,
                        out tempPeak);
                    if (tempPeak.Intensity > MinPeptideIntensity)
                    {
                        monoOrigIntensity = (int) tempPeak.Intensity;
                    }
                    else
                    {
                        // check to see if the mono is the parent here
                        var monoMz = (transformRecord2.MonoMw) / transformRecord2.ChargeState + CCMass;
                        if (Math.Abs(monoMz - _parentMz) < 0.01)
                            monoOrigIntensity = (int)_parentIntensity;
                        else
                            monoOrigIntensity = (int) MinPeptideIntensity;
                    }

                    transformRecord2.MonoIntensity = monoOrigIntensity;

                    // store in vector and clear
                    _transformRecords.Add(transformRecord2);
                    return true;
                }

                return false;
            }
            catch (Exception)
            {
#if DEBUG
                throw;
#else
                return false;
#endif
            }
        }

        /**
        Implemented support for zoom scans, the strategy here would be to assume that the peak with the highest intensity
        was selected for fragmentation. Future work would be to be smart about precursor selection in zoom scans
        */
        public bool GenerateDTAZoomScans(int msN_scan_number, int parent_scan_number, int msN_scan_index)
        {
            var found_precursor = true;
            var smoother = new SavGolSmoother();
            smoother.SetOptions(16, 16, 4);
            smoother.Smooth(ref _parentScanMzs, ref _parentScanIntensities);
#if DEBUG
            WriteSpectrumToFile(parent_scan_number, _parentScanMzs, _parentScanIntensities);
#endif

            //mobj_parent_peak_processor->ApplyMovingAverageFilter(&mvect_mzs_parent, &mvect_intensities_parent, 3) ;

            int numPeaks = _parentPeakProcessor.DiscoverPeaks(_parentScanMzs, _parentScanIntensities);

            if (numPeaks > 0)
            {
                found_precursor = GenerateDTALowRes(msN_scan_number, parent_scan_number, msN_scan_index);
            }
            else
            {
                found_precursor = false;
            }

            return found_precursor;
        }

        public bool GenerateDTA(int msN_scan_number, int parent_scan_number)
        {
            var foundTransformRecord = false;
            var foundPrecursor = false;
            var foundTransformThruSumming = false;
            var foundOtherPrecursors = false;

            // Clear previous and reset objects
            _transformRecords.Clear();
            _parentIntensity = 0;
            _parentMz = 0;
            var transformRecord2 = new clsHornTransformResults
            {
                Mz = 0,
                ChargeState = 0,
                MonoMw = 0.0,
                Fit = -1,
                FitCountBasis = 0,
                MonoIntensity = 0,
            };

            //check if we have enough values in the ms2
            var numPeaks = _msNScanMzs.Count;
            if (numPeaks < MinIonCount)
            {
                Console.WriteLine("Skipping scan {0} since too few peaks: {1} < {2}", msN_scan_number, numPeaks, MinIonCount);
                return false;
            }

            NumMSnScansProcessed++;

            //get parent
            var parentMz = RawDataDTA.GetParentMz(msN_scan_number);

            // if -C option
            /*if (ConsiderCharge > 0)
            {
                // see if we can detect the parent first to get intensity right
                var parentMatch = _parentPeakProcessor.GetClosestPeakMz(parentMz, out _parentPeak);
                if (_parentPeak.Intensity > MinPeptideIntensity)
                {
                    _parentMz = _parentPeak.Mz;
                    _parentIntensity = _parentPeak.Intensity;
                }
                else
                {
                    _parentMz = parentMz;
                    _parentIntensity = MinPeptideIntensity;
                }

                _transformRecords.Add(new clsHornTransformResults
                {
                    Mz = _parentMz,
                    ChargeState = ConsiderCharge,
                    MonoMw = (_parentMz - CCMass) * ConsiderCharge,
                    Fit = 1,
                    FitCountBasis = 0,
                    MonoIntensity = (int) _parentIntensity,
                });
                return true;
            }*/

            //start
            var parentMzMatch = _parentPeakProcessor.GetClosestPeakMz(parentMz, out _parentPeak);
            if (_parentPeak.Intensity > MinPeptideIntensity)
            {
                _parentMz = _parentPeak.Mz;
                _parentIntensity = _parentPeak.Intensity;
                var parentIndex = _parentPeak.PeakIndex;

                // Check for CS based detection first
                foundPrecursor = FindPrecursorForChargeStates();

                var minMZ = parentMzMatch - WindowSize;
                var maxMZ = parentMzMatch + WindowSize;

                clsPeak currentPeak;
                clsHornTransformResults transformRecord;
                var transformRecords = new List<clsHornTransformResults>();

                var foundPeak = _parentPeakProcessor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                var numDeisotoped = 0;

                while (foundPeak)
                {
                    if (currentPeak.Intensity < MinPeptideIntensity)
                        break;
                    var foundTransform = _massTransform.FindTransform(_parentPeakProcessor.PeakData,
                        ref currentPeak, out transformRecord);
                    if (foundTransform)
                    {
                        numDeisotoped++;
                        transformRecords.Add(transformRecord);
                    }
                    foundPeak = _parentPeakProcessor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                }

                if (numDeisotoped == 1)
                {
                    // only  one so get it in
                    foundTransformRecord = false;
                    transformRecord = new clsHornTransformResults(transformRecords[0]);
                    var numIsotopes = transformRecord.NumIsotopesObserved;
                    if (transformRecord.Fit < MinFitForSingleSpectra)
                        // AM: to give slight edge to summing
                    {
                        for (var isotopeNum = 0; isotopeNum < numIsotopes && !foundTransformRecord; isotopeNum++)
                        {
                            var isotopeIndex = transformRecord.IsotopePeakIndices[isotopeNum];
                            if (isotopeIndex == parentIndex)
                            {
                                foundTransformRecord = true;
                                transformRecord2 = transformRecord;
                                break;
                            }
                        }
                    }
                    if (foundTransformRecord)
                    {
                        _transformRecords.Add(transformRecord2);
                        return true;
                    }
                }
                else if (numDeisotoped > 1)
                {
                    if (!ConsiderMultiplePrecursors)
                    {
                        // more than one and consider only one precursor, so might be co-eluting peptides
                        // see if we mave a match first
                        foundTransformRecord = false;
                        for (var transformNum = 0;
                            transformNum < transformRecords.Count && !foundTransformRecord;
                            transformNum++)
                        {
                            transformRecord = new clsHornTransformResults(transformRecords[transformNum]);
                            var numIsotopes = transformRecord.NumIsotopesObserved;
                            if (transformRecord.Fit < MinFitForSingleSpectra)
                                // AM: to give slight edge to summing
                            {
                                for (var isotopeNum = 0; isotopeNum < numIsotopes; isotopeNum++)
                                {
                                    var isotopeIndex = transformRecord.IsotopePeakIndices[isotopeNum];
                                    if (isotopeIndex == parentIndex)
                                    {
                                        foundTransformRecord = true;
                                        transformRecord2 = transformRecord;
                                        break;
                                    }
                                }
                            }
                        }
                        if (foundTransformRecord)
                        {
                            if (foundPrecursor)
                            {
                                // found both THRASH and precursor, so if precursor is present in any of the
                                // transform records eliminate it
                                var foundPrecursorRecord = false;
                                var precursorRecord = _transformRecords[0];
                                for (var transformNum = 0;
                                    transformNum < transformRecords.Count && !foundPrecursorRecord;
                                    transformNum++)
                                {
                                    transformRecord = new clsHornTransformResults(transformRecords[transformNum]);
                                    var numIsotopes = transformRecord.NumIsotopesObserved;
                                    for (var isotopeNum = 0; isotopeNum < numIsotopes; isotopeNum++)
                                    {
                                        var isotopeIndex = transformRecord.IsotopePeakIndices[isotopeNum];
                                        if (isotopeIndex == precursorRecord.PeakIndex)
                                        {
                                            //found it, eject it
                                            foundPrecursorRecord = true;
                                            _transformRecords.Clear();
                                            _transformRecords.Add(new clsHornTransformResults(transformRecord2));
                                            break;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                _transformRecords.Add(new clsHornTransformResults(transformRecord2));
                                return true;
                            }
                        }
                    }
                    else
                    {
                        //consider multiple precursors in on, so spit dtas for all precursors with IsolationWindowSize from parent peak
                        for (var transformNum = 0; transformNum < transformRecords.Count; transformNum++)
                        {
                            transformRecord = new clsHornTransformResults(transformRecords[transformNum]);
                            if (Math.Abs(transformRecord.Mz - parentMzMatch) < IsolationWindowSize)
                            {
                                _transformRecords.Add(transformRecord);
                                foundOtherPrecursors = true;
                            }
                        }

                        if (foundOtherPrecursors)
                            return true;
                    }
                }
            }
            else
            {
                // to set parent intensity find the parent that is closest to the reported parent mz
                clsPeak tempPeak;
                _parentPeakProcessor.PeakData.FindPeak(parentMz - 0.02, parentMz + 0.02, out tempPeak);
                if (tempPeak.Intensity > 0)
                {
                    _parentMz = tempPeak.Mz;
                    _parentIntensity = tempPeak.Intensity;
                }
                else
                {
                    _parentMz = parentMz;
                    _parentIntensity = MinPeptideIntensity;
                }
            }

            if (!foundTransformRecord)
            {
                // Either THRASH failed or Peak was below noise floor for that scan so sum spectra
                GetParentScanSpectra(parent_scan_number, PeakBackgroundRatio, PeptideMinBackgroundRatio);
                foundTransformThruSumming = DeisotopeSummedSpectra(parent_scan_number, parentMz);
            }

            if (foundTransformThruSumming)
            {
                return true;
            }

            // passed conditions, couldn't THRASH nor FindPeak
            if (!foundPrecursor)
            {
                // assign default charge states 2 and 3
                _transformRecords.Add(new clsHornTransformResults
                {
                    Mz = _parentMz,
                    ChargeState = 2,
                    MonoMw = (_parentMz - CCMass) * 2,
                    Fit = 1,
                    FitCountBasis = 0,
                    MonoIntensity = (int)_parentIntensity,
                });

                _transformRecords.Add(new clsHornTransformResults
                {
                    Mz = _parentMz,
                    ChargeState = 3,
                    MonoMw = (_parentMz - CCMass) * 3,
                    Fit = 1,
                    FitCountBasis = 0,
                    MonoIntensity = (int) _parentIntensity,
                });
                return true;
            }

            return true;
        }

        public bool GenerateDTALowRes(int msNScanNumber, int parentScanNumber, int msNScanIndex)
        {
            //Apply a heavy smoothing using savitsky golay filter
            var smoother = new SavGolSmoother();
            smoother.SetOptions(16, 16, 4);
            smoother.Smooth(ref _parentScanMzs, ref _parentScanIntensities);

            var numPeaks = _parentPeakProcessor.DiscoverPeaks(_parentScanMzs, _parentScanIntensities) ;

            if ( numPeaks <= 0 ){
                return false;
            }

            _transformRecords.Clear();
            _parentIntensity = 0;
            _parentMz = 0;

            //check if we have enough values in the ms2
            numPeaks = _msNScanMzs.Count;
            if (numPeaks < MinIonCount)
                return false;

            // Get the parent m/z for that msn scan
            var parentMz = RawDataDTA.GetParentMz(msNScanNumber);

            // Now start
            var parentPeak = new clsPeak();
            var parentMzMatch = _parentPeakProcessor.GetClosestPeakMz(parentMz, out _parentPeak);
            if (_parentPeak.Intensity < MinPeptideIntensity)
            {
                _parentMz = parentMz;
                clsPeak tempPeak;
                _parentPeakProcessor.PeakData.FindPeak(parentMz - 0.1, parentMz + 0.1, out tempPeak);
                if (tempPeak.Intensity > 0)
                    _parentIntensity = tempPeak.Intensity;
                else
                    _parentIntensity = MinPeptideIntensity;
                parentPeak.Mz = parentMz;
                parentPeak.Intensity = _parentIntensity;
            }
            else
            {
                _parentMz = parentMzMatch;
                _parentIntensity = _parentPeak.Intensity;
                parentPeak.Mz = parentMzMatch;
                parentPeak.Intensity = _parentPeak.Intensity;
            }

            NumMSnScansProcessed++;

            // if -C option
            /*if (ConsiderCharge > 0)
            {
                _transformRecords.Add(new clsHornTransformResults
                {
                    Mz = parentMz,
                    ChargeState = ConsiderCharge,
                    MonoMw = (parentMz - CCMass) * ConsiderCharge,
                    Fit = 1,
                    FitCountBasis = 0,
                    MonoIntensity = (int) _parentIntensity,
                });
                return true;
            }*/

            //bypassed all test, proceed
            //check if +1
            var chargeOne = _svmChargeDetermination.IdentifyIfChargeOne(_msNScanMzs, _msNScanIntensities,
                parentPeak, parentScanNumber);
            if (chargeOne)
            {
                _transformRecords.Add(new clsHornTransformResults
                {
                    Mz = _parentMz,
                    ChargeState = 1,
                    MonoMw = (_parentMz - CCMass) * 1,
                    Fit = 1,
                    FitCountBasis = 0,
                    MonoIntensity = (int) _parentIntensity,
                });
                return true;
            }
            else
            {
                //get features and add to feature space
                _svmChargeDetermination.GetFeaturesForSpectra(_msNScanMzs, _msNScanIntensities, parentPeak,
                    msNScanNumber);
                _msNScanToParentMapping.Add(msNScanNumber, MSnScanIndex);
                MSnScanIndex++;

                if (IsZoomScan(parentScanNumber))
                {
                    _transformRecords.Add(new clsHornTransformResults
                    {
                        Mz = _parentMz,
                        ChargeState = 2,
                        MonoMw = (_parentMz - CCMass) * 2,
                        Fit = 1,
                        MonoIntensity = (int)_parentIntensity,
                    });

                    _transformRecords.Add(new clsHornTransformResults
                    {
                        Mz = _parentMz,
                        ChargeState = 3,
                        MonoMw = (_parentMz - CCMass) * 3,
                        Fit = 1,
                        MonoIntensity = (int) _parentIntensity,
                    });
                    //return true;
                }

                return false;
            }
        }

        public void DetermineChargeForEachScan()
        {
            _svmChargeDetermination.NormalizeDataSet();
            _svmChargeDetermination.DetermineClassForDataSet();
        }

        public void InitializeSVM(string fileName)
        {
            _svmChargeDetermination.SVMParamXmlPath = fileName;
            _svmChargeDetermination.LoadSVMFromXml();
            _svmChargeDetermination.InitializeLDA();
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public bool ContainsProfileData(int parentScan)
        {
            return RawDataDTA.IsProfileScan(parentScan);
        }

        public bool IsZoomScan(int parentScan)
        {
            if (DatasetType == FileType.FINNIGAN)
                return RawDataDTA.IsZoomScan(parentScan);

            return false;
        }

        public int GetSpectraType(int msNScanNumber)
        {
            return RawDataDTA.GetSpectrumType(msNScanNumber);
        }

        public bool IsFTData(int parentScan)
        {
            if (DatasetType == FileType.FINNIGAN)
                return RawDataDTA.IsFTScan(parentScan);
            else if (DatasetType == FileType.MZXMLRAWDATA)
            {
                bool set_FT = false;
                set_FT = RawDataDTA.IsFTScan(parentScan);
                if (set_FT)
                    return set_FT;
                else
                {
                    // return user preference
                    return IsProfileDataForMzXML;
                }
            }

            return false;
        }

        public void GetMsNSpectra(int msNScanNumber, double peakBkgRatio, double peptideMinBkgRatio)
        {
            _msNScanIntensities.Clear();
            _msNScanMzs.Clear();

            RawDataDTA.GetRawData(out _msNScanMzs, out _msNScanIntensities, msNScanNumber, DoCentroidMSn);

            var thres = DeconEngine.Utils.GetAverage(_msNScanIntensities, float.MaxValue);
            var backgroundIntensity = DeconEngine.Utils.GetAverage(_msNScanIntensities, (float) (5 * thres));
            _msNPeakProcessor.SetPeakIntensityThreshold(backgroundIntensity * peakBkgRatio);
            _msNPeakProcessor.SetPeaksProfileType(!DoCentroidMSn && RawDataDTA.IsProfileScan(msNScanNumber));

            var numPeaks = _msNPeakProcessor.DiscoverPeaks(_msNScanMzs, _msNScanIntensities);
            _msNPeakProcessor.PeakData.InitializeUnprocessedPeakData();
        }

        public void GetParentScanSpectra(int parentScanNumber, double peakBkgRatio, double peptideMinBkgRatio)
        {
            _parentScanIntensities.Clear();
            _parentScanMzs.Clear();

            RawDataDTA.GetRawData(out _parentScanMzs, out _parentScanIntensities, parentScanNumber, false);

            var thres = DeconEngine.Utils.GetAverage(_parentScanIntensities, float.MaxValue);
            var backgroundIntensity = DeconEngine.Utils.GetAverage(_parentScanIntensities, (float) (5 * thres));
            _parentPeakProcessor.SetPeakIntensityThreshold(backgroundIntensity * peakBkgRatio);
            _parentPeakProcessor.SetPeaksProfileType(RawDataDTA.IsProfileScan(parentScanNumber));

            var numPeaks = _parentPeakProcessor.DiscoverPeaks(_parentScanMzs, _parentScanIntensities);
            MinPeptideIntensity = backgroundIntensity * peptideMinBkgRatio;
            _parentPeakProcessor.PeakData.InitializeUnprocessedPeakData();
        }

        public void WriteToMGF(int msNScanNum, int parentScanNum)
        {
            //second line
            //double massplusH = 0;
            int numTransforms = _transformRecords.Count;

            //check size, else has failed params
            if (numTransforms == 0)
                return;

            try
            {
                var transformRecord = _transformRecords[0];

                // now there can only be two Entries - one through findPeak @[0] and other through THRASH @[1]
                for (var transformNum = 0; transformNum < numTransforms; transformNum++)
                {
                    transformRecord = _transformRecords[transformNum];
                    if (numTransforms > 1 && !ConsiderMultiplePrecursors)
                    {
                        // if different charges
                        if (_transformRecords[0].ChargeState == _transformRecords[1].ChargeState)
                        {
                            //changed so that THRASH is preferred by default 6/12/07
                            transformRecord = _transformRecords[1];
                            numTransforms--;
                        }
                    }
                }

                    //stick to range
                    if (transformRecord.MonoMw < MinMass ||
                        transformRecord.MonoMw > MaxMass)
                    {
                        return;
                        //continue;
                    }

                    NumDTARecords++;

                    System.Console.WriteLine(msNScanNum + "." + msNScanNum + "." + transformRecord.ChargeState);

                    var msNScanLevel = RawDataDTA.GetMSLevel(msNScanNum);
                    var parentScanLevel = RawDataDTA.GetMSLevel(parentScanNum);
                    CreateMSnRecord(transformRecord, msNScanNum, msNScanLevel, parentScanNum, parentScanLevel);
                    CreateProfileRecord(msNScanNum, parentScanNum);

                    if (CreateLogFileOnly)
                        return;

                    //first line
                    MGFFileWriter.WriteLine("BEGIN IONS");

                    //second line
                    MGFFileWriter.WriteLine("TITLE={0}.{1}.{2}.{3}.dta", DatasetName, msNScanNum, msNScanNum,
                        transformRecord.ChargeState);

                    //third line
                    var monoHmz = (transformRecord.MonoMw + (CCMass * transformRecord.ChargeState)) / transformRecord.ChargeState;
                    MGFFileWriter.WriteLine("PEPMASS={0:F12}", monoHmz);

                    //fourth line
                    if (numTransforms > 1)
                        MGFFileWriter.WriteLine("CHARGE={0}+ and {1}+", _transformRecords[0].ChargeState,
                            _transformRecords[1].ChargeState);
                    else
                        MGFFileWriter.WriteLine("CHARGE={0}+", transformRecord.ChargeState);

                    //start spectra
                    for (var i = 0; i < (int) _msNScanMzs.Count; i++)
                    {
                        var mz = _msNScanMzs[i];
                        var intensity = _msNScanIntensities[i];
                        MGFFileWriter.WriteLine("{0:F5} {1:F2}", mz, intensity);
                    }
                    MGFFileWriter.WriteLine("END IONS");
                    MGFFileWriter.WriteLine();
                //}
            }
            catch (Exception)
            {
#if DEBUG
                throw;
#else
                System.Console.Error.WriteLine("Error in creating .MGF");
#endif
            }
        }

        private void CreateProfileRecord(int msnScanNum, int parentScanNum)
        {
            var agcAccTime = RawDataDTA.GetAGCAccumulationTime(parentScanNum);
            var ticVal = RawDataDTA.GetTICForScan(parentScanNum);

            _profileRecords.Add(new ProfileRecord
            {
                MSnScanNum = msnScanNum,
                ParentScanNum = parentScanNum,
                AgcTime = agcAccTime,
                TotalIonCurrent = ticVal,
            });
        }

        private void CreateMSnRecord(clsHornTransformResults transformRecord, int msnScanNum, int msnScanLevel, int parentScan, int parentScanLevel)
        {
            var msnRecord = new MSnInformationRecord
            {
                MSnScanNum = msnScanNum,
                MSnScanLevel = msnScanLevel,
                ParentScanNum = parentScan,
                ParentScanLevel = parentScanLevel,
                ParentMz = _parentMz,
                MonoMz = transformRecord.Mz,
                Charge = (short)transformRecord.ChargeState,
                MonoMw = transformRecord.MonoMw,
                FitScore = transformRecord.Fit,
                ParentIntensity = (int)_parentIntensity,
                MonoIntensity = transformRecord.MonoIntensity,
            };

            if (transformRecord.MonoIntensity <= 0)
                msnRecord.MonoIntensity = (int)_parentIntensity;

            _msnRecords.Add(msnRecord);
        }

        public void WriteProfileFile()
        {
            using (
                var fout =
                    new StreamWriter(new FileStream(ProfileFilename, FileMode.Create, FileAccess.ReadWrite,
                        FileShare.None)))
            {
                fout.WriteLine("{0}\t{1}\t{2}\t{3}", "MSn_Scan", "Parent_Scan", "AGC_accumulation_time", "TIC");

                // sort all records wrt scan
                _profileRecords.Sort((x, y) => x.MSnScanNum.CompareTo(y.MSnScanNum));

                // now sorted output all
                foreach (var profileRecord in _profileRecords)
                {
                    fout.WriteLine("{0}\t{1}\t{2:F4}\t{3}", profileRecord.MSnScanNum,
                        profileRecord.ParentScanNum, profileRecord.AgcTime,
                        PRISM.StringUtilities.ValueToString(profileRecord.TotalIonCurrent, 7, 1e10));
                }
            }
        }

        public void WriteLogFile()
        {
            using (
                var fout =
                    new StreamWriter(new FileStream(LogFilename, FileMode.Create, FileAccess.ReadWrite,
                        FileShare.None)))
            {
                //TODO: Version number is hardcoded and needs to be read off assembly file
                fout.WriteLine("DeconMSn Version:" + "2.3.1.4");
                fout.WriteLine("Dataset:" + DatasetName);
                fout.WriteLine("Number of MSn scans processed:" + NumMSnScansProcessed);
                fout.WriteLine("Number of DTAs generated:" + NumDTARecords);
                fout.WriteLine("Date/Time:\t" + DateTime.Now.ToString("g", CultureInfo.InvariantCulture));
                fout.WriteLine("-----------------------------------------------------------\n\n");

                fout.WriteLine("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}\t{9}\t{10}", "MSn_Scan", "MSn_Level",
                    "Parent_Scan", "Parent_Scan_Level", "Parent_Mz", "Mono_Mz", "Charge_State", "Monoisotopic_Mass",
                    "Isotopic_Fit", "Parent_Intensity", "Mono_Intensity");

                // sort all records wrt scan
                _msnRecords.Sort((x, y) =>
                {
                    int result = x.MSnScanNum.CompareTo(y.MSnScanNum);
                    if (result == 0)
                    {
                        return x.Charge.CompareTo(y.Charge);
                    }
                    return result;
                });

                // now sorted output all
                foreach (var msnRecord in _msnRecords)
                {
                    fout.WriteLine("{0}\t{1}\t{2}\t{3}\t{4:F4}\t{5:F4}\t{6}\t{7:F4}\t{8:F4}\t{9}\t{10}",
                        msnRecord.MSnScanNum, msnRecord.MSnScanLevel,
                        msnRecord.ParentScanNum, msnRecord.ParentScanLevel,
                        msnRecord.ParentMz, msnRecord.MonoMz, msnRecord.Charge,
                        msnRecord.MonoMw, msnRecord.FitScore, msnRecord.ParentIntensity,
                        msnRecord.MonoIntensity);
                }
            }
        }

        // percentComplete is a value between 0 and 100
        public void WriteProgressFile(int scansProcessed, int totalScans, int percentComplete)
        {
            // get date and time
            var timeStamp = DateTime.Now.ToString("g", CultureInfo.InvariantCulture);
            try
            {
                using (var stream = new StreamWriter(new FileStream(ProgressFilename, FileMode.Create, FileAccess.Write, FileShare.Read)))
                {
                    stream.WriteLine("Percent complete:              {0}%", percentComplete);
                    stream.WriteLine("Date/Time:                     {0}", timeStamp);
                    stream.WriteLine("Number of MSn scans processed: {0}", scansProcessed);
                    stream.WriteLine("Total scans:                   {0}", totalScans);
                    stream.WriteLine("Dataset: {0}", DatasetName);
                }
            }
            catch (Exception e)
            {
                System.Console.Error.WriteLine("Exception writing progress to {0}: {1}", ProgressFilename, e.Message);
            }
        }

        public void WriteDTAFile(int msNScanNum, int parentScanNum)
        {
            //create file_name
            var massPlusH = 0d;
            var numTransforms = _transformRecords.Count;

            if (ConsiderMultiplePrecursors)
                throw new Exception(
                    "Can only consider multiple precursors for MGF creation. Change param value to false. ");

            //check size, else has failed params
            if (numTransforms == 0)
                return;

            // now there can only be two Entries - one through findPeak @[0] and other through THRASH @[1]
            for (int transformNum = 0; transformNum < numTransforms; transformNum++)
            {
                var transformRecord = _transformRecords[transformNum];
                if (numTransforms > 1)
                {
                    // if same charges
                    if (_transformRecords[0].ChargeState == _transformRecords[1].ChargeState)
                    {
                        //changed so that THRASH is preferred by default 6/12/07
                        transformRecord = _transformRecords[1];
                        numTransforms--;
                    }
                }

                if (ConsiderCharge != 0 && transformRecord.ChargeState != ConsiderCharge)
                {
                    return;
                }

                //stick to range
                if (transformRecord.MonoMw < MinMass || transformRecord.MonoMw > MaxMass)
                {
                    //return;
                    continue;
                }

                NumDTARecords++;

                // to get mono_mass  + H
                massPlusH = transformRecord.MonoMw + CCMass;

                System.Console.WriteLine(msNScanNum + "." + msNScanNum + "." + transformRecord.ChargeState);

                int msNScanLevel = RawDataDTA.GetMSLevel(msNScanNum);
                int parentScanLevel = RawDataDTA.GetMSLevel(parentScanNum);
                CreateMSnRecord(transformRecord, msNScanNum, msNScanLevel, parentScanNum, parentScanLevel);
                CreateProfileRecord(msNScanNum, parentScanNum);

                if (CreateLogFileOnly)
                    continue;

                var fileName = string.Format("{0}.{1}.{2}.{3}.dta", OutputFile, msNScanNum, msNScanNum,
                    transformRecord.ChargeState);
                /*// Purely for TomMetz's data
                var metz_mod = "";
                if (RawDataDTA.IsProfileScan(msNScanNum))
                    metz_mod = "_FTMS";
                else
                    metz_mod = "_ITMS";
                var fileName = string.Format("{0}.{1}.{2}.{3}.dta", OutputFile + metz_mod, msNScanNum, msNScanNum, transformRecord.ChargeState);
                */

                // for composite dta
                if (CreateCompositeDTA)
                {
                    if (!_firstScanWritten)
                    {
                        CombinedDTAFileWriter.WriteLine();
                        _firstScanWritten = true;
                    }

                    //fancy headers
                    CombinedDTAFileWriter.WriteLine(
                        "=================================== \"{0}.{1:D4}.{2:D4}.{3}.dta\" ==================================",
                        DatasetName, msNScanNum, msNScanNum, transformRecord.ChargeState);

                    // massH and cs
                    CombinedDTAFileWriter.WriteLine("{0:F7} {1}   scan={2} cs={3}", massPlusH, transformRecord.ChargeState,
                        msNScanNum, transformRecord.ChargeState);

                    for (var i = 0; i < _msNScanMzs.Count; i++)
                    {
                        var mz = _msNScanMzs[i];
                        var intensity = _msNScanIntensities[i];

                        if (intensity.Equals(0))
                        {
                            if (i > 0 && _msNScanIntensities[i - 1].Equals(0))
                            {
                                if (i == _msNScanMzs.Count - 1)
                                    continue;
                                else if (_msNScanIntensities[i + 1].Equals(0))
                                    continue;
                            }
                        }

                        try
                        {
                            CombinedDTAFileWriter.WriteLine("{0:F5} {1:F2}", mz, intensity);
                        }
                        catch (Exception)
                        {
#if DEBUG
                            throw;
#else
                            throw new Exception("Trouble with writing out Peaks in CDTA");
#endif
                        }
                    }
                    CombinedDTAFileWriter.WriteLine();

                    continue;
                }

                using (
                    var fout =
                        new StreamWriter(new FileStream(fileName, FileMode.Create, FileAccess.ReadWrite, FileShare.None))
                    )
                {
                    fout.WriteLine("{0:F12} {1}", massPlusH, transformRecord.ChargeState);

                    for (var i = 0; i < _msNScanMzs.Count; i++)
                    {
                        var mz = _msNScanMzs[i];
                        var intensity = _msNScanIntensities[i];
                        fout.WriteLine("{0:F5} {1:F2}", mz, intensity);
                    }
                }
            }
        }

        public void WriteLowResolutionMGFFile()
        {
            foreach (var item in _msNScanToParentMapping)
            {
                var msNScan = item.Key;
                var msNScanIndex = item.Value;
                var parentScan = RawDataDTA.GetParentScan(msNScan);

                GetMsNSpectra(msNScan, PeakBackgroundRatio, PeptideMinBackgroundRatio);
                var parentMz = RawDataDTA.GetParentMz(msNScan);
                GetParentScanSpectra(parentScan, PeakBackgroundRatio, PeptideMinBackgroundRatio);
                clsPeak parentPeak;
                var parentMzMatch = _parentPeakProcessor.GetClosestPeakMz(parentMz, out parentPeak);

                var classVal = _svmChargeDetermination.GetClassAtScanIndex(msNScanIndex);

                _chargeStateList.Clear();
                _transformRecords.Clear();
                switch (classVal)
                {
                    case 0:
                        _chargeStateList.Add(2);
                        _chargeStateList.Add(3);
                        break;
                    case 1:
                        _chargeStateList.Add(1);
                        break;
                    case 2:
                        _chargeStateList.Add(2);
                        break;
                    case 3:
                        _chargeStateList.Add(3);
                        break;
                    case 4:
                        _chargeStateList.Add(4);
                        break;
                    default:
                        break;
                }

                var numCharges = _chargeStateList.Count;

                //deconvolute with all charges states in chargeList
                for (var chargeNum = 0; chargeNum < numCharges; chargeNum++)
                {
                    var cs = _chargeStateList[chargeNum];
                    _transformRecords.Add(new clsHornTransformResults
                    {
                        Mz = parentMzMatch,
                        ChargeState = cs,
                        MonoMw = (parentMzMatch - CCMass) * cs,
                        Fit = 1,
                        FitCountBasis = 1,
                        MonoIntensity = (int) _parentIntensity,
                    });
                }

                WriteToMGF(msNScan, parentScan);
            }

            _svmChargeDetermination.ClearMemory();
        }

        public void WriteLowResolutionDTAFile()
        {
            foreach (var item in _msNScanToParentMapping)
            {
                var msNScan = item.Key;
                var msNScanIndex = item.Value;

                GetMsNSpectra(msNScan, PeakBackgroundRatio, PeptideMinBackgroundRatio);

                var parentScan = RawDataDTA.GetParentScan(msNScan);
                GetParentScanSpectra(parentScan, PeakBackgroundRatio, PeptideMinBackgroundRatio);

                var parentMz = RawDataDTA.GetParentMz(msNScan);
                var parentMzMatch = _parentPeakProcessor.GetClosestPeakMz(parentMz, out _parentPeak);
                if (_parentPeak.Intensity < MinPeptideIntensity)
                {
                    _parentMz = parentMz;
                    clsPeak tempPeak;
                    _parentPeakProcessor.PeakData.FindPeak(parentMz - 0.1, parentMz + 0.1, out tempPeak);
                    if (tempPeak.Intensity > 0)
                        _parentIntensity = tempPeak.Intensity;
                    else
                        _parentIntensity = MinPeptideIntensity;
                }
                else
                {
                    _parentMz = parentMzMatch;
                    _parentIntensity = _parentPeak.Intensity;
                }

                var classVal = _svmChargeDetermination.GetClassAtScanIndex(msNScanIndex);
                var score = _svmChargeDetermination.GetScoreAtScanIndex(msNScanIndex);

                _chargeStateList.Clear();
                _transformRecords.Clear();
                switch (classVal)
                {
                    case 0:
                        _chargeStateList.Add(2);
                        _chargeStateList.Add(3);
                        break;
                    case 1:
                        _chargeStateList.Add(1);
                        break;
                    case 2:
                        _chargeStateList.Add(2);
                        break;
                    case 3:
                        _chargeStateList.Add(3);
                        break;
                    case 4:
                        _chargeStateList.Add(4);
                        break;
                    default:
                        break;
                }

                var numCharges = _chargeStateList.Count;

                //deconvolute with all charges states in chargeList
                for (var chargeNum = 0; chargeNum < numCharges; chargeNum++)
                {
                    var cs = _chargeStateList[chargeNum];
                    _transformRecords.Add(new clsHornTransformResults
                    {
                        Mz = _parentMz,
                        ChargeState = cs,
                        MonoMw = (_parentMz - CCMass) * cs,
                        Fit = 1,
                        FitCountBasis = 1,
                        MonoIntensity = (int) _parentIntensity,
                    });
                }
                WriteDTAFile(msNScan, parentScan);
            }

            //clear up
            _svmChargeDetermination.ClearMemory();
        }
    }
}
