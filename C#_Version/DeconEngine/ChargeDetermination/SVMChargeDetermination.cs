#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using DeconToolsV2.Peaks;
using MathNet.Numerics.LinearAlgebra;

namespace Engine.ChargeDetermination
{
    [Obsolete("Only used by Decon2LS.UI", false)]
    internal class SVMChargeDetermine
    {
        private const int num_features = 19;
        private const int num_bins = 4;

        //Param file and params
        private string mchar_svm_param_xml_file;
        private List<double> mvect_w = new List<double>();
        private List<double> mvect_w_aux = new List<double>();
        private List<double> mvect_b = new List<double>();
        private List<double> mvect_b_aux = new List<double>();
        private List<double> mvect_nbsv = new List<double>();
        private List<double> mvect_aux = new List<double>();
        /*/
        private Matrix mmat_vote = new Matrix();
        private double[,] mmat_vote_val = new double[1, 1];
        private Matrix mmat_discriminant_scores = new Matrix();
        private double[,] mmat_discriminant_scores_val = new double[1, 1];
        /*/
        private Matrix<double> mmat_vote;
        private Matrix<double> mmat_discriminant_scores;
        /**/

        private List<Engine.ChargeDetermination.FeatureList> mvect_xsup = new List<FeatureList>();
        private Engine.ChargeDetermination.FeatureList mobj_support_features;
        //private List<Engine.ChargeDetermination.FeatureList> mvect_xsup_aux = new List<FeatureList>();

        //Test and result
        private List<Engine.ChargeDetermination.FeatureList> mvect_xtest = new List<FeatureList>();
        //private List<Engine.ChargeDetermination.FeatureList> mvect_xtest_aux = new List<FeatureList>();

        private List<double> mvect_ypredict = new List<double>();
        private double[] marr_min_values = new double[num_features];
        private double[] marr_max_values = new double[num_features];

        // Input scan
        private Engine.ChargeDetermination.FeatureList mobj_scan_features;
        private List<double> mvect_mzs = new List<double>();
        private List<double> mvect_intensities = new List<double>();
        private int[] marr_pk_distribution = new int[num_bins];
        private int[] marr_intensity_distribution = new int[num_bins];

        //For features
        /*/
        private Matrix marr_mean_charge2;
        private Matrix marr_mean_charge3;
        private Matrix mmat_C;
        /*/
        private Matrix<double> marr_mean_charge2;
        private Matrix<double> marr_mean_charge3;
        private Matrix<double> mmat_C;
        /**/
        private double mdbl_mass_CO;
        private double mdbl_mass_H2O;
        private double mdbl_mass_NH3;

        public SVMChargeDetermine()
        {
            mobj_scan_features = new Engine.ChargeDetermination.FeatureList();
            mobj_support_features = new Engine.ChargeDetermination.FeatureList();

            /*/
            mmat_C = Matrix.matrix_allocate(2, 2, sizeof (double));
            marr_mean_charge2 = Matrix.matrix_allocate(1, 2, sizeof (double));
            marr_mean_charge3 = Matrix.matrix_allocate(1, 2, sizeof(double));
            /*/
            mmat_C = Matrix<double>.Build.Dense(2, 2, 0);
            marr_mean_charge2 = Matrix<double>.Build.Dense(1, 2, 0);
            marr_mean_charge3 = Matrix<double>.Build.Dense(1, 2, 0);
            /**/
            for (int i = 0; i < num_bins; i++)
            {
                marr_pk_distribution[i] = 0;
                marr_intensity_distribution[i] = 0;
            }

            mdbl_mass_CO = 27.9949141;
            mdbl_mass_H2O = 18.0105633;
            mdbl_mass_NH3 = 17.0265458;

            for (int i = 0; i < mobj_scan_features.GetNumFeatures(); i++)
            {
                marr_min_values[i] = double.MaxValue;
                marr_max_values[i] = 0; //double.MinValue;
            }

            mvect_nbsv.Add(0);
        }

        public void SetSVMParamFile(string file_name)
        {
            mchar_svm_param_xml_file = file_name;
        }

        public void InitVectors()
        {
            mvect_intensities.Clear();
            mvect_mzs.Clear();
            for (int bin_num = 0; bin_num < num_bins; bin_num++)
            {
                marr_pk_distribution[bin_num] = 0;
                marr_intensity_distribution[bin_num] = 0;
            }
        }

        public void InitializeLDA()
        {
            /*/
            double[,] C;
            double[,] mean2, mean3;

            C = mmat_C.ptr;
            mean2 = marr_mean_charge2.ptr;
            mean3 = marr_mean_charge3.ptr;

            //notation (row, col)
            //These values were acquired from training across 3 datasets - Shew, QC, Human_tipid
            mean2[0, 0] = 202.54;
            mean2[0, 1] = 0.31098;
            mean3[0, 0] = 254.84;
            mean3[0, 1] = 14.152;

            C[0, 0] = 8422;
            C[0, 1] = 102.4;
            C[1, 0] = 102.4;
            C[1, 1] = 23.746;
            /*/
            //notation (row, col)
            //These values were acquired from training across 3 datasets - Shew, QC, Human_tipid
            marr_mean_charge2[0, 0] = 202.54;
            marr_mean_charge2[0, 1] = 0.31098;
            marr_mean_charge3[0, 0] = 254.84;
            marr_mean_charge3[0, 1] = 14.152;

            mmat_C[0, 0] = 8422;
            mmat_C[0, 1] = 102.4;
            mmat_C[1, 0] = 102.4;
            mmat_C[1, 1] = 23.746;
            /**/
        }

        public bool IdentifyIfChargeOne(List<double> mzs, List<double> intensities, clsPeak parentPeak,
            int parentScan)
        {
            int numPeaks = mzs.Count;
            double noLoss = 0;
            double parent_mz = parentPeak.Mz;
            double temp1 = 0;
            double temp2 = 0;
            double temp3 = 0;
            double temp4 = 0;

            //Get input spectra
            InitVectors();
            for (int i = 0; i < numPeaks; i++)
            {
                mvect_intensities.Add(intensities[i]);
                mvect_mzs.Add(mzs[i]);
            }

            CalculatePeakProbabilities(parent_mz, out temp1, out temp2, out temp3, out temp4);

            // If all frag peaks lie in first bin [0 - parent_Mz]
            if (marr_pk_distribution[0] > 0.9 * numPeaks)
                return true;
            else
                return false;
        }

        public void GetFisherScores(out double fscore2, out double fscore3)
        {
            /*/
            Matrix m1, m2, m3, m4;
            Matrix first_term2, second_term2;
            Matrix first_term3, second_term3;
            Matrix invC, x, x_T;
            double[] prob = new double[2];
            double totalPeaks = 0;

            for (int i = 0; i < num_bins; i ++)
                totalPeaks += marr_pk_distribution[i];

            prob[0] = (double) marr_pk_distribution[1] / totalPeaks;
            prob[1] = (double) marr_pk_distribution[2] / totalPeaks;

            x = Matrix.matrix_allocate(1, 2, sizeof (double));
            double[,] xvalue;
            xvalue = x.ptr;

            xvalue[0, 0] = (double) marr_pk_distribution[1];
            xvalue[0, 1] = (double) marr_pk_distribution[2];

            x_T = Matrix.matrix_transpose(x);
            invC = Matrix.matrix_invert(mmat_C);
            m1 = Matrix.matrix_mult(marr_mean_charge2, invC);
            m2 = Matrix.matrix_transpose(marr_mean_charge2);
            first_term2 = Matrix.matrix_mult(m1, x_T);
            second_term2 = Matrix.matrix_mult(m1, m2);

            double[,] first_term2_value;
            double[,] second_term2_value;
            first_term2_value = first_term2.ptr;
            second_term2_value = second_term2.ptr;

            fscore2 = first_term2_value[0, 0] - 0.5 * second_term2_value[0, 0] + prob[0];

            m3 = Matrix.matrix_mult(marr_mean_charge3, invC);
            m4 = Matrix.matrix_transpose(marr_mean_charge3);
            first_term3 = Matrix.matrix_mult(m3, x_T);
            second_term3 = Matrix.matrix_mult(m3, m4);

            double[,] first_term3_value;
            double[,] second_term3_value;
            first_term3_value = first_term3.ptr;
            second_term3_value = second_term3.ptr;
            fscore3 = first_term3_value[0, 0] - 0.5 * second_term3_value[0, 0] + prob[1];
            /*/
            Matrix<double> m1, m2, m3, m4;
            Matrix<double> first_term2, second_term2;
            Matrix<double> first_term3, second_term3;
            Matrix<double> invC, x, x_T;
            double[] prob = new double[2];
            double totalPeaks = 0;

            for (int i = 0; i < num_bins; i++)
                totalPeaks += marr_pk_distribution[i];

            prob[0] = (double)marr_pk_distribution[1] / totalPeaks;
            prob[1] = (double)marr_pk_distribution[2] / totalPeaks;

            x = Matrix<double>.Build.Dense(1, 2, 0);

            x[0, 0] = (double)marr_pk_distribution[1];
            x[0, 1] = (double)marr_pk_distribution[2];

            x_T = x.Transpose();
            invC = mmat_C.Inverse();
            m1 = marr_mean_charge2.Multiply(invC);
            m2 = marr_mean_charge2.Transpose();
            first_term2 = m1.Multiply(x_T);
            second_term2 = m1.Multiply(m2);

            fscore2 = first_term2[0, 0] - 0.5 * second_term2[0, 0] + prob[0];

            m3 = marr_mean_charge3.Multiply(invC);
            m4 = marr_mean_charge3.Transpose();
            first_term3 = m3.Multiply(x_T);
            second_term3 = m3.Multiply(m4);

            fscore3 = first_term3[0, 0] - 0.5 * second_term3[0, 0] + prob[1];
            /**/
        }

        public void GetFeaturesForSpectra(List<double> mzs, List<double> intensities, clsPeak parentPeak,
            int msNscan)
        {
            double xscore2;
            double xscore3;
            double xscore_ratio;
            double bscore2;
            double bscore3;
            double pk1_prob;
            double pk2_prob;
            double pk3_prob;
            double pk4_prob;
            double fscore2;
            double fscore3;
            double xscore2_CO;
            double xscore3_CO;
            double xscore2_H2O;
            double xscore3_H2O;
            double xscore2_NH3;
            double xscore3_NH3;

            int numPeaks = mzs.Count;

            double noLoss = 0;
            double parent_mz = parentPeak.Mz;

            //Get input spectra
            InitVectors();
            for (int i = 0; i < numPeaks; i++)
            {
                mvect_intensities.Add(intensities[i]);
                mvect_mzs.Add(mzs[i]);
            }

            //Start with feature detection
            CalculatePeakProbabilities(parent_mz, out pk1_prob, out pk2_prob, out pk3_prob, out pk4_prob);
            GetFisherScores(out fscore2, out fscore3);
            NormalizeSpectra();
            GetXScores(parent_mz, out xscore2, out xscore3, noLoss);
            if (!xscore2.Equals(0))
                xscore_ratio = xscore3 / xscore2;
            else
                xscore_ratio = 0;
            GetBScores(parent_mz, out bscore2, out bscore3);
            GetXScores(parent_mz, out xscore2_CO, out xscore3_CO, mdbl_mass_CO);
            GetXScores(parent_mz, out xscore2_H2O, out xscore3_H2O, mdbl_mass_H2O);
            GetXScores(parent_mz, out xscore2_NH3, out xscore3_NH3, mdbl_mass_NH3);

            ReadValues(msNscan, parent_mz, xscore2, xscore3, xscore_ratio, bscore2, bscore3, pk1_prob,
                pk2_prob, pk3_prob, pk4_prob, fscore2, fscore3, xscore2_CO, xscore3_CO, xscore2_H2O,
                xscore3_H2O, xscore2_NH3, xscore3_NH3);
        }

        public void ReadValues(int scan_num, double parent_Mz, double x_score2, double x_score3, double x_score_ratio,
            double b_score2, double b_score3, double pk1_prb, double pk2_prb, double pk3_prob,
            double pk4_prb, double fscore2, double fscore3, double x_score2_CO, double x_score3_CO, double x_score2_H2O,
            double x_score3_H20, double x_score2_NH3, double x_score3_NH3)
        {
            List<double> vect_features = new List<double>();
            double val = 0;
            vect_features.Add(scan_num);
            vect_features.Add(parent_Mz);
            vect_features.Add(x_score2);
            vect_features.Add(x_score3);
            vect_features.Add(x_score_ratio);
            vect_features.Add(b_score2);
            vect_features.Add(b_score3);
            vect_features.Add(pk1_prb);
            vect_features.Add(pk2_prb);
            vect_features.Add(pk3_prob);
            vect_features.Add(pk4_prb);
            vect_features.Add(fscore2);
            vect_features.Add(fscore3);
            vect_features.Add(x_score2_CO);
            vect_features.Add(x_score3_CO);
            vect_features.Add(x_score2_H2O);
            vect_features.Add(x_score3_H20);
            vect_features.Add(x_score2_NH3);
            vect_features.Add(x_score3_NH3);

            mobj_scan_features.InitValues(vect_features);
            mvect_xtest.Add(new FeatureList(mobj_scan_features));

            for (int i = 0; i < num_features; i++)
            {
                val = vect_features[i];
                if (val <= marr_min_values[i])
                    marr_min_values[i] = val;
                if (val >= marr_max_values[i])
                    marr_max_values[i] = val;
            }
        }

        public void NormalizeSpectra()
        {
            double threshold = 5;
            double median = 0;
            int size = mvect_intensities.Count;
            List<double> vect_sort_intensities = new List<double>();

            for (int i = 0; i < size; i++)
            {
                vect_sort_intensities.Add(mvect_intensities[i]);
            }

            vect_sort_intensities.Sort();

            if (size % 2 == 0)
                median = vect_sort_intensities[size / 2];
            else
                median = (vect_sort_intensities[(size - 1) / 2] + vect_sort_intensities[(size + 1) / 2]) / 2;

            for (int i = 0; i < size; i++)
            {
                double val = mvect_intensities[i] / median;
                if (val > threshold)
                    mvect_intensities[i] = val;
                else
                    mvect_intensities[i] = 0;
            }
        }

        public void GetXScores(double parent_Mz, out double xscore2, out double xscore3, double neutralLoss)
        {
            parent_Mz = parent_Mz - neutralLoss;
            xscore2 = 0;
            xscore3 = 0;
            double SumMForwardCS2 = 0;
            double mzForwardBeginCS2 = 0;
            double mzForwardEndCS2 = 0;

            //for xscore3
            double SumMForwardCS3_1 = 0;
            double SumMForwardCS3_2 = 0;
            double mzForwardBeginCS3_1 = 0;
            double mzForwardEndCS3_1 = 0;
            double mzForwardBeginCS3_2 = 0;
            double mzForwardEndCS3_2 = 0;

            double SumMReverse = 0;
            double mzReverseBegin = 0;
            double mzReverseEnd = 0;
            double mz = 0;

            for (int i = 0; i < parent_Mz; i ++)
            {
                //for xscore2
                SumMForwardCS2 = 0;
                mzForwardBeginCS2 = (parent_Mz + i) - 0.5;
                mzForwardEndCS2 = (parent_Mz + i) + 0.5;

                //for xscore3
                SumMForwardCS3_1 = 0;
                SumMForwardCS3_2 = 0;
                mzForwardBeginCS3_1 = (3 * parent_Mz - 2 * i) - 0.5;
                mzForwardEndCS3_1 = (3 * parent_Mz - 2 * i) + 0.5;
                mzForwardBeginCS3_2 = (3 * parent_Mz - i) / 2 - 0.5;
                mzForwardEndCS3_2 = (3 * parent_Mz - i) / 2 + 0.5;

                //for both
                SumMReverse = 0;
                mzReverseBegin = (parent_Mz - i) - 0.5;
                mzReverseEnd = (parent_Mz - i) + 0.5;

                for (int j = 0; j < (int) mvect_mzs.Count; j++)
                {
                    mz = mvect_mzs[j];
                    if (mz >= mzForwardBeginCS2 && mz <= mzForwardEndCS2)
                        SumMForwardCS2 += (mvect_intensities[j]);
                    if (mz >= mzForwardBeginCS3_1 && mz <= mzForwardEndCS3_1)
                        SumMForwardCS3_1 += (mvect_intensities[j]);
                    if (mz >= mzForwardBeginCS3_2 && mz <= mzForwardEndCS3_2)
                        SumMForwardCS3_2 += (mvect_intensities[j]);
                    if (mz >= mzReverseBegin && mz <= mzReverseEnd)
                        SumMReverse += (mvect_intensities[j]);
                }
                xscore2 += (SumMForwardCS2 * SumMReverse);
                xscore3 += ((SumMForwardCS3_1 * SumMReverse) + (SumMForwardCS3_2 * SumMReverse));
            }
        }

        public double GetScoreAtScanIndex(int index)
        {
            /*/
            double val = mmat_discriminant_scores_val[index, 3];
            return val;
            /*/
            return mmat_discriminant_scores[index, 3];
            /**/
        }

        public int GetClassAtScanIndex(int index)
        {
            /*/
            //return values 1 - +1, 2 - +2, 3 - +3, 4 - +4, 0 - +2 or +3
            int num_class = mmat_vote.cols;
            int charge = 0;
            for (int col = 0; col < num_class; col++)
            {
                int rank = (int) mmat_vote_val[index, col];
                if (mmat_vote_val[index, col] == 3)
                {
                    charge = col + 1;
                }
            }
            if (charge == 2 || charge == 3)
            {
                // As column 3 gives score between 2 and 3
                double val = mmat_discriminant_scores_val[index, 3];
                if (val > -5 && val < 2.5) //these thresholds were found using the score distribution curves
                    charge = 0;
            }
            return charge;
            /*/
            //return values 1 - +1, 2 - +2, 3 - +3, 4 - +4, 0 - +2 or +3
            int num_class = mmat_vote.ColumnCount;
            int charge = 0;
            for (int col = 0; col < num_class; col++)
            {
                int rank = (int)mmat_vote[index, col];
                if (mmat_vote[index, col] == 3)
                {
                    charge = col + 1;
                }
            }
            if (charge == 2 || charge == 3)
            {
                // As column 3 gives score between 2 and 3
                double val = mmat_discriminant_scores[index, 3];
                if (val > -5 && val < 2.5) //these thresholds were found using the score distribution curves
                    charge = 0;
            }
            return charge;
            /**/
        }

        public void ClearMemory()
        {
            // clearing the matrices/ vectors (separate from the destructor as
            // initialization is done separetely from the constructor)
            /*/
            Matrix.matrix_free(mmat_vote);
            Matrix.matrix_free(mmat_discriminant_scores);
            /*/
            /**/
            if (mvect_b.Count != 0)
                mvect_b.Clear();
            if (mvect_b_aux.Count != 0)
                mvect_b_aux.Clear();
            if (mvect_aux.Count != 0)
                mvect_aux.Clear();
            if (mvect_w.Count != 0)
                mvect_w.Clear();
            if (mvect_w_aux.Count != 0)
                mvect_w_aux.Clear();
            if (mvect_xsup.Count != 0)
                mvect_xsup.Clear();
            if (mvect_xtest.Count != 0)
                mvect_xtest.Clear();
            //if (mvect_xsup_aux.Count != 0)
            //    mvect_xsup_aux.Clear();
            //if (mvect_xtest_aux.Count != 0)
            //    mvect_xtest_aux.Clear();
            if (mvect_ypredict.Count != 0)
                mvect_ypredict.Clear();
        }

        public void ResolveIntoClass()
        {
            /*/
            int num_rows = mmat_vote.rows;
            int num_cols = mmat_vote.cols;
            for (int row = 0; row < num_rows; row++)
            {
                int charge = 1;
                for (int col = 0; col < num_cols; col++)
                {
                    if (mmat_vote_val[row, col] == 3)
                    {
                        charge = col + 1;
                        break;
                    }
                    mvect_ypredict[row] = charge;
                }
            }
            /*/
            int num_rows = mmat_vote.RowCount;
            int num_cols = mmat_vote.ColumnCount;
            for (int row = 0; row < num_rows; row++)
            {
                int charge = 1;
                for (int col = 0; col < num_cols; col++)
                {
                    if (mmat_vote[row, col].Equals(3))
                    {
                        charge = col + 1;
                        break;
                    }
                    mvect_ypredict[row] = charge;
                }
            }
            /**/
        }

        public void GetBScores(double parent_Mz, out double bscore2, out double bscore3)
        {
            bscore2 = 0;
            bscore3 = 0;

            double sumLeft = 0;
            double sumRight_CS2 = 0;
            double sumRight_CS3 = 0;
            double sumTotal_CS2 = 0;
            double sumTotal_CS3 = 0;
            double mz = 0;

            for (int i = 0; i < (int) mvect_mzs.Count; i++)
            {
                mz = mvect_mzs[i];
                if (mz < parent_Mz - 1)
                    sumLeft += mvect_intensities[i];
                if (mz > parent_Mz + 1 && mz <= (2 * parent_Mz))
                    sumRight_CS2 += mvect_intensities[i];
                if (mz > parent_Mz + 1 && mz <= (3 * parent_Mz))
                    sumRight_CS3 += mvect_intensities[i];
                if (mz <= (2 * parent_Mz))
                    sumTotal_CS2 += mvect_intensities[i];
                if (mz <= (3 * parent_Mz))
                    sumTotal_CS3 += mvect_intensities[i];
            }

            if (sumTotal_CS2 > 0)
                bscore2 = (sumLeft - sumRight_CS2) / sumTotal_CS2;
            if (sumTotal_CS3 > 0)
                bscore3 = (sumLeft - sumRight_CS3) / sumTotal_CS3;

            bscore2 = Math.Abs(bscore2);
            bscore3 = Math.Abs(bscore3);
        }

        public void CalculatePeakProbabilities(double parent_Mz, out double pk1, out double pk2, out double pk3,
            out double pk4)
        {
            int numPeaks = mvect_intensities.Count;
            int sum = 0;
            double mz = 0;
            double intensity = 0;
            pk1 = -1;
            pk2 = -1;
            pk3 = -1;
            pk4 = -1;

            for (int i = 1; i < numPeaks - 1; i++)
            {
                mz = mvect_mzs[i];
                intensity = mvect_intensities[i];
                if (mz > 0 && mz <= parent_Mz)
                {
                    marr_pk_distribution[0]++;
                    marr_intensity_distribution[0] += (int) mvect_intensities[i];
                }
                if (mz > parent_Mz && mz <= (2 * parent_Mz))
                {
                    marr_pk_distribution[1]++;
                    marr_intensity_distribution[1] += (int) mvect_intensities[i];
                }
                if (mz > (2 * parent_Mz) && mz <= (3 * parent_Mz))
                {
                    marr_pk_distribution[2]++;
                    marr_intensity_distribution[2] += (int) mvect_intensities[i];
                }
                if (mz > (3 * parent_Mz) && mz < (4 * parent_Mz))
                {
                    marr_pk_distribution[3]++;
                    marr_intensity_distribution[3] += (int) mvect_intensities[i];
                }
            }

            for (int i = 0; i < num_bins; i++)
            {
                int val = marr_pk_distribution[i];
                sum += marr_pk_distribution[i];
            }

            pk1 = (double) marr_pk_distribution[0] / (double) sum;
            pk2 = (double) marr_pk_distribution[1] / (double) sum;
            pk3 = (double) marr_pk_distribution[2] / (double) sum;
            pk4 = (double) marr_pk_distribution[3] / (double) sum;
        }

        public void NormalizeDataSet()
        {
            // Normalize Data Set
            int num_vectors = (int) mvect_xtest.Count;

            if (num_vectors < 2)
                return;

            double val = 0;

            for (int vector_num = 0; vector_num < num_vectors; vector_num++)
            {
                Engine.ChargeDetermination.FeatureList this_test_vector;
                this_test_vector = mvect_xtest[vector_num];

                for (int feature_num = 0; feature_num < num_features; feature_num++)
                {
                    double max_feature = marr_max_values[feature_num];
                    double min_feature = marr_min_values[feature_num];
                    val = this_test_vector.GetValueAt(feature_num);
                    if (marr_max_values[feature_num] > marr_min_values[feature_num])
                        val = (val - marr_min_values[feature_num]) /
                              (marr_max_values[feature_num] - marr_min_values[feature_num]);
                    else
                        val = 0;

                    this_test_vector.SetValueAt(feature_num, val);
                }
                //mvect_xtest[vector_num] = this_test_vector; // original is reference, don't need to re-assign
            }
        }

        /*/
        public Matrix GetKernel(List<Engine.ChargeDetermination.FeatureList> vect_xtest,
            List<Engine.ChargeDetermination.FeatureList> vect_xsup)
        {
            Engine.ChargeDetermination.FeatureList this_test_vector;
            Engine.ChargeDetermination.FeatureList this_support_vector;

            Matrix norm_x;
            Matrix norm_xsup;
            Matrix norm_xsup_t;
            Matrix metric;
            Matrix xsup;
            Matrix xsup_t;
            Matrix xsup2;
            Matrix x;
            Matrix x2;
            Matrix ps;
            Matrix psTemp;
            Matrix m1;
            Matrix m2;
            Matrix m3;
            Matrix kernel;

            double[,] norm_x_value;
            double[,] norm_xsup_value;
            double[,] metric_value;
            double[,] xsup_value;
            double[,] x_value;
            double[,] x2_value;
            double[,] ps_value;
            double[,] kernel_value;
            double[,] m1_value;
            double[,] m2_value;
            double[,] m3_value;

            int num_vectors = (int) vect_xsup.Count;
            int num_test = (int) vect_xtest.Count;
            int ps_rows;
            int ps_cols;

            //get vectors
            xsup = Matrix.matrix_allocate(num_vectors, num_features, sizeof (double));
            xsup_value = xsup.ptr;
            for (int i = 0; i < num_vectors; i++)
            {
                this_support_vector = vect_xsup[i];
                for (int j = 0; j < num_features; j++)
                {
                    double val_sup = this_support_vector.GetValueAt(j);
                    xsup_value[i, j] = val_sup;
                }
            }

            x = Matrix.matrix_allocate(num_test, num_features, sizeof (double));
            x_value = x.ptr;
            for (int i = 0; i < num_test; i++)
            {
                this_test_vector = vect_xtest[i];
                for (int j = 0; j < num_features; j++)
                {
                    double val_x = this_test_vector.GetValueAt(j);
                    x_value[i, j] = val_x;
                }
            }

            //Init metric
            metric = Matrix.matrix_allocate(num_features, num_features, sizeof (double));
            metric_value = metric.ptr;
            for (int i = 0; i < num_features; i++)
                metric_value[i, i] = 1;

            // Get ps
            m1 = Matrix.matrix_mult(x, metric);
            m1_value = m1.ptr;
            xsup_t = Matrix.matrix_transpose(xsup);
            double[,] xsup_t_value;
            xsup_t_value = xsup_t.ptr;

            ps = Matrix.matrix_mult(m1, xsup_t);
            double[,] ps_v;
            ps_v = ps.ptr;
            ps_rows = ps.rows; //num_test
            ps_cols = ps.cols; //num_support

            // Get norms
            norm_x = Matrix.matrix_allocate(ps_rows, ps_cols, sizeof (double));
            norm_x_value = norm_x.ptr;

            norm_xsup = Matrix.matrix_allocate(ps_cols, ps_rows, sizeof (double));
            norm_xsup_value = norm_xsup.ptr;

            x2 = Matrix.matrix_mult_pwise(x, x);
            x2_value = x2.ptr;
            m2 = Matrix.matrix_mult(x2, metric);
            m2_value = m2.ptr;

            for (int row_num = 0; row_num < m2.rows; row_num++)
            {
                double sumX = 0;
                for (int col_num = 0; col_num < m2.cols; col_num++)
                {
                    double val_m2 = m2_value[row_num, col_num];
                    sumX = sumX + val_m2;
                }
                //This is for ease of matrix addition
                for (int col_num = 0; col_num < norm_x.cols; col_num++)
                    norm_x_value[row_num, col_num] = sumX;
            }

            xsup2 = Matrix.matrix_mult_pwise(xsup, xsup);
            m3 = Matrix.matrix_mult(xsup2, metric);
            m3_value = m3.ptr;
            for (int row_num = 0; row_num < m3.rows; row_num ++)
            {
                double sumXsup = 0;
                for (int col_num = 0; col_num < m3.cols; col_num++)
                {
                    double val_m3 = m3_value[row_num, col_num];
                    sumXsup = sumXsup + val_m3;
                }

                for (int col_num = 0; col_num < norm_xsup.cols; col_num++)
                    norm_xsup_value[row_num, col_num] = sumXsup;
            }

            norm_xsup_t = Matrix.matrix_transpose(norm_xsup);
            double[,] norm_xsup_t_val;
            norm_xsup_t_val = norm_xsup_t.ptr;

            double scale = -2.0;
            psTemp = Matrix.matrix_scale(ps, scale);
            double[,] psTemp_val;
            psTemp_val = psTemp.ptr;

            Matrix ps2;
            Matrix ps3;
            ps2 = Matrix.matrix_add(psTemp, norm_x);
            double[,] ps2_val;
            ps2_val = ps2.ptr;
            ps3 = Matrix.matrix_add(ps2, norm_xsup_t);
            double[,] ps3_val;
            ps3_val = ps3.ptr;

            Matrix.matrix_free(ps);
            ps = Matrix.matrix_scale(ps3, 1 / scale);
            ps_value = ps.ptr;
            Matrix.matrix_free(psTemp);
            Matrix.matrix_free(ps2);
            Matrix.matrix_free(ps3);

            kernel = Matrix.matrix_allocate(ps.rows, ps.cols, ps.element_size);
            kernel_value = kernel.ptr;

            for (int row_num = 0; row_num < ps.rows; row_num ++)
            {
                for (int col_num = 0; col_num < ps.cols; col_num++)
                {
                    double val = ps_value[row_num, col_num];
                    double eval = Math.Exp(val);
                    kernel_value[row_num, col_num] = eval;
                }
            }

            Matrix.matrix_free(norm_x);
            Matrix.matrix_free(norm_xsup);
            Matrix.matrix_free(norm_xsup_t);
            Matrix.matrix_free(metric);
            Matrix.matrix_free(xsup);
            Matrix.matrix_free(xsup_t);
            Matrix.matrix_free(xsup2);
            Matrix.matrix_free(x);
            Matrix.matrix_free(x2);
            Matrix.matrix_free(ps);
            Matrix.matrix_free(m1);
            Matrix.matrix_free(m2);
            Matrix.matrix_free(m3);

            return kernel;
        /*/
        public Matrix<double> GetKernel(List<Engine.ChargeDetermination.FeatureList> vect_xtest,
            List<Engine.ChargeDetermination.FeatureList> vect_xsup)
        {
            Engine.ChargeDetermination.FeatureList this_test_vector;
            Engine.ChargeDetermination.FeatureList this_support_vector;

            Matrix<double> norm_x;
            Matrix<double> norm_xsup;
            Matrix<double> norm_xsup_t;
            Matrix<double> metric;
            Matrix<double> xsup;
            Matrix<double> xsup_t;
            Matrix<double> xsup2;
            Matrix<double> x;
            Matrix<double> x2;
            Matrix<double> ps;
            Matrix<double> psTemp;
            Matrix<double> m1;
            Matrix<double> m2;
            Matrix<double> m3;
            Matrix<double> kernel;

            int num_vectors = (int)vect_xsup.Count;
            int num_test = (int)vect_xtest.Count;
            int ps_rows;
            int ps_cols;

            //get vectors
            xsup = Matrix<double>.Build.Dense(num_vectors, num_features, 0);
            for (int i = 0; i < num_vectors; i++)
            {
                this_support_vector = vect_xsup[i];
                for (int j = 0; j < num_features; j++)
                {
                    double val_sup = this_support_vector.GetValueAt(j);
                    xsup[i, j] = val_sup;
                }
            }

            x = Matrix<double>.Build.Dense(num_test, num_features, 0);
            for (int i = 0; i < num_test; i++)
            {
                this_test_vector = vect_xtest[i];
                for (int j = 0; j < num_features; j++)
                {
                    double val_x = this_test_vector.GetValueAt(j);
                    x[i, j] = val_x;
                }
            }

            //Init metric
            metric = Matrix<double>.Build.Dense(num_features, num_features, 0);
            for (int i = 0; i < num_features; i++)
                metric[i, i] = 1;

            // Get ps
            m1 = x.Multiply(metric);
            xsup_t = xsup.Transpose();

            ps = m1.Multiply(xsup_t);
            ps_rows = ps.RowCount; //num_test
            ps_cols = ps.ColumnCount; //num_support

            // Get norms
            norm_x = Matrix<double>.Build.Dense(ps_rows, ps_cols, 0);
            norm_xsup = Matrix<double>.Build.Dense(ps_cols, ps_rows, 0);

            x2 = x.PointwiseMultiply(x);
            m2 = x2.Multiply(metric);

            for (int row_num = 0; row_num < m2.RowCount; row_num++)
            {
                double sumX = 0;
                for (int col_num = 0; col_num < m2.ColumnCount; col_num++)
                {
                    double val_m2 = m2[row_num, col_num];
                    sumX = sumX + val_m2;
                }
                //This is for ease of matrix addition
                for (int col_num = 0; col_num < norm_x.ColumnCount; col_num++)
                    norm_x[row_num, col_num] = sumX;
            }

            xsup2 = xsup.PointwiseMultiply(xsup);
            m3 = xsup2.Multiply(metric);
            for (int row_num = 0; row_num < m3.RowCount; row_num++)
            {
                double sumXsup = 0;
                for (int col_num = 0; col_num < m3.ColumnCount; col_num++)
                {
                    double val_m3 = m3[row_num, col_num];
                    sumXsup = sumXsup + val_m3;
                }

                for (int col_num = 0; col_num < norm_xsup.ColumnCount; col_num++)
                    norm_xsup[row_num, col_num] = sumXsup;
            }

            norm_xsup_t = norm_xsup.Transpose();

            double scale = -2.0;
            psTemp = ps.Multiply(scale);
            Matrix<double> ps2 = psTemp.Add(norm_x);
            Matrix<double> ps3 = ps2.Add(norm_xsup_t);
            ps = ps3.Multiply(1 / scale);

            kernel = Matrix<double>.Build.Dense(ps.RowCount, ps.ColumnCount, 0);

            for (int row_num = 0; row_num < ps.RowCount; row_num++)
            {
                for (int col_num = 0; col_num < ps.ColumnCount; col_num++)
                {
                    double val = ps[row_num, col_num];
                    double eval = Math.Exp(val);
                    kernel[row_num, col_num] = eval;
                }
            }

            return kernel;
        /**/
        }

        public void DetermineClassForDataSet()
        {
            /*/
            int length_nbsv = 0;
            int num_class = 0;
            int num_test = 0;
            int k = 0;
            int num_iterations = 6;
            int iter_num = 0;

            length_nbsv = mvect_nbsv.Count;
            num_class = 4; //(int)(1 + (int)(Math.Sqrt(1+4*2*length_nbsv)))/2;
            num_test = (int) mvect_xtest.Count;
            mmat_vote = Matrix.matrix_allocate(num_test, num_class, sizeof (double));
            mmat_vote_val = mmat_vote.ptr;
            mmat_discriminant_scores = Matrix.matrix_allocate(num_test, num_iterations, sizeof (double));
            mmat_discriminant_scores_val = mmat_discriminant_scores.ptr;
            CalculateCumSum();

            for (int row = 0; row < (int) mvect_xtest.Count; row++)
                mvect_ypredict.Add(0);

            for (int row = 0; row < mmat_vote.rows; row++)
            {
                for (int col = 0; col < mmat_vote.cols; col++)
                {
                    mmat_vote_val[row, col] = 0;
                }
                for (int col = 0; col < mmat_discriminant_scores.cols; col++)
                {
                    mmat_discriminant_scores_val[row, col] = 0;
                }
            }

            for (int i = 0; i < num_class; i++)
            {
                for (int j = i + 1; j < num_class; j++)
                {
                    int startIndexToConsider = (int) mvect_aux[k];
                    int stopIndexToConsider = (int) (mvect_aux[k] + mvect_nbsv[k + 1]) - 1;
                    SVMClassification(startIndexToConsider, stopIndexToConsider, k);
                    for (int row = 0; row < mmat_vote.rows; row++)
                    {
                        double val = mvect_ypredict[row];

                        mmat_discriminant_scores_val[row, iter_num] = val;

                        mvect_ypredict[row] = 0;
                        if (val >= 0)
                        {
                            double val2 = mmat_vote_val[row, i];
                            val2++;
                            mmat_vote_val[row, i] = val2;
                        }
                        else
                        {
                            double val1 = mmat_vote_val[row, j];
                            val1++;
                            mmat_vote_val[row, j] = val1;
                        }
                    }
                    k++;
                    iter_num++;
                }
            }
            /*/
            int length_nbsv = 0;
            int num_class = 0;
            int num_test = 0;
            int k = 0;
            int num_iterations = 6;
            int iter_num = 0;

            length_nbsv = mvect_nbsv.Count;
            num_class = 4; //(int)(1 + (int)(Math.Sqrt(1+4*2*length_nbsv)))/2;
            num_test = (int)mvect_xtest.Count;
            mmat_vote = Matrix<double>.Build.Dense(num_test, num_class, 0);
            mmat_discriminant_scores = Matrix<double>.Build.Dense(num_test, num_iterations, 0);
            CalculateCumSum();

            for (int row = 0; row < (int)mvect_xtest.Count; row++)
                mvect_ypredict.Add(0);

            for (int row = 0; row < mmat_vote.RowCount; row++)
            {
                for (int col = 0; col < mmat_vote.ColumnCount; col++)
                {
                    mmat_vote[row, col] = 0;
                }
                for (int col = 0; col < mmat_discriminant_scores.ColumnCount; col++)
                {
                    mmat_discriminant_scores[row, col] = 0;
                }
            }

            for (int i = 0; i < num_class; i++)
            {
                for (int j = i + 1; j < num_class; j++)
                {
                    int startIndexToConsider = (int)mvect_aux[k];
                    int stopIndexToConsider = (int)(mvect_aux[k] + mvect_nbsv[k + 1]) - 1;
                    SVMClassification(startIndexToConsider, stopIndexToConsider, k);
                    for (int row = 0; row < mmat_vote.RowCount; row++)
                    {
                        double val = mvect_ypredict[row];

                        mmat_discriminant_scores[row, iter_num] = val;

                        mvect_ypredict[row] = 0;
                        if (val >= 0)
                        {
                            double val2 = mmat_vote[row, i];
                            val2++;
                            mmat_vote[row, i] = val2;
                        }
                        else
                        {
                            double val1 = mmat_vote[row, j];
                            val1++;
                            mmat_vote[row, j] = val1;
                        }
                    }
                    k++;
                    iter_num++;
                }
            }
            /**/
        }

        public void CalculateCumSum()
        {
            double sum = 0;
            for (int i = 0; i < (int) mvect_nbsv.Count; i++)
            {
                sum = 0;
                for (int j = 0; j <= i; j++)
                {
                    sum += mvect_nbsv[j];
                }
                mvect_aux.Add(sum);
            }
        }

        public void SVMClassification(int startIndex, int stopIndex, int k_bias)
        {
            List<double> span = new List<double>();
            List<double> y = new List<double>();
            List<Engine.ChargeDetermination.FeatureList> chunk_xtest = new List<FeatureList>();
            List<Engine.ChargeDetermination.FeatureList> chunk_xsup = new List<FeatureList>();
            Engine.ChargeDetermination.FeatureList this_test_vector;
            Engine.ChargeDetermination.FeatureList this_support_vector;

            int num_support = stopIndex - startIndex;
            int num_test = (int) mvect_xtest.Count;
            const int chunksize = 100;
            List<int> ind1 = new List<int>();
            List<int> ind2 = new List<int>();

            int chunks1 = (int) (num_support / chunksize) + 1;
            int chunks2 = (int) (num_test / chunksize) + 1;

            // Performing y2(ind2)=y2(ind2)+ kchunk*w(ind1);
            for (int ch1 = 1; ch1 <= chunks1; ch1++)
            {
                //Get ind1
                int low_ind1_index = (ch1 - 1) * chunksize + startIndex;
                int high_ind1_index = (ch1 * chunksize) - 1 + startIndex;
                if (high_ind1_index > stopIndex)
                    high_ind1_index = stopIndex;
                ind1.Clear();
                for (int index = 0; index <= (high_ind1_index - low_ind1_index); index++)
                    ind1.Add(index + low_ind1_index);

                //Get support vectors
                chunk_xsup.Clear();
                for (int j = 0; j < (int) ind1.Count; j++)
                {
                    int xsupIndex = ind1[j];
                    this_support_vector = new FeatureList(mvect_xsup[xsupIndex]);
                    chunk_xsup.Add(this_support_vector);
                }

                for (int ch2 = 1; ch2 <= chunks2; ch2++)
                {
                    //Get ind2
                    int low_ind2_index = (ch2 - 1) * chunksize;
                    int high_ind2_index = (ch2 * chunksize) - 1;
                    if (high_ind2_index > num_test)
                        high_ind2_index = num_test - 1;
                    ind2.Clear();
                    for (int index2 = 0; index2 <= (high_ind2_index - low_ind2_index); index2++)
                        ind2.Add(index2 + low_ind2_index);

                    //Get X vector
                    chunk_xtest.Clear();
                    for (int j = 0; j < (int) ind2.Count; j++)
                    {
                        int xIndex = ind2[j];
                        this_test_vector = mvect_xtest[xIndex];
                        chunk_xtest.Add(new FeatureList(this_test_vector));
                    }
                    /*/
                    //Get the kernel
                    Matrix svm_kernel;
                    double[,] svm_kernel_val;
                    svm_kernel = GetKernel(chunk_xtest, chunk_xsup);
                    svm_kernel_val = svm_kernel.ptr;

                    //Read in the weights w(ind1)
                    Matrix w;
                    double[,] w_value;
                    w = Matrix.matrix_allocate((int) ind1.Count, 1, sizeof (double));
                    w_value = w.ptr;
                    for (int i = 0; i < (int) ind1.Count; i++)
                    {
                        int index = ind1[i];
                        w_value[i, 0] = mvect_w[index];
                    }

                    // m1 = kchunk*w(ind1)
                    Matrix m1;
                    double[,] m1_value;
                    m1 = Matrix.matrix_mult(svm_kernel, w);
                    m1_value = m1.ptr;

                    //y2(ind2) += m1;
                    for (int i = 0; i < (int) ind2.Count; i++)
                    {
                        int index = ind2[i];

                        int vale = (int) mvect_ypredict[index];
                        mvect_ypredict[index] += m1_value[i, 0];
                        vale = (int) mvect_ypredict[index];
                    }

                    Matrix.matrix_free(m1);
                    Matrix.matrix_free(svm_kernel);
                    Matrix.matrix_free(w);
                    /*/

                    //Get the kernel
                    Matrix<double> svm_kernel;
                    svm_kernel = GetKernel(chunk_xtest, chunk_xsup);

                    //Read in the weights w(ind1)
                    Matrix<double> w;
                    w = Matrix<double>.Build.Dense(ind1.Count, 1, 0);
                    for (int i = 0; i < (int)ind1.Count; i++)
                    {
                        int index = ind1[i];
                        w[i, 0] = mvect_w[index];
                    }

                    // m1 = kchunk*w(ind1)
                    Matrix<double> m1;
                    m1 = svm_kernel.Multiply(w);

                    //y2(ind2) += m1;
                    for (int i = 0; i < (int)ind2.Count; i++)
                    {
                        int index = ind2[i];

                        int vale = (int)mvect_ypredict[index];
                        mvect_ypredict[index] += m1[i, 0];
                        vale = (int)mvect_ypredict[index];
                    }
                    /**/
                }
            }

            //Add w0
            for (int i = 0; i < (int) mvect_ypredict.Count; i++)
            {
                double debug = mvect_ypredict[i];
                if (i == 6312)
                {
                    debug ++;
                    double b = mvect_b[k_bias];
                    b++;
                }
                mvect_ypredict[i] += mvect_b[k_bias];
            }
        }

        private const string svm_tag = "SVMParams";
        private const string b_tag = "b";
        private const string w_tag = "w";
        private const string nbsv_tag = "nbsv";
        private const string bias_tag = "Bias";
        private const string weights_tag = "Support_Weights";
        private const string support_tag = "Support_Vectors";
        private const string bias_support_tag = "Support_Bias";
        private const string xsup_tag = "xsup";
        private const string feature_tag = "feature";

        public void LoadSVMFromXml()
        {
            int weight_count = 0;
            int feature_count = 0;
            int support_count = 0;

            /* Format
             * <SVMParams>
             *   <Bias>
             *     <b>double</b>
             *     <b>double</b>
             *     ...
             *   </Bias>
             *   <Support_Bias>
             *     <nbsv>double</double>
             *     <nbsv>double</double>
             *     ...
             *   </Support_Bias>
             *   <Support_Weights>
             *     <w>double</w>
             *     <w>double</w>
             *     ...
             *   </Support_Weights>
             *   <Support_Vectors>
             *     <xsup>
             *       <feature>double</double>
             *       <feature>double</double>
             *       ...
             *     </xsup>
             *     <xsup>
             *       <feature>double</double>
             *       <feature>double</double>
             *       ...
             *     </xsup>
             *     ...
             *   </Support_Vectors>
             * </SVMParams>
             */

            XmlReaderSettings rdrSettings = new XmlReaderSettings {IgnoreWhitespace = true,};
            using (XmlReader rdr = XmlReader.Create(new FileStream(mchar_svm_param_xml_file, FileMode.Open), rdrSettings))
            {
                rdr.MoveToContent();
                //start walking down the tree
                if (rdr.NodeType == XmlNodeType.Element && rdr.Name == svm_tag) //svm_params
                {
                    //at elements
                    rdr.ReadStartElement(); // Read the SVMParams tag, to get to the contents
                    //first bias
                    while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                    {
                        switch (rdr.Name)
                        {
                            case bias_tag:
                                ReadXmlBias(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case b_tag:
                                double b = rdr.ReadElementContentAsDouble();
                                mvect_b.Add(b);
                                break;
                            case bias_support_tag:
                                ReadXmlSupportBias(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case nbsv_tag:
                                double nbsv = rdr.ReadElementContentAsDouble();
                                mvect_nbsv.Add(nbsv);
                                break;
                            case weights_tag:
                                weight_count += ReadXmlSupportWeights(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case w_tag:
                                double w = rdr.ReadElementContentAsDouble();
                                mvect_w.Add(w);
                                weight_count++;
                                break;
                            case support_tag:
                                support_count += ReadXmlSupportVectors(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case xsup_tag:
                                rdr.Skip();
                                break;
                            default:
                                rdr.Skip();
                                break;
                        }
                    }
                }
            }
        }

        private void ReadXmlBias(XmlReader rdr)
        {
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == bias_tag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Bias tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case b_tag:
                            double b = rdr.ReadElementContentAsDouble();
                            mvect_b.Add(b);
                            break;
                        default:
                            rdr.Skip();
                            break;
                    }
                }
                if (!rdr.EOF)
                    rdr.ReadEndElement();
            }
            rdr.Close();
        }

        private void ReadXmlSupportBias(XmlReader rdr)
        {
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == bias_support_tag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Support_Bias tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case nbsv_tag:
                            double nbsv = rdr.ReadElementContentAsDouble();
                            mvect_nbsv.Add(nbsv);
                            break;
                        default:
                            rdr.Skip();
                            break;
                    }
                }
                if (!rdr.EOF)
                    rdr.ReadEndElement();
            }
            rdr.Close();
        }

        private int ReadXmlSupportWeights(XmlReader rdr)
        {
            int weight_count = 0;
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == weights_tag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Support_Weights tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case w_tag:
                            double w = rdr.ReadElementContentAsDouble();
                            mvect_w.Add(w);
                            weight_count++;
                            break;
                        default:
                            rdr.Skip();
                            break;
                    }
                }
                if (!rdr.EOF)
                    rdr.ReadEndElement();
            }
            rdr.Close();
            return weight_count;
        }

        private int ReadXmlSupportVectors(XmlReader rdr)
        {
            int feature_count = 0;
            int support_count = 0;
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == support_tag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Support_Vectors tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case xsup_tag:
                            feature_count += ReadXmlSupportVectorFeatures(rdr.ReadSubtree());
                            rdr.ReadEndElement();
                            mvect_xsup.Add(new FeatureList(mobj_support_features));
                            support_count++;
                            break;
                        default:
                            rdr.Skip();
                            break;
                    }
                }
                if (!rdr.EOF)
                    rdr.ReadEndElement();
            }
            rdr.Close();
            return support_count;
        }

        private int ReadXmlSupportVectorFeatures(XmlReader rdr)
        {
            int feature_count = 0;
            List<double> vect_xsup = new List<double>();
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == xsup_tag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the xsup tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case feature_tag:
                            double feature = rdr.ReadElementContentAsDouble();
                            vect_xsup.Add(feature);
                            feature_count++;
                            break;
                        default:
                            rdr.Skip();
                            break;
                    }
                }
                if (!rdr.EOF)
                    rdr.ReadEndElement();

                mobj_support_features.InitValues(vect_xsup);
            }
            rdr.Close();
            return feature_count;
        }
    }
}
#endif