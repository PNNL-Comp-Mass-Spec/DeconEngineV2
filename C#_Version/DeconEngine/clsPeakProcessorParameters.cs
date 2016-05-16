#if !Disable_Obsolete
using System;
using System.Xml;

namespace DeconToolsV2.Peaks
{
    /// <summary>
    /// enumeration for type of fit.
    /// </summary>
    [Obsolete("Not accessed within DeconTools solution except through tests and OldDecon2LSParameters", false)]
    public enum PEAK_FIT_TYPE
    {
        APEX = 0,
        QUADRATIC,
        LORENTZIAN
    };

    [Obsolete("Not accessed within DeconTools solution except through tests and OldDecon2LSParameters", false)]
    public class clsPeakProcessorParameters : System.ICloneable
    {
        private double mdbl_SNThreshold;
        private double mdbl_PeakBackgroundRatio;
        private bool mbln_thresholded_data;
        private PEAK_FIT_TYPE menm_FitType;
        private bool mbln_writePeaksToTextFile;

        public virtual Object Clone()
        {
            clsPeakProcessorParameters new_params = new clsPeakProcessorParameters(mdbl_SNThreshold,
                mdbl_PeakBackgroundRatio, mbln_thresholded_data, menm_FitType);
            return new_params;
        }

        public bool ThresholdedData
        {
            get { return mbln_thresholded_data; }
            set { mbln_thresholded_data = value; }
        }

        public double PeakBackgroundRatio
        {
            get { return mdbl_PeakBackgroundRatio; }
            set { mdbl_PeakBackgroundRatio = value; }
        }

        public double SignalToNoiseThreshold
        {
            get { return mdbl_SNThreshold; }
            set { mdbl_SNThreshold = value; }
        }

        public PEAK_FIT_TYPE PeakFitType
        {
            get { return menm_FitType; }
            set { menm_FitType = value; }
        }

        public bool WritePeaksToTextFile
        {
            get { return mbln_writePeaksToTextFile; }
            set { mbln_writePeaksToTextFile = value; }
        }

        public clsPeakProcessorParameters()
        {
            mdbl_SNThreshold = 3.0;
            mdbl_PeakBackgroundRatio = 5.0;
            menm_FitType = PEAK_FIT_TYPE.QUADRATIC;
        }

        public clsPeakProcessorParameters(double sn, double peak_bg_ratio, bool thresholded_data, PEAK_FIT_TYPE fit_type)
        {
            mdbl_SNThreshold = sn;
            mdbl_PeakBackgroundRatio = peak_bg_ratio;
            menm_FitType = fit_type;
            mbln_thresholded_data = thresholded_data;
        }

        public void SaveV1PeakParameters(System.Xml.XmlTextWriter xwriter)
        {
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteStartElement("PeakParameters");
            xwriter.WriteWhitespace("\n\t\t");

            xwriter.WriteElementString("PeakBackgroundRatio", System.Convert.ToString(this.PeakBackgroundRatio));
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("SignalToNoiseThreshold", this.SignalToNoiseThreshold.ToString());
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("PeakFitType", this.PeakFitType.ToString());
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("WritePeaksToTextFile", this.WritePeaksToTextFile.ToString());

            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteEndElement();
        }

        public void LoadV1PeakParameters(XmlReader rdr)
        {
            //Read each node in the tree.
            while (rdr.Read())
            {
                switch (rdr.NodeType)
                {
                    case XmlNodeType.Element:
                        if (rdr.Name.Equals("PeakBackgroundRatio"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new System.Exception(
                                    "Missing information for PeakBackgroundRatio in parameter file");
                            }
                            this.PeakBackgroundRatio = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("SignalToNoiseThreshold"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new System.Exception(
                                    "Missing information for SignalToNoiseThreshold in parameter file");
                            }
                            this.SignalToNoiseThreshold = double.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("PeakFitType"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new System.Exception(
                                    "Missing information for SignalToNoiseThreshold in parameter file");
                            }
                            if (rdr.Value.Equals(DeconToolsV2.Peaks.PEAK_FIT_TYPE.QUADRATIC.ToString()))
                            {
                                this.PeakFitType = DeconToolsV2.Peaks.PEAK_FIT_TYPE.QUADRATIC;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.Peaks.PEAK_FIT_TYPE.LORENTZIAN.ToString()))
                            {
                                this.PeakFitType = DeconToolsV2.Peaks.PEAK_FIT_TYPE.LORENTZIAN;
                            }
                            else if (rdr.Value.Equals(DeconToolsV2.Peaks.PEAK_FIT_TYPE.APEX.ToString()))
                            {
                                this.PeakFitType = DeconToolsV2.Peaks.PEAK_FIT_TYPE.APEX;
                            }
                        }
                        else if (rdr.Name.Equals("WritePeaksToTextFile"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new System.Exception(
                                    "Missing information for 'WritePeaksToTextFile' parameter in parameter file");
                            }
                            this.WritePeaksToTextFile = bool.Parse(rdr.Value);
                        }
                        break;
                    case XmlNodeType.EndElement:
                        if (rdr.Name.Equals("PeakParameters"))
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