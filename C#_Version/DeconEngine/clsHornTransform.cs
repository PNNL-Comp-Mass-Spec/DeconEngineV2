using System;
using System.Collections.Generic;
using System.Linq;
using DeconToolsV2.Peaks;
using Engine.ChargeDetermination;
using Engine.PeakProcessing;

namespace DeconToolsV2.HornTransform
{
    public class clsHornTransform
    {
        // Ignore Spelling: Da, Deisotoping, isotopically

        private const int MaxIsotopes = 16;

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

        public bool DebugFlag;

        public clsHornTransform()
        {
            TransformParameters = new clsHornTransformParameters
            {
                IsotopeFitType = enmIsotopeFitType.AREA,
                MaxCharge = 10,
                MaxMW = 10000,
                MaxFit = 0.15,
                MinS2N = 5,
                DeleteIntensityThreshold = 1,
                MinIntensityForScore = 1,
                O16O18Media = false,
                //Charge carrier mass = [atomic mass of hydrogen (1.007825) - atomic mass of an electron (0.00054858)]
                CCMass = 1.00727638,
                NumPeaksForShoulder = 1,
                CheckAllPatternsAgainstCharge1 = false,
                IsActualMonoMZUsed = false,
                LeftFitStringencyFactor = 1,
                RightFitStringencyFactor = 1,
            };

            //_reportO18Plus2Da = false;
            DebugFlag = false;
        }

        /// <summary>
        /// Set the Horn Transform Parameters. To change the internal settings, must update the parameter object and re-set this property.
        /// </summary>
        public clsHornTransformParameters TransformParameters { get; set; }

        public int PercentDone { get; private set; }
        public string StatusMessage { get; private set; }

        public void PerformTransform(float backgroundIntensity, float minPeptideIntensity, ref float[] mzs,
            ref float[] intensities, ref clsPeak[] peaks, ref clsHornTransformResults[] transformResults)
        {
            PercentDone = 0;
            var numPoints = mzs.Length;

            if (mzs.Length == 0)
                return;

            // m/z values should be in sorted order
            double minMz = mzs[0];
            double maxMz = mzs[numPoints - 1];
            var mzList = new List<double>(mzs.Select(x => (double) x));
            var intensityList = new List<double>(intensities.Select(x => (double) x));

            var peakData = new PeakData();
            peakData.SetPeaks(peaks);
            peakData.MzList = mzList;
            peakData.IntensityList = intensityList;

            if (TransformParameters.UseMZRange)
            {
                minMz = TransformParameters.MinMZ;
                maxMz = TransformParameters.MaxMZ;
            }

            //loads 'currentPeak' with the most intense peak within minMZ and maxMZ
            var found = peakData.GetNextPeak(minMz, maxMz, out var currentPeak);
            //var fwhm_SN = currentPeak.FWHM;

            var transformRecords = new List<clsHornTransformResults>();
            var numTotalPeaks = peakData.GetNumPeaks();
            StatusMessage = "Performing Horn Transform on peaks";

            while (found)
            {
                var numPeaksLeft = peakData.GetNumUnprocessedPeaks();
                PercentDone = 100 * (numTotalPeaks - numPeaksLeft) / numTotalPeaks;
                if (PercentDone % 5 == 0)
                {
                    StatusMessage = string.Concat("Done with ", Convert.ToString(numTotalPeaks - numPeaksLeft), " of ",
                        Convert.ToString(numTotalPeaks), " peaks.");
                }
                if (currentPeak.Intensity < minPeptideIntensity)
                    break;

                //--------------------- Transform performed ------------------------------
                var foundTransform = FindTransform(peakData, ref currentPeak, out var transformRecord, backgroundIntensity);
                if (foundTransform && transformRecord.ChargeState <= TransformParameters.MaxCharge)
                {
                    if (TransformParameters.IsActualMonoMZUsed)
                    {
                        //retrieve experimental monoisotopic peak
                        var monoPeakIndex = transformRecord.IsotopePeakIndices[0];
                        peakData.GetPeak(monoPeakIndex, out var monoPeak);

                        //set threshold at 20% less than the expected 'distance' to the next peak
                        var errorThreshold = 1.003 / transformRecord.ChargeState;
                        errorThreshold = errorThreshold - errorThreshold * 0.2;

                        var calcMonoMz = transformRecord.MonoMw / transformRecord.ChargeState + 1.00727638;

                        if (Math.Abs(calcMonoMz - monoPeak.Mz) < errorThreshold)
                        {
                            transformRecord.MonoMw = monoPeak.Mz * transformRecord.ChargeState -
                                                     1.00727638 * transformRecord.ChargeState;
                        }
                    }
                    transformRecords.Add(transformRecord);
                }
                found = peakData.GetNextPeak(minMz, maxMz, out currentPeak);
            }
            PercentDone = 100;

            // Done with the transform. Lets copy them all to the given memory structure.
            //Console.WriteLine("Done with Mass Transform. Found " + transformRecords.Count + " features");

            transformResults = transformRecords.ToArray();
            PercentDone = 100;
        }

        public virtual bool FindTransform(PeakData peakData, ref clsPeak peak, out clsHornTransformResults record,
            double backgroundIntensity = 0)
        {
            record = new clsHornTransformResults();
            if (peak.SignalToNoise < TransformParameters.MinS2N || peak.FWHM.Equals(0))
            {
                return false;
            }

            //var resolution = peak.Mz / peak.FWHM;
            var chargeState = AutoCorrelationChargeDetermination.GetChargeState(peak, peakData, DebugFlag);

            if (chargeState == -1 && TransformParameters.CheckAllPatternsAgainstCharge1)
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

            if ((peak.Mz + TransformParameters.CCMass) * chargeState > TransformParameters.MaxMW)
            {
                return false;
            }

            if (TransformParameters.O16O18Media)
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
            var deleteThreshold = TransformParameters.DeleteIntensityThreshold;
            var bestFit = TransformParameters.IsotopeFitScorer.GetFitScore(peakData, chargeState, ref peak, out record, deleteThreshold,
                TransformParameters.MinIntensityForScore, TransformParameters.LeftFitStringencyFactor, TransformParameters.RightFitStringencyFactor, out var fitCountBasis,
                DebugFlag);

            // When deleting an isotopic profile, this value is set to the first m/z to perform deletion at.
            // When deleting an isotopic profile, this value is set to the last m/z to perform deletion at.
            TransformParameters.IsotopeFitScorer.GetZeroingMassRange(out var zeroingStartMz, out var zeroingStopMz, record.DeltaMz, deleteThreshold,
                DebugFlag);
            //bestFit = _isotopeFitter.GetFitScore(peakData, chargeState, peak, record, _deleteIntensityThreshold, _minTheoreticalIntensityForScore, DebugFlag);
            //_isotopeFitter.GetZeroingMassRange(_zeroingStartMz, _zeroingStopMz, record.DeltaMz, _deleteIntensityThreshold, DebugFlag);

            if (TransformParameters.CheckAllPatternsAgainstCharge1 && chargeState != 1)
            {
                var bestFitCharge1 = TransformParameters.IsotopeFitScorer.GetFitScore(peakData, 1, ref peakCharge1, out var recordCharge1,
                    deleteThreshold, TransformParameters.MinIntensityForScore, TransformParameters.LeftFitStringencyFactor,
                    TransformParameters.RightFitStringencyFactor, out var fitCountBasisCharge1, DebugFlag);

                //double bestFitCharge1 = _isotopeFitter.GetFitScore(peakData, 1, peakCharge1, recordCharge1, _deleteIntensityThreshold, _minTheoreticalIntensityForScore, DebugFlag);
                //_isotopeFitter.GetZeroingMassRange(_zeroingStartMz, _zeroingStopMz, record.DeltaMz, _deleteIntensityThreshold, DebugFlag);
                TransformParameters.IsotopeFitScorer.GetZeroingMassRange(out var startMz1, out var stopMz1, record.DeltaMz, deleteThreshold, DebugFlag);
                if (bestFit > TransformParameters.MaxFit && bestFitCharge1 < TransformParameters.MaxFit)
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

            if (bestFit > TransformParameters.MaxFit) // check if fit is good enough
                return false;

            if (DebugFlag)
                Console.Error.WriteLine("\tBack with fit = " + record.Fit);

            // Applications using this DLL should use Abundance instead of AbundanceInt
            record.Abundance = peak.Intensity;
            record.ChargeState = chargeState;

            var monoMz = record.MonoMw / record.ChargeState + TransformParameters.CCMass;

            // used when _reportO18Plus2Da is true.
            var monoPlus2Mz = record.MonoMw / record.ChargeState + 2.0 / record.ChargeState + TransformParameters.CCMass;

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
            var peakIndices = new List<int> { peak.PeakIndex };
            var mzDelta = record.DeltaMz;

            if (debug)
            {
                Console.Error.WriteLine("Clearing peak data for " + peak.Mz + " Delta = " + mzDelta);
                Console.Error.WriteLine("Zeroing range = " + zeroingStartMz + " to " + zeroingStopMz);
            }

            double maxMz = 0;
            if (TransformParameters.O16O18Media)
                maxMz = (monoMw + 3.5) / chargeState + TransformParameters.CCMass;

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
                SetPeakToZero(peak.DataIndex, ref peakData.IntensityList, ref peakData.MzList, debug);
            }

            peakData.RemovePeaks(peak.Mz - peak.FWHM, peak.Mz + peak.FWHM, debug);

            if (1 / (peak.FWHM * chargeState) < 3) // gord:  ??
            {
                record.IsotopePeakIndices.Add(peak.PeakIndex);
                peakData.RemovePeaks(zeroingStartMz, zeroingStopMz, debug);
                return;
            }

            // Delete isotopes of m/z values higher than mz of starting isotope
            for (var peakMz = peak.Mz + 1.003 / chargeState;
                (!TransformParameters.O16O18Media || peakMz <= maxMz) && peakMz <= zeroingStopMz + 2 * peak.FWHM;
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
                SetPeakToZero(nextPeak.DataIndex, ref peakData.IntensityList, ref peakData.MzList, debug);

                peakData.RemovePeaks(nextPeak.Mz - peak.FWHM, nextPeak.Mz + peak.FWHM, debug);
                peakMz = nextPeak.Mz;
            }

            // Delete isotopes of m/z values lower than mz of starting isotope
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
                SetPeakToZero(nextPeak.DataIndex, ref peakData.IntensityList, ref peakData.MzList, debug);
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

        private void SetPeakToZero(int index, ref List<double> intensities, ref List<double> mzs, bool debug = false)
        {
            var lastIntensity = intensities[index];
            var count = -1;
            //double mz1, mz2;

            if (debug)
                Console.Error.WriteLine("\t\tNum Peaks for Shoulder =" + TransformParameters.NumPeaksForShoulder);

            for (var i = index - 1; i >= 0; i--)
            {
                var thisIntensity = intensities[i];
                if (thisIntensity <= lastIntensity)
                    count = 0;
                else
                {
                    count++;
                    //mz1 = mzs[i];
                    if (count >= TransformParameters.NumPeaksForShoulder)
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
                    if (count >= TransformParameters.NumPeaksForShoulder)
                        break;
                }
                intensities[i] = 0;
                lastIntensity = thisIntensity;
            }
        }
    }
}