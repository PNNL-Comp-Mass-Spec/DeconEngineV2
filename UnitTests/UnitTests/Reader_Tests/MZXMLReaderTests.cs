using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using DeconToolsV2.Readers;

namespace UnitTests.Reader_Tests
{
    [TestFixture]
    public class MZXMLReaderTests
    {
        [Test]
        public void Test1()
        {
            string testFile = @"D:\Data\Waters\temp\LC_MS_pHis_Caulo_meth_110207.mzXML";

            DeconToolsV2.Readers.clsRawData rawdata = new DeconToolsV2.Readers.clsRawData(testFile, DeconToolsV2.Readers.FileType.MZXMLRAWDATA);


        }

        [Test]
        public void test2()
        {
            string testFile = @"\\protoapps\UserData\Slysz\DeconTools_TestFiles\mzXML\QC_Shew_08_04-pt5-2_11Jan09_Sphinx_08-11-18.mzXML";

            var rawdata = new clsRawData(testFile, FileType.MZXMLRAWDATA);

        }


    }
}
