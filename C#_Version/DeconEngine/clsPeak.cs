using System;
#if !Disable_Obsolete
using Engine.PeakProcessing;
#endif

namespace DeconToolsV2.Peaks
{
    public class clsPeak : IComparable, IComparable<clsPeak>
    {
        /// <summary>
        ///     index in mzs, intensity vectors that were used to create the peaks in
        ///     <see cref="Engine.PeakProcessing.PeakProcessor.DiscoverPeaks" />.
        /// </summary>
        public int DataIndex;

        /// <summary>
        ///     Full width at half maximum for peak.
        /// </summary>
        public double FWHM;

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
            Mz = 0;
            Intensity = 0;
            SignalToNoise = 0;
            PeakIndex = -1;
            DataIndex = -1;
            FWHM = 0;
        }

        /// <summary>
        /// Copy constructor
        /// </summary>
        /// <param name="pk"></param>
        public clsPeak(clsPeak pk)
        {
            Mz = pk.Mz;
            FWHM = pk.FWHM;
            Intensity = pk.Intensity;
            SignalToNoise = pk.SignalToNoise;
            DataIndex = pk.DataIndex;
            PeakIndex = pk.PeakIndex;
        }

        /// <summary>
        ///     Sets the members of the clsPeak.
        /// </summary>
        /// <param name="mz">m/z of the peak.</param>
        /// <param name="intensity">intensity of the peak</param>
        /// <param name="signalToNoise">signal2noise of the peak look at PeakProcessor.PeakStatistician.FindSignalToNoise</param>
        /// <param name="peakIndex">
        ///     index of the peak in PeakData.mvect_peak_tops List of the PeakData instance that was used to
        ///     generate these peaks.
        /// </param>
        /// <param name="dataIndex">
        ///     index of the peak top in the mz, intensity vectors that are the raw data input into
        ///     PeakData.DiscoverPeaks
        /// </param>
        /// <param name="fwhm">
        ///     full width half max of the peak. For details about how this is calculated look at
        ///     PeakProcessor.PeakStatistician.FindFWHM.
        /// </param>
        public clsPeak(double mz, double intensity, double signalToNoise, int peakIndex, int dataIndex, double fwhm)
        {
            Mz = mz;
            Intensity = intensity;
            SignalToNoise = signalToNoise;
            PeakIndex = peakIndex;
            DataIndex = dataIndex;
            FWHM = fwhm;
        }

#if !Disable_Obsolete
        [Obsolete("Replacing use of Peak class", false)]
        internal clsPeak(Peak pk)
        {
            Mz = pk.Mz;
            FWHM = pk.FWHM;
            Intensity = pk.Intensity;
            SignalToNoise = pk.SignalToNoise;
            DataIndex = pk.DataIndex;
            PeakIndex = pk.PeakIndex;
        }
#endif
        
#if !Disable_Obsolete
        /// <summary>
        ///     mz of the peak.
        /// </summary>
        [Obsolete("Use Mz", false)]
        public double mdbl_mz
        {
            get => Mz;
            set => Mz = value;
        }

        /// <summary>
        ///     intensity of peak.
        /// </summary>
        [Obsolete("Use Intensity", false)]
        public double mdbl_intensity
        {
            get => Intensity;
            set => Intensity = value;
        }

        /// <summary>
        ///     Signal to noise ratio
        /// </summary>
        [Obsolete("Use SignalToNoise", false)]
        public double mdbl_SN
        {
            get => SignalToNoise;
            set => SignalToNoise = value;
        }

        /// <summary>
        ///     index in <see cref="Engine.PeakProcessing.PeakData.PeakTops" /> List.
        /// </summary>
        [Obsolete("Use PeakIndex", false)]
        public int mint_peak_index
        {
            get => PeakIndex;
            set => PeakIndex = value;
        }

        /// <summary>
        ///     index in mzs, intensity vectors that were used to create the peaks in
        ///     <see cref="Engine.PeakProcessing.PeakProcessor.DiscoverPeaks" />.
        /// </summary>
        [Obsolete("Use DataIndex", false)]
        public int mint_data_index
        {
            get => DataIndex;
            set => DataIndex = value;
        }

        /// <summary>
        ///     Full width at half maximum for peak.
        /// </summary>
        [Obsolete("Use FwHm", false)]
        public double mdbl_FWHM
        {
            get => FWHM;
            set => FWHM = value;
        }
#endif

        /// <summary>
        ///     Compare 2 peaks, for sorting by intensity. Follow Sort() call with a Reverse() call to get sorted by descending
        ///     intensity
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        /// <remarks>
        ///     Used by the sort algorithms to sort List of peaks in descending order of mdbl_intensity.
        ///     Function used to sort peaks in a descending order.
        /// </remarks>
        public int CompareTo(object obj)
        {
            var other = obj as clsPeak;
            if (other == null)
            {
                throw new NotImplementedException();
            }
            return CompareTo(other);
        }

        /// <summary>
        ///     Compare 2 peaks, for sorting by intensity. Follow Sort() call with a Reverse() call to get sorted by descending
        ///     intensity
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        /// <remarks>
        ///     Used by the sort algorithms to sort List of peaks in descending order of mdbl_intensity.
        ///     Function used to sort peaks in a descending order.
        /// </remarks>
        public int CompareTo(clsPeak obj)
        {
            var result = Intensity.CompareTo(obj.Intensity);
            if (result == 0)
            {
                result = Mz.CompareTo(obj.Mz);
            }
            return result;
        }

        public override string ToString()
        {
            return Mz + " " + Intensity + " " + FWHM + " " + SignalToNoise + " " + DataIndex + " " +
                   PeakIndex + "\n";
        }

#if !Disable_Obsolete
        [Obsolete("Unused - use the CompareTo functions.", true)]
        public static bool PeakIntensityComparison(clsPeak pk1, clsPeak pk2)
        {
            if (pk1.Intensity > pk2.Intensity)
                return true;
            if (pk1.Intensity < pk2.Intensity)
                return false;
            return pk1.Mz > pk2.Mz;
        }
#endif
    }
}