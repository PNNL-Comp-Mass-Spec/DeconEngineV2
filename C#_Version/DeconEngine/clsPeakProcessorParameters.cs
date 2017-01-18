using System;
using System.Xml;

namespace DeconToolsV2.Peaks
{
    /// <summary>
    /// enumeration for type of fit.
    /// </summary>
    public enum PEAK_FIT_TYPE
    {
        APEX = 0,
        QUADRATIC,
        LORENTZIAN
    };

    public class clsPeakProcessorParameters : System.ICloneable
    {
        [Obsolete("Not accessed within DeconTools solution except through tests and OldDecon2LSParameters", false)]
        public virtual Object Clone()
        {
            clsPeakProcessorParameters newParams = new clsPeakProcessorParameters
            {
                SignalToNoiseThreshold = this.SignalToNoiseThreshold,
                PeakBackgroundRatio = this.PeakBackgroundRatio,
                PeakFitType = this.PeakFitType,
            };
#if Enable_Obsolete
            newParams.ThresholdedData = ThresholdedData;
#endif
            return newParams;
        }

#if Enable_Obsolete
        [Obsolete("Not accessed within DeconTools solution except through tests and OldDecon2LSParameters", false)]
        public bool ThresholdedData { get; set; }
#endif

        public double PeakBackgroundRatio { get; set; }

        public double SignalToNoiseThreshold { get; set; }

        public PEAK_FIT_TYPE PeakFitType { get; set; }

        public bool WritePeaksToTextFile { get; set; }

        public clsPeakProcessorParameters()
        {
            SignalToNoiseThreshold = 3.0;
            PeakBackgroundRatio = 5.0;
            PeakFitType = PEAK_FIT_TYPE.QUADRATIC;
        }

#if Enable_Obsolete
        [Obsolete("Not accessed within DeconTools solution except through tests and OldDecon2LSParameters", false)]
        public clsPeakProcessorParameters(double sn, double peak_bg_ratio, bool thresholded_data, PEAK_FIT_TYPE fit_type)
        {
            SignalToNoiseThreshold = sn;
            PeakBackgroundRatio = peak_bg_ratio;
            PeakFitType = fit_type;
            ThresholdedData = thresholded_data;
        }

        [Obsolete("Not accessed within DeconTools solution except through tests and OldDecon2LSParameters", false)]
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
#endif

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
