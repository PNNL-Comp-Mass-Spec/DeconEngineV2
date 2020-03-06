using System;
using NUnit.Framework;

namespace DeconEngineTests
{
    public class TestThermoFinnigan
    {
        [Test]
        public void TestThermoRaw()
        {

            // Note: We can only read Thermo .raw file data when compiled as x64

            var thermoReader = new Engine.Readers.ThermoRawData();

            var scanNum = 5000;
            thermoReader.Open(@"\\proto-2\UnitTest_Files\DeconTools_TestFiles\QC_Shew_08_04-pt5-2_11Jan09_Sphinx_08-11-18.RAW");

            var elutionTime = thermoReader.GetScanTime(scanNum);
            var msLevel = thermoReader.GetMSLevel(scanNum);
            var parentIonMz = thermoReader.GetParentMz(scanNum);
            var agcTime = thermoReader.GetAGCAccumulationTime(scanNum);
            var chargeState = thermoReader.GetMonoChargeFromHeader(scanNum);
            var monoisotopicMz = thermoReader.GetMonoMZFromHeader(scanNum);

            Console.WriteLine("{0,-15} {1}", "Scan number:", scanNum);
            Console.WriteLine("{0,-15} {1:F2}", "Elution time:", elutionTime);
            Console.WriteLine("{0,-15} {1:F2}", "Parent Ion m/z:", parentIonMz);
            Console.WriteLine("{0,-15} {1}", "Charge:", chargeState);
            Console.WriteLine("{0,-15} {1:F3}", "Ion injection time (ms):", agcTime);
            Console.WriteLine("{0,-15} {1:F3}", "Monoisotopic m/z:", monoisotopicMz);

            Assert.AreEqual(27.8, elutionTime, 0.01);
            Assert.AreEqual(694.38, parentIonMz, 0.01);
            Assert.AreEqual(2, chargeState);
            Assert.AreEqual(7.523, agcTime, 0.01);
            Assert.AreEqual(694.379, monoisotopicMz, 0.01);


        }
    }
}
