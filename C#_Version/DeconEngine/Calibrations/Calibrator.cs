#if Enable_Obsolete
using System;
using System.Collections.Generic;
using DeconToolsV2.Readers;
using Engine.Utilities;

namespace Engine.Calibrations
{
    /// <summary>
    /// Calibrator
    /// </summary>
    /// <remarks>Used by RawData Readers, which are mostly obsolete</remarks>
    internal class Calibrator
    {
        private const double MaxMass = 10000000;

        protected double CalibConstA;
        protected double CalibConstB;
        protected double CalibConstC;

        protected CalibrationType CalibType;
        protected double FrequencyShift; // don't know what it is yet exactly. initialize to 0.

        public Calibrator()
        {
            CalibType = CalibrationType.UNDEFINED;
        }

        public Calibrator(int calType) : this((CalibrationType) calType)
        {
        }

        public Calibrator(CalibrationType calType)
        {
            CalibType = calType;
            FrequencyShift = 0;
            LowMassFrequency = 0;
            CalibConstA = CalibConstB = CalibConstC = 0;
            IntensityNormalizer = 1;
        }

        public double IntensityNormalizer { get; set; }
        public int ByteOrder { get; set; }

        /// <summary>
        ///     Normalizer for calibrator type 2.
        /// </summary>
        public double LowMassFrequency { get; set; }

        public int NumPointsInScan { get; set; }

        [Obsolete("Use NumPointsInScan", true)]
        public int Size
        {
            get => NumPointsInScan;
            set => NumPointsInScan = value;
        }

        public double SampleRate { get; set; }

        public CalibrationType GetCalibrationType()
        {
            return CalibType;
        }

        public double Freq2Mass(double freq)
        {
            return GetMassFromFreq(freq);
        }

        protected double GetMassFromFreq0(double freq)
        {
            if (freq.Equals(0))
            {
                return MaxMass;
            }
            return CalibConstA / freq + CalibConstB / (freq * freq) + CalibConstC / (freq * freq * freq);
        }

        protected double GetMassFromFreq1(double freq)
        {
            if (freq.Equals(0))
            {
                return MaxMass;
            }
            return CalibConstA / freq + CalibConstB / (freq * freq);
        }

        protected double GetMassFromFreq2(double freq)
        {
            if (freq.Equals(0))
            {
                return MaxMass;
            }
            return CalibConstA / freq + CalibConstB / (freq * freq) + CalibConstC * IntensityNormalizer / (freq * freq);
        }

        protected double GetMassFromFreq3(double freq)
        {
            if (freq.Equals(0))
            {
                return MaxMass;
            }
            return CalibConstA / freq + CalibConstB / (freq * freq) + CalibConstC;
        }

        protected double GetMassFromFreq4(double freq)
        {
            return CalibConstA * freq + CalibConstB;
        }

        protected double GetMassFromFreq5(double freq)
        {
            return freq;
        }

        protected double GetMassFromFreq6(double freq)
        {
            if (!(freq + CalibConstB).Equals(0))
                return CalibConstA / (freq + CalibConstB);
            if (freq - CalibConstB <= 0)
                return CalibConstA;
            return 0;
        }

        protected double GetMassFromFreq7(double freq)
        {
            if (freq.Equals(0))
            {
                return MaxMass;
            }
            if (freq - CalibConstB + CalibConstC * IntensityNormalizer < 0)
            {
                return CalibConstA / freq;
            }
            return CalibConstA / (freq + CalibConstB + CalibConstC * IntensityNormalizer);
        }

        protected double GetMassFromFreq9(double freq)
        {
            double mass;
            if (freq.Equals(0))
                mass = MaxMass;
            else
                mass = (-CalibConstA - Math.Sqrt(CalibConstA * CalibConstA - 4 * (CalibConstB - freq) * CalibConstC)) /
                       (2 * (CalibConstB - freq));

            if (mass > 810 && mass < 813)
            {
                Console.WriteLine("{0}\t{1}\t{2}\t{3:F15}", CalibConstA, CalibConstB, CalibConstC, freq);
            }
            return mass;
        }

        protected double Mass2Freq0(double mass)
        {
            // Reverse the Freq2Mass equation and solve for frequency.
            mass = Math.Abs(mass);
            var p = -CalibConstA / mass;
            var q = -CalibConstB / mass;
            var r = -CalibConstC / mass;
            return GetFrequencyFromCubicEquation0(p, q, r, mass);
        }

        protected double Mass2Freq1(double mass)
        {
            // Reverse the Freq2Mass equation and solve for frequency.
            mass = Math.Abs(mass);
            var b = -CalibConstA / mass;
            var c = -CalibConstB / mass;
            return GetFrequencyFromQuadraticEquation1(b, c, mass);
        }

        protected double Mass2Freq2(double mass)
        {
            // Reverse the Freq2Mass equation and solve for frequency.
            mass = Math.Abs(mass);
            return GetFrequencyFromQuadraticEquation2(-1 * CalibConstA / mass,
                -1 * (CalibConstB + CalibConstC * IntensityNormalizer) / mass, mass);
        }

        protected double Mass2Freq3(double mass)
        {
            // Reverse the Freq2Mass equation and solve for frequency.
            mass = Math.Abs(mass);
            var a = mass - CalibConstC;
            if (a.Equals(0))
                a = 1;
            return GetFrequencyFromQuadraticEquation3(-1 * CalibConstA / a, -1 * CalibConstB / a, mass);
        }

        protected double Mass2Freq4(double mass)
        {
            // Reverse the Freq2Mass equation and solve for frequency.
            return (Math.Abs(mass) - CalibConstB) / CalibConstA;
        }

        protected double Mass2Freq5(double mass)
        {
            return mass;
        }

        protected double Mass2Freq6(double mass)
        {
            // Reverse the Freq2Mass equation and solve for frequency.
            return CalibConstA / Math.Abs(mass) - CalibConstB;
        }

        protected double Mass2Freq7(double mass)
        {
            if (mass.Equals(0))
                mass = 1;
            return CalibConstA / mass - CalibConstB - CalibConstC * IntensityNormalizer;
        }

        protected double Mass2Freq9(double mass)
        {
            // Reverse the Freq2Mass equation and solve for frequency.
            mass = Math.Abs(mass);
            if (!mass.Equals(0))
                mass = CalibConstA / mass + CalibConstC / (mass * mass) + CalibConstB;
            return mass;
        }

        protected double GetFrequencyFromCubicEquation0(double p, double q, double r, double mass)
        {
            // Solves the equation F^3 + p F^2 + q F + r = 0  and chooses root that
            // is closes to mass.
            if (mass.Equals(0))
                mass = 1;

            double[] roots;
            var numRoots = Helpers.CubeRoots(p, q, r, out roots);

            var best = 0;
            var bestDelta = double.MaxValue;
            for (var i = 0; i < numRoots; i++)
            {
                var delta = Math.Abs(GetMassFromFreq(roots[i]) - mass);
                if (delta < bestDelta)
                {
                    bestDelta = delta;
                    best = i;
                }
            }
            return roots[best];
        }

        protected double GetFrequencyFromQuadraticEquation1(double b, double c, double mass)
        {
            double[] roots;
            Helpers.SolveQuadraticEquation(b, c, out roots);
            if (roots[0] < 0 && roots[1] < 0)
                // both are negative frequencies. return MAX_MASS
                return 0;
            // otherwise return the positive one, if one of them is negative.
            if (roots[0] < 0)
                return roots[1];
            if (roots[1] < 0)
                return roots[0];

            // otherwise, return the one that gives a mass value closer to the one return.
            var mass1 = GetMassFromFreq1(roots[0]);
            var massDelta1 = mass1 - mass;
            var mass2 = GetMassFromFreq1(roots[1]);
            var massDelta2 = mass2 - mass;
            if (Math.Abs(massDelta1) < Math.Abs(massDelta2))
                return roots[0];
            return roots[1];
        }

        protected double GetFrequencyFromQuadraticEquation2(double b, double c, double mass)
        {
            double[] roots;
            Helpers.SolveQuadraticEquation(-1 * CalibConstA / mass,
                -1 * (CalibConstB + CalibConstC * IntensityNormalizer) / mass, out roots);
            if (roots[0] < 0 && roots[1] < 0)
                // both are negative frequencies. return MAX_MASS
                return 0;
            // otherwise return the positive one, if one of them is negative.
            if (roots[0] < 0)
                return roots[1];
            if (roots[1] < 0)
                return roots[0];

            // otherwise, return the one that gives a mass value closer to the one return.
            var mass1 = GetMassFromFreq2(roots[0]);
            var massDelta1 = mass1 - mass;
            var mass2 = GetMassFromFreq2(roots[1]);
            var massDelta2 = mass2 - mass;

            if (Math.Abs(massDelta1) < Math.Abs(massDelta2))
                return roots[0];
            return roots[1];
        }

        protected double GetFrequencyFromQuadraticEquation3(double b, double c, double mass)
        {
            double[] roots;
            Helpers.SolveQuadraticEquation(b, c, out roots);
            if (roots[0] < 0 && roots[1] < 0)
                // both are negative frequencies. return MAX_MASS
                return 0;
            // otherwise return the positive one, if one of them is negative.
            if (roots[0] < 0)
                return roots[1];
            if (roots[1] < 0)
                return roots[0];

            // otherwise, return the one that gives a mass value closer to the one return.
            var mass1 = GetMassFromFreq(roots[0]);
            var massDelta1 = mass1 - mass;
            var mass2 = GetMassFromFreq(roots[1]);
            var massDelta2 = mass2 - mass;
            if (Math.Abs(massDelta1) < Math.Abs(massDelta2))
                return roots[0];
            return roots[1];
        }

        protected int GetRawPointsApplyFFT5(ref float[] dataArray, out List<double> mzs, out List<double> intensities,
            int numPtsUsed)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            // no fft required here.
            if (numPtsUsed > NumPointsInScan)
                numPtsUsed = NumPointsInScan;

            var capacity = mzs.Capacity;
            if (capacity < numPtsUsed)
            {
                mzs.Capacity = numPtsUsed;
                intensities.Capacity = numPtsUsed;
            }
            for (var i = 0; i < numPtsUsed; i++)
            {
                var ms = GetMassFromFreq(i * SampleRate);
                double it = dataArray[i];
                mzs.Add(ms);
                intensities.Add(it);
            }
            return numPtsUsed;
        }

        public double Mass2Freq(double mass)
        {
            switch (CalibType)
            {
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C_OVERFCUBE:
                    return Mass2Freq0(mass);
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ:
                    return Mass2Freq1(mass);
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ_PLUS_CI_OVERFSQ:
                    return Mass2Freq2(mass);
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C:
                    return Mass2Freq3(mass);
                case CalibrationType.AF_PLUS_B:
                    return Mass2Freq4(mass);
                case CalibrationType.F:
                    return Mass2Freq5(mass);
                case CalibrationType.A_OVER_F_PLUS_B:
                    return Mass2Freq6(mass);
                case CalibrationType.A_OVER_F_PLUS_B_PLUS_CI:
                    return Mass2Freq7(mass);
                case CalibrationType.BRUKER_CALMET:
                    return Mass2Freq9(mass);
                default:
                    throw new Exception("Calibration Type not Supported");
            }
        }

        public double GetMassFromFreq(double freq)
        {
            switch (CalibType)
            {
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C_OVERFCUBE:
                    return GetMassFromFreq0(freq);
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ:
                    return GetMassFromFreq1(freq);
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ_PLUS_CI_OVERFSQ:
                    return GetMassFromFreq2(freq);
                case CalibrationType.A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C:
                    return GetMassFromFreq3(freq);
                case CalibrationType.AF_PLUS_B:
                    return GetMassFromFreq4(freq);
                case CalibrationType.F:
                    return GetMassFromFreq5(freq);
                case CalibrationType.A_OVER_F_PLUS_B:
                    return GetMassFromFreq6(freq);
                case CalibrationType.A_OVER_F_PLUS_B_PLUS_CI:
                    return GetMassFromFreq7(freq);
                case CalibrationType.BRUKER_CALMET:
                    return GetMassFromFreq9(freq);
                default:
                    throw new Exception("Calibration Type not Supported");
            }
        }

        public double MZVal(int index)
        {
            // should never be called here with CalibrationType = 5.
            if (CalibType == CalibrationType.F)
                return 0;

            double mz;
            if (index >= NumPointsInScan / 2)
                index = NumPointsInScan / 2 - 1;
            if (LowMassFrequency.Equals(0))
            {
                mz = GetMassFromFreq(FrequencyShift + index * SampleRate / NumPointsInScan);
                if (mz > 0)
                    return mz;
                mz = GetMassFromFreq(index * SampleRate / NumPointsInScan);
                if (mz > 0)
                    return mz;
                return 0;
            }
            mz = GetMassFromFreq(LowMassFrequency - FrequencyShift - index * SampleRate / NumPointsInScan);
            if (mz > 0)
                return mz;
            return 0;
        }

        public double TimeVal(int index)
        {
            if (NumPointsInScan - 1 < index)
                return -1;

            return index / SampleRate;
        }

        public int GetRawPointsApplyFFT(ref float[] dataArray, out List<double> mzs, out List<double> intensities)
        {
            return GetRawPointsApplyFFT(ref dataArray, out mzs, out intensities, NumPointsInScan);
        }

        public int GetRawPointsApplyFFT(ref float[] dataArray, out List<double> mzs, out List<double> intensities,
            int numPtsUsed)
        {
            var currentSize = NumPointsInScan;
            NumPointsInScan = numPtsUsed;
            if (CalibType == CalibrationType.F)
                return GetRawPointsApplyFFT5(ref dataArray, out mzs, out intensities, numPtsUsed);

            mzs = new List<double>();
            intensities = new List<double>();

            if (mzs.Capacity < numPtsUsed / 2)
                mzs.Capacity = numPtsUsed / 2;

            if (intensities.Capacity < numPtsUsed / 2)
                intensities.Capacity = numPtsUsed / 2;

            var sign = 1;
            var result = FFT.realft(numPtsUsed, ref dataArray, sign);

            // intensities need to be put in increasing order of mz. But here they are in
            // descending order. So insert and then reverse.
            //UPDATE:  [gord]   Bruker Solarix mz data is not in descending order!  so must check this.

            var numIncreasingValuesOutOfFirst100 = 0;
            var numDecreasingValuesOutOfFirst100 = 0;

            double previousMzValue = 0;

            for (var i = 0; i < numPtsUsed / 2; i++)
            {
                var mzVal = MZVal(i);
                if (i > 1 && i < 100)
                    // checking the first 100 points of the loop will give an idea if mz's are increasing or decreasing
                {
                    if (mzVal > previousMzValue)
                    {
                        numIncreasingValuesOutOfFirst100++;
                    }
                    else if (mzVal < previousMzValue)
                    {
                        numDecreasingValuesOutOfFirst100++;
                    }
                }

                double point1 = dataArray[2 * i];
                double point2 = dataArray[2 * i + 1];

                var intensityVal = Math.Sqrt(point2 * point2 + point1 * point1);

                if (mzVal.Equals(0))
                    continue;
                mzs.Add(mzVal);

                intensities.Add(intensityVal);
                previousMzValue = mzVal;
            }

            var numPts = intensities.Count;

            var mzValuesAreDecreasing = numDecreasingValuesOutOfFirst100 > 0;
            if (mzValuesAreDecreasing)
            {
                //reverses the array
                intensities.Reverse();
                mzs.Reverse();
            }

            var origNumPts = numPts;
            while (numPts > 1 && mzs[numPts - 1] < mzs[numPts - 2])
            {
                numPts--;
            }
            var diff = mzs[numPts - 1] - mzs[numPts - 2];
            while (numPts < origNumPts)
            {
                mzs[numPts - 1] = mzs[numPts - 2] + diff;
                numPts++;
            }
            NumPointsInScan = currentSize;
            return numPts;
        }

        public void SetCalibrationEquationParams(double a, double b, double c)
        {
            CalibConstA = a;
            CalibConstB = b;
            CalibConstC = c;
        }

        public int GetRawPointsFromTransformedData(ref float[] dataArray, out List<double> mzs,
            out List<double> intensities)
        {
            mzs = new List<double>();
            intensities = new List<double>();

            if (mzs.Capacity < NumPointsInScan / 2)
                mzs.Capacity = NumPointsInScan / 2;

            if (intensities.Capacity < NumPointsInScan)
                intensities.Capacity = NumPointsInScan / 2;

            var lastMz = double.MinValue;
            for (var i = 0; i < NumPointsInScan / 2; i++)
            {
                var mzVal = MZVal(i);
                if (mzVal < lastMz)
                {
                    // probably went over the edge of the frequency domain. Stop Here.
                    return i;
                }
                var intensityVal =
                    Math.Sqrt(dataArray[2 * i] * dataArray[2 * i] + dataArray[2 * i + 1] * dataArray[2 * i + 1]);
                mzs.Add(mzVal);
                intensities.Add(intensityVal);
                lastMz = mzVal;
            }
            return NumPointsInScan / 2;
        }

        public int FindIndexByMass(double val)
        {
            int index;
            if (LowMassFrequency.Equals(0))
            {
                var freq = Mass2Freq(val);
                index = (int) ((freq - FrequencyShift) / SampleRate * NumPointsInScan);
            }
            else
            {
                var freq = Mass2Freq(val);
                if (freq > LowMassFrequency)
                    freq = LowMassFrequency;
                if (freq < LowMassFrequency - SampleRate / 2)
                    freq = LowMassFrequency - SampleRate / 2;
                index = (int) ((LowMassFrequency - freq - FrequencyShift) / SampleRate * NumPointsInScan);
            }
            if (index > NumPointsInScan / 2)
                index = NumPointsInScan / 2;
            if (index < 0)
                index = 0;
            // since the order of  things is reversed.
            index = NumPointsInScan / 2 - index;
            return index;
        }
    }
}
#endif