using System;
using DeconToolsV2.Readers;

namespace Engine.Utilities
{
    [Obsolete("Only used by Decon2LS.UI (maybe)", false)]
    internal class Apodization
    {
        private static void PerformInvertedTriangleApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            float temp1, temp2;
            float val1, val2, A;
            long li;

            A = 1.0f;
            temp1 = 0.5f * (float) (stopIndex - startIndex);
            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[2 * li];
                val2 = vectIntensities[2 * li + 1];
                temp2 = (float) (li) / temp1;
                if (temp2 > 1.0)
                    temp2 = 2.0f - temp2;
                if (A == 0.0)
                    temp2 = 1.0f - temp2;
                val1 *= temp2;
                val2 *= temp2;
                vectIntensities[2 * li] = val1;
                vectIntensities[2 * li + 1] = val2;
            }
        }

        private static void PerformInvertedWelchApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            double temp1, temp2;
            float val1, val2, A;
            long li;

            A = 1.0f;
            temp1 = 0.5 * (double) (stopIndex - startIndex - 1);
            temp2 = 0.5 * (double) (stopIndex - startIndex + 1);
            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[2 * li];
                val2 = vectIntensities[2 * li + 1];
                val1 *= Math.Abs(A - (float) ((((double) (li) - temp1) / temp2) * (((double) (li) - temp1) / temp2)));
                val2 *= Math.Abs(A - (float) ((((double) (li) - temp1) / temp2) * (((double) (li) - temp1) / temp2)));
                vectIntensities[2 * li] = val1;
                vectIntensities[2 * li + 1] = val2;
            }
        }

        private static void PerformInvertedHanningApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            double temp1, temp2;
            float val1, val2, A;
            long li;

            A = 1.0f;
            temp1 = 2.0 * 3.141592654;
            temp2 = (double) (stopIndex - startIndex - 1);
            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[2 * li];
                val2 = vectIntensities[2 * li + 1];
                val1 *= Math.Abs(A / 2.0f - (float) Math.Cos(((double) (li) * temp1) / temp2) / 2.0f);
                val2 *= Math.Abs(A / 2.0f - (float) Math.Cos(((double) (li) * temp1) / temp2) / 2.0f);
                vectIntensities[2 * li] = val1;
                vectIntensities[2 * li + 1] = val2;
            }
        }

        private static void PerformInvertedParzenApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            double temp1, temp2;
            float val1, val2, A;
            long li;

            A = 1.0f;
            temp1 = 0.5 * (double) (stopIndex - startIndex - 1);
            temp2 = 0.5 * (double) (stopIndex - startIndex + 1);
            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[2 * li];
                val2 = vectIntensities[2 * li + 1];
                val1 *= Math.Abs(A - (float) Math.Abs(((double) (li) - temp1) / temp2));
                val2 *= Math.Abs(A - (float) Math.Abs(((double) (li) - temp1) / temp2));
                vectIntensities[2 * li] = val1;
                vectIntensities[2 * li + 1] = val2;
            }
        }

        private static void PerformInvertedSquareApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            return;
        }

        private static void PerformTriangleApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            double temp1, temp2;
            float val1, A;
            long li;

            A = 1.0f;
            temp1 = 0.5 * (double) (stopIndex - startIndex);
            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[li];
                temp2 = (double) (li) / temp1;
                if (temp2 > 1.0)
                    temp2 = 2.0 - temp2;
                if (A == 0.0)
                    temp2 = 1.0 - temp2;
                val1 = (float) (temp2 * val1);
                vectIntensities[li] = val1;
            }
        }

        private static void PerformWelchApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            double temp1, temp2;
            float val1, A;
            long li;

            A = 1.0f;
            temp1 = 0.5 * (double) (stopIndex - startIndex - 1);
            temp2 = 0.5 * (double) (stopIndex - startIndex + 1);

            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[li];
                val1 *= Math.Abs(A - (float) ((((double) (li) - temp1) / temp2) * (((double) (li) - temp1) / temp2)));
                vectIntensities[li] = val1;
            }
        }

        private static void PerformHanningApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            double temp1, temp2;
            float val1, A;
            long li;

            A = 1.0f;
            temp1 = 2.0 * 3.141592654;
            temp2 = (double) (stopIndex - startIndex - 1);

            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[li];
                val1 *= Math.Abs(A / 2.0f - (float) Math.Cos(((double) (li) * temp1) / temp2) / 2.0f);
                vectIntensities[li] = val1;
            }
        }

        private static void PerformParzenApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            float val1 = 0;
            long li;
            var A = 1.0f;
            var temp1 = 0.5 * (double) (stopIndex - startIndex - 1);
            var temp2 = 0.5 * (double) (stopIndex - startIndex + 1);

            for (li = startIndex; li <= stopIndex; li++)
            {
                val1 = vectIntensities[li];
                val1 *= Math.Abs(A - (float) (Math.Abs(((double) (li) - temp1) / temp2)));
                vectIntensities[li] = val1;
            }
        }

        private static void PerformSquareApodization(int startIndex, int stopIndex, float[] vectIntensities)
        {
            return;
        }

        public static void Apodize(double minX, double maxX, double sampleRate, bool invert, ApodizationType type,
            float[] vectIntensities, int numPts, int apexPosition = 50)
        {
            var startIndex = (int) (minX * sampleRate);
            var stopIndex = (int) (maxX * sampleRate);
            if (startIndex > stopIndex)
            {
                var temp = startIndex;
                startIndex = stopIndex;
                stopIndex = startIndex;
            }
            if (stopIndex >= numPts)
                stopIndex = numPts - 1;

            if (apexPosition != 50)
            {
                var apexIndex = startIndex + ((stopIndex - startIndex) * apexPosition) / 100;
                for (var index = startIndex; index < apexIndex; index++)
                {
                    vectIntensities[index] = vectIntensities[index] * (index - startIndex) / (apexIndex - startIndex);
                }
                for (var index = apexIndex; index < stopIndex; index++)
                {
                    vectIntensities[index] = vectIntensities[index] * (stopIndex - index) / (stopIndex - apexIndex);
                }
            }
            if (!invert)
            {
                switch (type)
                {
                    case ApodizationType.SQUARE:
                        PerformSquareApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.PARZEN:
                        PerformParzenApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.HANNING:
                        PerformHanningApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.WELCH:
                        PerformWelchApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.TRIANGLE:
                        PerformTriangleApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch (type)
                {
                    case ApodizationType.SQUARE:
                        PerformInvertedSquareApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.PARZEN:
                        PerformInvertedParzenApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.HANNING:
                        PerformInvertedHanningApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.WELCH:
                        PerformInvertedWelchApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    case ApodizationType.TRIANGLE:
                        PerformInvertedTriangleApodization(startIndex, stopIndex, vectIntensities);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
