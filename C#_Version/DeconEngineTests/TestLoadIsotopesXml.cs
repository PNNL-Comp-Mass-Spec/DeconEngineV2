#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Engine.TheoreticalProfile;
using NUnit.Framework;

namespace DeconEngineTests
{
    public class TestLoadIsotopesXml
    {
        [Test]
        public void LoadIsotopesXml()
        {
            //var isotopesXmlFilePath = @"..\..\..\..\ExampleFiles\isotope.xml";
            var isotopesXmlFilePath = @"D:\Users\gibb166\Documents\git\DeconEngineCS\ExampleFiles\isotope.xml";
            var atomicInfo = new AtomicInformation();
            atomicInfo.LoadData(isotopesXmlFilePath);
        }

        [Test]
        public void WriteIsotopesXml()
        {
            //var isotopesOutXmlFilePath = @"..\..\..\..\ExampleFiles\isotope_write.xml";
            var isotopesOutXmlFilePath = @"D:\Users\gibb166\Documents\git\DeconEngineCS\ExampleFiles\isotope_write.xml";
            var atomicInfo = new AtomicInformation();
            atomicInfo.WriteData(isotopesOutXmlFilePath);
        }

        [Test]
        public void ReadWriteIsotopesXml()
        {
            //var isotopesXmlFilePath = @"..\..\..\..\ExampleFiles\isotope.xml";
            //var isotopesOutXmlFilePath = @"..\..\..\..\ExampleFiles\isotope_write.xml";
            var isotopesXmlFilePath = @"D:\Users\gibb166\Documents\git\DeconEngineCS\ExampleFiles\isotope.xml";
            var isotopesOutXmlFilePath = @"D:\Users\gibb166\Documents\git\DeconEngineCS\ExampleFiles\isotope_write.xml";
            var atomicInfo = new AtomicInformation();
            atomicInfo.LoadData(isotopesXmlFilePath);
            atomicInfo.WriteData(isotopesOutXmlFilePath);
        }
    }
}
#endif