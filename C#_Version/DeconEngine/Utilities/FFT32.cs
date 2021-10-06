using System;
using System.Collections.Generic;

namespace Engine
{
    internal static class FFT
    {
        private const double Pi = 3.14159265358979323846;
        private const double TwoPi = 2 * 3.14159265358979323846;

        public static void Swap<T>(ref T a, ref T b)
        {
            var temp = a;
            a = b;
            b = a;
        }

        /*
        * Replaces data by its discrete Fourier transfor, if isign is input as 1;
        * or replaces data by nn times its inverse discrete Fourier transform, if
        * isign is input as -1. data is a complex array of length nn, input as a
        * real array data[1...2*nn]. nn must be an integer power of 2 (this is not
        * checked for !).
        *
        *  Numerical Recipes in C.
        *  William H. Press
        *  Brian P. Flannery
        *  Saul A. Teukolsky
        *  William T. Vetterling
        */
        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static void four(int nn, ref float[] data, int isign)
        {
            long n, mmax, m, j, istep, i, im1, jm1;
            double wtemp, wr, wpr, wpi, wi, theta;
            float tempr, tempi;

            n = nn << 1;
            j = 1;
            for (i = 1; i < n; i += 2)
            {
                if (j > i)
                {
                    Swap(ref data[j - 1], ref data[i - 1]);
                    Swap(ref data[j], ref data[i]);
                }
                m = n >> 1;
                while (m >= 2 && j > m)
                {
                    j -= m;
                    m >>= 1;
                }
                j += m;
            }
            mmax = 2;
            while (n > mmax)
            {
                istep = 2 * mmax;
                theta = 6.28318530717959 / (isign * mmax);
                wtemp = Math.Sin(0.5 * theta);
                wpr = -2.0 * wtemp * wtemp;
                wpi = Math.Sin(theta);
                wr = 1.0;
                wi = 0.0;
                for (m = 1; m < mmax; m += 2)
                {
                    for (i = m; i <= n; i += istep)
                    {
                        j = i + mmax;
                        jm1 = j - 1;
                        im1 = i - 1;
                        tempr = (float) (wr * data[jm1] - wi * data[j]);
                        tempi = (float) (wr * data[j] + wi * data[jm1]);
                        data[jm1] = data[im1] - tempr;
                        data[j] = data[i] - tempi;
                        data[im1] += tempr;
                        data[i] += tempi;
                    }
                    wr = (wtemp = wr) * wpr - wi * wpi + wr;
                    wi = wi * wpr + wtemp * wpi + wi;
                }
                mmax = istep;
            }
        }

        /*
        * Replaces data by its discrete Fourier transfor, if isign is input as 1;
        * or replaces data by nn times its inverse discrete Fourier transform, if
        * isign is input as -1. data is a complex array of length nn, input as a
        * real array data[1...2*nn]. nn must be an integer power of 2 (this is not
        * checked for !).
        *
        *  Numerical Recipes in C.
        *  William H. Press
        *  Brian P. Flannery
        *  Saul A. Teukolsky
        *  William T. Vetterling
        */
        // hArray is a 16 bit selector for a 16:32 buffer
        [Obsolete("Not used anywhere", true)]
        public static int four1(int nn, ref float[] lpdata, int isign)
        {
            float[] data;

            data = lpdata;
            four(nn, ref data, isign);
            return 0;
        }

        [Obsolete("Was only used by MercuryIsotopeDistribution, which now uses MathNet.Numerics", false)]
        public static void Four1(int nn, ref List<double> Data, int isign)
        {
            /* Perform bit reversal of Data[] */
            var n = nn << 1;
            var j = 1;
            for (var i = 1; i < n; i += 2)
            {
                if (j > i)
                {
                    var wtemp = Data[i];
                    Data[i] = Data[j];
                    Data[j] = wtemp;
                    wtemp = Data[i + 1];
                    Data[i + 1] = Data[j + 1];
                    Data[j + 1] = wtemp;
                }
                var m = n >> 1;
                while (m >= 2 && j > m)
                {
                    j -= m;
                    m >>= 1;
                }
                j += m;
            }

            /* Perform Danielson-Lanczos section of FFT */
            n = nn << 1;
            var mmax = 2;
            while (n > mmax) /* Loop executed log(2)nn times */
            {
                var istep = mmax << 1;
                var theta = isign * (TwoPi / mmax); /* Initialize the trigonimetric recurrance */
                var wtemp = Math.Sin(0.5 * theta);
                var wpr = -2.0 * wtemp * wtemp;
                var wpi = Math.Sin(theta);
                var wr = 1.0;
                var wi = 0.0;
                for (var m = 1; m < mmax; m += 2)
                {
                    for (var i = m; i <= n; i += istep)
                    {
                        j = i + mmax; /* The Danielson-Lanczos formula */
                        var tempr = wr * Data[j] - wi * Data[j + 1];
                        var tempi = wr * Data[j + 1] + wi * Data[j];
                        Data[j] = Data[i] - tempr;
                        Data[j + 1] = Data[i + 1] - tempi;
                        Data[i] += tempr;
                        Data[i + 1] += tempi;
                    }
                    wr = (wtemp = wr) * wpr - wi * wpi + wr;
                    wi = wi * wpr + wtemp * wpi + wi;
                }
                mmax = istep;
            }
            /* Normalize if FT */
            if (isign == 1)
            {
                for (var i = 1; i <= nn; i++)
                {
                    Data[2 * i - 1] /= nn;
                    Data[2 * i] /= nn;
                }
            }
        } /* End of Four1() */

        /*
        * Calculate the Fourier Transform of a set of n real valued data points.
        * Replaces this data which is stored in array data[1..n]) by the positive
        * frequency half of its complex Fourier transform. The real valued first
        * and last components of the complex transform are returned as elements
        * data[1] and data[2] respectively. n must be a power of 2. This routine
        * also calculates the inverse transform of a complex data array if it is
        * the transform of real data.
        *
        * isign = 1 for FFT.
        * isign = -1 for IFFT.
        */
        // hArray is a 16 bit selector for a 16:32 buffer
        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static int realft(int n, ref float[] data, int isign)
        {
            int i, i1, i2, i3, i4, n2p3;
            float c1 = 0.5f, c2, hir, h1i, h2r, h2i;
            double wpr, wpi, wi, wr, theta, wtemp;

            n = n / 2;
            theta = 3.141592653589793 / n;
            if (isign == 1)
            {
                c2 = -0.5f;
                four(n, ref data, 1);
            }
            else
            {
                c2 = 0.5f;
                theta = -theta;
            }
            wtemp = Math.Sin(0.5 * theta);
            wpr = -2.0 * wtemp * wtemp;
            wpi = Math.Sin(theta);
            wr = 1.0 + wpr;
            wi = wpi;
            n2p3 = 2 * n + 3;
            for (i = 2; i <= n / 2; i++)
            {
                i4 = 1 + (i3 = n2p3 - (i2 = 1 + (i1 = i + i - 1)));
                hir = c1 * (data[i1 - 1] + data[i3 - 1]);
                h1i = c1 * (data[i2 - 1] - data[i4 - 1]);
                h2r = -c2 * (data[i2 - 1] + data[i4 - 1]);
                h2i = c2 * (data[i1 - 1] - data[i3 - 1]);
                data[i1 - 1] = (float) (hir + wr * h2r - wi * h2i);
                data[i2 - 1] = (float) (h1i + wr * h2i + wi * h2r);
                data[i3 - 1] = (float) (hir - wr * h2r + wi * h2i);
                data[i4 - 1] = (float) (-h1i + wr * h2i + wi * h2r);
                wr = (wtemp = wr) * wpr - wi * wpi + wr;
                wi = wi * wpr + wtemp * wpi + wi;
            }
            if (isign == 1)
            {
                data[0] = (hir = data[0]) + data[1];
                data[1] = hir - data[1];
                //      for(i=0;i<(n*2);i++) data[i] /= (n);  // GAA 50-30-00
            }
            else
            {
                data[0] = c1 * ((hir = data[0]) + data[0]);
                data[1] = c1 * (hir - data[1]);
                four(n, ref data, -1);
                for (i = 0; i < n * 2; i++)
                {
                    data[i] /= n;
                }
            }
            return 0;
        }
    }
}
