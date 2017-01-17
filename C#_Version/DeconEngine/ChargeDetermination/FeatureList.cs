using System;
using System.Collections.Generic;

namespace Engine.ChargeDetermination
{
    internal class FeatureList
    {
        public const int NumFeatures = 19;
        private readonly double[] _features = new double[NumFeatures];

        public FeatureList()
        {
            for (var i = 0; i < NumFeatures; i ++)
                _features[i] = 0;
        }

        public FeatureList(FeatureList a)
        {
            Array.Copy(a._features, _features, NumFeatures);
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="features">features, needs 19 points <see cref="NumFeatures"/></param>
        public FeatureList(IEnumerable<double> features)
        {
            var i = 0;
            foreach (var feature in features)
            {
                _features[i] = feature;
                i++;
                if (i >= 19)
                {
                    break;
                }
            }
        }

        public void InitValues(List<double> features)
        {
            for (var i = 0; i < NumFeatures && i < features.Count; i++)
                _features[i] = features[i];
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
