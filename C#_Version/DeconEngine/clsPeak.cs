using System;
using Engine.PeakProcessing;

namespace DeconToolsV2.Peaks
{
    public class clsPeak
    {
        /// <summary>
        ///     index in mzs, intensity vectors that were used to create the peaks in
        ///     <see cref="Engine.PeakProcessing.PeakProcessor.DiscoverPeaks" />.
        /// </summary>
        public int DataIndex;

        /// <summary>
        ///     Full width at half maximum for peak.
        /// </summary>
        public double FwHm;

        /// <summary>
        ///     intensity of peak.
        /// </summary>
        public double Intensity;

        /// <summary>
        ///     mz of the peak.
        /// </summary>
        public double Mz;

        /// <summary>
        ///     index in <see cref="Engine.PeakProcessing.PeakData.PeakTops" /> List.
        /// </summary>
        public int PeakIndex;

        /// <summary>
        ///     Signal to noise ratio
        /// </summary>
        public double SignalToNoise;

        public clsPeak()
        {
        }

        internal clsPeak(Peak pk)
        {
            Mz = pk.Mz;
            FwHm = pk.FWHM;
            Intensity = pk.Intensity;
            SignalToNoise = pk.SignalToNoise;
            DataIndex = pk.DataIndex;
            PeakIndex = pk.PeakIndex;
        }
        
#if !Disable_Obsolete
        /// <summary>
        ///     mz of the peak.
        /// </summary>
        [Obsolete("Use Mz", false)]
        public double mdbl_mz
        {
            get { return Mz; }
            set { Mz = value; }
        }

        /// <summary>
        ///     intensity of peak.
        /// </summary>
        [Obsolete("Use Intensity", false)]
        public double mdbl_intensity
        {
            get { return Intensity; }
            set { Intensity = value; }
        }

        /// <summary>
        ///     Signal to noise ratio
        /// </summary>
        [Obsolete("Use SignalToNoise", false)]
        public double mdbl_SN
        {
            get { return SignalToNoise; }
            set { SignalToNoise = value; }
        }

        /// <summary>
        ///     index in <see cref="Engine.PeakProcessing.PeakData.PeakTops" /> List.
        /// </summary>
        [Obsolete("Use PeakIndex", false)]
        public int mint_peak_index
        {
            get { return PeakIndex; }
            set { PeakIndex = value; }
        }

        /// <summary>
        ///     index in mzs, intensity vectors that were used to create the peaks in
        ///     <see cref="Engine.PeakProcessing.PeakProcessor.DiscoverPeaks" />.
        /// </summary>
        [Obsolete("Use DataIndex", false)]
        public int mint_data_index
        {
            get { return DataIndex; }
            set { DataIndex = value; }
        }

        /// <summary>
        ///     Full width at half maximum for peak.
        /// </summary>
        [Obsolete("Use FwHm", false)]
        public double mdbl_FWHM
        {
            get { return FwHm; }
            set { FwHm = value; }
        }
#endif
    }
}