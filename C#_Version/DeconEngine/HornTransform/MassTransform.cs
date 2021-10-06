using System;
using System.Collections.Generic;
using DeconToolsV2;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;
using Engine.ChargeDetermination;
using Engine.PeakProcessing;

namespace Engine.HornTransform
{
    /// <summary>
    /// Mass transform
    /// (note that all functionality in this class has been merged into clsHornTransform)
    /// </summary>
    /// <remarks>Used by used for Decon2LS.UI and DeconMSn</remarks>
    internal class MassTransform
    {
        private const int MaxIsotopes = 16;

        /// <summary>
        /// mass of charge carrier
        /// </summary>
        private double _chargeCarrierMass;

        /// <summary>
        /// Check feature against charge 1.
        /// </summary>
        private bool _checkAgainstCharge1;

        /// <summary>
        /// Is the medium a mixture of O16 and O18 labeled peptides.
        /// </summary>
        private bool _checkO18Pairs;

        /// <summary>
        /// After deisotoping is done, we delete the isotopic profile.  This threshold sets the value of the minimum
        /// intensity of a peak to delete. Note that this intensity is in the theoretical profile which is scaled to
        /// where the maximum peak has an intensity of 100.
        /// </summary>
        /// <seealso cref="IsotopicProfileFitScorer.GetZeroingMassRange" />
        private double _deleteIntensityThreshold;

        private IsotopicProfileFitScorer _isotopeFitter;
        private enmIsotopeFitType _isotopeFitType;
        private double _leftFitStringencyFactor;

        /// <summary>
        /// maximum charge to check while deisotoping
        /// </summary>
        private short _maxCharge;

        /// <summary>
        /// maximum fit value to report a deisotoped peak
        /// </summary>
        private double _maxFit;

        /// <summary>
        /// Maximum MW for deisotoping
        /// </summary>
        private double _maxMw;

        /// <summary>
        /// minimum signal to noise for a peak to consider it for deisotoping.
        /// </summary>
        private double _minSignalToNoise;

        /// <summary>
        /// minimum intensity of a point in the theoretical profile of a peptide for it to be considered in scoring.
        /// </summary>
        /// <seealso cref="IsotopicProfileFitScorer.GetIsotopeDistribution" />
        private double _minTheoreticalIntensityForScore;

        /// <summary>
        /// Number of peaks from the monoisotope before the shoulder
        /// </summary>
        /// <remarks>
        /// After deisotoping is performed, we delete points corresponding to the isotopic profile, To do so, we move
        /// to the left and right of each isotope peak and delete points till the shoulder of the peak is reached. To
        /// decide if the current point is a shoulder, we check if the next (_numPeaksForShoulder) # of
        /// points are of continuously increasing intensity.
        /// </remarks>
        /// <seealso cref="SetPeakToZero" />
        private short _numPeaksForShoulder;

        /*
        /// <summary>
        /// If +2Da pair peaks should be reported for O18 labeling
        /// </summary>
        /// <remarks>
        /// This is usually set for O16/O18 labeling to get the intensity of the singly isotopically labeled O16 peak.
        /// i.e. When a peptide is labeled with O18 media, usually the mass is expected to shift by 4 Da because of
        /// replacement of two O16 atoms with two O18 atoms. However, sometimes because of incomplete replacement, only
        /// one O16 ends up getting replaced. This results in isotope pairs separated by 2 Da. When this flag is set,
        /// the intensity of this +2Da pair peak is reported to adjust intensity of the O18 pair subsequent to analysis.
        /// </remarks>
        private bool _reportO18Plus2Da;
        */

        private double _rightFitStringencyFactor;

        private clsHornTransformParameters _transformParameters = new clsHornTransformParameters();

        public bool DebugFlag;

        public MassTransform()
        {
            _isotopeFitType = enmIsotopeFitType.AREA;
            _isotopeFitter = new AreaFitScorer();

            _maxCharge = 10;
            _maxMw = 10000;
            _maxFit = 0.15;
            _minSignalToNoise = 5;
            _deleteIntensityThreshold = 1;
            _minTheoreticalIntensityForScore = 1;
            _checkO18Pairs = false;

            //Charge carrier mass = [atomic mass of hydrogen (1.007825) - atomic mass of an electron (0.00054858)]
            _chargeCarrierMass = 1.00727638;

            // Deprecated: _reportO18Plus2Da = false;
            DebugFlag = false;
            _numPeaksForShoulder = 1;
            _checkAgainstCharge1 = false;
            _leftFitStringencyFactor = 1;
            _rightFitStringencyFactor = 1;
        }

        public enmIsotopeFitType IsotopeFitType
        {
            get { return _isotopeFitType; }
            set
            {
                if (value != _isotopeFitType)
                {
                    _isotopeFitType = value;

                    // Until new options are provided, save the current one.
                    var oldFit = _isotopeFitter;
                    switch (_isotopeFitType)
                    {
                        case enmIsotopeFitType.PEAK:
                            _isotopeFitter = new PeakFitScorer();
                            break;
                        case enmIsotopeFitType.AREA:
                            _isotopeFitter = new AreaFitScorer();
                            break;
                        case enmIsotopeFitType.CHISQ:
                            _isotopeFitter = new ChiSqFitScorer();
                            break;
                    }
                    // copy options
                    _isotopeFitter.CloneSettings(oldFit);
                }
            }
        }

        public clsElementIsotopes ElementalIsotopeComposition
        {
            get => _isotopeFitter.ElementalIsotopeComposition;
            set => _isotopeFitter.ElementalIsotopeComposition = value;
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public clsHornTransformParameters TransformParameters
        {
            get => _transformParameters;
            set
            {
                _transformParameters = value.Clone();
                IsotopeFitType = _transformParameters.IsotopeFitType;
                _maxCharge = _transformParameters.MaxCharge;
                _maxMw = _transformParameters.MaxMW;
                _maxFit = _transformParameters.MaxFit;
                _minSignalToNoise = _transformParameters.MinS2N;
                _chargeCarrierMass = _transformParameters.CCMass;
                _deleteIntensityThreshold = _transformParameters.DeleteIntensityThreshold;
                _minTheoreticalIntensityForScore = _transformParameters.MinIntensityForScore;
                _numPeaksForShoulder = _transformParameters.NumPeaksForShoulder;
                _isotopeFitter.UseIsotopeDistributionCaching = _transformParameters.UseMercuryCaching;
                _checkO18Pairs = _transformParameters.O16O18Media;
                _checkAgainstCharge1 = _transformParameters.CheckAllPatternsAgainstCharge1;
                _leftFitStringencyFactor = _transformParameters.LeftFitStringencyFactor;
                _rightFitStringencyFactor = _transformParameters.RightFitStringencyFactor;

                _isotopeFitter.SetOptions(_transformParameters.AveragineFormula, _transformParameters.TagFormula,
                    _transformParameters.CCMass, _transformParameters.ThrashOrNot, _transformParameters.CompleteFit);

                ElementalIsotopeComposition = _transformParameters.ElementIsotopeComposition;
            }
        }

        /// <summary>
        /// Get options
        /// </summary>
        /// <param name="maxCharge"></param>
        /// <param name="maxMw"></param>
        /// <param name="maxFit"></param>
        /// <param name="minSignalToNoise"></param>
        /// <param name="chargeCarrierMass"></param>
        /// <param name="deleteThresholdIntensity"></param>
        /// <param name="minTheoreticalIntensityForScore"></param>
        /// <param name="numPeaksForShoulder"></param>
        /// <param name="checkFitAgainstCharge1"></param>
        /// <param name="useMercuryCaching"></param>
        /// <param name="isLabelledMedia"></param>
        /// <remarks>Used by Decon2LS.UI and DeconMSn</remarks>
        public void GetOptions(out short maxCharge, out double maxMw, out double maxFit, out double minSignalToNoise,
            out double chargeCarrierMass, out double deleteThresholdIntensity,
            out double minTheoreticalIntensityForScore, out short numPeaksForShoulder, out bool checkFitAgainstCharge1,
            out bool useMercuryCaching, out bool isLabelledMedia)
        {
            maxCharge = _maxCharge;
            maxMw = _maxMw;
            maxFit = _maxFit;
            minSignalToNoise = _minSignalToNoise;
            chargeCarrierMass = _chargeCarrierMass;
            deleteThresholdIntensity = _deleteIntensityThreshold;
            minTheoreticalIntensityForScore = _minTheoreticalIntensityForScore;
            numPeaksForShoulder = _numPeaksForShoulder;
            checkFitAgainstCharge1 = _checkAgainstCharge1;
            useMercuryCaching = _isotopeFitter.UseIsotopeDistributionCaching;
            isLabelledMedia = _checkO18Pairs;
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void GetIsotopeFitOptions(out string averagineFormula, out string tagFormula, out bool useThrash,
            out bool completeFitThrash)
        {
            _isotopeFitter.GetOptions(out averagineFormula, out tagFormula, out useThrash, out completeFitThrash);
        }

        /// <summary>
        /// Set options
        /// </summary>
        /// <param name="maxCharge"></param>
        /// <param name="maxMw"></param>
        /// <param name="maxFit"></param>
        /// <param name="minSignalToNoise"></param>
        /// <param name="chargeCarrierMass"></param>
        /// <param name="deleteIntensityThreshold"></param>
        /// <param name="minTheoreticalIntensityForScore"></param>
        /// <param name="numPeaksForShoulder"></param>
        /// <param name="checkFitAgainstCharge1"></param>
        /// <param name="useMercuryCaching"></param>
        /// <param name="o16O18Media"></param>
        /// <remarks>Used by Decon2LS.UI and DeconMSn</remarks>
        public void SetOptions(short maxCharge, double maxMw, double maxFit, double minSignalToNoise,
            double chargeCarrierMass, double deleteIntensityThreshold, double minTheoreticalIntensityForScore,
            short numPeaksForShoulder, bool checkFitAgainstCharge1, bool useMercuryCaching, bool o16O18Media)
        {
            _maxCharge = maxCharge;
            _maxMw = maxMw;
            _maxFit = maxFit;
            _minSignalToNoise = minSignalToNoise;
            _chargeCarrierMass = chargeCarrierMass;
            _isotopeFitter.SetChargeCarrierMass(chargeCarrierMass);
            _deleteIntensityThreshold = deleteIntensityThreshold;
            _minTheoreticalIntensityForScore = minTheoreticalIntensityForScore;
            _numPeaksForShoulder = numPeaksForShoulder;
            _checkO18Pairs = o16O18Media;
            _isotopeFitter.UseIsotopeDistributionCaching = useMercuryCaching;
            _checkAgainstCharge1 = checkFitAgainstCharge1;
        }

        /// <summary>
        /// Set options
        /// </summary>
        /// <remarks>Used by DeconMSn</remarks>
        public void SetOptions(short maxCharge, double maxMw, double maxFit, double minSignalToNoise,
            double chargeCarrierMass, double deleteIntensityThreshold, double minTheoreticalIntensityForScore,
            short numPeaksForShoulder, bool checkFitAgainstCharge1, bool useMercuryCaching, bool o16O18Media,
            double leftFitStringencyFactor, double rightFitStringencyFactor, bool isActualMonoMzUsed)
        {
            _leftFitStringencyFactor = leftFitStringencyFactor;
            _rightFitStringencyFactor = rightFitStringencyFactor;

            SetOptions(maxCharge, maxMw, maxFit, minSignalToNoise, chargeCarrierMass, deleteIntensityThreshold,
                minTheoreticalIntensityForScore, numPeaksForShoulder, checkFitAgainstCharge1, useMercuryCaching,
                o16O18Media);
        }

        /// <summary>
        /// Set isotope fit options
        /// </summary>
        /// <param name="averagineFormula"></param>
        /// <param name="tagFormula"></param>
        /// <param name="useThrash"></param>
        /// <param name="completeFitThrash"></param>
        /// <remarks>Used by Decon2LS.UI and DeconMSn</remarks>
        [Obsolete("Only used by Decon2LS.UI", false)]
        public void SetIsotopeFitOptions(string averagineFormula, string tagFormula, bool useThrash,
            bool completeFitThrash)
        {
            _isotopeFitter.SetOptions(averagineFormula, tagFormula, _chargeCarrierMass, useThrash, completeFitThrash);
        }

        public virtual bool FindTransform(PeakData peakData, ref clsPeak peak, out clsHornTransformResults record,
            double backgroundIntensity = 0)
        {
            record = new clsHornTransformResults();
            if (peak.SignalToNoise < _minSignalToNoise || peak.FWHM.Equals(0))
            {
                return false;
            }

            //var resolution = peak.Mz / peak.FWHM;
            var chargeState = AutoCorrelationChargeDetermination.GetChargeState(peak, peakData, DebugFlag);

            if (chargeState == -1 && _checkAgainstCharge1)
            {
                chargeState = 1;
            }

            if (DebugFlag)
            {
                Console.Error.WriteLine("Deisotoping :" + peak.Mz);
                Console.Error.WriteLine("Charge = " + chargeState);
            }

            if (chargeState == -1)
            {
                return false;
            }

            if ((peak.Mz + _chargeCarrierMass) * chargeState > _maxMw)
            {
                return false;
            }

            if (_checkO18Pairs)
            {
                if (peak.FWHM < 1.0 / chargeState)
                {
                    // move back by 4 Da and see if there is a peak.
                    var minMz = peak.Mz - 4.0 / chargeState - peak.FWHM;
                    var maxMz = peak.Mz - 4.0 / chargeState + peak.FWHM;
                    var found = peakData.GetPeak(minMz, maxMz, out var o16Peak);
                    if (found && !o16Peak.Mz.Equals(peak.Mz))
                    {
                        // put back the current into the to be processed list of peaks.
                        peakData.AddPeakToProcessingList(peak);
                        // reset peak to the right peak so that the calling function may
                        // know that the peak might have changed in the O16/O18 business
                        peak = o16Peak;
                        peakData.RemovePeak(peak);
                        return FindTransform(peakData, ref peak, out record, backgroundIntensity);
                    }
                }
            }

            var peakCharge1 = new clsPeak(peak);

            // Until now, we have been using constant theoretical delete intensity threshold..
            // instead, from now, we should use one that is proportional to intensity, for more intense peaks.
            // However this will not solve all problems. If thrashing occurs, then the peak intensity will
            // change when the function returns and we may not delete far enough.
            //double deleteThreshold = backgroundIntensity / peak.Intensity * 100;
            //if (backgroundIntensity ==0 || deleteThreshold > _deleteIntensityThreshold)
            //  deleteThreshold = _deleteIntensityThreshold;
            var deleteThreshold = _deleteIntensityThreshold;
            var bestFit = _isotopeFitter.GetFitScore(peakData, chargeState, ref peak, out record, deleteThreshold,
                _minTheoreticalIntensityForScore, _leftFitStringencyFactor, _rightFitStringencyFactor, out var fitCountBasis,
                DebugFlag);

            // When deleting an isotopic profile, this value is set to the first m/z to perform deletion at.
            // When deleting an isotopic profile, this value is set to the last m/z to perform deletion at.
            _isotopeFitter.GetZeroingMassRange(out var zeroingStartMz, out var zeroingStopMz, record.DeltaMz, deleteThreshold,
                DebugFlag);
            //bestFit = _isotopeFitter.GetFitScore(peakData, chargeState, peak, record, _deleteIntensityThreshold, _minTheoreticalIntensityForScore, DebugFlag);
            //_isotopeFitter.GetZeroingMassRange(_zeroingStartMz, _zeroingStopMz, record.DeltaMz, _deleteIntensityThreshold, DebugFlag);

            if (_checkAgainstCharge1 && chargeState != 1)
            {
                var bestFitCharge1 = _isotopeFitter.GetFitScore(peakData, 1, ref peakCharge1, out var recordCharge1,
                    deleteThreshold, _minTheoreticalIntensityForScore, _leftFitStringencyFactor,
                    _rightFitStringencyFactor, out var fitCountBasisCharge1, DebugFlag);

                //double bestFitCharge1 = _isotopeFitter.GetFitScore(peakData, 1, peakCharge1, recordCharge1, _deleteIntensityThreshold, _minTheoreticalIntensityForScore, DebugFlag);
                //_isotopeFitter.GetZeroingMassRange(_zeroingStartMz, _zeroingStopMz, record.DeltaMz, _deleteIntensityThreshold, DebugFlag);
                _isotopeFitter.GetZeroingMassRange(out var startMz1, out var stopMz1, record.DeltaMz, deleteThreshold, DebugFlag);
                if (bestFit > _maxFit && bestFitCharge1 < _maxFit)
                {
                    bestFit = bestFitCharge1;
                    fitCountBasis = fitCountBasisCharge1;
                    peak = peakCharge1;
                    record = new clsHornTransformResults(recordCharge1);
                    zeroingStartMz = startMz1;
                    zeroingStopMz = stopMz1;
                    chargeState = 1;
                }
            }

            if (bestFit > _maxFit) // check if fit is good enough
                return false;

            if (DebugFlag)
                Console.Error.WriteLine("\tBack with fit = " + record.Fit);

            // Applications using this DLL should use Abundance instead of AbundanceInt
            record.Abundance = peak.Intensity;
            record.ChargeState = chargeState;

            var monoMz = record.MonoMw / record.ChargeState + _chargeCarrierMass;

            // used when _reportO18Plus2Da is true.
            var monoPlus2Mz = record.MonoMw / record.ChargeState + 2.0 / record.ChargeState + _chargeCarrierMass;

            peakData.FindPeak(monoMz - peak.FWHM, monoMz + peak.FWHM, out var monoPeak);
            peakData.FindPeak(monoPlus2Mz - peak.FWHM, monoPlus2Mz + peak.FWHM, out var m3Peak);

            record.MonoIntensity = (int) monoPeak.Intensity;
            record.MonoPlus2Intensity = (int) m3Peak.Intensity;
            record.SignalToNoise = peak.SignalToNoise;
            record.FWHM = peak.FWHM;
            record.PeakIndex = peak.PeakIndex;

            SetIsotopeDistributionToZero(peakData, peak, zeroingStartMz, zeroingStopMz, record.MonoMw, chargeState, true,
                record, DebugFlag);
            if (DebugFlag)
            {
                Console.Error.WriteLine("Performed deisotoping of " + peak.Mz);
            }
            return true;
        }

        private void SetIsotopeDistributionToZero(PeakData peakData, clsPeak peak, double zeroingStartMz,
            double zeroingStopMz, double monoMw, int chargeState, bool clearSpectrum, clsHornTransformResults record,
            bool debug = false)
        {
            var peakIndices = new List<int> {
                peak.PeakIndex
            };

            var mzDelta = record.DeltaMz;

            if (debug)
            {
                Console.Error.WriteLine("Clearing peak data for " + peak.Mz + " Delta = " + mzDelta);
                Console.Error.WriteLine("Zeroing range = " + zeroingStartMz + " to " + zeroingStopMz);
            }

            double maxMz = 0;
            if (_checkO18Pairs)
                maxMz = (monoMw + 3.5) / chargeState + _chargeCarrierMass;

            var numUnprocessedPeaks = peakData.GetNumUnprocessedPeaks();
            if (numUnprocessedPeaks == 0)
            {
                record.IsotopePeakIndices.Add(peak.PeakIndex);
                return;
            }

            if (clearSpectrum)
            {
                if (debug)
                    Console.Error.WriteLine("Deleting main peak :" + peak.Mz);
                SetPeakToZero(peak.DataIndex, ref peakData.IntensityList, debug);
            }

            peakData.RemovePeaks(peak.Mz - peak.FWHM, peak.Mz + peak.FWHM, debug);

            if (1 / (peak.FWHM * chargeState) < 3) // gord:  ??
            {
                record.IsotopePeakIndices.Add(peak.PeakIndex);
                peakData.RemovePeaks(zeroingStartMz, zeroingStopMz, debug);
                return;
            }

            // Delete isotopes of m/z values higher than m/z of starting isotope
            for (var peakMz = peak.Mz + 1.003 / chargeState;
                (!_checkO18Pairs || peakMz <= maxMz) && peakMz <= zeroingStopMz + 2 * peak.FWHM;
                peakMz += 1.003 / chargeState)
            {
                if (debug)
                {
                    Console.Error.WriteLine("\tFinding next peak top from " + (peakMz - 2 * peak.FWHM) + " to " +
                                            (peakMz + 2 * peak.FWHM) + " pk = " + peakMz + " FWHM = " + peak.FWHM);
                }

                peakData.GetPeakFromAll(peakMz - 2 * peak.FWHM, peakMz + 2 * peak.FWHM, out var nextPeak);

                if (nextPeak.Mz.Equals(0))
                {
                    if (debug)
                        Console.Error.WriteLine("\t\tNo peak found.");
                    break;
                }
                if (debug)
                {
                    Console.Error.WriteLine("\t\tFound peak to delete =" + nextPeak.Mz);
                }

                // Before assuming that the next peak is indeed an isotope, we must check for the height of this
                // isotope. If the height is greater than expected by a factor of 3, lets not delete it.
                peakIndices.Add(nextPeak.PeakIndex);
                SetPeakToZero(nextPeak.DataIndex, ref peakData.IntensityList, debug);

                peakData.RemovePeaks(nextPeak.Mz - peak.FWHM, nextPeak.Mz + peak.FWHM, debug);
                peakMz = nextPeak.Mz;
            }

            // Delete isotopes of m/z values lower than m/z of starting isotope
            // TODO: Use the delta m/z to make sure to remove 1- peaks from the unprocessed list, but not from the list of peaks?
            for (var peakMz = peak.Mz - 1.003 / chargeState;
                peakMz > zeroingStartMz - 2 * peak.FWHM;
                peakMz -= 1.003 / chargeState)
            {
                if (debug)
                {
                    Console.Error.WriteLine("\tFinding previous peak top from " + (peakMz - 2 * peak.FWHM) + " to " +
                                            (peakMz + 2 * peak.FWHM) + " pk = " + peakMz + " FWHM = " + peak.FWHM);
                }

                peakData.GetPeakFromAll(peakMz - 2 * peak.FWHM, peakMz + 2 * peak.FWHM, out var nextPeak);
                if (nextPeak.Mz.Equals(0))
                {
                    if (debug)
                        Console.Error.WriteLine("\t\tNo peak found.");
                    break;
                }
                if (debug)
                {
                    Console.Error.WriteLine("\t\tFound peak to delete =" + nextPeak.Mz);
                }
                peakIndices.Add(nextPeak.PeakIndex);
                SetPeakToZero(nextPeak.DataIndex, ref peakData.IntensityList, debug);
                peakData.RemovePeaks(nextPeak.Mz - peak.FWHM, nextPeak.Mz + peak.FWHM, debug);
                peakMz = nextPeak.Mz;
            }

            if (debug)
            {
                Console.Error.WriteLine("Done Clearing peak data for " + peak.Mz);
            }

            peakIndices.Sort();
            // now insert into array.
            var numPeaksObserved = peakIndices.Count;
            var numIsotopesObserved = 0;
            var lastIsotopeNumObserved = int.MinValue;

            for (var i = 0; i < numPeaksObserved; i++)
            {
                var currentIndex = peakIndices[i];
                var currentPeak = new clsPeak(peakData.PeakTops[currentIndex]);
                var isotopeNum = (int) (Math.Abs((currentPeak.Mz - peak.Mz) * chargeState / 1.003) + 0.5);
                if (currentPeak.Mz < peak.Mz)
                    isotopeNum = -1 * isotopeNum;
                if (isotopeNum > lastIsotopeNumObserved)
                {
                    lastIsotopeNumObserved = isotopeNum;
                    numIsotopesObserved++;
                    if (numIsotopesObserved > MaxIsotopes)
                        break;
                    record.IsotopePeakIndices.Add(peakIndices[i]);
                }
                else
                {
                    record.IsotopePeakIndices[numIsotopesObserved - 1] = peakIndices[i];
                }
            }
            if (debug)
            {
                Console.Error.WriteLine("Copied " + record.NumIsotopesObserved + " isotope peak indices into record ");
            }
        }

        private void SetPeakToZero(int index, ref List<double> intensities, bool debug = false)
        {
            var lastIntensity = intensities[index];
            var count = -1;
            //double mz1, mz2;

            if (debug)
                Console.Error.WriteLine("\t\tNum Peaks for Shoulder =" + _numPeaksForShoulder);

            for (var i = index - 1; i >= 0; i--)
            {
                var thisIntensity = intensities[i];
                if (thisIntensity <= lastIntensity)
                    count = 0;
                else
                {
                    count++;
                    //mz1 = mzs[i];
                    if (count >= _numPeaksForShoulder)
                        break;
                }
                intensities[i] = 0;
                lastIntensity = thisIntensity;
            }
            count = 0;

            lastIntensity = intensities[index];
            for (var i = index; i < intensities.Count; i++)
            {
                var thisIntensity = intensities[i];
                if (thisIntensity <= lastIntensity)
                    count = 0;
                else
                {
                    count++;
                    //mz2 = mzs[i];
                    if (count >= _numPeaksForShoulder)
                        break;
                }
                intensities[i] = 0;
                lastIntensity = thisIntensity;
            }
        }

        public void Reset()
        {
            _isotopeFitter?.Reset();
        }
    }
}