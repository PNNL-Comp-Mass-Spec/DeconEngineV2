#if Enable_Obsolete
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace Engine.Utilities
{
    internal static class Helpers
    {
        [Obsolete("Only used by DeconTools for IMFRun; BrukerV2 exists, but has no use path; Other uses have been otherwise handled", false)]
        public static double atof(string str)
        {
            double val = 0;
            var dblChars = new Regex(@"[+\-]?[\d]*\.?[\d]+([eE]?[+\-]?[\d]+)?");
            var digits = dblChars.Match(str).Value;
            if (double.TryParse(digits, out val))
            {
                return val;
            }
            return 0;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static bool GetInt16(string option_str, string search_str, int start, out short val)
        {
            val = 0;
            var temp = 0;
            if (GetInt32(option_str, search_str, start, out temp))
            {
                val = (short) temp;
                return true;
            }
            return false;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static bool GetInt32(string option_str, string search_str, int start, out int val)
        {
            val = 0;
            var found = option_str.IndexOf(search_str);
            if (found == -1)
                return false;
            found += search_str.Length;
            var intChars = new Regex(@"[+\-][\d]+");
            var digits = intChars.Match(option_str, found).Value;
            if (int.TryParse(digits, out val))
            {
                return true;
            }
            return false;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static bool GetDouble(string option_str, string search_str, int start, out double val)
        {
            val = 0;
            var found = option_str.IndexOf(search_str);
            if (found == -1)
                return false;
            found += search_str.Length;
            var dblChars = new Regex(@"[+\-]?[\d]*\.?[\d]+([eE]?[+\-]?[\d]+)?");
            var digits = dblChars.Match(option_str, found).Value;
            if (double.TryParse(digits, out val))
            {
                return true;
            }
            return false;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static long atol(string str)
        {
            long val = 0;
            var intChars = new Regex(@"[+\-]?[\d]+");
            var digits = intChars.Match(str).Value;
            if (long.TryParse(digits, out val))
            {
                return val;
            }
            return 0;
        }

        // TODO: Use BinaryReader instead...
        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static char ReadByte(Stream file, long pos)
        {
            file.Seek(pos, SeekOrigin.Begin);
            return (char) file.ReadByte();
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static char ReadByte(BinaryReader file, long pos)
        {
            file.BaseStream.Seek(pos, SeekOrigin.Begin);
            return file.ReadChar();
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static short ReadInt16(Stream file, long pos)
        {
            file.Seek(pos, SeekOrigin.Begin);
            var bytes = new byte[2];
            var bytesRead = 0;
            bytesRead = file.Read(bytes, 0, 2);
            if (bytesRead < 2)
            {
                return 0;
            }
            if (BitConverter.IsLittleEndian)
            {
                Array.Reverse(bytes);
            }
            return BitConverter.ToInt16(bytes, 0);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static int ReadInt32(Stream file, long pos)
        {
            file.Seek(pos, SeekOrigin.Begin);
            var bytes = new byte[4];
            var bytesRead = 0;
            bytesRead = file.Read(bytes, 0, 4);
            if (bytesRead < 4)
            {
                return 0;
            }
            if (BitConverter.IsLittleEndian)
            {
                Array.Reverse(bytes);
            }
            return BitConverter.ToInt32(bytes, 0);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static string ReadFileString(Stream file, long pos, long size)
        {
            var result = "";
            file.Seek(pos, SeekOrigin.Begin);
            var bytes = new byte[size];
            var bytesRead = 0;
            if (size > int.MaxValue)
            {
                throw new System.Exception("Very large contiguous reads from a file not currently supported");
            }
            bytesRead = file.Read(bytes, 0, (int) size);
            using (var stream = new StreamReader(new MemoryStream(bytes), Encoding.ASCII))
            {
                result = stream.ReadToEnd();
            }
            return result;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static double sgn(double a)
        {
            if (a < 0.0) return -1.0;
            return 1.0;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static void SolveQuadraticEquation(double b, double c, out double[] roots)
        {
            roots = new double[2];
            // x^2 + b x + c = 0. return in roots.
            var discriminant = b * b - 4 * c;
            if (discriminant < 0)
            {
                roots[0] = -1;
                roots[1] = -1;
                return;
            }
            discriminant = Math.Sqrt(discriminant);
            roots[0] = (-b - discriminant) / 2;
            roots[1] = (-b + discriminant) / 2;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static int CubeRoots(double a, double b, double c, out double[] roots)
        {
            const double PI = 3.1415926;
            double Q, R;
            double A, B;
            double theta;
            roots = new double[3];

            Q = (a * a - 3.0 * b) / 9.0;
            R = (2.0 * a * a * a - 9.0 * a * b + 27.0 * c) / 54.0;
            if (R * R < Q * Q * Q)
            {
                // here with three real roots.
                theta = Math.Acos(R / Math.Sqrt(Q * Q * Q));
                roots[0] = -2.0 * Math.Sqrt(Q) * Math.Cos(theta / 3.0) - a / 3.0;
                roots[1] = -2.0 * Math.Sqrt(Q) * Math.Cos((theta + 2.0 * PI) / 3.0) - a / 3.0;
                roots[2] = -2.0 * Math.Sqrt(Q) * Math.Cos((theta - 2.0 * PI) / 3.0) - a / 3.0;
                return 3;
            }
            // here with one real root...
            A = -1.0 * sgn(R) * Math.Pow(Math.Abs(R) + Math.Sqrt(Math.Abs(R * R - Q * Q * Q)), 1.0 / 3.0);
            if (A != 0.0) B = Q / A;
            else B = 0.0;
            roots[0] = A + B - a / 3.0;
            return 1;
        }

        /// <summary>
        ///     Gets the average intensity for points in a scan.
        /// </summary>
        /// <param name="mzs">is List of m/z values</param>
        /// <param name="intensities">is List of intensity values.</param>
        /// <param name="max_mz">is maximum m\z value to look at while calculating the average.</param>
        /// <param name="max_intensity">is the maximum intensity to used in the calculation.</param>
        /// <returns>returns the average intensity.</returns>
        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static double GetAverage(List<double> mzs, List<double> intensities, double max_mz, double max_intensity)
        {
            var num_pts = intensities.Count;
            if (num_pts == 0)
                return 0;

            double background_intensity = 0;
            double current_intensity;

            var num_pts_used = 0;

            for (var i = 0; i < num_pts; i++)
            {
                if (mzs[i] > max_mz)
                {
                    break;
                }
                if (intensities[i] < max_intensity)
                {
                    current_intensity = intensities[i];
                    background_intensity += current_intensity;
                    num_pts_used++;
                }
            }

            return background_intensity / num_pts_used;
        }
    }
}
#endif