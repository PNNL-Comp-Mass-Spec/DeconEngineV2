#if Enable_Obsolete
using System;
using System.Collections.Generic;
using DeconEngine;
using Engine.PeakProcessing;

namespace DeconToolsV2
{
    [Obsolete("Only used by clsPeakProcessor", false)]
    public enum enmProfileType
    {
        CENTROIDED = 0,
        PROFILE
    }
}

namespace DeconToolsV2.Peaks
{
    [Obsolete("Not accessed within DeconTools solution except through tests", false)]
    public class clsPeakProcessor
    {
        private readonly PeakProcessor _peakProcessor;
        private clsPeakProcessorParameters _parameters;

        public clsPeakProcessor()
        {
            _peakProcessor = new PeakProcessor();
            _parameters = new clsPeakProcessorParameters();
            _peakProcessor.SetOptions(_parameters.SignalToNoiseThreshold, 0, false,
                (PeakFitType) _parameters.PeakFitType);
            ProfileType = enmProfileType.PROFILE;
        }

        public enmProfileType ProfileType { get; set; }

        public double GetBackgroundIntensity(ref float[] intensities)
        {
            var thres = Utils.GetAverage(intensities, float.MaxValue);
            thres = Utils.GetAverage(intensities, (float) (5 * thres));
            return thres;
        }

        public void DiscoverPeaks(ref float[] mzs, ref float[] intensities, ref clsPeak[] peaks, float startMz,
            float stopMz)
        {
            var vectMzs = new List<double>();
            var vectIntensities = new List<double>();
            var numPoints = mzs.Length;
            for (var ptNum = 0; ptNum < numPoints; ptNum++)
            {
                vectMzs.Add(mzs[ptNum]);
                vectIntensities.Add(intensities[ptNum]);
            }

            var backgroundIntensity = GetBackgroundIntensity(ref intensities);
            _peakProcessor.SetPeakIntensityThreshold(backgroundIntensity * _parameters.PeakBackgroundRatio);

            var numPeaks = _peakProcessor.DiscoverPeaks(vectMzs, vectIntensities, startMz, stopMz);

            peaks = new clsPeak[numPeaks];
            for (var pkNum = 0; pkNum < numPeaks; pkNum++)
            {
                clsPeak pk;
                _peakProcessor.PeakData.GetPeak(pkNum, out pk);
                peaks[pkNum] = new clsPeak(pk);
            }

            vectMzs.Clear();
            vectIntensities.Clear();
        }

        public void SetOptions(clsPeakProcessorParameters parameters)
        {
            _parameters = parameters;
            // the minimum intensity is not set till the actual data is available in DiscoverPeaks
            _peakProcessor.SetOptions(_parameters.SignalToNoiseThreshold, 0, _parameters.ThresholdedData,
                (PeakFitType) _parameters.PeakFitType);
        }
    }
}

#endif