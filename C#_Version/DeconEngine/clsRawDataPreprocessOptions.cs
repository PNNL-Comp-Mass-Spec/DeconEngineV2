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
        // Type 9: This is to support Bruker calmet 1
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

    /// <summary>
    /// Raw preprocessor options
    /// </summary>
    /// <remarks>Used by  DeconTools for ICR2LSRun and IMFRun; also used by DeconMSn</remarks>
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

        public ApodizationType ApodizationType
        {
            get => menmApodizationType;
            set => menmApodizationType = value;
        }

        public bool IsToBePreprocessed => menmApodizationType != ApodizationType.NOAPODIZATION ||
                                          menmCalibrationType != CalibrationType.UNDEFINED;

        public double ApodizationMinX
        {
            get => mdbl_apodization_min_x;
            set => mdbl_apodization_min_x = value;
        }

        public double ApodizationMaxX
        {
            get => mdbl_apodization_max_x;
            set => mdbl_apodization_max_x = value;
        }

        public int ApodizationPercent
        {
            get => mintApodizationPercent;
            set => mintApodizationPercent = value;
        }

        public short NumZeroFills
        {
            get => mshort_num_zeros;
            set => mshort_num_zeros = value;
        }

        public bool ApplyCalibration => menmCalibrationType != CalibrationType.UNDEFINED;

        public CalibrationType CalibrationType
        {
            get => menmCalibrationType;
            set => menmCalibrationType = value;
        }

        public double A
        {
            get => mdbl_A;
            set => mdbl_A = value;
        }

        public double B
        {
            get => mdbl_B;
            set => mdbl_B = value;
        }

        public double C
        {
            get => mdbl_C;
            set => mdbl_C = value;
        }

        public void SaveV1FTICRPreProcessOptions(XmlTextWriter xwriter)
        {
            xwriter.WriteStartElement("FTICRRawPreProcessingOptions");
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationType", ApodizationType.ToString());
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationMinX", Convert.ToString(ApodizationMinX));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationMaxX", Convert.ToString(ApodizationMaxX));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("ApodizationPercent", Convert.ToString(ApodizationPercent));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("NumZeroFills", Convert.ToString(NumZeroFills));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteStartElement("Calibration");
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("ApplyCalibration", Convert.ToString(ApplyCalibration));
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("CalibrationType", CalibrationType.ToString());
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("A", Convert.ToString(A));
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("B", Convert.ToString(B));
            xwriter.WriteWhitespace("\n\t\t\t");
            xwriter.WriteElementString("C", Convert.ToString(C));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteEndElement();
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteEndElement();
            xwriter.WriteWhitespace("\n\t");
        }

        public Object Clone()
        {
            var new_params = new clsRawDataPreprocessOptions();
            new_params.ApodizationMaxX = ApodizationMaxX;
            new_params.ApodizationMinX = ApodizationMinX;
            new_params.ApodizationPercent = ApodizationPercent;
            new_params.ApodizationType = ApodizationType;
            new_params.NumZeroFills = NumZeroFills;
            new_params.CalibrationType = CalibrationType;
            new_params.A = A;
            new_params.B = B;
            new_params.C = C;
            return new_params;
        }

        public void LoadV1FTICRPreProcessOptions(XmlReader rdr)
        {
            //Add code to handle empty nodes.
            var readingCalibrations = false;
            //Read each node in the tree.
            while (rdr.Read())
            {
                switch (rdr.NodeType)
                {
                    case XmlNodeType.Element:
                        if (rdr.Name.Equals("ApodizationType"))
                        {
                            if (rdr.IsEmptyElement)
                            {
                                ApodizationType = ApodizationType.NOAPODIZATION;
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
                                throw new Exception("Missing information for ApodizationType in parameter file");
                            }
                            if (rdr.Value.Equals(ApodizationType.HANNING.ToString()))
                            {
                                ApodizationType = ApodizationType.HANNING;
                            }
                            else if (rdr.Value.Equals(ApodizationType.PARZEN.ToString()))
                            {
                                ApodizationType = ApodizationType.PARZEN;
                            }
                            else if (rdr.Value.Equals(ApodizationType.SQUARE.ToString()))
                            {
                                ApodizationType = ApodizationType.SQUARE;
                            }
                            else if (rdr.Value.Equals(ApodizationType.TRIANGLE.ToString()))
                            {
                                ApodizationType = ApodizationType.TRIANGLE;
                            }
                            else if (rdr.Value.Equals(ApodizationType.WELCH.ToString()))
                            {
                                ApodizationType = ApodizationType.WELCH;
                            }
                            else
                            {
                                ApodizationType = ApodizationType.NOAPODIZATION;
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
                                ApodizationMinX = double.Parse(rdr.Value);
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
                                ApodizationMaxX = double.Parse(rdr.Value);
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
                                ApodizationPercent = int.Parse(rdr.Value);
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
                                NumZeroFills = short.Parse(rdr.Value);
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
                                throw new Exception("No value for calibration constant A was specified in parameter file");
                            }
                            else
                            {
                                A = double.Parse(rdr.Value);
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
                                B = double.Parse(rdr.Value);
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
                                C = double.Parse(rdr.Value);
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

                            var type = typeof (CalibrationType);
                            //System.Type.GetType("DeconToolsV2.Readers.CalibrationType");
                            var calibrationType =
                                (CalibrationType) Enum.Parse(type, rdr.Value);
                            CalibrationType = calibrationType;
                        }
                        else if (rdr.Name.Equals("Calibration"))
                        {
                            readingCalibrations = true;
                        }
                        break;
                    case XmlNodeType.EndElement:
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