#if Enable_Obsolete
using System;
using System.Collections.Generic;

namespace Engine.ChargeDetermination
{
    [Obsolete("Only used by Decon2LS.UI", false)]
    internal class FeatureList
    {
        private const int _numFeatures = 19;
        private readonly double[] _features = new double[_numFeatures];

        public FeatureList()
        {
            for (var i = 0; i < _numFeatures; i ++)
                _features[i] = 0;
        }

        public FeatureList(FeatureList a)
        {
            Array.Copy(a._features, _features, _numFeatures);
        }

        public void InitValues(List<double> features)
        {
            for (var i = 0; i < _numFeatures && i < features.Count; i++)
                _features[i] = features[i];
        }

        public int GetNumFeatures()
        {
            return _numFeatures;
        }

        public double GetValueAt(int index)
        {
            return _features[index];
        }

        public void SetValueAt(int index, double value)
        {
            _features[index] = value;
        }
    }
}
#endif