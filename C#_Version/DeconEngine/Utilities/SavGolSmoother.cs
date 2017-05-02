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
        // number of points to the left while applying Savitzky Golay filter.
        private int mint_Nleft_golay;

        // the order of the Savitzky Golay smoothing filter.
        private int mint_golay_order;

        // the number of points to the right while applying Savitzky Golay filter.
        private int mint_Nright_golay;

        private int mint_num_coeffs;

        private readonly List<double> mvect_temp_x = new List<double>();
        private readonly List<double> mvect_temp_y = new List<double>();
        private readonly List<double> mvect_coefficients = new List<double>();

        public SavGolSmoother(int num_left, int num_right, int order)
        {
            SetOptions(num_left, num_right, order);
        }

        public SavGolSmoother()
        {
            SetOptions(3, 3, 2);
        }

        public void SetOptions(int num_left, int num_right, int order)
        {
            mvect_coefficients.Clear();

            mint_golay_order = order;
            mint_Nleft_golay = num_left;
            mint_Nright_golay = num_right;
            var np = mint_Nleft_golay + mint_Nright_golay + 1;

            var golay_coeffs = new float[np + 2];

            for (var i = 0; i < np + 2; i++)
                golay_coeffs[i] = 0;

            var res = savgol(golay_coeffs, np, mint_Nleft_golay, mint_Nright_golay, 0, mint_golay_order);

            mint_num_coeffs = mint_Nright_golay * 2;
            if (mint_Nleft_golay > mint_Nright_golay)
            {
                mint_num_coeffs = mint_Nleft_golay * 2;
            }

            // unwrap golay coeffs
            mvect_coefficients.Clear();
            for (var i = 0; i < mint_Nleft_golay + mint_Nright_golay + 1; i++)
                mvect_coefficients.Add(0);

            for (var i = 0; i <= mint_Nleft_golay; i++)
            {
                mvect_coefficients[mint_num_coeffs / 2 - i] = golay_coeffs[i + 1];
            }
            for (var i = 1; i <= mint_Nright_golay; i++)
            {
                mvect_coefficients[mint_num_coeffs / 2 + i] = golay_coeffs[mint_num_coeffs - i];
            }
        }

        public void Smooth(ref List<double> mzs, ref List<double> intensities)
        {
            var width = (mint_num_coeffs / 2);
            var size = mzs.Count;
            mvect_temp_x.Clear();
            mvect_temp_y.Clear();

            for (var i = 0; i < size; i++)
            {
                var start_index = i - mint_Nleft_golay;
                var stop_index = i + mint_Nright_golay + 1;

                if (start_index < 0 || stop_index >= size)
                {
                    // dont worry about smoothing just push back and forget.
                    mvect_temp_x.Add(mzs[i]);
                    mvect_temp_y.Add(intensities[i]);
                    continue;
                }
                double sum = 0;
                for (var j = start_index; j < stop_index; j++)
                {
                    var sum_before = sum;
                    var val = intensities[j] * mvect_coefficients[j - start_index];
                    sum = sum_before + val;
                }
                if (sum < 0)
                    sum = 0;
                mvect_temp_x.Add(mzs[i]);
                mvect_temp_y.Add(sum);
            }
            mzs.Clear();
            intensities.Clear();
            mzs.AddRange(mvect_temp_x);
            intensities.AddRange(mvect_temp_y);
        }

        private const float TINY = 1.0e-20f;

        private int ludcmp(IReadOnlyList<float[]> a, int n, IList<int> indx, out float d)
        {
            var imax = 0;
            float big;
            var vv = new float[n];
            for (var j = 0; j < n; j++)
            {
                vv[j] = 0;
            }

            d = 1.0f;
            for (var i = 1; i <= n; i++)
            {
                big = 0.0f;
                for (var j = 1; j <= n; j++)
                {
                    float temp;
                    if ((temp = Math.Abs(a[i - 1][j - 1])) > big)
                    {
                        big = temp;
                    }
                }
                if (Math.Abs(big) < float.Epsilon)
                {
                    return -1;
                }
                vv[i - 1] = 1.0f / big;
            }
            for (var j = 1; j <= n; j++)
            {
                float sum;
                for (var i = 1; i < j; i++)
                {
                    sum = a[i - 1][j - 1];
                    for (var k = 1; k < i; k++)
                    {
                        sum -= a[i - 1][k - 1] * a[k - 1][j - 1];
                    }
                    a[i - 1][j - 1] = sum;
                }
                big = 0.0f;
                float dum;
                for (var i = j; i <= n; i++)
                {
                    sum = a[i - 1][j - 1];
                    for (var k = 1; k < j; k++)
                    {
                        sum -= a[i - 1][k - 1] * a[k - 1][j - 1];
                    }
                    a[i - 1][j - 1] = sum;
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
                        dum = a[imax - 1][k - 1];
                        a[imax - 1][k - 1] = a[j - 1][k - 1];
                        a[j - 1][k - 1] = dum;
                    }
                    d = -d;
                    vv[imax - 1] = vv[j - 1];
                }
                indx[j - 1] = imax;
                if (Math.Abs(a[j - 1][j - 1]) < float.Epsilon)
                {
                    a[j - 1][j - 1] = TINY;
                }
                if (j != n)
                {
                    dum = 1.0f / (a[j - 1][j - 1]);
                    for (var i = j + 1; i <= n; i++)
                    {
                        a[i - 1][j - 1] *= dum;
                    }
                }
            }
            return 0;
        }

        private void lubksb(IReadOnlyList<float[]> a, int n, IReadOnlyList<int> indx, IList<float> b)
        {
            var ii = 0;
            float sum;

            for (var i = 1; i <= n; i++)
            {
                var ip = indx[i - 1];
                sum = b[ip - 1];
                b[ip - 1] = b[i - 1];
                if (ii != 0)
                {
                    for (var j = ii; j <= i - 1; j++)
                    {
                        sum -= a[i - 1][j - 1] * b[j - 1];
                    }
                }
                else if (Math.Abs(sum) > float.Epsilon)
                {
                    ii = i;
                }
                b[i - 1] = sum;
            }
            for (var i = n; i >= 1; i--)
            {
                sum = b[i - 1];
                for (var j = i + 1; j <= n; j++)
                {
                    sum -= a[i - 1][j - 1] * b[j - 1];
                }
                b[i - 1] = sum / a[i - 1][i - 1];
            }
        }

        private int savgol(IList<float> c, int np, int nl, int nr, int ld, int m)
        {
            int mm;
            float d;
            float sum;

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

            var a = new float[m + 1][];
            for (var j = 0; j < m + 1; j++)
            {
                a[j] = new float[m + 1];
                for (var i = 0; i < m + 1; i++)
                {
                    a[j][i] = 0;
                }
            }

            for (var ipj = 0; ipj <= (m << 1); ipj++)
            {
                sum = (ipj != 0 ? 0.0f : 1.0f);
                for (var k = 1; k <= nr; k++) sum += (float) Math.Pow(k, ipj);
                for (var k = 1; k <= nl; k++) sum += (float) Math.Pow(-k, ipj);
                mm = Math.Min(ipj, 2 * m - ipj);
                for (var imj = -mm; imj <= mm; imj += 2) a[(ipj + imj) / 2][(ipj - imj) / 2] = sum;
            }
            var ret_val = ludcmp(a, m + 1, indx, out d);
            if (ret_val == -1)
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
                sum = b[0];
                var fac = 1.0f;
                for (mm = 1; mm <= m; mm++)
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