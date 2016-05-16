using System;
using System.Collections.Generic;
using System.Linq;
using DeconEngine;
using DeconToolsV2.Peaks;
using Engine.HornTransform;
using Engine.PeakProcessing;

namespace DeconToolsV2.HornTransform
{
    public class clsHornTransform
    {
        private readonly MassTransform _transform = new MassTransform();
        private clsHornTransformParameters _transformParameters = new clsHornTransformParameters();

        public clsHornTransformParameters TransformParameters
        {
            get { return _transform.TransformParameters; }
            set
            {
                _transformParameters = (clsHornTransformParameters) value.Clone();
                _transform.TransformParameters = value;
            }
        }

        public int PercentDone { get; private set; }
        public string StatusMessage { get; private set; }

        public void PerformTransform(float backgroundIntensity, float minPeptideIntensity, ref float[] mzs,
            ref float[] intensities, ref clsPeak[] peaks, ref clsHornTransformResults[] transformResults)
        {
            PercentDone = 0;
            var numPoints = mzs.Length;

            if (mzs.Length == 0)
                return;

            // mzs should be in sorted order
            double minMz = mzs[0];
            double maxMz = mzs[numPoints - 1];
            var mzList = new List<double>(mzs.Select(x => (double) x));
            var intensityList = new List<double>(intensities.Select(x => (double) x));

            var peakData = new PeakData();
            Utils.SetPeaks(ref peakData, ref peaks);
            peakData.MzList = mzList;
            peakData.IntensityList = intensityList;

            if (_transformParameters.UseMZRange)
            {
                minMz = _transformParameters.MinMZ;
                maxMz = _transformParameters.MaxMZ;
            }

            //loads 'currentPeak' with the most intense peak within minMZ and maxMZ
            Peak currentPeak;
            var found = peakData.GetNextPeak(minMz, maxMz, out currentPeak);
            //var fwhm_SN = currentPeak.FWHM;

            var vectTransformRecord =
                new List<IsotopeFitRecord>();

            var numTotalPeaks = peakData.GetNumPeaks();
            StatusMessage = "Performing Horn Transform on peaks";
            //mobj_transform.mbln_debug = true;
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
                IsotopeFitRecord transformRecord;
                var foundTransform = _transform.FindTransform(peakData, ref currentPeak, out transformRecord,
                    backgroundIntensity);
                if (foundTransform && transformRecord.ChargeState <= _transformParameters.MaxCharge)
                {
                    if (_transformParameters.IsActualMonoMZUsed)
                    {
                        //retrieve experimental monoisotopic peak
                        var monoPeakIndex = transformRecord.IsotopePeakIndices[0];
                        Peak monoPeak;
                        peakData.GetPeak(monoPeakIndex, out monoPeak);

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
                    vectTransformRecord.Add(transformRecord);
                }
                found = peakData.GetNextPeak(minMz, maxMz, out currentPeak);
            }
            PercentDone = 100;

            // Done with the transform. Lets copy them all to the given memory structure.
            var numTransformed = vectTransformRecord.Count;
            //mstr_status_mesg = string .Concat("Done with Mass Transform. Found ", System.Convert.ToString(numTransformed), S" features");
            //Console.WriteLine(mstr_status_mesg);

            transformResults = new clsHornTransformResults[numTransformed];
            for (var transformNum = 0; transformNum < numTransformed; transformNum++)
            {
                transformResults[transformNum] = new clsHornTransformResults(vectTransformRecord[transformNum]);
            }
            PercentDone = 100;
        }
    }
}