#if Enable_Obsolete
using System;
using System.IO;
using DeconToolsV2.HornTransform;

namespace Engine.HornTransform
{
    /// <summary>
    ///     class to store results of isotope fitting.
    /// </summary>
    [Obsolete("Use clsHornTransformResults instead", false)]
    internal class IsotopeFitRecord
    {
        public const int MaxIsotopes = 16;
        public const int MaxIdLen = 256;

        /// <summary>
        ///     intensity of feature (as a double)
        /// </summary>
        public double Abundance;

        /// <summary>
        ///     intensity of feature; maximum value that can be represented is 2147483648
        /// </summary>
        public int AbundanceInt;

        /// <summary>
        ///     average mw for the feature.
        /// </summary>
        public double AverageMw;

        /// <summary>
        ///     charge state
        /// </summary>
        public int ChargeState;

        /// <summary>
        ///     difference between obsered m/z and m/z from theoretical distribution of composition from Averagine
        /// </summary>
        public double DeltaMz;

        /// <summary>
        ///     fit value.
        /// </summary>
        public double Fit;

        /// <summary>
        ///     Number of data points used to compute the fit value
        /// </summary>
        public int FitCountBasis;

        /// <summary>
        ///     full width at half maximum of the peak.
        /// </summary>
        public double FWHM;

        /// <summary>
        ///     indices of the peak of the isotopes in the array of peaks discovered.
        /// </summary>
        public int[] IsotopePeakIndices = new int[MaxIsotopes];

        /// <summary>
        ///     intensity of monoisotopic peak observed.
        /// </summary>
        public int MonoIntensity;

        /// <summary>
        ///     monoisotopic mw of feature.
        /// </summary>
        public double MonoMw;

        /// <summary>
        ///     intensity of the third isotopic peak observed. Used by other software for processing of O16/O18  data.
        /// </summary>
        public int MonoPlus2Intensity;

        /// <summary>
        ///     mw at the most intense isotope.
        /// </summary>
        public double MostIntenseMw;

        /// <summary>
        ///     m/z value of most abundant peak in the feature.
        /// </summary>
        public double Mz;

        /// <summary>
        ///     number of isotopes
        /// </summary>
        public int NumIsotopesObserved;

        /// <summary>
        ///     index of the peak in this data.
        /// </summary>
        public int PeakIndex;

        /// <summary>
        ///     scan number of peak
        /// </summary>
        public int ScanNum;

        /// <summary>
        ///     signal to noise for the most intense isotopic peak.
        /// </summary>
        public double SignalToNoise;

        /// <summary>
        ///     if it is possible that the record is linked to any other distribution
        /// </summary>
        //bool FlagIsotopeLink;

        // default constructor
        public IsotopeFitRecord()
        {
            AbundanceInt = 0;
            Abundance = 0;
            ChargeState = -1;
            Mz = 0;
            Fit = 1;
            AverageMw = 0;
            MonoMw = 0;
            MostIntenseMw = 0;
            FWHM = 0;
            SignalToNoise = 0;
            MonoIntensity = 0;
            MonoPlus2Intensity = 0;
            PeakIndex = -1;
            NumIsotopesObserved = 0;
            //FlagIsotopeLink = false;
        }

        public IsotopeFitRecord(IsotopeFitRecord a)
        {
            PeakIndex = a.PeakIndex;
            ScanNum = a.ScanNum;
            ChargeState = a.ChargeState;
            AbundanceInt = a.AbundanceInt;
            Abundance = a.Abundance;
            Mz = a.Mz;
            Fit = a.Fit;
            FitCountBasis = a.FitCountBasis;
            AverageMw = a.AverageMw;
            MonoMw = a.MonoMw;
            MostIntenseMw = a.MostIntenseMw;
            FWHM = a.FWHM;
            SignalToNoise = a.SignalToNoise;
            MonoIntensity = a.MonoIntensity;
            MonoPlus2Intensity = a.MonoPlus2Intensity;
            DeltaMz = a.DeltaMz;
            NumIsotopesObserved = a.NumIsotopesObserved;
            IsotopePeakIndices = new int[MaxIsotopes];
            Array.Copy(a.IsotopePeakIndices, IsotopePeakIndices, MaxIsotopes);
        }

        public IsotopeFitRecord(clsHornTransformResults a)
        {
            PeakIndex = a.PeakIndex;
            ScanNum = a.ScanNum;
            ChargeState = a.ChargeState;
            //AbundanceInt = a.AbundanceInt;
            Abundance = a.Abundance;
            Mz = a.Mz;
            Fit = a.Fit;
            FitCountBasis = a.FitCountBasis;
            AverageMw = a.AverageMw;
            MonoMw = a.MonoMw;
            MostIntenseMw = a.MostIntenseMw;
            FWHM = a.FWHM;
            SignalToNoise = a.SignalToNoise;
            MonoIntensity = a.MonoIntensity;
            MonoPlus2Intensity = a.MonoPlus2Intensity;
            DeltaMz = a.DeltaMz;
            NumIsotopesObserved = a.NumIsotopesObserved;
            IsotopePeakIndices = new int[MaxIsotopes];
            for (var i = 0; i < NumIsotopesObserved; i++)
            {
                IsotopePeakIndices[i] = a.IsotopePeakIndices[i];
            }
        }

        /// <summary>
        ///     Write the values of this record to a binary writer
        /// </summary>
        /// <param name="stream"></param>
        [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files)", false)]
        public void WriteToBinaryStream(BinaryWriter stream)
        {
            // int: 4 bytes, 23 total = 92
            // short: 2 bytes, 1 total = 2
            // double: 8 bytes, 9 total = 72

            stream.Write(PeakIndex);
            stream.Write(ScanNum);
            stream.Write(ChargeState);
            stream.Write(AbundanceInt);
            stream.Write(Abundance);
            stream.Write(Mz);
            stream.Write(Fit);
            stream.Write(FitCountBasis);
            stream.Write(AverageMw);
            stream.Write(MonoMw);
            stream.Write(MostIntenseMw);
            stream.Write(FWHM);
            stream.Write(SignalToNoise);
            stream.Write(MonoIntensity);
            stream.Write(MonoPlus2Intensity);
            stream.Write(DeltaMz);
            for (var i = 0; i < MaxIsotopes; i++)
            {
                stream.Write(IsotopePeakIndices[i]);
            }
            stream.Write(NumIsotopesObserved);
        }

        /// <summary>
        ///     Read a record from a binary stream
        /// </summary>
        /// <param name="stream"></param>
        /// <returns></returns>
        [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files)", false)]
        public static IsotopeFitRecord ReadFromBinaryStream(BinaryReader stream)
        {
            var record = new IsotopeFitRecord();
            record.PeakIndex = stream.ReadInt32();
            record.ScanNum = stream.ReadInt32();
            record.ChargeState = stream.ReadInt16();
            record.AbundanceInt = stream.ReadInt32();
            record.Abundance = stream.ReadDouble();
            record.Mz = stream.ReadDouble();
            record.Fit = stream.ReadDouble();
            record.FitCountBasis = stream.ReadInt32();
            record.AverageMw = stream.ReadDouble();
            record.MonoMw = stream.ReadDouble();
            record.MostIntenseMw = stream.ReadDouble();
            record.FWHM = stream.ReadDouble();
            record.SignalToNoise = stream.ReadDouble();
            record.MonoIntensity = stream.ReadInt32();
            record.MonoPlus2Intensity = stream.ReadInt32();
            record.DeltaMz = stream.ReadDouble();
            record.IsotopePeakIndices = new int[MaxIsotopes];
            for (var i = 0; i < MaxIsotopes; i++)
            {
                record.IsotopePeakIndices[i] = stream.ReadInt32();
            }
            record.NumIsotopesObserved = stream.ReadInt32();
            return record;
        }
    }
}
#endif