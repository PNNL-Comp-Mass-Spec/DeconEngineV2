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

    public class clsDTAGenerationParameters
    {
        private readonly List<int> _msnLevelsToIgnore = new List<int>();

#if Enable_Obsolete
        [Obsolete("Only used by Decon2LS.UI", false)]
        public clsDTAGenerationParameters Clone()
        {
            clsDTAGenerationParameters newParams = new clsDTAGenerationParameters();

            newParams.CCMass = this.CCMass;
            newParams.ConsiderChargeValue = this.ConsiderChargeValue;
            newParams.MaxMass = this.MaxMass;
            newParams.MaxScan = this.MaxScan;
            newParams.MinMass = this.MinMass;
            newParams.MinScan = (int) this.MinMass;
            newParams.WindowSizetoCheck = this.WindowSizetoCheck;
            newParams.MinIonCount = this.MinIonCount;
            newParams.OutputType = this.OutputType;
            newParams.SpectraType = this.SpectraType;
            newParams.SVMParamFile = this.SVMParamFile;
            newParams.ConsiderMultiplePrecursors = this.ConsiderMultiplePrecursors;
            newParams.CentroidMSn = this.CentroidMSn;
            newParams.IsolationWindowSize = this.IsolationWindowSize;
            newParams.IsProfileDataForMzXML = this.IsProfileDataForMzXML;
            newParams.WriteProgressFile = this.WriteProgressFile;
            newParams.IgnoreMSnScans = this.IgnoreMSnScans;
            newParams.NumMSnLevelsToIgnore = this.NumMSnLevelsToIgnore;

            return newParams;
        }
#endif

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

#if Enable_Obsolete
        [Obsolete("Only used by Decon2LS.UI", false)]
        public void SaveV1DTAGenerationParameters(System.Xml.XmlTextWriter xwriter)
        {
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteStartElement("DTAGenerationParameters");
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MinScan", System.Convert.ToString(this.MinScan));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MaxScan", System.Convert.ToString(this.MaxScan));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MinMass", System.Convert.ToString(this.MinMass));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("MaxMass", System.Convert.ToString(this.MaxMass));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("MinIonCount", System.Convert.ToString(this.MinIonCount));
            xwriter.WriteWhitespace("\n\t\t");

            if (this.ConsiderChargeValue > 0)
            {
                xwriter.WriteElementString("ConsiderCharge", System.Convert.ToString(true));
                xwriter.WriteWhitespace("\n\t\t");
            }
            else
            {
                xwriter.WriteElementString("ConsiderCharge", System.Convert.ToString(false));
                xwriter.WriteWhitespace("\n\t\t");
            }
            xwriter.WriteElementString("ChargeValueToConsider", System.Convert.ToString(this.ConsiderChargeValue));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("CCMass", System.Convert.ToString(this.CCMass));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("WindowSizeToCheck", System.Convert.ToString(this.WindowSizetoCheck));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("ConsiderMultiplePrecursors",
                System.Convert.ToString(this.ConsiderMultiplePrecursors));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("IsolationWindowSize", System.Convert.ToString(this.IsolationWindowSize));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("OutputType", this.OutputType.ToString());
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("IsProfileDataForMzXML", System.Convert.ToString(this.IsProfileDataForMzXML));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("WriteProgressFile", System.Convert.ToString(this.WriteProgressFile));
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("IgnoreMSnScans", System.Convert.ToString(this.IgnoreMSnScans));

            if (this.IgnoreMSnScans)
            {
                int numLevels = this.NumMSnLevelsToIgnore;
                for (int levelNum = 0; levelNum < numLevels; levelNum++)
                {
                    int level = _msnLevelsToIgnore[levelNum];
                    xwriter.WriteWhitespace("\n\t\t");
                    xwriter.WriteElementString("MSnLevelToIgnore", System.Convert.ToString(level));
                }
            }
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteEndElement();
        }
#endif

        public void LoadV1DTAGenerationParameters(System.Xml.XmlReader rdr)
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
                                throw new System.Exception("No information for minimum mass in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.MinMass = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MaxMass"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for maximum mass in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.MaxMass = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MinScan"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for minimum scan in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.MinScan = int.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MaxScan"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for maximum scan in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.MaxScan = int.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MinIonCount"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for minimum ion count in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.MinIonCount = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("ChargeValueToConsider"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for considering charge in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.ConsiderChargeValue = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("ConsiderMultiplePrecursors"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for considering multiple precursors");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.ConsiderMultiplePrecursors = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("IsolationWindowSize"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for isolation window size");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.IsolationWindowSize = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("IsProfileDataForMzXML"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information mzXML profile data in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.IsProfileDataForMzXML = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("WriteProgressFile"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information about writing progress file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.WriteProgressFile = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("IgnoreMSnScans"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information about ignoring MSn scans");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.IgnoreMSnScans = bool.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("MSnLevelToIgnore"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information about which MSn level to ignore");
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
                                throw new System.Exception("No information for charge carrier mass in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.CCMass = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("WindowSizeToCheck"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for window size to check in parameter file");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            this.WindowSizetoCheck = short.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("OutputType"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                throw new System.Exception("No information for output type in parameter file");
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
                                this.OutputType = OUTPUT_TYPE.DTA;
                            }
                            else if (rdr.Value.Equals(OUTPUT_TYPE.LOG.ToString()))
                            {
                                this.OutputType = OUTPUT_TYPE.LOG;
                            }
                            else if (rdr.Value.Equals(OUTPUT_TYPE.CDTA.ToString()))
                            {
                                this.OutputType = OUTPUT_TYPE.CDTA;
                            }
                            else if (rdr.Value.Equals(OUTPUT_TYPE.MGF.ToString()))
                            {
                                this.OutputType = OUTPUT_TYPE.MGF;
                            }
                        }
                        break;
                    case XmlNodeType.EndElement:
                        if (rdr.Name.Equals("DTAGenerationParameters"))
                            return;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
