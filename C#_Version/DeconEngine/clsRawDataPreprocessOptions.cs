#if !Disable_Obsolete
using System;
using System.Xml;

namespace DeconToolsV2.Readers
{
    public enum ApodizationType
    {
        SQUARE = 0,
        PARZEN,
        HANNING,
        WELCH,
        TRIANGLE,
        NOAPODIZATION
    }

    public enum CalibrationType
    {
        // Type 0: m/z = A/f + B/f^2 + C/f^3
        // Type 1: m/z = A/f + |Vt|B/f^2
        // Type 2: m/z = A/f + |Vt|B/f^2 + I|Vt|C/f^2
        // Type 3: m/z = A/f + |Vt|B/f^2 + C
        // Type 4: m/z = f
        // Type 5: m/z = A/(f+B)
        // Type 6: m/z = A/(f+B+CI)
        // Type 7: t = A*t^2 + B*t + C
        // Type 9: This is to support bruker calmet 1
        //         m/z = (-A - SQRT(A^2 - 4(B-f)C))/2(B-f)
        //         f   = A/mz + C/mz^2 +B
        A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C_OVERFCUBE = 0,
        A_OVER_F_PLUS_B_OVER_FSQ,
        A_OVER_F_PLUS_B_OVER_FSQ_PLUS_CI_OVERFSQ,
        A_OVER_F_PLUS_B_OVER_FSQ_PLUS_C,
        AF_PLUS_B,
        F,
        A_OVER_F_PLUS_B,
        A_OVER_F_PLUS_B_PLUS_CI,
        TIME_A_TSQ_PLUS_B_T_PLUS_C,
        BRUKER_CALMET,
        UNDEFINED
    }

    [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
    public class clsRawDataPreprocessOptions
    {
        private ApodizationType menmApodizationType;
        private CalibrationType menmCalibrationType;
        private int mintApodizationPercent;
        private double mdbl_apodization_min_x;
        private double mdbl_apodization_max_x;
        private short mshort_num_zeros;
        private double mdbl_A;
        private double mdbl_B;
        private double mdbl_C;

        public clsRawDataPreprocessOptions()
        {
            menmApodizationType = ApodizationType.NOAPODIZATION;
            mintApodizationPercent = 50;
            mdbl_apodization_min_x = 0;
            mdbl_apodization_max_x = 0.9437166;
            mshort_num_zeros = 0;
            menmCalibrationType = CalibrationType.UNDEFINED;
            mdbl_A = 0;
            mdbl_B = 0;
            mdbl_C = 0;
        }

        public DeconToolsV2.Readers.ApodizationType ApodizationType
        {
            get { return menmApodizationType; }
            set { menmApodizationType = value; }
        }

        public bool IsToBePreprocessed
        {
            get
            {
                return menmApodizationType != ApodizationType.NOAPODIZATION ||
                       menmCalibrationType != CalibrationType.UNDEFINED;
            }
        }

        public double ApodizationMinX
        {
            get { return mdbl_apodization_min_x; }
            set { mdbl_apodization_min_x = value; }
        }

        public double ApodizationMaxX
        {
            get { return mdbl_apodization_max_x; }
            set { mdbl_apodization_max_x = value; }
        }

        public int ApodizationPercent
        {
            get { return mintApodizationPercent; }
            set { mintApodizationPercent = value; }
        }

        public short NumZeroFills
        {
            get { return mshort_num_zeros; }
            set { mshort_num_zeros = value; }
        }

        public bool ApplyCalibration
        {
            get { return menmCalibrationType != CalibrationType.UNDEFINED; }
        }

        public DeconToolsV2.Readers.CalibrationType CalibrationType
        {
            get { return menmCalibrationType; }
            set { menmCalibrationType = value; }
        }

        public double A
        {
            get { return mdbl_A; }
            set { mdbl_A = value; }
        }

        public double B
        {
            get { return mdbl_B; }
            set { mdbl_B = value; }
        }

        public double C
        {
            get { return mdbl_C; }
            set { mdbl_C = value; }
        }

        public void SaveV1FTICRPreProcessOptions(System.Xml.XmlTextWriter xwriter)
        {
            xwriter.WriteStartElement("FTICRRawPreProcessingOptions");
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationType", this.ApodizationType.ToString());
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationMinX", Convert.ToString(this.ApodizationMinX));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationMaxX", Convert.ToString(this.ApodizationMaxX));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationPercent", Convert.ToString(this.ApodizationPercent));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("NumZeroFills", Convert.ToString(this.NumZeroFills));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteStartElement("Calibration");
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("ApplyCalibration", Convert.ToString(this.ApplyCalibration));
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("CalibrationType", this.CalibrationType.ToString());
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("A", Convert.ToString(this.A));
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("B", Convert.ToString(this.B));
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("C", Convert.ToString(this.C));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteEndElement();
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteEndElement();
            xwriter.WriteWhitespace("\n\t");
        }

        public Object Clone()
        {
            clsRawDataPreprocessOptions new_params = new clsRawDataPreprocessOptions();
            new_params.ApodizationMaxX = this.ApodizationMaxX;
            new_params.ApodizationMinX = this.ApodizationMinX;
            new_params.ApodizationPercent = this.ApodizationPercent;
            new_params.ApodizationType = this.ApodizationType;
            new_params.NumZeroFills = this.NumZeroFills;
            new_params.CalibrationType = this.CalibrationType;
            new_params.A = this.A;
            new_params.B = this.B;
            new_params.C = this.C;
            return new_params;
        }

        public void LoadV1FTICRPreProcessOptions(System.Xml.XmlReader rdr)
        {
            //Add code to handle empty nodes.
            bool readingCalibrations = false;
            //Read each node in the tree.
            while (rdr.Read())
            {
                switch (rdr.NodeType)
                {
                    case System.Xml.XmlNodeType.Element:
                        if (rdr.Name.Equals("ApodizationType"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                this.ApodizationType = ApodizationType.NOAPODIZATION;
                                continue;
                            }

                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new System.Exception("Missing information for ApodizationType in parameter file");
                            }
                            if (rdr.Value.Equals(DeconToolsV2.Readers.ApodizationType.HANNING.ToString()))
                            {
                                this.ApodizationType = DeconToolsV2.Readers.ApodizationType.HANNING;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.Readers.ApodizationType.PARZEN.ToString()))
                            {
                                this.ApodizationType = DeconToolsV2.Readers.ApodizationType.PARZEN;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.Readers.ApodizationType.SQUARE.ToString()))
                            {
                                this.ApodizationType = DeconToolsV2.Readers.ApodizationType.SQUARE;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.Readers.ApodizationType.TRIANGLE.ToString()))
                            {
                                this.ApodizationType = DeconToolsV2.Readers.ApodizationType.TRIANGLE;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.Readers.ApodizationType.WELCH.ToString()))
                            {
                                this.ApodizationType = DeconToolsV2.Readers.ApodizationType.WELCH;
                            }
                            else
                            {
                                this.ApodizationType = DeconToolsV2.Readers.ApodizationType.NOAPODIZATION;
                            }
                        }
                        else if (rdr.Name.Equals("ApodizationMinX"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("No Apodization Min X value was specified in parameter file");
                            }
                            else
                            {
                                this.ApodizationMinX = double.Parse(rdr.Value);
                            }
                        }
                        else if (rdr.Name.Equals("ApodizationMaxX"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("No Apodization Max X value was specified in parameter file");
                            }
                            else
                            {
                                this.ApodizationMaxX = double.Parse(rdr.Value);
                            }
                        }
                        else if (rdr.Name.Equals("ApodizationPercent"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("No ApodizationPercent value was specified in parameter file");
                            }
                            else
                            {
                                this.ApodizationPercent = int.Parse(rdr.Value);
                            }
                        }
                        else if (rdr.Name.Equals("NumZeroFills"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("No NumZeroFills value was specified in parameter file");
                            }
                            else
                            {
                                this.NumZeroFills = short.Parse(rdr.Value);
                            }
                        }
                        else if (rdr.Name.Equals("A"))
                        {
                            if (!readingCalibrations)
                            {
                                throw new Exception("Node A, needs to be inside node: Calibration.");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("No value for calibration constant Awas specified in parameter file");
                            }
                            else
                            {
                                this.A = double.Parse(rdr.Value);
                            }
                        }
                        else if (rdr.Name.Equals("B"))
                        {
                            if (!readingCalibrations)
                            {
                                throw new Exception("Node B, needs to be inside node: Calibration.");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception(
                                    "No value for calibration constant B was specified in parameter file");
                            }
                            else
                            {
                                this.B = double.Parse(rdr.Value);
                            }
                        }
                        else if (rdr.Name.Equals("C"))
                        {
                            if (!readingCalibrations)
                            {
                                throw new Exception("Node C, needs to be inside node: Calibration.");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception(
                                    "No value for calibration constant C was specified in parameter file");
                            }
                            else
                            {
                                this.C = double.Parse(rdr.Value);
                            }
                        }
                        else if (rdr.Name.Equals("CalibrationType"))
                        {
                            if (!readingCalibrations)
                            {
                                throw new Exception("Node CalibrationType, needs to be inside node: Calibration.");
                            }
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }

                            System.Type type = typeof (DeconToolsV2.Readers.CalibrationType);
                            //System.Type.GetType("DeconToolsV2.Readers.CalibrationType");
                            DeconToolsV2.Readers.CalibrationType calibrationType =
                                (DeconToolsV2.Readers.CalibrationType) Enum.Parse(type, rdr.Value);
                            this.CalibrationType = calibrationType;
                        }
                        else if (rdr.Name.Equals("Calibration"))
                        {
                            readingCalibrations = true;
                        }
                        break;
                    case System.Xml.XmlNodeType.EndElement:
                        if (rdr.Name.Equals("HornTransformParameters"))
                            return;
                        if (!rdr.Name.Equals("Calibration"))
                            break;
                        else
                        {
                            // calibration read.
                            readingCalibrations = false;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
#endif