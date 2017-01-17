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
            List<int> vectIndicesToConsider = new List<int>();
            // go through all the transforms and find which ones have most abundant mass between current value and
            // x Daltons
            int numResults = marr_transformResults.Length;
            short maxCharge = 0;
            double massRange = 0;

            for (int transformNum = 0; transformNum < numResults; transformNum++)
            {
                DeconToolsV2.HornTransform.clsHornTransformResults result = marr_transformResults[transformNum];
                double massDiff = System.Math.Abs((result.MostIntenseMw - mostAbundantMW) / 1.003);
                double massDiffRound = (double) ((int) (massDiff + 0.5));
                if (massDiffRound > 3)
                    continue;
                double toleranceDiff = System.Math.Abs(massDiff - massDiffRound * 1.003);
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

            double minMZForOut = (mostAbundantMW - massRange / 2) / charge + 1.00727638;
            double maxMZForOut = (mostAbundantMW + massRange / 2) / charge + 1.00727638;
            const int numPointsForOut = 4 * 1024;
            double currentMZ = minMZForOut;
            double mzInterval = (maxMZForOut - minMZForOut) / numPointsForOut;
            sumMZs = new float[numPointsForOut];
            sumIntensities = new float[numPointsForOut];
            for (int ptNum = 0; ptNum < numPointsForOut; ptNum++)
            {
                sumMZs[ptNum] = (float) currentMZ;
                sumIntensities[ptNum] = 0;
                currentMZ += mzInterval;
            }

            Engine.Utilities.Interpolation interp = new Interpolation();
            List<double> vectMz = new List<double>();
            List<double> vectIntensity = new List<double>();
            int numPts = mzs.Length;
            for (int ptNum = 0; ptNum < numPts; ptNum++)
            {
                double mz = mzs[ptNum];
                vectMz.Add(mz);
                double intense = intensities[ptNum];
                vectIntensity.Add(intense);
            }

            interp.Spline(vectMz, vectIntensity, 0, 0);
            for (int index = 0; index < (int) vectIndicesToConsider.Count; index++)
            {
                DeconToolsV2.HornTransform.clsHornTransformResults result =
                    marr_transformResults[vectIndicesToConsider[index]];
                currentMZ = ((minMZForOut - 1.00727638) * charge) / result.ChargeState + 1.00727638;
                for (int ptNum = 0; ptNum < numPointsForOut; ptNum++)
                {
                    sumIntensities[ptNum] += (float) interp.Splint(vectMz, vectIntensity, currentMZ);
                    currentMZ += (mzInterval * charge) / result.ChargeState;
                }
            }
        }
    }
}
#endif