using System;

namespace DeconToolsV2
{
    public struct ElementIsotopeData
    {
        public double Mass { get; private set; }
        public double Probability { get; private set; }

        public ElementIsotopeData(double mass, double probability) : this()
        {
            Mass = mass;
            Probability = probability;
        }
    }

    public class ElementData
    {
        public const int MaxTagLength = 256;

        public const int NumElements = 103;
        public const double ElectronMass = 0.00054858;
        public int Atomicity;
        public double AverageMass;

        /// <summary>
        ///     Tin has 10 isotopes!!! Rather deal with extra fragmented memory than put separate allocations on heap each time.
        ///     especially because the number of elements is not changing any time soon till the aliens take over.
        /// </summary>
        //public double[] IsotopeMasses = new double[10];
        //public double[] IsotopeProbabilities = new double[10];
        public ElementIsotopeData[] Isotopes = new ElementIsotopeData[10];

        public double MassVariance;
        public string Name = "";

        public int NumberOfIsotopes;

        // Elemental symbol
        public string Symbol = "";

        public ElementData()
        {
        }

        public ElementData(ElementData a)
        {
            Symbol = a.Symbol;
            Name = a.Name;
            NumberOfIsotopes = a.NumberOfIsotopes;
            Atomicity = a.Atomicity;
            AverageMass = a.AverageMass;
            MassVariance = a.MassVariance;

            Isotopes = new ElementIsotopeData[NumberOfIsotopes];
            Array.Copy(a.Isotopes, Isotopes, NumberOfIsotopes);
        }
    }
}