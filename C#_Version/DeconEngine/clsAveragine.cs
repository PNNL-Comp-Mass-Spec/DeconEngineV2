using System;
using System.Text;
using Engine.TheoreticalProfile;

namespace DeconToolsV2.HornTransform
{
    public class clsAveragine
    {
        private readonly Averagine _averagine;
        private readonly clsElementIsotopes _elementIsotopes;

        public clsAveragine()
        {
            _averagine = new Averagine();
            _elementIsotopes = new clsElementIsotopes();
            _averagine.SetElementalIsotopeComposition(_elementIsotopes.ElementalIsotopeComposition);
        }

        public clsElementIsotopes ElementIsotopeComposition
        {
            get { return _elementIsotopes; }
            set
            {
                if (value != _elementIsotopes)
                {
                    _elementIsotopes.Assign(value);
                }
                _averagine.SetElementalIsotopeComposition(_elementIsotopes.ElementalIsotopeComposition);
            }
        }

        public double MonoMass
        {
            get { return 0; }
        }

        public double AverageMass
        {
            get { return 0; }
        }

        public string GenerateAveragineFormula(double averageMass, string averagineFormula, string tagFormula)
        {
            _averagine.AveragineFormula = averagineFormula;
            _averagine.TagFormula = tagFormula;
            MolecularFormula empiricalFormula = _averagine.GetAverageFormulaForMass(averageMass);

            var atomicInfo = _elementIsotopes.ElementalIsotopeComposition;

            // Convert to String.
            //string empiricalFormulaStr = "";
            var formulaBuilder = new StringBuilder();
            var numElements = empiricalFormula.ElementalComposition.Count;
            for (var elementNum = 0; elementNum < numElements; elementNum++)
            {
                var atomicCount = empiricalFormula.ElementalComposition[elementNum];
                //empiricalFormulaStr = string.Concat(empiricalFormulaStr, atomicInfo.ElementalIsotopesList[atomicCount.Index].Symbol, System.Convert.ToString(Convert.ToInt32(atomicCount.NumCopies)));
                formulaBuilder.Append(atomicInfo.ElementalIsotopesList[atomicCount.Index].Symbol);
                formulaBuilder.Append(Convert.ToInt32(atomicCount.NumCopies));
            }
            //return empiricalFormulaStr;
            return formulaBuilder.ToString();
        }
    }
}