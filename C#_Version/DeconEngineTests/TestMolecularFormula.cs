using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DeconToolsV2;
using NUnit.Framework;

namespace DeconEngineTests
{
    public class TestMolecularFormula
    {
        [Test]
        public void TestParseFormula()
        {
            var formula = "C4.9384 H7.7583 N1.3577 O1.4773 S0.0417";
            var elements = new clsElementIsotopes();
            var molecule = new MolecularFormula(formula, elements);

            var results = molecule.ElementalComposition;
            var carbon = results[0];
            var hydrogen = results[1];
            var nitrogen = results[2];
            var oxygen = results[3];
            var sulfur = results[4];
            Assert.AreEqual(carbon.Index, 5);
            Assert.AreEqual(carbon.NumCopies, 4.9384);
            Assert.AreEqual(hydrogen.Index, 0);
            Assert.AreEqual(hydrogen.NumCopies, 7.7583);
            Assert.AreEqual(nitrogen.Index, 6);
            Assert.AreEqual(nitrogen.NumCopies, 1.3577);
            Assert.AreEqual(oxygen.Index, 7);
            Assert.AreEqual(oxygen.NumCopies, 1.4773);
            Assert.AreEqual(sulfur.Index, 15);
            Assert.AreEqual(sulfur.NumCopies, 0.0417);
        }

        [Test]
        public void TestParseFormulaNew()
        {
            var formula = "C4.9384 H7.7583 N1.3577 O1.4773 S0.0417";
            //string formula = "Ct H7.7583 N1.3577 O1.4773 S0.0417";
            var elements = new clsElementIsotopes();
            //var molecule = new MolecularFormula(formula, elements);
            var molecule = new MolecularFormula();

            molecule.SetMolecularFormula(formula, elements);

            var results = molecule.ElementalComposition;
            var carbon = results[0];
            var hydrogen = results[1];
            var nitrogen = results[2];
            var oxygen = results[3];
            var sulfur = results[4];
            Assert.AreEqual(carbon.Index, 5);
            Assert.AreEqual(carbon.NumCopies, 4.9384);
            Assert.AreEqual(hydrogen.Index, 0);
            Assert.AreEqual(hydrogen.NumCopies, 7.7583);
            Assert.AreEqual(nitrogen.Index, 6);
            Assert.AreEqual(nitrogen.NumCopies, 1.3577);
            Assert.AreEqual(oxygen.Index, 7);
            Assert.AreEqual(oxygen.NumCopies, 1.4773);
            Assert.AreEqual(sulfur.Index, 15);
            Assert.AreEqual(sulfur.NumCopies, 0.0417);
        }
    }
}
