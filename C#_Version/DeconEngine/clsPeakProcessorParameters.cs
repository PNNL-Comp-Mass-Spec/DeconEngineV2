using System;
using System.Xml;

namespace DeconToolsV2.Peaks
{
    /// <summary>
    /// enumeration for type of fit.
    /// </summary>
    public enum PEAK_FIT_TYPE
    {
        /// <summary>
        ///     The peak is the m/z value higher than the points before and after it
        /// </summary>
        Apex = 0,

        /// <summary>
        ///     The peak is the m/z value which is a quadratic fit of the three points around the apex
        /// </summary>
        Quadratic,

        /// <summary>
        ///     The peak is the m/z value which is a lorentzian fit of the three points around the apex
        /// </summary>
        Lorentzian,

        /// <summary>
        /// Alias: The peak is the m/z value higher than the points before and after it
        /// </summary>
        [Obsolete("Use Apex.", true)]
        APEX = Apex,

        /// <summary>
        /// Alias: The peak is the m/z value which is a quadratic fit of the three points around the apex
        /// </summary>
        [Obsolete("Use Quadratic.", true)]
        QUADRATIC = Quadratic,

        /// <summary>
        /// Alias: The peak is the m/z value which is a lorentzian fit of the three points around the apex
        /// </summary>
        [Obsolete("Use Lorentzian.", true)]
        LORENTZIAN = Lorentzian
    };

    public class clsPeakProcessorParameters
    {
#if Enable_Obsolete
        [Obsolete("Not accessed within DeconTools solution except through tests and OldDecon2LSParameters", false)]
        public clsPeakProcessorParameters Clone()
        {
            var newParams = new clsPeakProcessorParameters
            {
                SignalToNoiseThreshold = this.SignalToNoiseThreshold,
                PeakBackgroundRatio = this.PeakBackgroundRatio,
                PeakFitType = this.PeakFitType,
            };
            newParams.ThresholdedData = ThresholdedData;
            return newParams;
        }

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
            PeakFitType = PEAK_FIT_TYPE.Quadratic;
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
                            if (rdr.Value.ToUpper().Equals(PEAK_FIT_TYPE.Quadratic.ToString().ToUpper()))
                            {
                                this.PeakFitType = PEAK_FIT_TYPE.Quadratic;
                            }
                            else if (rdr.Value.ToUpper().Equals(PEAK_FIT_TYPE.Lorentzian.ToString().ToUpper()))
                            {
                                this.PeakFitType = PEAK_FIT_TYPE.Lorentzian;
                            }
                            else if (rdr.Value.ToUpper().Equals(PEAK_FIT_TYPE.Apex.ToString().ToUpper()))
                            {
                                this.PeakFitType = PEAK_FIT_TYPE.Apex;
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
