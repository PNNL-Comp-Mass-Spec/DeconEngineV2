#if !Disable_Obsolete
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using DeconToolsV2;
using Engine;
using Engine.TheoreticalProfile;
using Engine.Utilities;
using DeconToolsV2.Peaks;
using Engine.PeakProcessing;
using ApodizationType = DeconToolsV2.Readers.ApodizationType;

namespace DeconEngine
{
    public class Utils
    {
        //calculates the background noise level on the intensities using the following algorithm:
        //Step 1: Calculate average of all intensities and number of points used
        //Step 2: Calculate standard deviation of all intensities and the number of points used
        //Step 3: Calculate average of all points within +/- 5 standard deviations
        //Step 4: Report background intensity level
        public static double GetBackgroundLevel(List<double> intensities, float maxIntensity)
        {
            var numPts = intensities.Count;
            double average = 0;

            if (numPts != 0)
            {
                //step 1
                average = GetAverage(intensities, maxIntensity);

                //step 2
                var stdDev = GetStandardDev(intensities, maxIntensity);

                //step 3:
                var lowLevel = average - (stdDev * 5);
                var highLevel = average + (stdDev * 5);

                var numPtsUsed = 0;
                average = 0.0;
                for (var i = 0; i < numPts; i++)
                {
                    if (lowLevel <= intensities[i] && intensities[i] <= highLevel)
                    {
                        average += intensities[i];
                        numPtsUsed++;
                    }
                }

                average /= numPtsUsed;
            }

            return average;
        }

        public static double GetBackgroundLevel(float[] intensities, float maxIntensity)
        {
            var numPts = intensities.Length;
            double average = 0;

            if (numPts != 0)
            {
                //step 1
                average = GetAverage(intensities, maxIntensity);

                //step 2
                var stdDev = GetStandardDev(intensities, maxIntensity);

                //step 3:
                var lowLevel = average - (stdDev * 5);
                var highLevel = average + (stdDev * 5);

                var numPtsUsed = 0;
                average = 0.0;
                for (var i = 0; i < numPts; i++)
                {
                    if (lowLevel <= intensities[i] && intensities[i] <= highLevel)
                    {
                        average += intensities[i];
                        numPtsUsed++;
                    }
                }

                average /= numPtsUsed;
            }

            return average;
        }



        // function calculating standard deviation
        public static double GetStandardDev(List<double> intensities, float maxIntensity)
        {
            var numPts = intensities.Count;
            double stdDev = 0;
            var numPtsUsed = 0;

            if (numPts != 0)
            {
                double sum = 0;
                double STD_DEV = 0; // returning zero's

                for (var i = 0; i < numPts; i++)
                {
                    if (intensities[i] <= maxIntensity && intensities[i] != 0)
                    {
                        sum += intensities[i];
                        stdDev += Math.Pow(intensities[i], 2);
                        numPtsUsed++;
                    }
                }
                stdDev = Math.Sqrt((stdDev / numPtsUsed) - (Math.Pow(sum / numPtsUsed, 2)));
            }

            return stdDev;
        }

        // function calculating standard deviation
        public static double GetStandardDev(float[] intensities, float maxIntensity)
        {
            var numPts = intensities.Length;
            double stdDev = 0;
            var numPtsUsed = 0;

            if (numPts != 0)
            {
                double sum = 0;
                double STD_DEV = 0; // returning zero's

                for (var i = 0; i < numPts; i++)
                {
                    if (intensities[i] <= maxIntensity && intensities[i] != 0)
                    {
                        sum += intensities[i];
                        stdDev += Math.Pow(intensities[i], 2);
                        numPtsUsed++;
                    }
                }
                stdDev = Math.Sqrt((stdDev / numPtsUsed) - (Math.Pow(sum / numPtsUsed, 2)));
            }

            return stdDev;
        }

       
        [Obsolete("Only used by Decon2LS.UI", false)]
        public static double GetAverage(float[] intensities, float maxIntensity)
        {
            var numPts = intensities.Length;
            if (numPts == 0)
                return 0;

            var filteredData = intensities.Where(x => x > 0 && x <= maxIntensity).ToList();
            if (filteredData.Count == 0)
                return 0;

            return filteredData.Average();

        }

        /// <summary>
        /// Compute the average of the non-zero data in intensities
        /// </summary>
        /// <param name="intensities"></param>
        /// <param name="maxIntensity"></param>
        /// <returns>Average value, or 0 if intensities is empty or if it only contains zerooes or values out of range</returns>
        /// <remarks>Used by DeisotopeSummedSpectra in DTAGeneration</remarks>
        public static double GetAverage(List<double> intensities, float maxIntensity)
        {
            var numPts = intensities.Count;
            if (numPts == 0)
                return 0;

            var filteredData = intensities.Where(x => x > 0 && x <= maxIntensity).ToList();
            if (filteredData.Count == 0)
                return 0;

            return filteredData.Average();

        }

        /// <summary>
        /// Get TIC
        /// </summary>
        /// <remarks> DeconTools for ICR2LSRun and IMFRun; also used by DeconMSn</remarks>
        public static double GetTIC(double min_mz, double max_mz, ref List<double> mzs, ref List<double> intensities,
            float minIntensity, ref double bpi, ref double bp_mz)
        {
            var numPts = intensities.Count;
            if (numPts == 0)
                return 0;

            double sum = 0;
            bpi = 0;
            for (var i = 0; i < numPts; i++)
            {
                if (mzs[i] >= min_mz && mzs[i] < max_mz && intensities[i] >= minIntensity)
                {
                    sum += intensities[i];
                    if (intensities[i] > bpi)
                    {
                        bpi = intensities[i];
                        bp_mz = mzs[i];
                    }
                }
            }
            return sum;
        }

        [Obsolete("Usage unknown, besides Decon2LS.UI - no path to usage exists within the DeconTools solution")]
        internal static void ConvertElementTableToFormula(
            ref clsElementIsotopes elemental_isotope_composition,
            Hashtable elementCounts, out DeconToolsV2.MolecularFormula formula)
        {
            var elements = elementCounts.Keys.GetEnumerator();

            formula = new DeconToolsV2.MolecularFormula();

            while (elements.MoveNext())
            {
                // Get the next element symbol in the table
                var element = (string)elements.Current;
                // Put it in a character array
                var count = (int)elementCounts[element];

                // find the index of the element in the AtomicInformation
                var index = elemental_isotope_composition.GetElementIndex(element);
                if (index == -1)
                {
                    throw new ApplicationException(string.Concat("Unknown element ", element));
                }
                var atomic_count = new DeconToolsV2.AtomicCount(index, count);
                var mono_mass =
                    elemental_isotope_composition.ElementalIsotopesList[index].Isotopes[0].Mass * count;
                var avg_mass = elemental_isotope_composition.ElementalIsotopesList[index].AverageMass *
                               count;
                formula.AddAtomicCount(atomic_count, mono_mass, avg_mass);
            }
        }

        [Obsolete("Use PeakData.SetPeaks instead", true)]
        internal static void SetPeaks(ref PeakData peakData, ref clsPeak[] peaks)
        {
            foreach (var pk in peaks)
            {
                peakData.AddPeak(new clsPeak(pk));
            }
            peakData.InitializeUnprocessedPeakData();
        }

        [Obsolete("Use Linq expressions instead", true)]
        public static void SetData(out List<double> vectData, float[] data)
        {
            vectData = new List<double>();
            var numPoints = data.Length;

            for (var ptNum = 0; ptNum < numPoints; ptNum++)
            {
                vectData.Add(data[ptNum]);
            }
        }

        [Obsolete("Use Linq expressions instead", true)]
        public static void SetData(out List<float> vectData, float[] data)
        {
            vectData = new List<float>();
            var numPoints = data.Length;

            for (var ptNum = 0; ptNum < numPoints; ptNum++)
            {
                var val = data[ptNum];
                vectData.Add(val);
            }
        }

        [Obsolete("Use Linq expressions instead", true)]
        public static void GetData(List<float> vectData, out float[] data)
        {
            var numPoints = vectData.Count;
            data = new float[numPoints];

            for (var ptNum = 0; ptNum < numPoints; ptNum++)
            {
                data[ptNum] = vectData[ptNum];
            }
        }

        [Obsolete("Use Linq expressions instead", true)]
        public static void GetData(List<double> vectData, out float[] data)
        {
            var numPoints = vectData.Count;
            data = new float[numPoints];

            for (var ptNum = 0; ptNum < numPoints; ptNum++)
            {
                data[ptNum] = (float)vectData[ptNum];
            }
        }

        [Obsolete("Not needed for C# native code", true)]
        public static void GetStr(string src, ref char[] dest)
        {
            if (string.IsNullOrEmpty(src))
            {
                dest[0] = '\0';
                return;
            }

            var len = src.Length;
            for (var i = 0; i < len; i++)
            {
                dest[i] = src[i];
            }
            dest[len] = '\0';
        }

        [Obsolete("Not needed for C# native code", true)]
        public static void GetStr(char[] src, out string dest)
        {
            if (src == null)
            {
                dest = null;
                return;
            }
            if (src.Length == 0)
            {
                dest = string.Empty;
                return;
            }

            dest = new string(src);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static void SavitzkyGolaySmooth(short num_left, short num_right, short order, ref float[] mzs,
            ref float[] intensities)
        {
            var sgSmoother = new SavGolSmoother(num_left, num_right, order);
            var vectX = new List<double>();
            var vectY = new List<double>();
            var num_pts = mzs.Length;

            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                vectX.Add(mzs[pt_num]);
                vectY.Add(intensities[pt_num]);
            }
            sgSmoother.Smooth(ref vectX, ref vectY);
            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                mzs[pt_num] = (float)vectX[pt_num];
                intensities[pt_num] = (float)vectY[pt_num];
            }
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static int ZeroFillUnevenData(ref float[] mzs, ref float[] intensities, int maxPtsToAdd)
        {
            var vectX = new List<float>();
            var vectY = new List<float>();
            var num_pts = mzs.Length;

            for (var pt_num = 0; pt_num < num_pts; pt_num++)
            {
                var currentMZ = mzs[pt_num];
                var currentIntensity = intensities[pt_num];
                vectX.Add(currentMZ);
                vectY.Add(currentIntensity);
            }
            var numPtsX = vectX.Count;
            var interp = new Interpolation();
            Interpolation.ZeroFillMissing(ref vectX, ref vectY, maxPtsToAdd);

            var num_pts_new = vectX.Count;
            intensities = new float[num_pts_new];
            mzs = new float[num_pts_new];

            for (var i = 0; i < num_pts_new; i++)
            {
                mzs[i] = vectX[i];
                intensities[i] = vectY[i];
            }
            return num_pts_new;
        }

        [Obsolete("Only used by Decon2LS.UI (maybe)", true)]
        public static void Apodize(double minX, double maxX, double sampleRate, int apexPositionPercent,
            ref float[] intensities, ApodizationType type)
        {
            var arrIntensities = new float[intensities.Length];
            for (var i = 0; i < intensities.Length; i++)
                arrIntensities[i] = intensities[i];
            Apodization.Apodize(minX, maxX, sampleRate, false, type,
                arrIntensities,
                intensities.Length, apexPositionPercent);
            for (var i = 0; i < intensities.Length; i++)
                intensities[i] = arrIntensities[i];
        }

        [Obsolete("Only used by Decon2LS.UI (maybe)", true)]
        public static void UnApodize(ref float[] intensities, ApodizationType type)
        {
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static void FourierTransform(ref float[] intensities)
        {
            var arrIntensities = new float[intensities.Length];
            for (var i = 0; i < intensities.Length; i++)
                arrIntensities[i] = intensities[i];
            FFT.realft(intensities.Length, ref arrIntensities, 1);
            for (var i = 0; i < intensities.Length; i++)
                intensities[i] = arrIntensities[i];
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static void InverseFourierTransform(ref float[] intensities)
        {
            var arrIntensities = new float[intensities.Length];
            for (var i = 0; i < intensities.Length; i++)
                arrIntensities[i] = intensities[i];
            FFT.realft(intensities.Length, ref arrIntensities, -1);
            for (var i = 0; i < intensities.Length; i++)
                intensities[i] = arrIntensities[i];
        }
    }
}
#endif