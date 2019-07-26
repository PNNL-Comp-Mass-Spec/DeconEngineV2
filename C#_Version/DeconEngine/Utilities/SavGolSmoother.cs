using System;
using System.Collections.Generic;

namespace Engine.Utilities
{
    /// <summary>
    /// Savitzky-golay smoother
    /// </summary>
    /// <remarks>Used by DeconTools for ICR2LSRun and IMFRun; also used by GenerateDTAZoomScans in DeconMSn</remarks>
    internal class SavGolSmoother
    {
        /// <summary>
        /// number of points to the left while applying Savitzky Golay filter.
        /// </summary>
        private int _numLeftPoints;
        /// <summary>
        /// the order of the Savitzky Golay smoothing filter.
        /// </summary>
        private int _golayOrder;
        /// <summary>
        /// the number of points to the right while applying Savitzky Golay filter.
        /// </summary>
        private int _numRightPoints;

        private int _numberCoefficients;

        private readonly List<double> _tempX = new List<double>();
        private readonly List<double> _tempY = new List<double>();
        private readonly List<double> _coefficients = new List<double>();

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public SavGolSmoother(int numLeft, int numRight, int order)
        {
            SetOptions(numLeft, numRight, order);
        }

        public SavGolSmoother()
        {
            SetOptions(3, 3, 2);
        }

        public void SetOptions(int numLeft, int numRight, int order)
        {
            _coefficients.Clear();

            _golayOrder = order;
            _numLeftPoints = numLeft;
            _numRightPoints = numRight;
            var np = _numLeftPoints + _numRightPoints + 1;

            var golayCoeffs = new float[np + 2];

            for (var i = 0; i < np + 2; i++)
                golayCoeffs[i] = 0;

            savgol(golayCoeffs, np, _numLeftPoints, _numRightPoints, 0, _golayOrder);

            _numberCoefficients = _numRightPoints * 2;
            if (_numLeftPoints > _numRightPoints)
            {
                _numberCoefficients = _numLeftPoints * 2;
            }

            // unwrap golay coeffs
            _coefficients.Clear();
            for (var i = 0; i < _numLeftPoints + _numRightPoints + 1; i++)
                _coefficients.Add(0);

            for (var i = 0; i <= _numLeftPoints; i++)
            {
                _coefficients[_numberCoefficients / 2 - i] = golayCoeffs[i + 1];
            }
            for (var i = 1; i <= _numRightPoints; i++)
            {
                _coefficients[_numberCoefficients / 2 + i] = golayCoeffs[_numberCoefficients - i];
            }
        }

        public void Smooth(ref List<double> mzs, ref List<double> intensities)
        {
            var size = mzs.Count;
            _tempX.Clear();
            _tempY.Clear();

            for (var i = 0; i < size; i++)
            {
                var startIndex = i - _numLeftPoints;
                var stopIndex = i + _numRightPoints + 1;

                if (startIndex < 0 || stopIndex >= size)
                {
                    // dont worry about smoothing just push back and forget.
                    _tempX.Add(mzs[i]);
                    _tempY.Add(intensities[i]);
                    continue;
                }
                var sum = 0d;
                for (var j = startIndex; j < stopIndex; j++)
                {
                    sum += intensities[j] * _coefficients[j - startIndex];
                }
                if (sum < 0)
                    sum = 0;
                _tempX.Add(mzs[i]);
                _tempY.Add(sum);
            }
            mzs.Clear();
            intensities.Clear();
            mzs.AddRange(_tempX);
            intensities.AddRange(_tempY);
        }

        private const float TinyVal = 1.0e-20f;

        private static int ludcmp(float[,] a, int n, int[] indx, out float d)
        {
            var vv = new float[n];
            for (var j = 0; j < n; j++)
            {
                vv[j] = 0;
            }

            d = 1.0f;
            for (var i = 1; i <= n; i++)
            {
                var big = 0.0f;
                for (var j = 1; j <= n; j++)
                {
                    float temp;
                    if ((temp = Math.Abs(a[i - 1, j - 1])) > big)
                    {
                        big = temp;
                    }
                }
                if (big.Equals(0f))
                {
                    return -1;
                }
                vv[i - 1] = 1.0f / big;
            }
            var imax = 0;
            for (var j = 1; j <= n; j++)
            {
                for (var i = 1; i < j; i++)
                {
                    var sum = a[i - 1, j - 1];
                    for (var k = 1; k < i; k++)
                    {
                        sum -= a[i - 1, k - 1] * a[k - 1, j - 1];
                    }
                    a[i - 1, j - 1] = sum;
                }
                var big = 0.0f;
                for (var i = j; i <= n; i++)
                {
                    var sum = a[i - 1, j - 1];
                    for (var k = 1; k < j; k++)
                    {
                        sum -= a[i - 1, k - 1] * a[k - 1, j - 1];
                    }
                    a[i - 1, j - 1] = sum;
                    float dum;
                    if ((dum = vv[i - 1] * Math.Abs(sum)) >= big)
                    {
                        big = dum;
                        imax = i;
                    }
                }
                if (j != imax)
                {
                    for (var k = 1; k <= n; k++)
                    {
                        var dum = a[imax - 1, k - 1];
                        a[imax - 1, k - 1] = a[j - 1, k - 1];
                        a[j - 1, k - 1] = dum;
                    }
                    d = -d;
                    vv[imax - 1] = vv[j - 1];
                }
                indx[j - 1] = imax;
                if (a[j - 1, j - 1].Equals(0f))
                {
                    a[j - 1, j - 1] = TinyVal;
                }
                if (j != n)
                {
                    var dum = 1.0f / (a[j - 1, j - 1]);
                    for (var i = j + 1; i <= n; i++)
                    {
                        a[i - 1, j - 1] *= dum;
                    }
                }
            }
            return 0;
        }

        private static void lubksb(float[,] a, int n, int[] indx, float[] b)
        {
            var ii = 0;

            for (var i = 1; i <= n; i++)
            {
                var ip = indx[i - 1];
                var sum = b[ip - 1];
                b[ip - 1] = b[i - 1];
                if (ii != 0)
                {
                    for (var j = ii; j <= i - 1; j++)
                    {
                        sum -= a[i - 1, j - 1] * b[j - 1];
                    }
                }
                else if (!sum.Equals(0f))
                {
                    ii = i;
                }
                b[i - 1] = sum;
            }
            for (var i = n; i >= 1; i--)
            {
                var sum = b[i - 1];
                for (var j = i + 1; j <= n; j++)
                {
                    sum -= a[i - 1, j - 1] * b[j - 1];
                }
                b[i - 1] = sum / a[i - 1, i - 1];
            }
        }

        /// <summary>
        /// Main Savitzky-Golay smoothing function
        /// </summary>
        /// <param name="c">Modified by the function</param>
        /// <param name="np"></param>
        /// <param name="nl"></param>
        /// <param name="nr"></param>
        /// <param name="ld"></param>
        /// <param name="m"></param>
        /// <returns></returns>
        private static int savgol(float[] c, int np, int nl, int nr, int ld, int m)
        {
            if (np < nl + nr + 1 || nl < 0 || nr < 0 || ld > m || nl + nr < m)
            {
                return -1;
            }
            var indx = new int[m + 1];
            var b = new float[m + 1];
            for (var j = 0; j < m + 1; j++)
            {
                indx[j] = 0;
                b[j] = 0;
            }

            var a = new float[m + 1, m + 1];
            for (var j = 0; j < m + 1; j++)
            {
                for (int i = 0; i < m + 1; i++)
                {
                    a[j, i] = 0;
                }
            }

            for (var ipj = 0; ipj <= (m << 1); ipj++)
            {
                var sum = (ipj != 0 ? 0.0f : 1.0f);
                for (var k = 1; k <= nr; k++)
                {
                    sum += (float) Math.Pow((double) k, (double) ipj);
                }
                for (var k = 1; k <= nl; k++)
                {
                    sum += (float) Math.Pow((double) -k, (double) ipj);
                }
                var mm = Math.Min(ipj, 2 * m - ipj);
                for (var imj = -mm; imj <= mm; imj += 2)
                {
                    a[(ipj + imj) / 2, (ipj - imj) / 2] = sum;
                }
            }

            var retVal = ludcmp(a, m + 1, indx, out var d);
            if (retVal == -1)
            {
                for (var kk = 1; kk <= np; kk++)
                {
                    c[kk] = 0.0f;
                }
                return -1;
            }

            for (var j = 1; j <= m + 1; j++)
            {
                b[j - 1] = 0.0f;
            }
            b[ld] = 1.0f;
            lubksb(a, m + 1, indx, b);
            for (var kk = 1; kk <= np; kk++)
            {
                c[kk] = 0.0f;
            }
            for (var k = -nl; k <= nr; k++)
            {
                var sum = b[0];
                var fac = 1.0f;
                for (var mm = 1; mm <= m; mm++)
                {
                    sum += b[mm] * (fac *= k);
                }
                var kk = ((np - k) % np) + 1;
                c[kk] = sum;
            }
            return 0;
        }
    }
}
