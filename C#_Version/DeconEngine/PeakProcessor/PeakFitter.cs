using System;
using System.Collections.Generic;
using DeconToolsV2.Peaks;

namespace Engine.PeakProcessing
{
    /*
    /// <summary>
    /// enumeration for type of fit.
    /// </summary>
    [Obsolete("Merged into DeconToolsV2.Peaks.PEAK_FIT_TYPE, and all usages changed", true)]
    internal enum PeakFitType
    {
        /// <summary>
        /// The peak is the m/z value higher than the points before and after it
        /// </summary>
        Apex = 0,

        /// <summary>
        /// The peak is the m/z value which is a quadratic fit of the three points around the apex
        /// </summary>
        Quadratic,

        /// <summary>
        /// The peak is the m/z value which is a Lorentzian fit of the three points around the apex
        /// </summary>
        [Obsolete("Not currently used by DeconMSn or others", false)]
        Lorentzian
    }
    */

    /// <summary>
    /// Used for detecting peaks in the data.
    /// </summary>
    internal class PeakFitter
    {
        // member variable to find out information about peaks such as signal to noise and full width at half maximum.
        private readonly PeakStatistician _peakStatistician = new PeakStatistician();
        // Type of fit function used to find the peaks
        private PEAK_FIT_TYPE _peakFitType;

        /// <summary>
        /// Default constructor.
        /// </summary>
        /// <remarks>By default uses Quadratic fit.</remarks>
        public PeakFitter()
        {
            _peakFitType = PEAK_FIT_TYPE.Quadratic;
        }

        /// <summary>
        /// Sets the type of fit.
        /// </summary>
        /// <param name="type">sets the type of fit function that this instance uses.</param>
        public void SetOptions(PEAK_FIT_TYPE type)
        {
            _peakFitType = type;
        }

        /// <summary>
        /// Gets the peak that fits the point at a given index by the specified peak fit function.
        /// </summary>
        /// <param name="index">index of the point in the m/z vectors which is the apex of the peak.</param>
        /// <param name="mzs">List of raw data of m/z values.</param>
        /// <param name="intensities">List of raw data of intensities.</param>
        /// <returns>returns the m/z of the peak.</returns>
        public double Fit(int index, List<double> mzs, List<double> intensities)
        {
            if (_peakFitType == PEAK_FIT_TYPE.Apex)
                return mzs[index];
            if (_peakFitType == PEAK_FIT_TYPE.Quadratic)
                return QuadraticFit(mzs, intensities, index);
            if (_peakFitType == PEAK_FIT_TYPE.Lorentzian)
            {
                var fwhm = _peakStatistician.FindFwhm(mzs, intensities, index);
                if (!fwhm.Equals(0))
                    return LorentzianFit(mzs, intensities, index, fwhm);
                return mzs[index];
            }
            return 0.0;
        }

        /// <summary>
        /// Gets the peak that fits the point at a given index with a quadratic fit.
        /// </summary>
        /// <param name="index">index of the point in the m/z vectors which is the apex of the peak.</param>
        /// <param name="mzs">List of raw data of m/z values.</param>
        /// <param name="intensities">List of raw data of intensities.</param>
        /// <returns>returns the m/z of the peak.</returns>
        private double QuadraticFit(List<double> mzs, List<double> intensities, int index)
        {
            if (index < 1)
                return mzs[0];
            if (index >= mzs.Count - 1)
                return mzs[mzs.Count - 1];

            var x1 = mzs[index - 1];
            var x2 = mzs[index];
            var x3 = mzs[index + 1];
            var y1 = intensities[index - 1];
            var y2 = intensities[index];
            var y3 = intensities[index + 1];

            var d = (y2 - y1) * (x3 - x2) - (y3 - y2) * (x2 - x1);
            if (d.Equals(0))
                return x2; // no good.  Just return the known peak
            d = (x1 + x2 - (y2 - y1) * (x3 - x2) * (x1 - x3) / d) / 2.0;
            return d; // Calculated new peak.  Return it.
        }

        /// <summary>
        /// Gets the peak that fits the point at a given index with a Lorentzian fit.
        /// </summary>
        /// <param name="index">index of the point in the m/z vectors which is the apex of the peak.</param>
        /// <param name="mzs">List of raw data of m/z values.</param>
        /// <param name="intensities">List of raw data of intensities.</param>
        /// <param name="fwhm"></param>
        /// <returns>returns the m/z of the peak.</returns>
        private double LorentzianFit(List<double> mzs, List<double> intensities, int index, double fwhm)
        {
            var a = intensities[index];
            var vo = mzs[index];
            var e = Math.Abs((vo - mzs[index + 1]) / 100);

            if (index < 1)
                return mzs[index];
            if (index == mzs.Count)
                return mzs[index];

            var indexStart = PeakIndex.GetNearest(mzs, vo + fwhm, index) + 1;
            var indexStop = PeakIndex.GetNearest(mzs, vo - fwhm, index) - 1;

            var currentE = LorentzianLS(mzs, intensities, a, fwhm, vo, indexStart, indexStop);
            for (var i = 0; i < 50; i++)
            {
                var lastE = currentE;
                vo = vo + e;
                currentE = LorentzianLS(mzs, intensities, a, fwhm, vo, indexStart, indexStop);
                if (currentE > lastE)
                    break;
            }

            vo = vo - e;
            currentE = LorentzianLS(mzs, intensities, a, fwhm, vo, indexStart, indexStop);
            for (var i = 0; i < 50; i++)
            {
                var lastE = currentE;
                vo = vo - e;
                currentE = LorentzianLS(mzs, intensities, a, fwhm, vo, indexStart, indexStop);
                if (currentE > lastE)
                    break;
            }
            vo = vo + e;
            return vo;
        }

        /// <summary>
        /// Gets the peak that fits the point at a given index with a Lorentzian least square fit.
        /// </summary>
        /// <param name="mzs">List of raw data of m/z values.</param>
        /// <param name="intensities">List of raw data of intensities.</param>
        /// <param name="a"></param>
        /// <param name="fwhm"></param>
        /// <param name="vo"></param>
        /// <param name="lstart"></param>
        /// <param name="lstop"></param>
        /// <returns>returns the m/z of the fit peak.</returns>
        private double LorentzianLS(List<double> mzs, List<double> intensities, double a, double fwhm, double vo,
            int lstart, int lstop)
        {
            double rmsError = 0;
            for (var index = lstart; index <= lstop; index++)
            {
                var u = 2 / fwhm * (mzs[index] - vo);
                double y1 = (int) (a / (1 + u * u));
                var y2 = intensities[index];
                rmsError = rmsError + (y1 - y2) * (y1 - y2);
            }
            return rmsError;
        }
    }
}