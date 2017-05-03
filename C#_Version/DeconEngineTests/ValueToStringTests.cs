using System;
using NUnit.Framework;

namespace DeconEngineTests
{
    class ValueToStringTests
    {
        [Test]
        [TestCase(1, 7, "1")]
        [TestCase(1.2, 7, "1.2")]
        [TestCase(1.23, 7, "1.23")]
        [TestCase(1.242, 7, "1.242")]
        [TestCase(1.2534, 7, "1.2534")]
        [TestCase(1.25343, 7, "1.25343")]
        [TestCase(1.25347, 7, "1.25347")]
        [TestCase(13432, 7, "13432")]
        [TestCase(132.2, 5, "132.2")]
        [TestCase(1322.2, 5, "1322.2")]
        [TestCase(13226.2, 5, "13226")]
        [TestCase(132296.2, 5, "1.323E+05")]
        [TestCase(1322969.2, 5, "1.323E+06")]
        [TestCase(13432.2123, 5, "13432")]
        [TestCase(113432.2242, 5, "1.1343E+05")]
        [TestCase(113432.22534, 5, "1.1343E+05")]
        [TestCase(113432.22534, 9, "1.13432225E+05")]
        [TestCase(1134323423.673, 7, "1.134323E+09")]
        [TestCase(11343234235.87, 7, "1.134323E+10")]
        [TestCase(513432342357.87, 7, "5.134323E+11")]
        public void TestValueToString1(double value, byte digitsOfPrecision, string expectedResult)
        {
            var result = PRISM.StringUtilities.ValueToString(value, digitsOfPrecision, 1e5);

            Console.WriteLine("{0} with {1} digits -> {2}", value, digitsOfPrecision, result);

            Assert.AreEqual(expectedResult, result);
        }

        [Test]
        [TestCase(1, 7, "1")]
        [TestCase(1.2, 7, "1.2")]
        [TestCase(1.23, 7, "1.23")]
        [TestCase(1.242, 7, "1.242")]
        [TestCase(1.2534, 7, "1.2534")]
        [TestCase(1.25343, 7, "1.25343")]
        [TestCase(1.25347, 7, "1.25347")]
        [TestCase(13432, 7, "13432")]
        [TestCase(132.2, 5, "132.2")]
        [TestCase(1322.2, 5, "1322.2")]
        [TestCase(13226.2, 5, "13226")]
        [TestCase(132296.2, 5, "132296")]
        [TestCase(1322969.2, 5, "1322969")]
        [TestCase(13432.2123, 5, "13432")]
        [TestCase(113432.2242, 5, "113432")]
        [TestCase(113432.22534, 5, "113432")]
        [TestCase(113432.22534, 9, "113432.225")]
        [TestCase(1134323423.673, 7, "1134323424")]
        [TestCase(11343234235.87, 7, "1.134323E+10")]
        [TestCase(513432342357.87, 7, "5.134323E+11")]
        public void TestValueToString2(double value, byte digitsOfPrecision, string expectedResult)
        {
            var result = PRISM.StringUtilities.ValueToString(value, digitsOfPrecision, 1e10);

            Console.WriteLine("{0} with {1} digits -> {2}", value, digitsOfPrecision, result);

            Assert.AreEqual(expectedResult, result);

        }
    }
}
