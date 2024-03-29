using System;
using System.Collections.Generic;
using System.Xml;

namespace DeconToolsV2.DTAGeneration
{
    public enum OUTPUT_TYPE
    {
        DTA = 0,
        MGF,
        LOG,
        CDTA,
        MZXML
    };

    public enum SPECTRA_TYPE
    {
        ALL = 0,
        CID,
        ETD,
        HCD
    };

    /// <summary>
    /// DTA Generation parameters
    /// </summary>
    /// <remarks>Used by DeconMSn</remarks>
    public class clsDTAGenerationParameters
    {
        private readonly List<int> _msnLevelsToIgnore = new List<int>();

        public virtual Object Clone()
        {
            var newParams = new clsDTAGenerationParameters
            {
                CCMass = CCMass,
                ConsiderChargeValue = ConsiderChargeValue,
                MaxMass = MaxMass,
                MaxScan = MaxScan,
                MinMass = MinMass,
                MinScan = (int)MinMass,
                WindowSizeToCheck = WindowSizeToCheck,
                MinIonCount = MinIonCount,
                OutputType = OutputType,
                SpectraType = SpectraType,
                SVMParamFile = SVMParamFile,
                ConsiderMultiplePrecursors = ConsiderMultiplePrecursors,
                CentroidMSn = CentroidMSn,
                IsolationWindowSize = IsolationWindowSize,
                IsProfileDataForMzXML = IsProfileDataForMzXML,
                WriteProgressFile = WriteProgressFile,
                IgnoreMSnScans = IgnoreMSnScans,
                NumMSnLevelsToIgnore = NumMSnLevelsToIgnore,
                CreateLogFileOnly = CreateLogFileOnly
            };

            return newParams;
        }

        public int get_MSnLevelToIgnore(int index)
        {
            return _msnLevelsToIgnore[index];
        }

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void set_MSnLevelToIgnore(int value)
        {
            _msnLevelsToIgnore.Add(value);
        }

        public bool IgnoreMSnScans { get; set; }

        public bool CreateLogFileOnly { get; set; }

        public bool CreateCompositeDTA => OutputType == OUTPUT_TYPE.CDTA;

        public int NumMSnLevelsToIgnore { get; set; }

        public bool IsProfileDataForMzXML { get; set; }

        public int MinScan { get; set; }

        public int MinIonCount { get; set; }

        public int MaxScan { get; set; }

        public double MinMass { get; set; }

        public double MaxMass { get; set; }

        public string SVMParamFile { get; set; }

        public bool ConsiderMultiplePrecursors { get; set; }

        public bool CentroidMSn { get; set; }

        public int IsolationWindowSize { get; set; }

        public int ConsiderChargeValue { get; set; }

        public double CCMass { get; set; }

        public int WindowSizeToCheck { get; set; }

        public bool WriteProgressFile { get; set; }

        public string OutputTypeName
        {
            get
            {
                switch (OutputType)
                {
                    case OUTPUT_TYPE.DTA:
                        return "DTA files";
                    case OUTPUT_TYPE.MGF:
                        return "MGF file";
                    case OUTPUT_TYPE.LOG:
                        return "Log file only";
                    case OUTPUT_TYPE.CDTA:
                        return "CDTA (_dta.txt)";
                    case OUTPUT_TYPE.MZXML:
                        return "MzXML";
                    default:
                        return "Unknown";
                }
            }
        }

        public OUTPUT_TYPE OutputType { get; set; }

        public SPECTRA_TYPE SpectraType { get; set; }

        public clsDTAGenerationParameters()
        {
            MinIonCount = 35;
            MinScan = 1;
            MaxScan = 1000000;
            ConsiderChargeValue = 0;
            WindowSizeToCheck = 5;
            MinMass = 200;
            MaxMass = 5000;
            CCMass = 1.00727638;
            OutputType = OUTPUT_TYPE.CDTA;
            SVMParamFile = "svm_params.xml";
            ConsiderMultiplePrecursors = false;
            CentroidMSn = true;
            IsolationWindowSize = 3;
            IsProfileDataForMzXML = false;
            WriteProgressFile = false;
            IgnoreMSnScans = false;
            NumMSnLevelsToIgnore = 0;
            CreateLogFileOnly = false;
        }

        /*  void clsDTAGenerationParameters.GetMSnLevelToIgnore(ref int[] msnLevelsToIgnore)
        {
            int numLevels = this.NumMSnLevelsToIgnore;
            msnLevelsToIgnore = new int [numLevels];

            for (int levelNum = 0; levelNum < numLevels; levelNum++)
            {
                int level = _msnLevelsToIgnore[levelNum];
                msnLevelsToIgnore[levelNum] = level;
            }
        }*/

        [Obsolete("Only used by Decon2LS.UI", false)]
        public void SaveV1DTAGenerationParameters(XmlTextWriter writer)
        {
            writer.WriteWhitespace("\n\t");
            writer.WriteStartElement("DTAGenerationParameters");
            writer.WriteWhitespace("\n\t\t");
            writer.WriteElementString("MinScan", MinScan.ToString());
            writer.WriteWhitespace("\n\t\t");
            writer.WriteElementString("MaxScan", MaxScan.ToString());
            writer.WriteWhitespace("\n\t\t");
            writer.WriteElementString("MinMass", MinMass.ToString("0.0000"));
            writer.WriteWhitespace("\n\t\t");
            writer.WriteElementString("MaxMass", MaxMass.ToString("0.0000"));
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("MinIonCount", MinIonCount.ToString());
            writer.WriteWhitespace("\n\t\t");

            if (ConsiderChargeValue > 0)
            {
                writer.WriteElementString("ConsiderCharge", "true");
                writer.WriteWhitespace("\n\t\t");
            }
            else
            {
                writer.WriteElementString("ConsiderCharge", "false");
                writer.WriteWhitespace("\n\t\t");
            }
            writer.WriteElementString("ChargeValueToConsider", ConsiderChargeValue.ToString());
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("CCMass", CCMass.ToString("0.0000"));
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("WindowSizeToCheck", WindowSizeToCheck.ToString("0.0000"));
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("ConsiderMultiplePrecursors", ConsiderMultiplePrecursors.ToString());
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("IsolationWindowSize", IsolationWindowSize.ToString());
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("OutputType", OutputType.ToString());
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("IsProfileDataForMzXML", IsProfileDataForMzXML.ToString());
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("WriteProgressFile", WriteProgressFile.ToString());
            writer.WriteWhitespace("\n\t\t");

            writer.WriteElementString("IgnoreMSnScans", IgnoreMSnScans.ToString());

            if (IgnoreMSnScans)
            {
                var numLevels = NumMSnLevelsToIgnore;
                for (var levelNum = 0; levelNum < numLevels; levelNum++)
                {
                    var level = _msnLevelsToIgnore[levelNum];
                    writer.WriteWhitespace("\n\t\t");
                    writer.WriteElementString("MSnLevelToIgnore", level.ToString());
                }
            }
            writer.WriteWhitespace("\n\t");
            writer.WriteEndElement();
        }

        public void LoadV1DTAGenerationParameters(XmlReader rdr)
        {
            while (rdr.Read())
            {
                switch (rdr.NodeType)
                {
                    case XmlNodeType.Element:
                        if (rdr.Name.Equals("MinMass"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for minimum mass in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            MinMass = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MaxMass"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for maximum mass in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            MaxMass = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MinScan"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for minimum scan in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            MinScan = int.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MaxScan"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for maximum scan in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            MaxScan = int.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MinIonCount"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for minimum ion count in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            MinIonCount = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("ChargeValueToConsider"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for considering charge in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            ConsiderChargeValue = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("ConsiderMultiplePrecursors"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for considering multiple precursors");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            ConsiderMultiplePrecursors = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("IsolationWindowSize"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for isolation window size");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            IsolationWindowSize = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("IsProfileDataForMzXML"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information mzXML profile data in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            IsProfileDataForMzXML = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("WriteProgressFile"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information about writing progress file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            WriteProgressFile = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("IgnoreMSnScans"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information about ignoring MSn scans");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            IgnoreMSnScans = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MSnLevelToIgnore"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information about which MSn level to ignore");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            NumMSnLevelsToIgnore++;
                            int level = short.Parse(rdr.Value);
                            _msnLevelsToIgnore.Add(level);
                        }
                        else if (rdr.Name.Equals("CCMass"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for charge carrier mass in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            CCMass = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("WindowSizeToCheck"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for window size to check in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            WindowSizeToCheck = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("OutputType"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new Exception("No information for output type in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            // TODO: can probably use Enum.TryParse()...
                            if (rdr.Value.Equals(OUTPUT_TYPE.DTA.ToString()))
                            {
                                OutputType = OUTPUT_TYPE.DTA;
                            }
                            else if (rdr.Value.Equals(OUTPUT_TYPE.LOG.ToString()))
                            {
                                OutputType = OUTPUT_TYPE.LOG;
                            }
                            else if (rdr.Value.Equals(OUTPUT_TYPE.CDTA.ToString()))
                            {
                                OutputType = OUTPUT_TYPE.CDTA;
                            }
                            else if (rdr.Value.Equals(OUTPUT_TYPE.MGF.ToString()))
                            {
                                OutputType = OUTPUT_TYPE.MGF;
                            }
                        }
                        break;
                    case XmlNodeType.EndElement:
                        if (rdr.Name.Equals("DTAGenerationParameters"))
                            return;
                        break;
                }
            }
        }
    }
}