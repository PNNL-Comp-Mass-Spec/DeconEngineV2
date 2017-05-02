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
    public class clsDTAGenerationParameters : ICloneable
    {
        private readonly List<int> mvect_msn_levels_to_ignore;

        public virtual Object Clone()
        {
            var new_params = new clsDTAGenerationParameters
            {
                CCMass = CCMass,
                ConsiderChargeValue = ConsiderChargeValue,
                MaxMass = MaxMass,
                MaxScan = (int)MaxScan,
                MinMass = MinMass,
                MinScan = (int)MinMass,
                WindowSizetoCheck = WindowSizetoCheck,
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
                NumMSnLevelsToIgnore = NumMSnLevelsToIgnore
            };


            return new_params;
        }

        public int get_MSnLevelToIgnore(int index)
        {
            return mvect_msn_levels_to_ignore[index];
        }

        public void set_MSnLevelToIgnore(int value)
        {
            mvect_msn_levels_to_ignore.Add(value);
        }

        public bool IgnoreMSnScans { get; set; }

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

        public int WindowSizetoCheck { get; set; }

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
            WindowSizetoCheck = 5;
            MinMass = 200;
            MaxMass = 5000;
            CCMass = 1.00727638;
            OutputType = OUTPUT_TYPE.CDTA;
            OutputType = OUTPUT_TYPE.DTA;
            SVMParamFile = "svm_params.xml";
            ConsiderMultiplePrecursors = false;
            CentroidMSn = true;
            IsolationWindowSize = 3;
            IsProfileDataForMzXML = false;
            WriteProgressFile = false;
            IgnoreMSnScans = false;
            NumMSnLevelsToIgnore = 0;
            mvect_msn_levels_to_ignore = new List<int>();
        }

        /*  void clsDTAGenerationParameters.GetMSnLevelToIgnore(ref int[] msnLevelsToIgnore)
        {
            int numLevels = this.NumMSnLevelsToIgnore;
            msnLevelsToIgnore = new int [numLevels];

            for (int levelNum = 0; levelNum < numLevels; levelNum++)
            {
                int level = mvect_msn_levels_to_ignore[levelNum];
                msnLevelsToIgnore[levelNum] = level;
            }
        }*/

        public void SaveV1DTAGenerationParameters(XmlTextWriter xwriter)
        {
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteStartElement("DTAGenerationParameters");
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MinScan", MinScan.ToString());
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MaxScan", MaxScan.ToString());
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MinMass", MinMass.ToString("0.0000"));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MaxMass", MaxMass.ToString("0.0000"));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("MinIonCount", MinIonCount.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            if (ConsiderChargeValue > 0)
            {
                xwriter.WriteElementString("ConsiderCharge", "true");
                xwriter.WriteWhitespace("\n\t\t");
            }
            else
            {
                xwriter.WriteElementString("ConsiderCharge", "false");
                xwriter.WriteWhitespace("\n\t\t");
            }
            xwriter.WriteElementString("ChargeValueToConsider", ConsiderChargeValue.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("CCMass", CCMass.ToString("0.0000"));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("WindowSizeToCheck", WindowSizetoCheck.ToString("0.0000"));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("ConsiderMultiplePrecursors", ConsiderMultiplePrecursors.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("IsolationWindowSize", IsolationWindowSize.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("OutputType", OutputType.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("IsProfileDataForMzXML", IsProfileDataForMzXML.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("WriteProgressFile", WriteProgressFile.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("IgnoreMSnScans", IgnoreMSnScans.ToString());

            if (IgnoreMSnScans)
            {
                var numLevels = NumMSnLevelsToIgnore;
                for (var levelNum = 0; levelNum < numLevels; levelNum++)
                {
                    var level = mvect_msn_levels_to_ignore[levelNum];
                    xwriter.WriteWhitespace("\n\t\t");
                    xwriter.WriteElementString("MSnLevelToIgnore", level.ToString());

                }
            }
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteEndElement();

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
                            mvect_msn_levels_to_ignore.Add(level);
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
                            WindowSizetoCheck = short.Parse(rdr.Value);
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