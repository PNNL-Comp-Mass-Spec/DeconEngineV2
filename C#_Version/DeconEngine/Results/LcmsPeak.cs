using System;
using System.IO;

namespace Engine.Results
{
    [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files) - only valid use is with the Peak class", false)]
    internal class LcmsPeak
    {
        /// <summary>
        ///     intensity of the peak.
        /// </summary>
        public double Intensity;

        /// <summary>
        ///     mz of the peak.
        /// </summary>
        public double Mz;

        /// <summary>
        ///     scan number for scan where peak occurred
        /// </summary>
        public int ScanNum;

        public LcmsPeak()
        {
            ScanNum = 0;
        }

        public LcmsPeak(LcmsPeak a)
        {
            ScanNum = a.ScanNum;
            Mz = a.Mz;
            Intensity = a.Intensity;
        }

        public virtual void WriteToBinaryStream(BinaryWriter stream)
        {
            stream.Write(ScanNum);
            stream.Write(Mz);
            stream.Write(Intensity);
        }

        public static LcmsPeak ReadFromBinaryStream(BinaryReader stream)
        {
            var peak = new LcmsPeak();
            peak.ScanNum = stream.ReadInt32();
            peak.Mz = stream.ReadDouble();
            peak.Intensity = stream.ReadDouble();
            return peak;
        }
    }
}