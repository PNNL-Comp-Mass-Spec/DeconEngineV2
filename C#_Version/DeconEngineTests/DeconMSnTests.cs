using NUnit.Framework;

namespace DeconEngineTests
{
    [TestFixture]
    public class DeconMSnTests
    {
        [Test]
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\Shed_PMA_Biotin_400o2000_010203.RAW", "-F500", "-L1000", "-XCDTA")] // LCQ_C3
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\QC_Shew_07_02_0pt5_500mM-a_25Mar07_Doc_SCX_06-09-01.raw", "-F500", "-L1000", "-XCDTA")] // LCQ_D2
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\SysVirol_AI_VN1203_1_12hr_2_12Aug09_Eagle_09-05-24.raw", "-F500", "-L1000", "-XCDTA")] // LTQ_ETD_1
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\QC_Shew_09_02-pt5-d_10Aug09_Griffin_09-07-16.raw", "-F500", "-L1000", "-XCDTA")] // LTQ_2
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\QC_Shew_09_02-pt5_2_17Aug09_Owl_09-05-16.raw", "-F500", "-L1000", "-XCDTA")] // LTQ_4
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\QC_Shew_09_03-pt5-4_10Aug09_Draco_09-05-04.raw", "-F500", "-L1000", "-XCDTA")] // LTQ_Orb_1
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\QC_Shew_09_01-pt5_a_27Apr09_Falcon_09-01-30.Raw", "-F500", "-L1000", "-XCDTA")] // LTQ_Orb_2
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\QC_Shew_09_02_200ng_150m_d_4May09_Hawk_09-01-12.raw", "-F500", "-L1000", "-XCDTA")] // VOrbiETD01
        [TestCase(@"\\proto-2\UnitTest_Files\DeconMSn\blah.raw", "-F500", "-L1000", "-XCDTA")] // Nice message when file doesn't exist, instead of exception
        public void TestRunDeconMSn(string filename, params string[] options)
        {
            // These tests will overwrite the existing files each time
            var args = new string[options.Length + 1];
            for (var i = 0; i < options.Length; i++)
            {
                args[i] = options[i];
            }
            args[options.Length] = filename;
            var deconMSn = new DeconMSn.DeconMSn();
            deconMSn.RunDeconMSn(args);
        }
    }
}
