using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using DeconToolsV2.Peaks;
using MathNet.Numerics.LinearAlgebra;

namespace Engine.ChargeDetermination
{
    internal class SVMChargeDetermine
    {
        private const int NumFeatures = 19;
        private const int NumBins = 4;

        //Param file data and params
        private readonly List<double> _weights = new List<double>();
        private readonly List<double> _biases = new List<double>();
        private readonly List<double> _boundedSupportVectors = new List<double>();
        private Matrix<double> _vote;
        private Matrix<double> _discriminantScores;

        private readonly List<Engine.ChargeDetermination.FeatureList> _supportVectors = new List<FeatureList>();

        //Test and result
        private readonly List<Engine.ChargeDetermination.FeatureList> _testVector = new List<FeatureList>();

        private readonly List<double> _predictedY = new List<double>();
        private readonly double[] _minValues = new double[NumFeatures];
        private readonly double[] _maxValues = new double[NumFeatures];

        // Input scan
        private readonly List<double> _mzs = new List<double>();
        private readonly List<double> _intensities = new List<double>();
        private readonly int[] _peakDistribution = new int[NumBins];
        private readonly int[] _intensityDistribution = new int[NumBins];

        //For features
        private readonly Matrix<double> _meanCharge2 = Matrix<double>.Build.Dense(1, 2, 0);
        private readonly Matrix<double> _meanCharge3 = Matrix<double>.Build.Dense(1, 2, 0);
        private readonly Matrix<double> _c = Matrix<double>.Build.Dense(2, 2, 0);

        private const double MassCO = 27.9949141;
        private const double MassH2O = 18.0105633;
        private const double MassNH3 = 17.0265458;

        public SVMChargeDetermine()
        {
            for (var i = 0; i < NumBins; i++)
            {
                _peakDistribution[i] = 0;
                _intensityDistribution[i] = 0;
            }

            for (var i = 0; i < FeatureList.NumFeatures; i++)
            {
                _minValues[i] = double.MaxValue;
                _maxValues[i] = 0; //double.MinValue;
            }

            _boundedSupportVectors.Add(0);
        }

        public string SVMParamXmlPath { get; set; }

        private void InitVectors()
        {
            _intensities.Clear();
            _mzs.Clear();
            for (var i = 0; i < NumBins; i++)
            {
                _peakDistribution[i] = 0;
                _intensityDistribution[i] = 0;
            }
        }

        public void InitializeLDA()
        {
            //notation (row, col)
            //These values were acquired from training across 3 datasets - Shew, QC, Human_tipid
            _meanCharge2[0, 0] = 202.54;
            _meanCharge2[0, 1] = 0.31098;
            _meanCharge3[0, 0] = 254.84;
            _meanCharge3[0, 1] = 14.152;

            _c[0, 0] = 8422;
            _c[0, 1] = 102.4;
            _c[1, 0] = 102.4;
            _c[1, 1] = 23.746;
        }

        public bool IdentifyIfChargeOne(List<double> mzs, List<double> intensities, clsPeak parentPeak,
            int parentScan)
        {
            var numPeaks = mzs.Count;
            var parentMz = parentPeak.Mz;

            //Get input spectra
            InitVectors();
            for (var i = 0; i < numPeaks; i++)
            {
                _intensities.Add(intensities[i]);
                _mzs.Add(mzs[i]);
            }

            CalculatePeakProbabilities(parentMz, out var temp1, out var temp2, out var temp3, out var temp4);

            // If all frag peaks lie in first bin [0 - parent_Mz]
            if (_peakDistribution[0] > 0.9 * numPeaks)
                return true;
            else
                return false;
        }

        public void GetFisherScores(out double fscore2, out double fscore3)
        {
            var totalPeaks = 0d;

            for (var i = 0; i < NumBins; i++)
                totalPeaks += _peakDistribution[i];

            var prob = new double[2];
            prob[0] = _peakDistribution[1] / totalPeaks;
            prob[1] = _peakDistribution[2] / totalPeaks;

            var x = Matrix<double>.Build.Dense(1, 2, 0);

            x[0, 0] = _peakDistribution[1];
            x[0, 1] = _peakDistribution[2];

            var x_T = x.Transpose();
            var invC = _c.Inverse();
            var m1 = _meanCharge2.Multiply(invC);
            var m2 = _meanCharge2.Transpose();
            var first_term2 = m1.Multiply(x_T);
            var second_term2 = m1.Multiply(m2);

            fscore2 = first_term2[0, 0] - 0.5 * second_term2[0, 0] + prob[0];

            var m3 = _meanCharge3.Multiply(invC);
            var m4 = _meanCharge3.Transpose();
            var first_term3 = m3.Multiply(x_T);
            var second_term3 = m3.Multiply(m4);

            fscore3 = first_term3[0, 0] - 0.5 * second_term3[0, 0] + prob[1];
        }

        public void GetFeaturesForSpectra(List<double> mzs, List<double> intensities, clsPeak parentPeak,
            int msNscan)
        {
            double xscore_ratio;

            var numPeaks = mzs.Count;

            var noLoss = 0d;
            var parentMz = parentPeak.Mz;

            //Get input spectra
            InitVectors();
            for (var i = 0; i < numPeaks; i++)
            {
                _intensities.Add(intensities[i]);
                _mzs.Add(mzs[i]);
            }

            //Start with feature detection
            CalculatePeakProbabilities(parentMz, out var pk1_prob, out var pk2_prob, out var pk3_prob, out var pk4_prob);
            GetFisherScores(out var fscore2, out var fscore3);
            NormalizeSpectra();
            GetXScores(parentMz, out var xscore2, out var xscore3, noLoss);
            if (!xscore2.Equals(0))
                xscore_ratio = xscore3 / xscore2;
            else
                xscore_ratio = 0;
            GetBScores(parentMz, out var bscore2, out var bscore3);
            GetXScores(parentMz, out var xscore2_CO, out var xscore3_CO, MassCO);
            GetXScores(parentMz, out var xscore2_H2O, out var xscore3_H2O, MassH2O);
            GetXScores(parentMz, out var xscore2_NH3, out var xscore3_NH3, MassNH3);

            ReadValues(msNscan, parentMz, xscore2, xscore3, xscore_ratio, bscore2, bscore3, pk1_prob,
                pk2_prob, pk3_prob, pk4_prob, fscore2, fscore3, xscore2_CO, xscore3_CO, xscore2_H2O,
                xscore3_H2O, xscore2_NH3, xscore3_NH3);
        }

        private void ReadValues(int scanNum, double parentMz, double xScore2, double xScore3, double xScoreRatio,
            double bScore2, double bScore3, double pk1Prob, double pk2Prob, double pk3Prob,
            double pk4Prob, double fscore2, double fscore3, double xScore2_CO, double xScore3_CO, double xScore2_H2O,
            double xScore3_H20, double xScore2_NH3, double xScore3_NH3)
        {
            var features = new List<double>
            {
                scanNum,
                parentMz,
                xScore2,
                xScore3,
                xScoreRatio,
                bScore2,
                bScore3,
                pk1Prob,
                pk2Prob,
                pk3Prob,
                pk4Prob,
                fscore2,
                fscore3,
                xScore2_CO,
                xScore3_CO,
                xScore2_H2O,
                xScore3_H20,
                xScore2_NH3,
                xScore3_NH3
            };

            var scanFeatures = new FeatureList(features);
            _testVector.Add(scanFeatures);

            for (var i = 0; i < NumFeatures; i++)
            {
                var val = features[i];
                if (val <= _minValues[i])
                    _minValues[i] = val;
                if (val >= _maxValues[i])
                    _maxValues[i] = val;
            }
        }

        private void NormalizeSpectra()
        {
            const double threshold = 5;
            double median;
            var size = _intensities.Count;
            var sortIntensities = new List<double>();

            for (var i = 0; i < size; i++)
            {
                sortIntensities.Add(_intensities[i]);
            }

            sortIntensities.Sort();

            if (size % 2 == 0)
                median = sortIntensities[size / 2];
            else
                median = (sortIntensities[(size - 1) / 2] + sortIntensities[(size + 1) / 2]) / 2;

            for (var i = 0; i < size; i++)
            {
                var val = _intensities[i] / median;
                if (val > threshold)
                    _intensities[i] = val;
                else
                    _intensities[i] = 0;
            }
        }

        private void GetXScores(double parentMz, out double xscore2, out double xscore3, double neutralLoss)
        {
            parentMz = parentMz - neutralLoss;
            xscore2 = 0;
            xscore3 = 0;

            for (var i = 0; i < parentMz; i++)
            {
                //for xscore2
                var SumMForwardCS2 = 0d;
                var mzForwardBeginCS2 = (parentMz + i) - 0.5;
                var mzForwardEndCS2 = (parentMz + i) + 0.5;

                //for xscore3
                var SumMForwardCS3_1 = 0d;
                var SumMForwardCS3_2 = 0d;
                var mzForwardBeginCS3_1 = (3 * parentMz - 2 * i) - 0.5;
                var mzForwardEndCS3_1 = (3 * parentMz - 2 * i) + 0.5;
                var mzForwardBeginCS3_2 = (3 * parentMz - i) / 2 - 0.5;
                var mzForwardEndCS3_2 = (3 * parentMz - i) / 2 + 0.5;

                //for both
                var SumMReverse = 0d;
                var mzReverseBegin = (parentMz - i) - 0.5;
                var mzReverseEnd = (parentMz - i) + 0.5;

                for (var j = 0; j < (int) _mzs.Count; j++)
                {
                    var mz = _mzs[j];
                    if (mz >= mzForwardBeginCS2 && mz <= mzForwardEndCS2)
                        SumMForwardCS2 += (_intensities[j]);
                    if (mz >= mzForwardBeginCS3_1 && mz <= mzForwardEndCS3_1)
                        SumMForwardCS3_1 += (_intensities[j]);
                    if (mz >= mzForwardBeginCS3_2 && mz <= mzForwardEndCS3_2)
                        SumMForwardCS3_2 += (_intensities[j]);
                    if (mz >= mzReverseBegin && mz <= mzReverseEnd)
                        SumMReverse += (_intensities[j]);
                }
                xscore2 += (SumMForwardCS2 * SumMReverse);
                xscore3 += ((SumMForwardCS3_1 * SumMReverse) + (SumMForwardCS3_2 * SumMReverse));
            }
        }

        public double GetScoreAtScanIndex(int index)
        {
            return _discriminantScores[index, 3];
        }

        public int GetClassAtScanIndex(int index)
        {
            //return values 1 - +1, 2 - +2, 3 - +3, 4 - +4, 0 - +2 or +3
            var numClass = _vote.ColumnCount;
            var charge = 0;
            for (var col = 0; col < numClass; col++)
            {
                if (_vote[index, col].Equals(3))
                {
                    charge = col + 1;
                }
            }
            if (charge == 2 || charge == 3)
            {
                // As column 3 gives score between 2 and 3
                var val = _discriminantScores[index, 3];
                if (val > -5 && val < 2.5) //these thresholds were found using the score distribution curves
                    charge = 0;
            }
            return charge;
        }

        public void ClearMemory()
        {
            // clearing the matrices/ vectors (separate from the destructor as
            // initialization is done separetely from the constructor)
            if (_biases.Count != 0)
                _biases.Clear();
            if (_weights.Count != 0)
                _weights.Clear();
            if (_supportVectors.Count != 0)
                _supportVectors.Clear();
            if (_testVector.Count != 0)
                _testVector.Clear();
            if (_predictedY.Count != 0)
                _predictedY.Clear();
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void ResolveIntoClass()
        {
            var numRows = _vote.RowCount;
            var numCols = _vote.ColumnCount;
            for (var row = 0; row < numRows; row++)
            {
                var charge = 1;
                for (var col = 0; col < numCols; col++)
                {
                    if (_vote[row, col].Equals(3))
                    {
                        charge = col + 1;
                        break;
                    }
                    _predictedY[row] = charge;
                }
            }
        }

        private void GetBScores(double parent_Mz, out double bscore2, out double bscore3)
        {
            bscore2 = 0;
            bscore3 = 0;

            double sumLeft = 0;
            double sumRightCS2 = 0;
            double sumRightCS3 = 0;
            double sumTotalCS2 = 0;
            double sumTotalCS3 = 0;

            for (var i = 0; i < (int) _mzs.Count; i++)
            {
                var mz = _mzs[i];
                if (mz < parent_Mz - 1)
                    sumLeft += _intensities[i];
                if (mz > parent_Mz + 1 && mz <= (2 * parent_Mz))
                    sumRightCS2 += _intensities[i];
                if (mz > parent_Mz + 1 && mz <= (3 * parent_Mz))
                    sumRightCS3 += _intensities[i];
                if (mz <= (2 * parent_Mz))
                    sumTotalCS2 += _intensities[i];
                if (mz <= (3 * parent_Mz))
                    sumTotalCS3 += _intensities[i];
            }

            if (sumTotalCS2 > 0)
                bscore2 = (sumLeft - sumRightCS2) / sumTotalCS2;
            if (sumTotalCS3 > 0)
                bscore3 = (sumLeft - sumRightCS3) / sumTotalCS3;

            bscore2 = Math.Abs(bscore2);
            bscore3 = Math.Abs(bscore3);
        }

        private void CalculatePeakProbabilities(double parentMz, out double pk1, out double pk2, out double pk3,
            out double pk4)
        {
            var numPeaks = _intensities.Count;
            var sum = 0;

            for (var i = 1; i < numPeaks - 1; i++)
            {
                var mz = _mzs[i];
                if (mz > 0 && mz <= parentMz)
                {
                    _peakDistribution[0]++;
                    _intensityDistribution[0] += (int) _intensities[i];
                }
                if (mz > parentMz && mz <= (2 * parentMz))
                {
                    _peakDistribution[1]++;
                    _intensityDistribution[1] += (int) _intensities[i];
                }
                if (mz > (2 * parentMz) && mz <= (3 * parentMz))
                {
                    _peakDistribution[2]++;
                    _intensityDistribution[2] += (int) _intensities[i];
                }
                if (mz > (3 * parentMz) && mz < (4 * parentMz))
                {
                    _peakDistribution[3]++;
                    _intensityDistribution[3] += (int) _intensities[i];
                }
            }

            for (var i = 0; i < NumBins; i++)
            {
                sum += _peakDistribution[i];
            }

            pk1 = _peakDistribution[0] / (double) sum;
            pk2 = _peakDistribution[1] / (double) sum;
            pk3 = _peakDistribution[2] / (double) sum;
            pk4 = _peakDistribution[3] / (double) sum;
        }

        public void NormalizeDataSet()
        {
            // Normalize Data Set
            var numVectors = _testVector.Count;

            if (numVectors < 2)
                return;

            for (var vectorNum = 0; vectorNum < numVectors; vectorNum++)
            {
                var this_test_vector = _testVector[vectorNum];

                for (var featureNum = 0; featureNum < NumFeatures; featureNum++)
                {
                    var val = this_test_vector.GetValueAt(featureNum);
                    if (_maxValues[featureNum] > _minValues[featureNum])
                        val = (val - _minValues[featureNum]) /
                              (_maxValues[featureNum] - _minValues[featureNum]);
                    else
                        val = 0;

                    this_test_vector.SetValueAt(featureNum, val);
                }
            }
        }

        private static Matrix<double> GetKernel(List<FeatureList> testVectors,
            List<FeatureList> supportVectors)
        {
            var numVectors = supportVectors.Count;
            var numTest = testVectors.Count;

            //get vectors
            var xsup = Matrix<double>.Build.Dense(numVectors, NumFeatures, 0);
            for (var i = 0; i < numVectors; i++)
            {
                var thisSupportVector = supportVectors[i];
                for (var j = 0; j < NumFeatures; j++)
                {
                    xsup[i, j] = thisSupportVector.GetValueAt(j);
                }
            }

            var x = Matrix<double>.Build.Dense(Math.Max(numTest, 1), NumFeatures, 0); // num_test == 0 causes an exception...
            for (var i = 0; i < numTest; i++)
            {
                var thisTestVector = testVectors[i];
                for (var j = 0; j < NumFeatures; j++)
                {
                    x[i, j] = thisTestVector.GetValueAt(j);
                }
            }

            //Init metric
            var metric = Matrix<double>.Build.Dense(NumFeatures, NumFeatures, 0);
            for (var i = 0; i < NumFeatures; i++)
                metric[i, i] = 1;

            // Get ps
            var m1 = x.Multiply(metric);
            var xsup_t = xsup.Transpose();

            var ps = m1.Multiply(xsup_t);
            var ps_rows = ps.RowCount; //num_test
            var ps_cols = ps.ColumnCount; //num_support

            // Get norms
            var norm_x = Matrix<double>.Build.Dense(ps_rows, ps_cols, 0);
            var norm_xsup = Matrix<double>.Build.Dense(ps_cols, ps_rows, 0);

            var x2 = x.PointwiseMultiply(x);
            var m2 = x2.Multiply(metric);

            for (var rowNum = 0; rowNum < m2.RowCount; rowNum++)
            {
                double sumX = 0;
                for (var colNum = 0; colNum < m2.ColumnCount; colNum++)
                {
                    sumX = sumX + m2[rowNum, colNum];
                }
                //This is for ease of matrix addition
                for (var colNum = 0; colNum < norm_x.ColumnCount; colNum++)
                    norm_x[rowNum, colNum] = sumX;
            }

            var xsup2 = xsup.PointwiseMultiply(xsup);
            var m3 = xsup2.Multiply(metric);
            for (var rowNum = 0; rowNum < m3.RowCount; rowNum++)
            {
                double sumXsup = 0;
                for (var colNum = 0; colNum < m3.ColumnCount; colNum++)
                {
                    sumXsup = sumXsup + m3[rowNum, colNum];
                }

                for (var colNum = 0; colNum < norm_xsup.ColumnCount; colNum++)
                    norm_xsup[rowNum, colNum] = sumXsup;
            }

            var norm_xsup_t = norm_xsup.Transpose();

            const double scale = -2.0;
            var psTemp = ps.Multiply(scale);
            var ps2 = psTemp.Add(norm_x);
            var ps3 = ps2.Add(norm_xsup_t);
            ps = ps3.Multiply(1 / scale);

            var kernel = Matrix<double>.Build.Dense(ps.RowCount, ps.ColumnCount, 0);

            for (var rowNum = 0; rowNum < ps.RowCount; rowNum++)
            {
                for (var colNum = 0; colNum < ps.ColumnCount; colNum++)
                {
                    kernel[rowNum, colNum] = Math.Exp(ps[rowNum, colNum]);
                }
            }

            return kernel;
        }

        public void DetermineClassForDataSet()
        {
            var k = 0;
            const int numIterations = 6;
            var iterationNumber = 0;

            var numClass = 4; //(int)(1 + (int)(Math.Sqrt(1+4*2*_boundedSupportVectors.Count)))/2;
            var numTest = _testVector.Count;
            _vote = Matrix<double>.Build.Dense(Math.Max(numTest, 1), numClass, 0); // num_test == 0 causes an exception...
            _discriminantScores = Matrix<double>.Build.Dense(Math.Max(numTest, 1), numIterations, 0); // num_test == 0 causes an exception...
            var cumSum = CalculateCumSum();

            for (var row = 0; row < _testVector.Count; row++)
                _predictedY.Add(0);

            for (var row = 0; row < _vote.RowCount && _testVector.Count > 0; row++)
            {
                for (var col = 0; col < _vote.ColumnCount; col++)
                {
                    _vote[row, col] = 0;
                }
                for (var col = 0; col < _discriminantScores.ColumnCount; col++)
                {
                    _discriminantScores[row, col] = 0;
                }
            }

            for (var i = 0; i < numClass; i++)
            {
                for (var j = i + 1; j < numClass; j++)
                {
                    var startIndexToConsider = (int) cumSum[k];
                    var stopIndexToConsider = (int) (cumSum[k] + _boundedSupportVectors[k + 1]) - 1;
                    SVMClassification(startIndexToConsider, stopIndexToConsider, k);
                    //for (int row = 0; row < mmat_vote.RowCount; row++)
                    for (var row = 0; row < numTest; row++)
                    {
                        var val = _predictedY[row];

                        _discriminantScores[row, iterationNumber] = val;

                        _predictedY[row] = 0;
                        if (val >= 0)
                        {
                            _vote[row, i]++;
                        }
                        else
                        {
                            _vote[row, j]++;
                        }
                    }
                    k++;
                    iterationNumber++;
                }
            }
        }

        private List<double> CalculateCumSum()
        {
            var cumSum = new List<double>();
            for (var i = 0; i < _boundedSupportVectors.Count; i++)
            {
                var sum = 0d;
                for (var j = 0; j <= i; j++)
                {
                    sum += _boundedSupportVectors[j];
                }
                cumSum.Add(sum);
            }
            return cumSum;
        }

        public void SVMClassification(int startIndex, int stopIndex, int k_bias)
        {
            var chunk_xtest = new List<FeatureList>();
            var chunk_xsup = new List<FeatureList>();

            var numSupport = stopIndex - startIndex;
            var numTest = _testVector.Count;
            const int chunksize = 100;
            var ind1 = new List<int>();
            var ind2 = new List<int>();

            var chunks1 = (numSupport / chunksize) + 1;
            var chunks2 = (numTest / chunksize) + 1;

            // Performing y2(ind2)=y2(ind2)+ kchunk*w(ind1);
            for (var ch1 = 1; ch1 <= chunks1; ch1++)
            {
                //Get ind1
                var low_ind1_index = (ch1 - 1) * chunksize + startIndex;
                var high_ind1_index = (ch1 * chunksize) - 1 + startIndex;
                if (high_ind1_index > stopIndex)
                    high_ind1_index = stopIndex;
                ind1.Clear();
                for (var index = 0; index <= (high_ind1_index - low_ind1_index); index++)
                    ind1.Add(index + low_ind1_index);

                //Get support vectors
                chunk_xsup.Clear();
                for (var j = 0; j < ind1.Count; j++)
                {
                    var xsupIndex = ind1[j];
                    var thisSupportVector = new FeatureList(_supportVectors[xsupIndex]);
                    chunk_xsup.Add(thisSupportVector);
                }

                for (var ch2 = 1; ch2 <= chunks2; ch2++)
                {
                    //Get ind2
                    var low_ind2_index = (ch2 - 1) * chunksize;
                    var high_ind2_index = (ch2 * chunksize) - 1;
                    if (high_ind2_index > numTest)
                        high_ind2_index = numTest - 1;
                    ind2.Clear();
                    for (var index2 = 0; index2 <= (high_ind2_index - low_ind2_index); index2++)
                        ind2.Add(index2 + low_ind2_index);

                    //Get X vector
                    chunk_xtest.Clear();
                    for (var j = 0; j < ind2.Count; j++)
                    {
                        var xIndex = ind2[j];
                        var thisTestVector = _testVector[xIndex];
                        chunk_xtest.Add(new FeatureList(thisTestVector));
                    }

                    //Get the kernel
                    var svmKernel = GetKernel(chunk_xtest, chunk_xsup);

                    //Read in the weights w(ind1)
                    var w = Matrix<double>.Build.Dense(ind1.Count, 1, 0);
                    for (var i = 0; i < ind1.Count; i++)
                    {
                        var index = ind1[i];
                        w[i, 0] = _weights[index];
                    }

                    // m1 = kchunk*w(ind1)
                    var m1 = svmKernel.Multiply(w);

                    //y2(ind2) += m1;
                    for (var i = 0; i < ind2.Count; i++)
                    {
                        var index = ind2[i];
                        _predictedY[index] += m1[i, 0];
                    }
                }
            }

            //Add w0
            for (var i = 0; i < _predictedY.Count; i++)
            {
                _predictedY[i] += _biases[k_bias];
            }
        }

        private const string XmlSvmTag = "SVMParams";
        private const string XmlBTag = "b";
        private const string XmlWTag = "w";
        private const string XmlNbsvTag = "nbsv";
        private const string XmlBiasTag = "Bias";
        private const string XmlWeightsTag = "Support_Weights";
        private const string XmlSupportTag = "Support_Vectors";
        private const string XmlBiasSupportTag = "Support_Bias";
        private const string XmlXsupTag = "xsup";
        private const string XmlFeatureTag = "feature";

        public void LoadSVMFromXml()
        {
            if (File.Exists(SVMParamXmlPath))
            {
                ReadXmlFromStream(new FileStream(SVMParamXmlPath, FileMode.Open, FileAccess.Read, FileShare.Read));
            }
            else
            {
                LoadDefaultSVM();
            }
        }

        private void ReadXmlFromStream(Stream stream)
        {
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

            var rdrSettings = new XmlReaderSettings {IgnoreWhitespace = true,};
            using (var rdr = XmlReader.Create(stream, rdrSettings))
            {
                rdr.MoveToContent();
                //start walking down the tree
                if (rdr.NodeType == XmlNodeType.Element && rdr.Name == XmlSvmTag) //svm_params
                {
                    //at elements
                    rdr.ReadStartElement(); // Read the SVMParams tag, to get to the contents
                    //first bias
                    while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                    {
                        switch (rdr.Name)
                        {
                            case XmlBiasTag:
                                ReadXmlBias(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case XmlBTag:
                                _biases.Add(rdr.ReadElementContentAsDouble());
                                break;
                            case XmlBiasSupportTag:
                                ReadXmlSupportBias(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case XmlNbsvTag:
                                _boundedSupportVectors.Add(rdr.ReadElementContentAsDouble());
                                break;
                            case XmlWeightsTag:
                                ReadXmlSupportWeights(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case XmlWTag:
                                _weights.Add(rdr.ReadElementContentAsDouble());
                                break;
                            case XmlSupportTag:
                                ReadXmlSupportVectors(rdr.ReadSubtree());
                                rdr.ReadEndElement();
                                break;
                            case XmlXsupTag:
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
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == XmlBiasTag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Bias tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case XmlBTag:
                            _biases.Add(rdr.ReadElementContentAsDouble());
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
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == XmlBiasSupportTag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Support_Bias tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case XmlNbsvTag:
                            _boundedSupportVectors.Add(rdr.ReadElementContentAsDouble());
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

        private void ReadXmlSupportWeights(XmlReader rdr)
        {
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == XmlWeightsTag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Support_Weights tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case XmlWTag:
                            _weights.Add(rdr.ReadElementContentAsDouble());
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

        private void ReadXmlSupportVectors(XmlReader rdr)
        {
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == XmlSupportTag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the Support_Vectors tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case XmlXsupTag:
                            ReadXmlSupportVectorFeatures(rdr.ReadSubtree(), out var supportFeatures);
                            rdr.ReadEndElement();
                            _supportVectors.Add(supportFeatures);
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

        private void ReadXmlSupportVectorFeatures(XmlReader rdr, out FeatureList supportFeatures)
        {
            var featurePoints = new List<double>();
            rdr.MoveToContent();
            //start walking down the tree
            if (rdr.NodeType == XmlNodeType.Element && rdr.Name == XmlXsupTag)
            {
                //at elements
                rdr.ReadStartElement(); // Read the xsup tag, to get to the contents
                //first bias
                while (rdr.NodeType != XmlNodeType.EndElement && !rdr.EOF)
                {
                    switch (rdr.Name)
                    {
                        case XmlFeatureTag:
                            featurePoints.Add(rdr.ReadElementContentAsDouble());
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

            supportFeatures = new FeatureList(featurePoints);
        }

        public void LoadDefaultSVM()
        {
            System.Console.WriteLine("Could not find svm param file \"{0}\"; Loading embedded defaults.", SVMParamXmlPath);
            var assembly = System.Reflection.Assembly.GetExecutingAssembly();
            ReadXmlFromStream(assembly.GetManifestResourceStream("DeconEngine.svm_params.xml"));
            using (var fileReader = new StreamReader(assembly.GetManifestResourceStream("DeconEngine.svm_params.xml")))
            using (var writer = new StreamWriter(new FileStream(SVMParamXmlPath, FileMode.Create, FileAccess.ReadWrite, FileShare.ReadWrite))
            )
            {
                try
                {
                    writer.Write(fileReader.ReadToEnd());
                }
                catch (Exception)
                {
                    // Swallow it, it doesn't matter
                }
            }
        }
    }
}
