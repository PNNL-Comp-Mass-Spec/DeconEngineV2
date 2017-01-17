#if Enable_Obsolete
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Engine.ChargeDetermination;
using NUnit.Framework;

namespace DeconEngineTests
{
    public class TestLoadSVMXml
    {
        [Test]
        public void LoadSVMXml()
        {
            //var svmXmlFilePath = @"..\..\..\..\ExampleFiles\svm_params.xml";
            var svmXmlFilePath = @"D:\Users\gibb166\Documents\git\DeconEngineCS\ExampleFiles\svm_params.xml";
            var svm = new SVMChargeDetermine();
            svm.SetSVMParamFile(svmXmlFilePath);
            svm.LoadSVMFromXml();
        }
    }
}
#endif