#if !Disable_Obsolete
using NUnit.Framework;

namespace DeconEngineTests
{
    public class TestThermoFinnigan
    {
        [Test]
        public void TestFinnigan()
        {
            var finReader = new Engine.Readers.FinniganRawData();
            int scan = 100;
            finReader.Open(@"E:\DeconMSn_data\QC_Shew_13_07-500ng_4a_01Sept14_Falcon_14-07-07.raw");
            double temp = finReader.GetAGCAccumulationTime(scan);
            System.Console.WriteLine(temp);
            temp = finReader.GetMonoChargeFromHeader(scan);
            System.Console.WriteLine(temp);
            temp = finReader.GetMonoMZFromHeader(scan);
            System.Console.WriteLine(temp);
        }
    }
}
#endif