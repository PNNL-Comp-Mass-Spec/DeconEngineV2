#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.Xml;

namespace DeconToolsV2.DTAGeneration
{
    [Obsolete("Only used by Decon2LS.UI", false)]
    public enum OUTPUT_TYPE
    {
        DTA = 0,
        MGF,
        LOG,
        CDTA,
        MZXML
    };

    [Obsolete("Only used by Decon2LS.UI", false)]
    public enum SPECTRA_TYPE
    {
        ALL = 0,
        CID,
        ETD,
        HCD
    };

    [Obsolete("Only used by Decon2LS.UI", false)]
    public class clsDTAGenerationParameters : System.ICloneable
    {
        private int mint_min_ion_count;
        private int mint_min_scan;
        private int mint_max_scan;
        private double mdbl_min_mass;
        private double mdbl_max_mass;
        private int mint_consider_charge;
        private int mint_window_size_to_check;
        private double mdbl_cc_mass;
        private string mstr_svm_file_name;

        private bool mbln_is_profile_data_for_mzXML;
        private bool mbln_consider_multiple_precursors;
        private bool mbln_centroid_msn;

        private bool mbln_write_progress_file;
        private bool mbln_ignore_msn_scans;
        private int mint_num_msn_levels_to_ignore;

        private List<int> mvect_msn_levels_to_ignore;

        private int mint_isolation_window_size;

        private OUTPUT_TYPE menm_output_type;
        private SPECTRA_TYPE menm_spectra_type;

        public virtual Object Clone()
        {
            clsDTAGenerationParameters new_params = new clsDTAGenerationParameters();

            new_params.CCMass = this.CCMass;
            new_params.ConsiderChargeValue = this.ConsiderChargeValue;
            new_params.MaxMass = this.MaxMass;
            new_params.MaxScan = (int) this.MaxScan;
            new_params.MinMass = this.MinMass;
            new_params.MinScan = (int) this.MinMass;
            new_params.WindowSizetoCheck = this.WindowSizetoCheck;
            new_params.MinIonCount = this.MinIonCount;
            new_params.OutputType = this.OutputType;
            new_params.SpectraType = this.SpectraType;
            new_params.SVMParamFile = this.mstr_svm_file_name;
            new_params.ConsiderMultiplePrecursors = this.ConsiderMultiplePrecursors;
            new_params.CentroidMSn = this.CentroidMSn;
            new_params.IsolationWindowSize = this.IsolationWindowSize;
            new_params.IsProfileDataForMzXML = this.IsProfileDataForMzXML;
            new_params.WriteProgressFile = this.WriteProgressFile;
            new_params.IgnoreMSnScans = this.IgnoreMSnScans;
            new_params.NumMSnLevelsToIgnore = this.NumMSnLevelsToIgnore;

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

        public bool IgnoreMSnScans
        {
            get { return mbln_ignore_msn_scans; }
            set { mbln_ignore_msn_scans = value; }
        }

        public int NumMSnLevelsToIgnore
        {
            get { return mint_num_msn_levels_to_ignore; }
            set { mint_num_msn_levels_to_ignore = value; }
        }

        public bool IsProfileDataForMzXML
        {
            get { return mbln_is_profile_data_for_mzXML; }
            set { mbln_is_profile_data_for_mzXML = value; }
        }

        public int MinScan
        {
            get { return mint_min_scan; }
            set { mint_min_scan = value; }
        }

        public int MinIonCount
        {
            get { return mint_min_ion_count; }
            set { mint_min_ion_count = value; }
        }

        public int MaxScan
        {
            get { return mint_max_scan; }
            set { mint_max_scan = value; }
        }

        public double MinMass
        {
            get { return mdbl_min_mass; }
            set { mdbl_min_mass = value; }
        }

        public double MaxMass
        {
            get { return mdbl_max_mass; }
            set { mdbl_max_mass = value; }
        }

        public string SVMParamFile
        {
            get { return mstr_svm_file_name; }
            set { mstr_svm_file_name = value; }
        }

        public bool ConsiderMultiplePrecursors
        {
            get { return mbln_consider_multiple_precursors; }
            set { mbln_consider_multiple_precursors = value; }
        }

        // Warning: the masses reported by GetMassListFromScanNum when centroiding are not properly calibrated and thus could be off by 0.3 m/z or more
        // See the definition of mbln_centroid_msn for an example
        public bool CentroidMSn
        {
            get { return mbln_centroid_msn; }
            set { mbln_centroid_msn = value; }
        }

        public int IsolationWindowSize
        {
            get { return mint_isolation_window_size; }
            set { mint_isolation_window_size = value; }
        }

        public int ConsiderChargeValue
        {
            get { return mint_consider_charge; }
            set { mint_consider_charge = value; }
        }

        public double CCMass
        {
            get { return mdbl_cc_mass; }
            set { mdbl_cc_mass = value; }
        }

        public int WindowSizetoCheck
        {
            get { return mint_window_size_to_check; }
            set { mint_window_size_to_check = value; }
        }

        public bool WriteProgressFile
        {
            get { return mbln_write_progress_file; }
            set { mbln_write_progress_file = value; }
        }

        public string OutputTypeName
        {
            get
            {
                switch (menm_output_type)
                {
                    case OUTPUT_TYPE.DTA:
                        return "DTA files";
                        break;
                    case OUTPUT_TYPE.MGF:
                        return "MGF file";
                        break;
                    case OUTPUT_TYPE.LOG:
                        return "Log file only";
                        break;
                    case OUTPUT_TYPE.CDTA:
                        return "CDTA (_dta.txt)";
                        break;
                    case OUTPUT_TYPE.MZXML:
                        return "MzXML";
                        break;
                    default:
                        return "Unknown";
                        break;
                }
            }
        }

        public OUTPUT_TYPE OutputType
        {
            get { return menm_output_type; }
            set { menm_output_type = value; }
        }

        public SPECTRA_TYPE SpectraType
        {
            get { return menm_spectra_type; }
            set { menm_spectra_type = value; }
        }

        public clsDTAGenerationParameters()
        {
            mint_min_ion_count = 35;
            mint_min_scan = 1;
            mint_max_scan = 1000000;
            mint_consider_charge = 0;
            mint_window_size_to_check = 5;
            mdbl_min_mass = 200;
            mdbl_max_mass = 5000;
            mdbl_cc_mass = 1.00727638;
            menm_output_type = OUTPUT_TYPE.CDTA;
            menm_output_type = OUTPUT_TYPE.DTA;
            mstr_svm_file_name = "svm_params.xml";
            mbln_consider_multiple_precursors = false;
            mbln_centroid_msn = true;
            mint_isolation_window_size = 3;
            mbln_is_profile_data_for_mzXML = false;
            mbln_write_progress_file = false;
            mbln_ignore_msn_scans = false;
            mint_num_msn_levels_to_ignore = 0;
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
                    int level = mvect_msn_levels_to_ignore[levelNum];
                    xwriter.WriteWhitespace("\n\t\t");
                    xwriter.WriteElementString("MSnLevelToIgnore", System.Convert.ToString(level));

                }
            }
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteEndElement();

        }

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
                            mint_num_msn_levels_to_ignore++;
                            int level = short.Parse(rdr.Value);
                            mvect_msn_levels_to_ignore.Add(level);
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
                            if (rdr.Value.Equals(DeconToolsV2.DTAGeneration.OUTPUT_TYPE.DTA.ToString()))
                            {
                                this.OutputType = DeconToolsV2.DTAGeneration.OUTPUT_TYPE.DTA;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.DTAGeneration.OUTPUT_TYPE.LOG.ToString()))
                            {
                                this.OutputType = DeconToolsV2.DTAGeneration.OUTPUT_TYPE.LOG;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.DTAGeneration.OUTPUT_TYPE.CDTA.ToString()))
                            {
                                this.OutputType = DeconToolsV2.DTAGeneration.OUTPUT_TYPE.CDTA;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.DTAGeneration.OUTPUT_TYPE.MGF.ToString()))
                            {
                                this.OutputType = DeconToolsV2.DTAGeneration.OUTPUT_TYPE.MGF;
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
#endif