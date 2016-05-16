using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;

namespace Engine.TheoreticalProfile
{
    internal class ElementData
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
        public double[] IsotopeMasses = new double[10];

        public double[] IsotopeProbabilities = new double[10];
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
            for (var i = 0; i < NumberOfIsotopes; i++)
            {
                IsotopeMasses[i] = a.IsotopeMasses[i];
                IsotopeProbabilities[i] = a.IsotopeProbabilities[i];
            }
            AverageMass = a.AverageMass;
            MassVariance = a.MassVariance;
        }
    }

    internal class AtomicInformation
    {
        private const string XmlElementsTag = "elements";
        private const string XmlElementTag = "element";
        private const string XmlNameTag = "name";
        private const string XmlSymbolTag = "symbol";
        private const string XmlAtomicityTag = "atomicity";
        private const string XmlNumIsotopesTag = "num_isotopes";
        private const string XmlIsotopesTag = "isotopes";
        private const string XmlIsotopeTag = "isotope";
        private const string XmlMassTag = "mass";
        private const string XmlProbabilityTag = "probability";
        public List<ElementData> ElementalIsotopesList = new List<ElementData>();
#if !Disable_Obsolete
        internal const string DefaultFileName = "Isotope.xml";
#endif

        public AtomicInformation()
        {
            SetDefaultIsotopeDistribution();
        }

        // Copy the given atomic information into a new copy
        public AtomicInformation(AtomicInformation a)
        {
            ElementalIsotopesList.AddRange(a.ElementalIsotopesList);
        }

        private void SetDefaultIsotopeDistribution()
        {
            var isotopes = new ElementData();
            isotopes.Symbol = "H";
            isotopes.Name = "Hydrogen";

            isotopes.AverageMass = 1.007976;
            isotopes.MassVariance = 0.000152;

            isotopes.Atomicity = 1;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 1.007825;
            isotopes.IsotopeProbabilities[0] = 0.999850;
            isotopes.IsotopeMasses[1] = 2.014102;
            isotopes.IsotopeProbabilities[1] = 0.000150;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "He";
            isotopes.Name = "Helium";

            isotopes.AverageMass = 4.002599;
            isotopes.MassVariance = 0.000001;

            isotopes.Atomicity = 2;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 3.016030;
            isotopes.IsotopeProbabilities[0] = 0.000001;
            isotopes.IsotopeMasses[1] = 4.002600;
            isotopes.IsotopeProbabilities[1] = 0.999999;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Li";
            isotopes.Name = "Lithium";

            isotopes.AverageMass = 6.940937;
            isotopes.MassVariance = 0.069497;

            isotopes.Atomicity = 3;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 6.015121;
            isotopes.IsotopeProbabilities[0] = 0.075000;
            isotopes.IsotopeMasses[1] = 7.016003;
            isotopes.IsotopeProbabilities[1] = 0.925000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Be";
            isotopes.Name = "Berellium";

            isotopes.AverageMass = 9.012182;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 4;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 9.012182;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "B";
            isotopes.Name = "Boron";

            isotopes.AverageMass = 10.811028;
            isotopes.MassVariance = 0.158243;

            isotopes.Atomicity = 5;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 10.012937;
            isotopes.IsotopeProbabilities[0] = 0.199000;
            isotopes.IsotopeMasses[1] = 11.009305;
            isotopes.IsotopeProbabilities[1] = 0.801000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "C";
            isotopes.Name = "Carbon";

            isotopes.AverageMass = 12.011107;
            isotopes.MassVariance = 0.011021;

            isotopes.Atomicity = 6;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 12.000000;
            isotopes.IsotopeProbabilities[0] = 0.988930;
            isotopes.IsotopeMasses[1] = 13.003355;
            isotopes.IsotopeProbabilities[1] = 0.011070;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "N";
            isotopes.Name = "Nitrogen";

            isotopes.AverageMass = 14.006724;
            isotopes.MassVariance = 0.003628;

            isotopes.Atomicity = 7;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 14.003072;
            isotopes.IsotopeProbabilities[0] = 0.996337;
            isotopes.IsotopeMasses[1] = 15.000109;
            isotopes.IsotopeProbabilities[1] = 0.003663;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "O";
            isotopes.Name = "Oxygen";

            isotopes.AverageMass = 15.999370;
            isotopes.MassVariance = 0.008536;

            isotopes.Atomicity = 8;
            isotopes.NumberOfIsotopes = 3;

            isotopes.IsotopeMasses[0] = 15.994914;
            isotopes.IsotopeProbabilities[0] = 0.997590;
            isotopes.IsotopeMasses[1] = 16.999132;
            isotopes.IsotopeProbabilities[1] = 0.000374;
            isotopes.IsotopeMasses[2] = 17.999162;
            isotopes.IsotopeProbabilities[2] = 0.002036;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "F";
            isotopes.Name = "Fluorine";

            isotopes.AverageMass = 18.998403;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 9;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 18.998403;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ne";
            isotopes.Name = "Neon";

            isotopes.AverageMass = 20.180041;
            isotopes.MassVariance = 0.337122;

            isotopes.Atomicity = 10;
            isotopes.NumberOfIsotopes = 3;

            isotopes.IsotopeMasses[0] = 19.992435;
            isotopes.IsotopeProbabilities[0] = 0.904800;
            isotopes.IsotopeMasses[1] = 20.993843;
            isotopes.IsotopeProbabilities[1] = 0.002700;
            isotopes.IsotopeMasses[2] = 21.991383;
            isotopes.IsotopeProbabilities[2] = 0.092500;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Na";
            isotopes.Name = "Sodium";

            isotopes.AverageMass = 22.989767;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 11;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 22.989767;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Mg";
            isotopes.Name = "Magnesium";

            isotopes.AverageMass = 24.305052;
            isotopes.MassVariance = 0.437075;

            isotopes.Atomicity = 12;
            isotopes.NumberOfIsotopes = 3;

            isotopes.IsotopeMasses[0] = 23.985042;
            isotopes.IsotopeProbabilities[0] = 0.789900;
            isotopes.IsotopeMasses[1] = 24.985837;
            isotopes.IsotopeProbabilities[1] = 0.100000;
            isotopes.IsotopeMasses[2] = 25.982593;
            isotopes.IsotopeProbabilities[2] = 0.110100;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Al";
            isotopes.Name = "Almunium";

            isotopes.AverageMass = 26.981539;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 13;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 26.981539;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Si";
            isotopes.Name = "Silicon";

            isotopes.AverageMass = 28.085509;
            isotopes.MassVariance = 0.158478;

            isotopes.Atomicity = 14;
            isotopes.NumberOfIsotopes = 3;

            isotopes.IsotopeMasses[0] = 27.976927;
            isotopes.IsotopeProbabilities[0] = 0.922300;
            isotopes.IsotopeMasses[1] = 28.976495;
            isotopes.IsotopeProbabilities[1] = 0.046700;
            isotopes.IsotopeMasses[2] = 29.973770;
            isotopes.IsotopeProbabilities[2] = 0.031000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "P";
            isotopes.Name = "Phosphrous";

            isotopes.AverageMass = 30.973762;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 15;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 30.973762;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "S";
            isotopes.Name = "Sulphur";

            isotopes.AverageMass = 32.064387;
            isotopes.MassVariance = 0.169853;

            isotopes.Atomicity = 16;
            isotopes.NumberOfIsotopes = 4;

            isotopes.IsotopeMasses[0] = 31.972070;
            isotopes.IsotopeProbabilities[0] = 0.950200;
            isotopes.IsotopeMasses[1] = 32.971456;
            isotopes.IsotopeProbabilities[1] = 0.007500;
            isotopes.IsotopeMasses[2] = 33.967866;
            isotopes.IsotopeProbabilities[2] = 0.042100;
            isotopes.IsotopeMasses[3] = 35.967080;
            isotopes.IsotopeProbabilities[3] = 0.000200;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Cl";
            isotopes.Name = "Chlorine";

            isotopes.AverageMass = 35.457551;
            isotopes.MassVariance = 0.737129;

            isotopes.Atomicity = 17;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 34.968853;
            isotopes.IsotopeProbabilities[0] = 0.755290;
            isotopes.IsotopeMasses[1] = 36.965903;
            isotopes.IsotopeProbabilities[1] = 0.244710;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ar";
            isotopes.Name = "Argon";

            isotopes.AverageMass = 39.947662;
            isotopes.MassVariance = 0.056083;

            isotopes.Atomicity = 18;
            isotopes.NumberOfIsotopes = 3;

            isotopes.IsotopeMasses[0] = 35.967545;
            isotopes.IsotopeProbabilities[0] = 0.003370;
            isotopes.IsotopeMasses[1] = 37.962732;
            isotopes.IsotopeProbabilities[1] = 0.000630;
            isotopes.IsotopeMasses[2] = 39.962384;
            isotopes.IsotopeProbabilities[2] = 0.996000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "K";
            isotopes.Name = "Potassium";

            isotopes.AverageMass = 39.098301;
            isotopes.MassVariance = 0.250703;

            isotopes.Atomicity = 19;
            isotopes.NumberOfIsotopes = 3;

            isotopes.IsotopeMasses[0] = 38.963707;
            isotopes.IsotopeProbabilities[0] = 0.932581;
            isotopes.IsotopeMasses[1] = 39.963999;
            isotopes.IsotopeProbabilities[1] = 0.000117;
            isotopes.IsotopeMasses[2] = 40.961825;
            isotopes.IsotopeProbabilities[2] = 0.067302;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ca";
            isotopes.Name = "Calcium";

            isotopes.AverageMass = 40.078023;
            isotopes.MassVariance = 0.477961;

            isotopes.Atomicity = 20;
            isotopes.NumberOfIsotopes = 6;

            isotopes.IsotopeMasses[0] = 39.962591;
            isotopes.IsotopeProbabilities[0] = 0.969410;
            isotopes.IsotopeMasses[1] = 41.958618;
            isotopes.IsotopeProbabilities[1] = 0.006470;
            isotopes.IsotopeMasses[2] = 42.958766;
            isotopes.IsotopeProbabilities[2] = 0.001350;
            isotopes.IsotopeMasses[3] = 43.955480;
            isotopes.IsotopeProbabilities[3] = 0.020860;
            isotopes.IsotopeMasses[4] = 45.953689;
            isotopes.IsotopeProbabilities[4] = 0.000040;
            isotopes.IsotopeMasses[5] = 47.952533;
            isotopes.IsotopeProbabilities[5] = 0.001870;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Sc";
            isotopes.Name = "Scandium";

            isotopes.AverageMass = 44.955910;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 21;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 44.955910;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ti";
            isotopes.Name = "Titanium";

            isotopes.AverageMass = 47.878426;
            isotopes.MassVariance = 0.656425;

            isotopes.Atomicity = 22;
            isotopes.NumberOfIsotopes = 5;

            isotopes.IsotopeMasses[0] = 45.952629;
            isotopes.IsotopeProbabilities[0] = 0.080000;
            isotopes.IsotopeMasses[1] = 46.951764;
            isotopes.IsotopeProbabilities[1] = 0.073000;
            isotopes.IsotopeMasses[2] = 47.947947;
            isotopes.IsotopeProbabilities[2] = 0.738000;
            isotopes.IsotopeMasses[3] = 48.947871;
            isotopes.IsotopeProbabilities[3] = 0.055000;
            isotopes.IsotopeMasses[4] = 49.944792;
            isotopes.IsotopeProbabilities[4] = 0.054000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "V";
            isotopes.Name = "Vanadium";

            isotopes.AverageMass = 50.941470;
            isotopes.MassVariance = 0.002478;

            isotopes.Atomicity = 23;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 49.947161;
            isotopes.IsotopeProbabilities[0] = 0.002500;
            isotopes.IsotopeMasses[1] = 50.943962;
            isotopes.IsotopeProbabilities[1] = 0.997500;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Cr";
            isotopes.Name = "Chromium";

            isotopes.AverageMass = 51.996125;
            isotopes.MassVariance = 0.359219;

            isotopes.Atomicity = 24;
            isotopes.NumberOfIsotopes = 4;

            isotopes.IsotopeMasses[0] = 49.946046;
            isotopes.IsotopeProbabilities[0] = 0.043450;
            isotopes.IsotopeMasses[1] = 51.940509;
            isotopes.IsotopeProbabilities[1] = 0.837900;
            isotopes.IsotopeMasses[2] = 52.940651;
            isotopes.IsotopeProbabilities[2] = 0.095000;
            isotopes.IsotopeMasses[3] = 53.938882;
            isotopes.IsotopeProbabilities[3] = 0.023650;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Mn";
            isotopes.Name = "Manganese";

            isotopes.AverageMass = 54.938047;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 25;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 54.938047;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Fe";
            isotopes.Name = "Iron";

            isotopes.AverageMass = 55.843820;
            isotopes.MassVariance = 0.258796;

            isotopes.Atomicity = 26;
            isotopes.NumberOfIsotopes = 4;

            isotopes.IsotopeMasses[0] = 53.939612;
            isotopes.IsotopeProbabilities[0] = 0.059000;
            isotopes.IsotopeMasses[1] = 55.934939;
            isotopes.IsotopeProbabilities[1] = 0.917200;
            isotopes.IsotopeMasses[2] = 56.935396;
            isotopes.IsotopeProbabilities[2] = 0.021000;
            isotopes.IsotopeMasses[3] = 57.933277;
            isotopes.IsotopeProbabilities[3] = 0.002800;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Co";
            isotopes.Name = "Cobalt";

            isotopes.AverageMass = 58.933198;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 27;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 58.933198;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ni";
            isotopes.Name = "Nickle";

            isotopes.AverageMass = 58.687889;
            isotopes.MassVariance = 1.473521;

            isotopes.Atomicity = 28;
            isotopes.NumberOfIsotopes = 5;

            isotopes.IsotopeMasses[0] = 57.935346;
            isotopes.IsotopeProbabilities[0] = 0.682700;
            isotopes.IsotopeMasses[1] = 59.930788;
            isotopes.IsotopeProbabilities[1] = 0.261000;
            isotopes.IsotopeMasses[2] = 60.931058;
            isotopes.IsotopeProbabilities[2] = 0.011300;
            isotopes.IsotopeMasses[3] = 61.928346;
            isotopes.IsotopeProbabilities[3] = 0.035900;
            isotopes.IsotopeMasses[4] = 63.927968;
            isotopes.IsotopeProbabilities[4] = 0.009100;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Cu";
            isotopes.Name = "Copper";

            isotopes.AverageMass = 63.552559;
            isotopes.MassVariance = 0.842964;

            isotopes.Atomicity = 29;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 62.939598;
            isotopes.IsotopeProbabilities[0] = 0.691700;
            isotopes.IsotopeMasses[1] = 64.927793;
            isotopes.IsotopeProbabilities[1] = 0.308300;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Zn";
            isotopes.Name = "Zinc";

            isotopes.AverageMass = 65.396363;
            isotopes.MassVariance = 2.545569;

            isotopes.Atomicity = 30;
            isotopes.NumberOfIsotopes = 5;

            isotopes.IsotopeMasses[0] = 63.929145;
            isotopes.IsotopeProbabilities[0] = 0.486000;
            isotopes.IsotopeMasses[1] = 65.926034;
            isotopes.IsotopeProbabilities[1] = 0.279000;
            isotopes.IsotopeMasses[2] = 66.927129;
            isotopes.IsotopeProbabilities[2] = 0.041000;
            isotopes.IsotopeMasses[3] = 67.924846;
            isotopes.IsotopeProbabilities[3] = 0.188000;
            isotopes.IsotopeMasses[4] = 69.925325;
            isotopes.IsotopeProbabilities[4] = 0.006000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ga";
            isotopes.Name = "Galium";

            isotopes.AverageMass = 69.723069;
            isotopes.MassVariance = 0.958287;

            isotopes.Atomicity = 31;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 68.925580;
            isotopes.IsotopeProbabilities[0] = 0.601080;
            isotopes.IsotopeMasses[1] = 70.924700;
            isotopes.IsotopeProbabilities[1] = 0.398920;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ge";
            isotopes.Name = "Germanium";

            isotopes.AverageMass = 72.632250;
            isotopes.MassVariance = 3.098354;

            isotopes.Atomicity = 32;
            isotopes.NumberOfIsotopes = 5;

            isotopes.IsotopeMasses[0] = 69.924250;
            isotopes.IsotopeProbabilities[0] = 0.205000;
            isotopes.IsotopeMasses[1] = 71.922079;
            isotopes.IsotopeProbabilities[1] = 0.274000;
            isotopes.IsotopeMasses[2] = 72.923463;
            isotopes.IsotopeProbabilities[2] = 0.078000;
            isotopes.IsotopeMasses[3] = 73.921177;
            isotopes.IsotopeProbabilities[3] = 0.365000;
            isotopes.IsotopeMasses[4] = 75.921401;
            isotopes.IsotopeProbabilities[4] = 0.078000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "As";
            isotopes.Name = "Arsenic";

            isotopes.AverageMass = 74.921594;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 33;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 74.921594;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Se";
            isotopes.Name = "Selenium";

            isotopes.AverageMass = 78.977677;
            isotopes.MassVariance = 2.876151;

            isotopes.Atomicity = 34;
            isotopes.NumberOfIsotopes = 6;

            isotopes.IsotopeMasses[0] = 73.922475;
            isotopes.IsotopeProbabilities[0] = 0.009000;
            isotopes.IsotopeMasses[1] = 75.919212;
            isotopes.IsotopeProbabilities[1] = 0.091000;
            isotopes.IsotopeMasses[2] = 76.919912;
            isotopes.IsotopeProbabilities[2] = 0.076000;
            isotopes.IsotopeMasses[3] = 77.919000;
            isotopes.IsotopeProbabilities[3] = 0.236000;
            isotopes.IsotopeMasses[4] = 79.916520;
            isotopes.IsotopeProbabilities[4] = 0.499000;
            isotopes.IsotopeMasses[5] = 81.916698;
            isotopes.IsotopeProbabilities[5] = 0.089000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Br";
            isotopes.Name = "Bromine";

            isotopes.AverageMass = 79.903527;
            isotopes.MassVariance = 0.997764;

            isotopes.Atomicity = 35;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 78.918336;
            isotopes.IsotopeProbabilities[0] = 0.506900;
            isotopes.IsotopeMasses[1] = 80.916289;
            isotopes.IsotopeProbabilities[1] = 0.493100;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Kr";
            isotopes.Name = "Krypton";

            isotopes.AverageMass = 83.800003;

            isotopes.MassVariance = 1.741449;

            isotopes.Atomicity = 36;
            isotopes.NumberOfIsotopes = 6;

            isotopes.IsotopeMasses[0] = 77.914000;
            isotopes.IsotopeProbabilities[0] = 0.003500;
            isotopes.IsotopeMasses[1] = 79.916380;
            isotopes.IsotopeProbabilities[1] = 0.022500;
            isotopes.IsotopeMasses[2] = 81.913482;
            isotopes.IsotopeProbabilities[2] = 0.116000;
            isotopes.IsotopeMasses[3] = 82.914135;
            isotopes.IsotopeProbabilities[3] = 0.115000;
            isotopes.IsotopeMasses[4] = 83.911507;
            isotopes.IsotopeProbabilities[4] = 0.570000;
            isotopes.IsotopeMasses[5] = 85.910616;
            isotopes.IsotopeProbabilities[5] = 0.173000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Rb";
            isotopes.Name = "Rubidium";

            isotopes.AverageMass = 85.467668;
            isotopes.MassVariance = 0.801303;

            isotopes.Atomicity = 37;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 84.911794;
            isotopes.IsotopeProbabilities[0] = 0.721700;
            isotopes.IsotopeMasses[1] = 86.909187;
            isotopes.IsotopeProbabilities[1] = 0.278300;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Sr";
            isotopes.Name = "Strontium";

            isotopes.AverageMass = 87.616651;
            isotopes.MassVariance = 0.468254;

            isotopes.Atomicity = 38;
            isotopes.NumberOfIsotopes = 4;

            isotopes.IsotopeMasses[0] = 83.913430;
            isotopes.IsotopeProbabilities[0] = 0.005600;
            isotopes.IsotopeMasses[1] = 85.909267;
            isotopes.IsotopeProbabilities[1] = 0.098600;
            isotopes.IsotopeMasses[2] = 86.908884;
            isotopes.IsotopeProbabilities[2] = 0.070000;
            isotopes.IsotopeMasses[3] = 87.905619;
            isotopes.IsotopeProbabilities[3] = 0.825800;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Y";
            isotopes.Name = "Yttrium";

            isotopes.AverageMass = 88.905849;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 39;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 88.905849;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Zr";
            isotopes.Name = "Zirconium";

            isotopes.AverageMass = 91.223646;
            isotopes.MassVariance = 2.851272;

            isotopes.Atomicity = 40;
            isotopes.NumberOfIsotopes = 5;

            isotopes.IsotopeMasses[0] = 89.904703;
            isotopes.IsotopeProbabilities[0] = 0.514500;
            isotopes.IsotopeMasses[1] = 90.905644;
            isotopes.IsotopeProbabilities[1] = 0.112200;
            isotopes.IsotopeMasses[2] = 91.905039;
            isotopes.IsotopeProbabilities[2] = 0.171500;
            isotopes.IsotopeMasses[3] = 93.906314;
            isotopes.IsotopeProbabilities[3] = 0.173800;
            isotopes.IsotopeMasses[4] = 95.908275;
            isotopes.IsotopeProbabilities[4] = 0.028000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Nb";
            isotopes.Name = "Niobium";

            isotopes.AverageMass = 92.906377;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 41;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 92.906377;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Mo";
            isotopes.Name = "Molybdenum";

            isotopes.AverageMass = 95.931290;
            isotopes.MassVariance = 5.504460;

            isotopes.Atomicity = 42;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 91.906808;
            isotopes.IsotopeProbabilities[0] = 0.148400;
            isotopes.IsotopeMasses[1] = 93.905085;
            isotopes.IsotopeProbabilities[1] = 0.092500;
            isotopes.IsotopeMasses[2] = 94.905840;
            isotopes.IsotopeProbabilities[2] = 0.159200;
            isotopes.IsotopeMasses[3] = 95.904678;
            isotopes.IsotopeProbabilities[3] = 0.166800;
            isotopes.IsotopeMasses[4] = 96.906020;
            isotopes.IsotopeProbabilities[4] = 0.095500;
            isotopes.IsotopeMasses[5] = 97.905406;
            isotopes.IsotopeProbabilities[5] = 0.241300;
            isotopes.IsotopeMasses[6] = 99.907477;
            isotopes.IsotopeProbabilities[6] = 0.096300;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Tc";
            isotopes.Name = "Technetium";

            isotopes.AverageMass = 98.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 43;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 98.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ru";
            isotopes.Name = "Ruthenium";

            isotopes.AverageMass = 101.066343;
            isotopes.MassVariance = 4.148678;

            isotopes.Atomicity = 44;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 95.907599;
            isotopes.IsotopeProbabilities[0] = 0.055400;
            isotopes.IsotopeMasses[1] = 97.905287;
            isotopes.IsotopeProbabilities[1] = 0.018600;
            isotopes.IsotopeMasses[2] = 98.905939;
            isotopes.IsotopeProbabilities[2] = 0.127000;
            isotopes.IsotopeMasses[3] = 99.904219;
            isotopes.IsotopeProbabilities[3] = 0.126000;
            isotopes.IsotopeMasses[4] = 100.905582;
            isotopes.IsotopeProbabilities[4] = 0.171000;
            isotopes.IsotopeMasses[5] = 101.904348;
            isotopes.IsotopeProbabilities[5] = 0.316000;
            isotopes.IsotopeMasses[6] = 103.905424;
            isotopes.IsotopeProbabilities[6] = 0.186000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Rh";
            isotopes.Name = "Rhodium";

            isotopes.AverageMass = 102.905500;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 45;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 102.905500;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Pd";
            isotopes.Name = " Palladium";

            isotopes.AverageMass = 106.415327;
            isotopes.MassVariance = 3.504600;

            isotopes.Atomicity = 46;
            isotopes.NumberOfIsotopes = 6;

            isotopes.IsotopeMasses[0] = 101.905634;
            isotopes.IsotopeProbabilities[0] = 0.010200;
            isotopes.IsotopeMasses[1] = 103.904029;
            isotopes.IsotopeProbabilities[1] = 0.111400;
            isotopes.IsotopeMasses[2] = 104.905079;
            isotopes.IsotopeProbabilities[2] = 0.223300;
            isotopes.IsotopeMasses[3] = 105.903478;
            isotopes.IsotopeProbabilities[3] = 0.273300;
            isotopes.IsotopeMasses[4] = 107.903895;
            isotopes.IsotopeProbabilities[4] = 0.264600;
            isotopes.IsotopeMasses[5] = 109.905167;
            isotopes.IsotopeProbabilities[5] = 0.117200;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ag";
            isotopes.Name = "Silver";

            isotopes.AverageMass = 107.868151;
            isotopes.MassVariance = 0.998313;

            isotopes.Atomicity = 47;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 106.905092;
            isotopes.IsotopeProbabilities[0] = 0.518390;
            isotopes.IsotopeMasses[1] = 108.904757;
            isotopes.IsotopeProbabilities[1] = 0.481610;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Cd";
            isotopes.Name = "Cadmium";

            isotopes.AverageMass = 112.411552;
            isotopes.MassVariance = 3.432071;

            isotopes.Atomicity = 48;
            isotopes.NumberOfIsotopes = 8;

            isotopes.IsotopeMasses[0] = 105.906461;
            isotopes.IsotopeProbabilities[0] = 0.012500;
            isotopes.IsotopeMasses[1] = 107.904176;
            isotopes.IsotopeProbabilities[1] = 0.008900;
            isotopes.IsotopeMasses[2] = 109.903005;
            isotopes.IsotopeProbabilities[2] = 0.124900;
            isotopes.IsotopeMasses[3] = 110.904182;
            isotopes.IsotopeProbabilities[3] = 0.128000;
            isotopes.IsotopeMasses[4] = 111.902758;
            isotopes.IsotopeProbabilities[4] = 0.241300;
            isotopes.IsotopeMasses[5] = 112.904400;
            isotopes.IsotopeProbabilities[5] = 0.122200;
            isotopes.IsotopeMasses[6] = 113.903357;
            isotopes.IsotopeProbabilities[6] = 0.287300;
            isotopes.IsotopeMasses[7] = 115.904754;
            isotopes.IsotopeProbabilities[7] = 0.074900;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "In";
            isotopes.Name = "Indium";

            isotopes.AverageMass = 114.817888;
            isotopes.MassVariance = 0.164574;

            isotopes.Atomicity = 49;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 112.904061;
            isotopes.IsotopeProbabilities[0] = 0.043000;
            isotopes.IsotopeMasses[1] = 114.903880;
            isotopes.IsotopeProbabilities[1] = 0.957000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Sn";
            isotopes.Name = "Tin";

            isotopes.AverageMass = 118.710213;
            isotopes.MassVariance = 4.707888;

            isotopes.Atomicity = 50;
            isotopes.NumberOfIsotopes = 10;

            isotopes.IsotopeMasses[0] = 111.904826;
            isotopes.IsotopeProbabilities[0] = 0.009700;
            isotopes.IsotopeMasses[1] = 113.902784;
            isotopes.IsotopeProbabilities[1] = 0.006500;
            isotopes.IsotopeMasses[2] = 114.903348;
            isotopes.IsotopeProbabilities[2] = 0.003600;
            isotopes.IsotopeMasses[3] = 115.901747;
            isotopes.IsotopeProbabilities[3] = 0.145300;
            isotopes.IsotopeMasses[4] = 116.902956;
            isotopes.IsotopeProbabilities[4] = 0.076800;
            isotopes.IsotopeMasses[5] = 117.901609;
            isotopes.IsotopeProbabilities[5] = 0.242200;
            isotopes.IsotopeMasses[6] = 118.903310;
            isotopes.IsotopeProbabilities[6] = 0.085800;
            isotopes.IsotopeMasses[7] = 119.902200;
            isotopes.IsotopeProbabilities[7] = 0.325900;
            isotopes.IsotopeMasses[8] = 121.903440;
            isotopes.IsotopeProbabilities[8] = 0.046300;
            isotopes.IsotopeMasses[9] = 123.905274;
            isotopes.IsotopeProbabilities[9] = 0.057900;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Sb";
            isotopes.Name = "Antimony";

            isotopes.AverageMass = 121.755989;
            isotopes.MassVariance = 0.978482;

            isotopes.Atomicity = 51;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 120.903821;
            isotopes.IsotopeProbabilities[0] = 0.574000;
            isotopes.IsotopeMasses[1] = 122.904216;
            isotopes.IsotopeProbabilities[1] = 0.426000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Te";
            isotopes.Name = "Tellerium";

            isotopes.AverageMass = 127.590074;
            isotopes.MassVariance = 4.644177;

            isotopes.Atomicity = 52;
            isotopes.NumberOfIsotopes = 8;

            isotopes.IsotopeMasses[0] = 119.904048;
            isotopes.IsotopeProbabilities[0] = 0.000950;
            isotopes.IsotopeMasses[1] = 121.903054;
            isotopes.IsotopeProbabilities[1] = 0.025900;
            isotopes.IsotopeMasses[2] = 122.904271;
            isotopes.IsotopeProbabilities[2] = 0.009050;
            isotopes.IsotopeMasses[3] = 123.902823;
            isotopes.IsotopeProbabilities[3] = 0.047900;
            isotopes.IsotopeMasses[4] = 124.904433;
            isotopes.IsotopeProbabilities[4] = 0.071200;
            isotopes.IsotopeMasses[5] = 125.903314;
            isotopes.IsotopeProbabilities[5] = 0.189300;
            isotopes.IsotopeMasses[6] = 127.904463;
            isotopes.IsotopeProbabilities[6] = 0.317000;
            isotopes.IsotopeMasses[7] = 129.906229;
            isotopes.IsotopeProbabilities[7] = 0.338700;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "I";
            isotopes.Name = "Iodine";

            isotopes.AverageMass = 126.904473;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 53;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 126.904473;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Xe";
            isotopes.Name = "Xenon";

            isotopes.AverageMass = 131.293076;
            isotopes.MassVariance = 4.622071;

            isotopes.Atomicity = 54;
            isotopes.NumberOfIsotopes = 9;

            isotopes.IsotopeMasses[0] = 123.905894;
            isotopes.IsotopeProbabilities[0] = 0.001000;
            isotopes.IsotopeMasses[1] = 125.904281;
            isotopes.IsotopeProbabilities[1] = 0.000900;
            isotopes.IsotopeMasses[2] = 127.903531;
            isotopes.IsotopeProbabilities[2] = 0.019100;
            isotopes.IsotopeMasses[3] = 128.904780;
            isotopes.IsotopeProbabilities[3] = 0.264000;
            isotopes.IsotopeMasses[4] = 129.903509;
            isotopes.IsotopeProbabilities[4] = 0.041000;
            isotopes.IsotopeMasses[5] = 130.905072;
            isotopes.IsotopeProbabilities[5] = 0.212000;
            isotopes.IsotopeMasses[6] = 131.904144;
            isotopes.IsotopeProbabilities[6] = 0.269000;
            isotopes.IsotopeMasses[7] = 133.905395;
            isotopes.IsotopeProbabilities[7] = 0.104000;
            isotopes.IsotopeMasses[8] = 135.907214;
            isotopes.IsotopeProbabilities[8] = 0.089000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Cs";
            isotopes.Name = "Casium";

            isotopes.AverageMass = 132.905429;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 55;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 132.905429;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ba";
            isotopes.Name = "Barium";

            isotopes.AverageMass = 137.326825;
            isotopes.MassVariance = 1.176556;

            isotopes.Atomicity = 56;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 129.906282;
            isotopes.IsotopeProbabilities[0] = 0.001060;
            isotopes.IsotopeMasses[1] = 131.905042;
            isotopes.IsotopeProbabilities[1] = 0.001010;
            isotopes.IsotopeMasses[2] = 133.904486;
            isotopes.IsotopeProbabilities[2] = 0.024200;
            isotopes.IsotopeMasses[3] = 134.905665;
            isotopes.IsotopeProbabilities[3] = 0.065930;
            isotopes.IsotopeMasses[4] = 135.904553;
            isotopes.IsotopeProbabilities[4] = 0.078500;
            isotopes.IsotopeMasses[5] = 136.905812;
            isotopes.IsotopeProbabilities[5] = 0.112300;
            isotopes.IsotopeMasses[6] = 137.905232;
            isotopes.IsotopeProbabilities[6] = 0.717000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "La";
            isotopes.Name = "Lanthanum";

            isotopes.AverageMass = 138.905448;
            isotopes.MassVariance = 0.000898;

            isotopes.Atomicity = 57;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 137.907110;
            isotopes.IsotopeProbabilities[0] = 0.000900;
            isotopes.IsotopeMasses[1] = 138.906347;
            isotopes.IsotopeProbabilities[1] = 0.999100;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ce";
            isotopes.Name = "Cerium";

            isotopes.AverageMass = 140.115861;
            isotopes.MassVariance = 0.442985;

            isotopes.Atomicity = 58;
            isotopes.NumberOfIsotopes = 4;

            isotopes.IsotopeMasses[0] = 135.907140;
            isotopes.IsotopeProbabilities[0] = 0.001900;
            isotopes.IsotopeMasses[1] = 137.905985;
            isotopes.IsotopeProbabilities[1] = 0.002500;
            isotopes.IsotopeMasses[2] = 139.905433;
            isotopes.IsotopeProbabilities[2] = 0.884300;
            isotopes.IsotopeMasses[3] = 141.909241;
            isotopes.IsotopeProbabilities[3] = 0.111300;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Pr";
            isotopes.Name = "Praseodymium";

            isotopes.AverageMass = 140.907647;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 59;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 140.907647;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Nd";
            isotopes.Name = "Neodynium";

            isotopes.AverageMass = 144.242337;
            isotopes.MassVariance = 4.834797;

            isotopes.Atomicity = 60;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 141.907719;
            isotopes.IsotopeProbabilities[0] = 0.271300;
            isotopes.IsotopeMasses[1] = 142.909810;
            isotopes.IsotopeProbabilities[1] = 0.121800;
            isotopes.IsotopeMasses[2] = 143.910083;
            isotopes.IsotopeProbabilities[2] = 0.238000;
            isotopes.IsotopeMasses[3] = 144.912570;
            isotopes.IsotopeProbabilities[3] = 0.083000;
            isotopes.IsotopeMasses[4] = 145.913113;
            isotopes.IsotopeProbabilities[4] = 0.171900;
            isotopes.IsotopeMasses[5] = 147.916889;
            isotopes.IsotopeProbabilities[5] = 0.057600;
            isotopes.IsotopeMasses[6] = 149.920887;
            isotopes.IsotopeProbabilities[6] = 0.056400;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Pm";
            isotopes.Name = "Promethium";

            isotopes.AverageMass = 145.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 61;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 145.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Sm";
            isotopes.Name = "Samarium";

            isotopes.AverageMass = 150.360238;
            isotopes.MassVariance = 7.576609;

            isotopes.Atomicity = 62;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 143.911998;
            isotopes.IsotopeProbabilities[0] = 0.031000;
            isotopes.IsotopeMasses[1] = 146.914895;
            isotopes.IsotopeProbabilities[1] = 0.150000;
            isotopes.IsotopeMasses[2] = 147.914820;
            isotopes.IsotopeProbabilities[2] = 0.113000;
            isotopes.IsotopeMasses[3] = 148.917181;
            isotopes.IsotopeProbabilities[3] = 0.138000;
            isotopes.IsotopeMasses[4] = 149.917273;
            isotopes.IsotopeProbabilities[4] = 0.074000;
            isotopes.IsotopeMasses[5] = 151.919729;
            isotopes.IsotopeProbabilities[5] = 0.267000;
            isotopes.IsotopeMasses[6] = 153.922206;
            isotopes.IsotopeProbabilities[6] = 0.227000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Eu";
            isotopes.Name = "Europium";

            isotopes.AverageMass = 151.964566;
            isotopes.MassVariance = 0.999440;

            isotopes.Atomicity = 63;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 150.919847;
            isotopes.IsotopeProbabilities[0] = 0.478000;
            isotopes.IsotopeMasses[1] = 152.921225;
            isotopes.IsotopeProbabilities[1] = 0.522000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Gd";
            isotopes.Name = "Gadolinium";

            isotopes.AverageMass = 157.252118;
            isotopes.MassVariance = 3.157174;

            isotopes.Atomicity = 64;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 151.919786;
            isotopes.IsotopeProbabilities[0] = 0.002000;
            isotopes.IsotopeMasses[1] = 153.920861;
            isotopes.IsotopeProbabilities[1] = 0.021800;
            isotopes.IsotopeMasses[2] = 154.922618;
            isotopes.IsotopeProbabilities[2] = 0.148000;
            isotopes.IsotopeMasses[3] = 155.922118;
            isotopes.IsotopeProbabilities[3] = 0.204700;
            isotopes.IsotopeMasses[4] = 156.923956;
            isotopes.IsotopeProbabilities[4] = 0.156500;
            isotopes.IsotopeMasses[5] = 157.924099;
            isotopes.IsotopeProbabilities[5] = 0.248400;
            isotopes.IsotopeMasses[6] = 159.927049;
            isotopes.IsotopeProbabilities[6] = 0.218600;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Tb";
            isotopes.Name = "Terbium";

            isotopes.AverageMass = 158.925342;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 65;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 158.925342;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Dy";
            isotopes.Name = "Dysprosium";

            isotopes.AverageMass = 162.497531;
            isotopes.MassVariance = 1.375235;

            isotopes.Atomicity = 66;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 155.925277;
            isotopes.IsotopeProbabilities[0] = 0.000600;
            isotopes.IsotopeMasses[1] = 157.924403;
            isotopes.IsotopeProbabilities[1] = 0.001000;
            isotopes.IsotopeMasses[2] = 159.925193;
            isotopes.IsotopeProbabilities[2] = 0.023400;
            isotopes.IsotopeMasses[3] = 160.926930;
            isotopes.IsotopeProbabilities[3] = 0.189000;
            isotopes.IsotopeMasses[4] = 161.926795;
            isotopes.IsotopeProbabilities[4] = 0.255000;
            isotopes.IsotopeMasses[5] = 162.928728;
            isotopes.IsotopeProbabilities[5] = 0.249000;
            isotopes.IsotopeMasses[6] = 163.929171;
            isotopes.IsotopeProbabilities[6] = 0.282000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ho";
            isotopes.Name = "Holmium";

            isotopes.AverageMass = 164.930319;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 67;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 164.930319;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Er";
            isotopes.Name = "Erbium";

            isotopes.AverageMass = 167.255701;
            isotopes.MassVariance = 2.024877;

            isotopes.Atomicity = 68;
            isotopes.NumberOfIsotopes = 6;

            isotopes.IsotopeMasses[0] = 161.928775;
            isotopes.IsotopeProbabilities[0] = 0.001400;
            isotopes.IsotopeMasses[1] = 163.929198;
            isotopes.IsotopeProbabilities[1] = 0.016100;
            isotopes.IsotopeMasses[2] = 165.930290;
            isotopes.IsotopeProbabilities[2] = 0.336000;
            isotopes.IsotopeMasses[3] = 166.932046;
            isotopes.IsotopeProbabilities[3] = 0.229500;
            isotopes.IsotopeMasses[4] = 167.932368;
            isotopes.IsotopeProbabilities[4] = 0.268000;
            isotopes.IsotopeMasses[5] = 169.935461;
            isotopes.IsotopeProbabilities[5] = 0.149000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Tm";
            isotopes.Name = "Thulium";

            isotopes.AverageMass = 168.934212;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 69;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 168.934212;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Yb";
            isotopes.Name = "Ytterbium";

            isotopes.AverageMass = 173.034187;
            isotopes.MassVariance = 2.556093;

            isotopes.Atomicity = 70;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 167.933894;
            isotopes.IsotopeProbabilities[0] = 0.001300;
            isotopes.IsotopeMasses[1] = 169.934759;
            isotopes.IsotopeProbabilities[1] = 0.030500;
            isotopes.IsotopeMasses[2] = 170.936323;
            isotopes.IsotopeProbabilities[2] = 0.143000;
            isotopes.IsotopeMasses[3] = 171.936378;
            isotopes.IsotopeProbabilities[3] = 0.219000;
            isotopes.IsotopeMasses[4] = 172.938208;
            isotopes.IsotopeProbabilities[4] = 0.161200;
            isotopes.IsotopeMasses[5] = 173.938859;
            isotopes.IsotopeProbabilities[5] = 0.318000;
            isotopes.IsotopeMasses[6] = 175.942564;
            isotopes.IsotopeProbabilities[6] = 0.127000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Lu";
            isotopes.Name = "Lutetium";

            isotopes.AverageMass = 174.966719;
            isotopes.MassVariance = 0.025326;

            isotopes.Atomicity = 71;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 174.940770;
            isotopes.IsotopeProbabilities[0] = 0.974100;
            isotopes.IsotopeMasses[1] = 175.942679;
            isotopes.IsotopeProbabilities[1] = 0.025900;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Hf";
            isotopes.Name = "Hafnium";

            isotopes.AverageMass = 178.486400;
            isotopes.MassVariance = 1.671265;

            isotopes.Atomicity = 72;
            isotopes.NumberOfIsotopes = 6;

            isotopes.IsotopeMasses[0] = 173.940044;
            isotopes.IsotopeProbabilities[0] = 0.001620;
            isotopes.IsotopeMasses[1] = 175.941406;
            isotopes.IsotopeProbabilities[1] = 0.052060;
            isotopes.IsotopeMasses[2] = 176.943217;
            isotopes.IsotopeProbabilities[2] = 0.186060;
            isotopes.IsotopeMasses[3] = 177.943696;
            isotopes.IsotopeProbabilities[3] = 0.272970;
            isotopes.IsotopeMasses[4] = 178.945812;
            isotopes.IsotopeProbabilities[4] = 0.136290;
            isotopes.IsotopeMasses[5] = 179.946545;
            isotopes.IsotopeProbabilities[5] = 0.351000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ta";
            isotopes.Name = "Tantalum";

            isotopes.AverageMass = 180.947872;
            isotopes.MassVariance = 0.000120;

            isotopes.Atomicity = 73;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 179.947462;
            isotopes.IsotopeProbabilities[0] = 0.000120;
            isotopes.IsotopeMasses[1] = 180.947992;
            isotopes.IsotopeProbabilities[1] = 0.999880;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "W";
            isotopes.Name = "Tungsten";

            isotopes.AverageMass = 183.849486;
            isotopes.MassVariance = 2.354748;

            isotopes.Atomicity = 74;
            isotopes.NumberOfIsotopes = 5;

            isotopes.IsotopeMasses[0] = 179.946701;
            isotopes.IsotopeProbabilities[0] = 0.001200;
            isotopes.IsotopeMasses[1] = 181.948202;
            isotopes.IsotopeProbabilities[1] = 0.263000;
            isotopes.IsotopeMasses[2] = 182.950220;
            isotopes.IsotopeProbabilities[2] = 0.142800;
            isotopes.IsotopeMasses[3] = 183.950928;
            isotopes.IsotopeProbabilities[3] = 0.307000;
            isotopes.IsotopeMasses[4] = 185.954357;
            isotopes.IsotopeProbabilities[4] = 0.286000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Re";
            isotopes.Name = "Rhenium";

            isotopes.AverageMass = 186.206699;
            isotopes.MassVariance = 0.939113;

            isotopes.Atomicity = 75;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 184.952951;
            isotopes.IsotopeProbabilities[0] = 0.374000;
            isotopes.IsotopeMasses[1] = 186.955744;
            isotopes.IsotopeProbabilities[1] = 0.626000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Os";
            isotopes.Name = "Osmium";

            isotopes.AverageMass = 190.239771;
            isotopes.MassVariance = 2.665149;

            isotopes.Atomicity = 76;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 183.952488;
            isotopes.IsotopeProbabilities[0] = 0.000200;
            isotopes.IsotopeMasses[1] = 185.953830;
            isotopes.IsotopeProbabilities[1] = 0.015800;
            isotopes.IsotopeMasses[2] = 186.955741;
            isotopes.IsotopeProbabilities[2] = 0.016000;
            isotopes.IsotopeMasses[3] = 187.955860;
            isotopes.IsotopeProbabilities[3] = 0.133000;
            isotopes.IsotopeMasses[4] = 188.958137;
            isotopes.IsotopeProbabilities[4] = 0.161000;
            isotopes.IsotopeMasses[5] = 189.958436;
            isotopes.IsotopeProbabilities[5] = 0.264000;
            isotopes.IsotopeMasses[6] = 191.961467;
            isotopes.IsotopeProbabilities[6] = 0.410000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ir";
            isotopes.Name = "Iridium";

            isotopes.AverageMass = 192.216047;
            isotopes.MassVariance = 0.937668;

            isotopes.Atomicity = 77;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 190.960584;
            isotopes.IsotopeProbabilities[0] = 0.373000;
            isotopes.IsotopeMasses[1] = 192.962917;
            isotopes.IsotopeProbabilities[1] = 0.627000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Pt";
            isotopes.Name = "Platinum";

            isotopes.AverageMass = 195.080105;
            isotopes.MassVariance = 1.293292;

            isotopes.Atomicity = 78;
            isotopes.NumberOfIsotopes = 6;

            isotopes.IsotopeMasses[0] = 189.959917;
            isotopes.IsotopeProbabilities[0] = 0.000100;
            isotopes.IsotopeMasses[1] = 191.961019;
            isotopes.IsotopeProbabilities[1] = 0.007900;
            isotopes.IsotopeMasses[2] = 193.962655;
            isotopes.IsotopeProbabilities[2] = 0.329000;
            isotopes.IsotopeMasses[3] = 194.964766;
            isotopes.IsotopeProbabilities[3] = 0.338000;
            isotopes.IsotopeMasses[4] = 195.964926;
            isotopes.IsotopeProbabilities[4] = 0.253000;
            isotopes.IsotopeMasses[5] = 197.967869;
            isotopes.IsotopeProbabilities[5] = 0.072000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Au";
            isotopes.Name = "Gold";

            isotopes.AverageMass = 196.966543;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 79;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 196.966543;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Hg";
            isotopes.Name = "Mercury";

            isotopes.AverageMass = 200.596438;
            isotopes.MassVariance = 2.625230;

            isotopes.Atomicity = 80;
            isotopes.NumberOfIsotopes = 7;

            isotopes.IsotopeMasses[0] = 195.965807;
            isotopes.IsotopeProbabilities[0] = 0.001500;
            isotopes.IsotopeMasses[1] = 197.966743;
            isotopes.IsotopeProbabilities[1] = 0.100000;
            isotopes.IsotopeMasses[2] = 198.968254;
            isotopes.IsotopeProbabilities[2] = 0.169000;
            isotopes.IsotopeMasses[3] = 199.968300;
            isotopes.IsotopeProbabilities[3] = 0.231000;
            isotopes.IsotopeMasses[4] = 200.970277;
            isotopes.IsotopeProbabilities[4] = 0.132000;
            isotopes.IsotopeMasses[5] = 201.970617;
            isotopes.IsotopeProbabilities[5] = 0.298000;
            isotopes.IsotopeMasses[6] = 203.973467;
            isotopes.IsotopeProbabilities[6] = 0.068500;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Tl";
            isotopes.Name = "Thallium";

            isotopes.AverageMass = 204.383307;
            isotopes.MassVariance = 0.834026;

            isotopes.Atomicity = 81;
            isotopes.NumberOfIsotopes = 2;

            isotopes.IsotopeMasses[0] = 202.972320;
            isotopes.IsotopeProbabilities[0] = 0.295240;
            isotopes.IsotopeMasses[1] = 204.974401;
            isotopes.IsotopeProbabilities[1] = 0.704760;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Pb";
            isotopes.Name = "Lead";

            isotopes.AverageMass = 207.216883;
            isotopes.MassVariance = 0.834636;

            isotopes.Atomicity = 82;
            isotopes.NumberOfIsotopes = 4;

            isotopes.IsotopeMasses[0] = 203.973020;
            isotopes.IsotopeProbabilities[0] = 0.014000;
            isotopes.IsotopeMasses[1] = 205.974440;
            isotopes.IsotopeProbabilities[1] = 0.241000;
            isotopes.IsotopeMasses[2] = 206.975872;
            isotopes.IsotopeProbabilities[2] = 0.221000;
            isotopes.IsotopeMasses[3] = 207.976627;
            isotopes.IsotopeProbabilities[3] = 0.524000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Bi";
            isotopes.Name = "Bismuth";

            isotopes.AverageMass = 208.980374;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 83;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 208.980374;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Po";
            isotopes.Name = "Polonium";

            isotopes.AverageMass = 209.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 84;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 209.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "At";
            isotopes.Name = "Astatine";

            isotopes.AverageMass = 210.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 85;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 210.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Rn";
            isotopes.Name = "Radon";

            isotopes.AverageMass = 222.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 86;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 222.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Fr";
            isotopes.Name = "Francium";

            isotopes.AverageMass = 223.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 87;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 223.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ra";
            isotopes.Name = "Radium";

            isotopes.AverageMass = 226.025000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 88;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 226.025000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Ac";
            isotopes.Name = "Actinium";

            isotopes.AverageMass = 227.028000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 89;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 227.028000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Th";
            isotopes.Name = "Thorium";

            isotopes.AverageMass = 232.038054;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 90;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 232.038054;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Pa";
            isotopes.Name = "Protactinium";

            isotopes.AverageMass = 231.035900;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 91;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 231.035900;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "U";
            isotopes.Name = "Uranium";

            isotopes.AverageMass = 238.028914;
            isotopes.MassVariance = 0.065503;

            isotopes.Atomicity = 92;
            isotopes.NumberOfIsotopes = 3;

            isotopes.IsotopeMasses[0] = 234.040946;
            isotopes.IsotopeProbabilities[0] = 0.000055;
            isotopes.IsotopeMasses[1] = 235.043924;
            isotopes.IsotopeProbabilities[1] = 0.007200;
            isotopes.IsotopeMasses[2] = 238.050784;
            isotopes.IsotopeProbabilities[2] = 0.992745;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Np";
            isotopes.Name = "Neptunium";

            isotopes.AverageMass = 237.048000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 93;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 237.048000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Pu";
            isotopes.Name = "Plutonium";

            isotopes.AverageMass = 244.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 94;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 244.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Am";
            isotopes.Name = "Americium";

            isotopes.AverageMass = 243.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 95;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 243.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Cm";
            isotopes.Name = "Curium";

            isotopes.AverageMass = 247.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 96;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 247.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Bk";
            isotopes.Name = "Berkelium";

            isotopes.AverageMass = 247.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 97;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 247.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Cm";
            isotopes.Name = "Californium";

            isotopes.AverageMass = 251.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 98;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 251.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Es";
            isotopes.Name = "Einsteinium";

            isotopes.AverageMass = 252.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 99;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 252.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Fm";
            isotopes.Name = "Fernium";

            isotopes.AverageMass = 257.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 100;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 257.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Md";
            isotopes.Name = "Medelevium";

            isotopes.AverageMass = 258.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 101;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 258.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "No";
            isotopes.Name = "Nobelium";

            isotopes.AverageMass = 259.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 102;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 259.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            isotopes = new ElementData();

            isotopes.Symbol = "Lr";
            isotopes.Name = "Lawrencium";

            isotopes.AverageMass = 260.000000;
            isotopes.MassVariance = 0.000000;

            isotopes.Atomicity = 103;
            isotopes.NumberOfIsotopes = 1;

            isotopes.IsotopeMasses[0] = 260.000000;
            isotopes.IsotopeProbabilities[0] = 1.000000;

            ElementalIsotopesList.Add(isotopes);
            //isotopes = new ElementalIsotopes();

            //Engine.TheoreticalProfile.ElementalIsotopes isotopes;
            //std.cout.precision(6);
            //std.cout.setf(std.ios.fixed, std.ios.floatfield);

            //for (int element_num = 0; element_num < mvect_elemental_isotopes.Count; element_num++)
            //{
            //  isotopes = mvect_elemental_isotopes[element_num];

            //  System.Console.WriteLine("strcpy(isotopes.marr_symbol, \""<<isotopes.marr_symbol<<"\");\n";
            //  System.Console.WriteLine("strcpy(isotopes.marr_name, \""<<isotopes.marr_name<<"\");\n\n";
            //  System.Console.WriteLine("isotopes.mdbl_average_mass = "<<isotopes.mdbl_average_mass<<";\n";
            //  System.Console.WriteLine("isotopes.mdbl_mass_variance = "<<isotopes.mdbl_mass_variance<<";\n\n";
            //  System.Console.WriteLine("isotopes.mint_atomicity = "<<isotopes.mint_atomicity<<";\n";
            //  System.Console.WriteLine("isotopes.mint_num_isotopes = "<<isotopes.mint_num_isotopes<<";\n\n";
            //  for (int isotope_num = 0; isotope_num < isotopes.mint_num_isotopes; isotope_num++)
            //  {
            //      System.Console.WriteLine("isotopes.marr_isotope_mass["<<isotope_num<<"] = "<<isotopes.marr_isotope_mass[isotope_num]<<";\n";
            //      System.Console.WriteLine("isotopes.marr_isotope_prob["<<isotope_num<<"] = "<<isotopes.marr_isotope_prob[isotope_num]<<";\n";
            //  }
            //  System.Console.WriteLine("\nmvect_elemental_isotopes.Add(isotopes); \n\n";
            //}
        }
        
#if !Disable_Obsolete
        [Obsolete("Only used by Decon2LS.UI (maybe)", false)]
        public void LoadData(string fileName)
        {
            ElementalIsotopesList.Clear();
            /** Function that reads from xml file, creates a DOM tree and writes the isotope config
            into mvect_elemental_isotopes **/

            /*
            * <elements>
            *   <element>
            *     <name>Hydrogen</name>
            *     <symbol>H</symbol>
            *     <atomicity>1</atomicity>
            *     <num_isotopes>2</num_isotopes>
            *     <isotopes>
            *       <isotope>
            *         <mass>1.007825</mass>
            *         <probability>0.999850</probability>
            *       </isotope>
            *       <isotope>
            *         <mass>2.014102</mass>
            *         <probability>0.000150</probability>
            *       </isotope>
            *       ...
            *     </isotopes>
            *   </element>
            * 
            *   <element>
            *     <name>Helium</name>
            *     <symbol>He</symbol>
            *     <atomicity>2</atomicity>
            *     <num_isotopes>2</num_isotopes>
            *     <isotopes>
            *       <isotope>
            *         <mass>3.016030</mass>
            *         <probability>0.000001</probability>
            *       </isotope>
            *       <isotope>
            *         <mass>4.002600</mass>
            *         <probability>0.999999</probability>
            *       </isotope>
            *       ...
            *     </isotopes>
            *   </element>
            *   ...
            * </elements>
            */

            ElementalIsotopesList.Clear();
            //Initialize the XML
            var rdrSettings = new XmlReaderSettings {IgnoreWhitespace = true};
            using (var rdr = XmlReader.Create(new FileStream(fileName, FileMode.Open), rdrSettings))
            {
                try
                {
                    rdr.MoveToContent();
                    //create DOM tree
                    //start walking down the tree
                    if (rdr.NodeType == XmlNodeType.Element && rdr.Name == XmlElementsTag) //elements
                    {
                        rdr.ReadStartElement(); // Read the elements tag, to get to the contents
                        while (rdr.NodeType != XmlNodeType.EndElement)
                        {
                            switch (rdr.Name)
                            {
                                case XmlElementTag:
                                    //if (isotopes != null)
                                    //{
                                    //    mvect_elemental_isotopes.Add(isotopes);
                                    //}
                                    //isotopes = new ElementalIsotopes();
                                    ElementalIsotopesList.Add(ReadElementFromXml(rdr.ReadSubtree()));
                                    rdr.ReadEndElement();
                                    break;
                                case XmlNameTag:
                                    rdr.Skip();
                                    break;
                                case XmlSymbolTag:
                                    rdr.Skip();
                                    break;
                                case XmlAtomicityTag:
                                    rdr.Skip();
                                    break;
                                case XmlNumIsotopesTag:
                                    rdr.Skip();
                                    break;
                                case XmlIsotopesTag:
                                    rdr.Skip();
                                    break;
                                case XmlIsotopeTag:
                                    rdr.Skip();
                                    break;
                                case XmlMassTag:
                                    rdr.Skip();
                                    break;
                                case XmlProbabilityTag:
                                    rdr.Skip();
                                    break;
                                default:
                                    rdr.Skip();
                                    break;
                            }
                        }
                    }
                }
                catch (System.Exception e)
                {
#if DEBUG
                    throw e;
#endif
                }
            }
        }

        private ElementData ReadElementFromXml(XmlReader rdr)
        {
            var isotopes = new ElementData();
            rdr.MoveToContent();
            if (rdr.Name != XmlElementTag)
            {
                throw new System.Exception("Bad tag! Reading Isotopes XML file");
            }
            rdr.ReadStartElement(); // Read the element tag, to get to the contents
            while (rdr.NodeType != XmlNodeType.EndElement)
            {
                switch (rdr.Name)
                {
                    case XmlNameTag:
                        isotopes.Name = rdr.ReadElementContentAsString();
                        break;
                    case XmlSymbolTag:
                        isotopes.Symbol = rdr.ReadElementContentAsString();
                        break;
                    case XmlAtomicityTag:
                        isotopes.Atomicity = rdr.ReadElementContentAsInt();
                        break;
                    case XmlNumIsotopesTag:
                        isotopes.NumberOfIsotopes = rdr.ReadElementContentAsInt();
                        break;
                    case XmlIsotopesTag:
                        var irdr = rdr.ReadSubtree();
                        irdr.MoveToContent();
                        irdr.ReadStartElement();
                        var pos = 0;
                        while (irdr.NodeType != XmlNodeType.EndElement)
                        {
                            switch (irdr.Name)
                            {
                                case XmlIsotopeTag:
                                    var irdr2 = irdr.ReadSubtree();
                                    irdr2.MoveToContent();
                                    irdr2.ReadStartElement();
                                    while (irdr2.NodeType != XmlNodeType.EndElement)
                                    {
                                        switch (irdr2.Name)
                                        {
                                            case XmlMassTag:
                                                isotopes.IsotopeMasses[pos] = irdr2.ReadElementContentAsDouble();
                                                break;
                                            case XmlProbabilityTag:
                                                isotopes.IsotopeProbabilities[pos] = irdr2.ReadElementContentAsDouble();
                                                break;
                                            default:
                                                rdr.Skip();
                                                break;
                                        }
                                    }
                                    isotopes.AverageMass += isotopes.IsotopeMasses[pos] *
                                                            isotopes.IsotopeProbabilities[pos];
                                    pos++;
                                    irdr2.Close();
                                    irdr.ReadEndElement();
                                    break;
                                case XmlMassTag:
                                    rdr.Skip();
                                    break;
                                case XmlProbabilityTag:
                                    rdr.Skip();
                                    break;
                                default:
                                    rdr.Skip();
                                    break;
                            }
                        }
                        irdr.Close();
                        rdr.ReadEndElement();
                        break;
                    case XmlIsotopeTag:
                        rdr.Skip();
                        break;
                    case XmlMassTag:
                        rdr.Skip();
                        break;
                    case XmlProbabilityTag:
                        rdr.Skip();
                        break;
                    default:
                        rdr.Skip();
                        break;
                }
            }
            for (var j = 0; j < isotopes.NumberOfIsotopes; j++)
            {
                var massDiff = isotopes.IsotopeMasses[j] - isotopes.AverageMass;
                isotopes.MassVariance += massDiff * massDiff * isotopes.IsotopeProbabilities[j];
            }
            return isotopes;
        }
#endif

#if !Disable_Obsolete
        [Obsolete("Not used anywhere", true)]
        public void LoadData()
        {
            LoadData(DefaultFileName);
        }
#endif
        
#if !Disable_Obsolete
        [Obsolete("Only used by Decon2LS.UI (maybe)", false)]
        public void WriteData(string fileName)
        {
            var wrtrSettings = new XmlWriterSettings();
            wrtrSettings.CheckCharacters = false;
            wrtrSettings.CloseOutput = true;
            wrtrSettings.Encoding = Encoding.UTF8;
            wrtrSettings.Indent = true;
            wrtrSettings.IndentChars = "  ";
            wrtrSettings.WriteEndDocumentOnClose = true;

            using (var wrtr = XmlWriter.Create(new StreamWriter(
                new FileStream(fileName, FileMode.Create, FileAccess.ReadWrite, FileShare.None)), wrtrSettings))
            {
                wrtr.WriteStartDocument();
                wrtr.WriteStartElement(XmlElementsTag); // Open elements tag
                foreach (var element in ElementalIsotopesList)
                {
                    wrtr.WriteStartElement(XmlElementTag); // Open element tag
                    wrtr.WriteElementString(XmlNameTag, element.Name);
                    wrtr.WriteElementString(XmlSymbolTag, element.Symbol);
                    wrtr.WriteStartElement(XmlAtomicityTag);
                    wrtr.WriteValue(element.Atomicity);
                    wrtr.WriteEndElement();
                    wrtr.WriteStartElement(XmlNumIsotopesTag);
                    wrtr.WriteValue(element.NumberOfIsotopes);
                    wrtr.WriteEndElement();
                    wrtr.WriteStartElement(XmlIsotopesTag);
                    for (var i = 0; i < element.NumberOfIsotopes; i++)
                    {
                        wrtr.WriteStartElement(XmlIsotopeTag); // Open isotope tag
                        wrtr.WriteStartElement(XmlMassTag);
                        wrtr.WriteValue(string.Format("{0:F6}", element.IsotopeMasses[i]));
                        wrtr.WriteEndElement();
                        wrtr.WriteStartElement(XmlProbabilityTag);
                        wrtr.WriteValue(string.Format("{0:F6}", element.IsotopeProbabilities[i]));
                        wrtr.WriteEndElement();
                        wrtr.WriteEndElement(); // Close isotope tag
                    }
                    wrtr.WriteEndElement(); // Close isotopes tag
                    wrtr.WriteEndElement(); // Close element tag
                }
                wrtr.WriteEndElement(); // Close elements tag
            }
        }
#endif

        public int GetNumElements()
        {
            return ElementalIsotopesList.Count;
        }

        public int GetElementIndex(string symbol)
        {
            var numElements = ElementalIsotopesList.Count;
            for (var elementNum = 0; elementNum < numElements; elementNum++)
            {
                if (
                    string.Compare(ElementalIsotopesList[elementNum].Symbol, symbol, StringComparison.InvariantCulture) ==
                    0)
                {
                    // found the element. Return the index.
                    return elementNum;
                }
            }
            return -1;
        }
    }
}