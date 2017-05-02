#if Enable_Obsolete
using System;
using System.Collections.Generic;
using Engine.Utilities;

namespace DeconToolsV2
{
    [Obsolete("Only used by Decon2LS.UI", false)]
    public class clsHyperTransform
    {
        public void GetHyperTransformSpectrum(
            ref DeconToolsV2.HornTransform.clsHornTransformResults[] marr_transformResults, double mostAbundantMW,
            short charge, ref float[] sumMZs, ref float[] sumIntensities, ref float[] mzs,
            ref float[] intensities)
        {
            var vectIndicesToConsider = new List<int>();
            // go through all the transforms and find which ones have most abundant mass between current value and
            // x Daltons
            var numResults = marr_transformResults.Length;
            short maxCharge = 0;
            double massRange = 0;

            for (var transformNum = 0; transformNum < numResults; transformNum++)
            {
                var result = marr_transformResults[transformNum];
                var massDiff = System.Math.Abs((result.MostIntenseMw - mostAbundantMW) / 1.003);
                var massDiffRound = (double) ((int) (massDiff + 0.5));
                if (massDiffRound > 3)
                    continue;
                var toleranceDiff = System.Math.Abs(massDiff - massDiffRound * 1.003);
                if (toleranceDiff < System.Math.Max(0.2, result.FWHM * 5))
                {
                    // consider this peak for addition.
                    vectIndicesToConsider.Add(transformNum);
                    if (result.ChargeState > maxCharge)
                        maxCharge = (short)result.ChargeState;
                    if (result.MostIntenseMw - result.MonoMw > 2 * massRange)
                    {
                        massRange = 2 * (result.MostIntenseMw - result.MonoMw);
                    }
                }
            }

            if (massRange < 8)
            {
                massRange = 8;
            }
            if (massRange > 16)
            {
                massRange = 16;
            }

            var minMZForOut = (mostAbundantMW - massRange / 2) / charge + 1.00727638;
            var maxMZForOut = (mostAbundantMW + massRange / 2) / charge + 1.00727638;
            const int numPointsForOut = 4 * 1024;
            var currentMZ = minMZForOut;
            var mzInterval = (maxMZForOut - minMZForOut) / numPointsForOut;
            sumMZs = new float[numPointsForOut];
            sumIntensities = new float[numPointsForOut];
            for (var ptNum = 0; ptNum < numPointsForOut; ptNum++)
            {
                sumMZs[ptNum] = (float) currentMZ;
                sumIntensities[ptNum] = 0;
                currentMZ += mzInterval;
            }

            var interp = new Interpolation();
            var vectMz = new List<double>();
            var vectIntensity = new List<double>();
            var numPts = mzs.Length;
            for (var ptNum = 0; ptNum < numPts; ptNum++)
            {
                double mz = mzs[ptNum];
                vectMz.Add(mz);
                double intense = intensities[ptNum];
                vectIntensity.Add(intense);
            }

            interp.Spline(vectMz, vectIntensity, 0, 0);
            for (var index = 0; index < (int) vectIndicesToConsider.Count; index++)
            {
                var result =
                    marr_transformResults[vectIndicesToConsider[index]];
                currentMZ = ((minMZForOut - 1.00727638) * charge) / result.ChargeState + 1.00727638;
                for (var ptNum = 0; ptNum < numPointsForOut; ptNum++)
                {
                    sumIntensities[ptNum] += (float) interp.Splint(vectMz, vectIntensity, currentMZ);
                    currentMZ += (mzInterval * charge) / result.ChargeState;
                }
            }
        }
    }
}
#endif