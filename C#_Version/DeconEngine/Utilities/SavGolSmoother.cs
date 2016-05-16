#if !Disable_Obsolete
using System;
using System.Collections.Generic;

namespace Engine.Utilities
{
    [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
    internal class SavGolSmoother
    {
        // number of points to the left while applying Savitzky Golay filter.
        private int mint_Nleft_golay;
        // the order of the Savitzky Golay smoothing filter.
        private int mint_golay_order;
        // the number of points to the right while applying Savitzky Golay filter.
        private int mint_Nright_golay;

        private int mint_num_coeffs;

        private List<double> mvect_temp_x = new List<double>();
        private List<double> mvect_temp_y = new List<double>();
        private List<double> mvect_coefficients = new List<double>();

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
            int np = mint_Nleft_golay + mint_Nright_golay + 1;

            float[] golay_coeffs = new float[np + 2];

            for (int i = 0; i < np + 2; i++)
                golay_coeffs[i] = 0;

            int res = savgol(golay_coeffs, np, mint_Nleft_golay, mint_Nright_golay, 0, mint_golay_order);

            mint_num_coeffs = mint_Nright_golay * 2;
            if (mint_Nleft_golay > mint_Nright_golay)
            {
                mint_num_coeffs = mint_Nleft_golay * 2;
            }

            // unwrap golay coeffs
            mvect_coefficients.Clear();
            for (int i = 0; i < mint_Nleft_golay + mint_Nright_golay + 1; i++)
                mvect_coefficients.Add(0);

            for (int i = 0; i <= mint_Nleft_golay; i++)
            {
                mvect_coefficients[mint_num_coeffs / 2 - i] = (double) golay_coeffs[i + 1];
            }
            for (int i = 1; i <= mint_Nright_golay; i++)
            {
                mvect_coefficients[mint_num_coeffs / 2 + i] = (double) golay_coeffs[mint_num_coeffs - i];
            }
        }

        public void Smooth(ref List<double> mzs, ref List<double> intensities)
        {
            int width = (mint_num_coeffs / 2);
            int size = (int) mzs.Count;
            mvect_temp_x.Clear();
            mvect_temp_y.Clear();

            for (int i = 0; i < size; i++)
            {
                int start_index = i - mint_Nleft_golay;
                int stop_index = i + mint_Nright_golay + 1;

                if (start_index < 0 || stop_index >= size)
                {
                    // dont worry about smoothing just push back and forget.
                    mvect_temp_x.Add(mzs[i]);
                    mvect_temp_y.Add(intensities[i]);
                    continue;
                }
                double sum = 0;
                double sum_before = 0;
                for (int j = start_index; j < stop_index; j++)
                {
                    sum_before = sum;
                    double val = intensities[j] * mvect_coefficients[j - start_index];
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

        private int ludcmp(float[][] a, int n, int[] indx, out float d)
        {
            int i, imax, j, k;
            imax = 0;
            float big = 0, dum = 0, sum = 0, temp = 0;
            float[] vv = new float[n];
            for (j = 0; j < n; j++)
            {
                vv[j] = 0;
            }

            d = 1.0f;
            for (i = 1; i <= n; i++)
            {
                big = 0.0f;
                for (j = 1; j <= n; j++)
                {
                    if ((temp = Math.Abs(a[i - 1][j - 1])) > big)
                    {
                        big = temp;
                    }
                }
                if (big == 0.0)
                {
                    return -1;
                }
                vv[i - 1] = 1.0f / big;
            }
            for (j = 1; j <= n; j++)
            {
                for (i = 1; i < j; i++)
                {
                    sum = a[i - 1][j - 1];
                    for (k = 1; k < i; k++)
                    {
                        sum -= a[i - 1][k - 1] * a[k - 1][j - 1];
                    }
                    a[i - 1][j - 1] = sum;
                }
                big = 0.0f;
                for (i = j; i <= n; i++)
                {
                    sum = a[i - 1][j - 1];
                    for (k = 1; k < j; k++)
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
                    for (k = 1; k <= n; k++)
                    {
                        dum = a[imax - 1][k - 1];
                        a[imax - 1][k - 1] = a[j - 1][k - 1];
                        a[j - 1][k - 1] = dum;
                    }
                    d = -d;
                    vv[imax - 1] = vv[j - 1];
                }
                indx[j - 1] = imax;
                if (a[j - 1][j - 1] == 0.0)
                {
                    a[j - 1][j - 1] = TINY;
                }
                if (j != n)
                {
                    dum = 1.0f / (a[j - 1][j - 1]);
                    for (i = j + 1; i <= n; i++)
                    {
                        a[i - 1][j - 1] *= dum;
                    }
                }
            }
            return 0;
        }

        private void lubksb(float[][] a, int n, int[] indx, float[] b)
        {
            int i = 0;
            int ii = 0;
            int ip = 0;
            int j = 0;
            float sum = 0;

            for (i = 1; i <= n; i++)
            {
                ip = indx[i - 1];
                sum = b[ip - 1];
                b[ip - 1] = b[i - 1];
                if (ii != 0)
                {
                    for (j = ii; j <= i - 1; j++)
                    {
                        sum -= a[i - 1][j - 1] * b[j - 1];
                    }
                }
                else if (sum != 0)
                {
                    ii = i;
                }
                b[i - 1] = sum;
            }
            for (i = n; i >= 1; i--)
            {
                sum = b[i - 1];
                for (j = i + 1; j <= n; j++)
                {
                    sum -= a[i - 1][j - 1] * b[j - 1];
                }
                b[i - 1] = sum / a[i - 1][i - 1];
            }
        }

        private int savgol(float[] c, int np, int nl, int nr, int ld, int m)
        {
            int mm = 0;
            int[] indx;
            float d = 0, fac = 0, sum = 0;
            float[][] a;
            float[] b;

            if (np < nl + nr + 1 || nl < 0 || nr < 0 || ld > m || nl + nr < m)
            {
                return -1;
            }
            indx = new int[m + 1];
            b = new float[m + 1];
            for (int j = 0; j < m + 1; j++)
            {
                indx[j] = 0;
                b[j] = 0;
            }

            a = new float[m + 1][];
            for (int j = 0; j < m + 1; j++)
            {
                a[j] = new float[m + 1];
                for (int i = 0; i < m + 1; i++)
                {
                    a[j][i] = 0;
                }
            }

            for (int ipj = 0; ipj <= (m << 1); ipj++)
            {
                sum = (ipj != 0 ? 0.0f : 1.0f);
                for (int k = 1; k <= nr; k++) sum += (float) Math.Pow((double) k, (double) ipj);
                for (int k = 1; k <= nl; k++) sum += (float) Math.Pow((double) -k, (double) ipj);
                mm = Math.Min(ipj, 2 * m - ipj);
                for (int imj = -mm; imj <= mm; imj += 2) a[(ipj + imj) / 2][(ipj - imj) / 2] = sum;
            }
            int ret_val = ludcmp(a, m + 1, indx, out d);
            if (ret_val == -1)
            {
                for (int kk = 1; kk <= np; kk++)
                {
                    c[kk] = 0.0f;
                }
                return -1;
            }

            for (int j = 1; j <= m + 1; j++)
            {
                b[j - 1] = 0.0f;
            }
            b[ld] = 1.0f;
            lubksb(a, m + 1, indx, b);
            for (int kk = 1; kk <= np; kk++)
            {
                c[kk] = 0.0f;
            }
            for (int k = -nl; k <= nr; k++)
            {
                sum = b[0];
                fac = 1.0f;
                for (mm = 1; mm <= m; mm++)
                {
                    sum += b[mm] * (fac *= k);
                }
                int kk = ((np - k) % np) + 1;
                c[kk] = sum;
            }
            return 0;

        }
    }
}
#endif