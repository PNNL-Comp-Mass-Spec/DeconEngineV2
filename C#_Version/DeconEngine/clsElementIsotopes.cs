using System;
using System.Globalization;
using System.Xml;
using Engine.TheoreticalProfile;

namespace DeconToolsV2
{
    public class clsElementIsotopes : ICloneable
    {
        internal AtomicInformation AtomicInfo;

        public clsElementIsotopes()
        {
            AtomicInfo = new AtomicInformation();
        }

#if !Disable_Obsolete
        [Obsolete("Only used by Decon2LS.UI (maybe)", false)]
        public clsElementIsotopes(string fileName)
        {
            AtomicInfo = new AtomicInformation();
            try
            {
                Load(fileName);
            }
            catch (Exception e)
            {
                throw e;
            }
        }
#endif

        internal AtomicInformation ElementalIsotopeComposition
        {
            get { return AtomicInfo; }
            set { AtomicInfo = value; }
        }

        public virtual object Clone()
        {
            var elemIsotopes = new clsElementIsotopes();
            elemIsotopes.AtomicInfo = new AtomicInformation(ElementalIsotopeComposition);
            return elemIsotopes;
        }

        public virtual clsElementIsotopes Assign(clsElementIsotopes otherOne)
        {
            AtomicInfo = new AtomicInformation(otherOne.ElementalIsotopeComposition);
            return this;
        }

#if !Disable_Obsolete
        [Obsolete("Only used by Decon2LS.UI (maybe)", false)]
        public void Load(string fileName)
        {
            AtomicInfo.LoadData(fileName);
        }
#endif

#if !Disable_Obsolete
        [Obsolete("Only used by Decon2LS.UI (maybe)", false)]
        public void Write(string fileName)
        {
            AtomicInfo.WriteData(fileName);
        }
#endif

        public int GetNumberOfElements()
        {
            return AtomicInfo.ElementalIsotopesList.Count;
        }

        public void GetElementalIsotope(int index, ref int atomicity, ref int numIsotopes, ref string elementName,
            ref string elementSymbol, ref float averageMass, ref float massVariance, ref float[] isotopeMass,
            ref float[] isotopeProb)
        {
            GetElementalIsotopeOut(index, out atomicity, out numIsotopes, out elementName, out elementSymbol,
                out averageMass, out massVariance, out isotopeMass, out isotopeProb);
        }

        public void GetElementalIsotopeOut(int index, out int atomicity, out int numIsotopes, out string elementName,
            out string elementSymbol, out float averageMass, out float massVariance, out float[] isotopeMass,
            out float[] isotopeProb)
        {
            var elementIsotopes = AtomicInfo.ElementalIsotopesList[index];
            atomicity = elementIsotopes.Atomicity;
            numIsotopes = elementIsotopes.NumberOfIsotopes;
            elementName = elementIsotopes.Name;
            elementSymbol = elementIsotopes.Symbol;
            averageMass = (float) elementIsotopes.AverageMass;
            massVariance = (float) elementIsotopes.MassVariance;

            isotopeMass = new float[numIsotopes];
            isotopeProb = new float[numIsotopes];
            for (var isotopeNum = 0; isotopeNum < numIsotopes; isotopeNum++)
            {
                isotopeMass[isotopeNum] = (float) elementIsotopes.IsotopeMasses[isotopeNum];
                isotopeProb[isotopeNum] = (float) elementIsotopes.IsotopeProbabilities[isotopeNum];
            }
        }

        public void UpdateElementalIsotope(int index, ref int atomicity, ref int isotopeNum, ref string elementName,
            ref string elementSymbol, ref double isotopeMass, ref double isotopeProb)
        {
            var elementIsotopes = AtomicInfo.ElementalIsotopesList[index];
            elementIsotopes.Atomicity = atomicity;
            elementName = elementIsotopes.Name;
            elementSymbol = elementIsotopes.Symbol;
            elementIsotopes.IsotopeMasses[isotopeNum] = isotopeMass;
            elementIsotopes.IsotopeProbabilities[isotopeNum] = isotopeProb;
            AtomicInfo.ElementalIsotopesList[index] = elementIsotopes;
            elementIsotopes.AverageMass = 0;
            for (var isoNum = 0; isoNum < elementIsotopes.NumberOfIsotopes; isoNum++)
                elementIsotopes.AverageMass += elementIsotopes.IsotopeMasses[isoNum] *
                                               elementIsotopes.IsotopeProbabilities[isoNum];
        }

#if !Disable_Obsolete
        [Obsolete("Only used by OldDeconToolsParameters", false)]
        public void SaveV1ElementIsotopes(XmlTextWriter xwriter)
        {
            var elementSymbol = "";
            var elementName = "";
            var atomicity = 0;
            var numIsotopes = 0;
            var isotopeMass = new float[1];
            var isotopeProbability = new float[1];
            float averageMass = 0;
            float massVariance = 0;

            var numElements = GetNumberOfElements();

            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteStartElement("ElementIsotopes");
            xwriter.WriteWhitespace("\n\t\t");
            xwriter.WriteElementString("NumElements", numElements.ToString());

            for (var elementNum = 0; elementNum < numElements; elementNum++)
            {
                GetElementalIsotope(elementNum, ref atomicity, ref numIsotopes, ref elementName, ref elementSymbol,
                    ref averageMass, ref massVariance, ref isotopeMass, ref isotopeProbability);

                xwriter.WriteWhitespace("\n\t\t");
                xwriter.WriteStartElement("Element");
                xwriter.WriteWhitespace("\n\t\t\t");
                xwriter.WriteElementString("Symbol", elementSymbol);
                xwriter.WriteWhitespace("\n\t\t\t");
                xwriter.WriteElementString("Name", elementName);
                xwriter.WriteWhitespace("\n\t\t\t");
                xwriter.WriteElementString("NumIsotopes", numIsotopes.ToString());
                xwriter.WriteWhitespace("\n\t\t\t");
                xwriter.WriteElementString("Atomicity", atomicity.ToString());

                for (var isotopeNum = 0; isotopeNum < numIsotopes; isotopeNum++)
                {
                    xwriter.WriteWhitespace("\n\t\t\t");
                    xwriter.WriteStartElement("Isotope");
                    xwriter.WriteWhitespace("\n\t\t\t\t");
                    xwriter.WriteElementString("Mass", isotopeMass[isotopeNum].ToString(CultureInfo.InvariantCulture));
                    xwriter.WriteWhitespace("\n\t\t\t\t");
                    xwriter.WriteElementString("Probability", isotopeProbability[isotopeNum].ToString(CultureInfo.InvariantCulture));
                    xwriter.WriteWhitespace("\n\t\t\t");
                    xwriter.WriteEndElement();
                }
                xwriter.WriteWhitespace("\n\t\t");
                xwriter.WriteEndElement();
            }
            xwriter.WriteWhitespace("\n\t");
            xwriter.WriteEndElement();
            xwriter.WriteWhitespace("\n");
        }
#endif

#if !Disable_Obsolete
        [Obsolete("Only used by OldDeconToolsParameters", false)]
        public void LoadV1ElementIsotopes(XmlReader rdr)
        {
            var atomicity = 0;
            var isotopeNum = 0;
            var elementNum = 0;
            double isotopeMass = 0;
            double isotopeProbability = 0;
            var symbol = "";
            var name = "";
            //Read each node in the tree.
            while (rdr.Read())
            {
                switch (rdr.NodeType)
                {
                    case XmlNodeType.Element:
                        if (rdr.Name.Equals("NumElements"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception(
                                    "Missing information for number of elements in parameter file");
                            }
                            isotopeNum = 0;
                        }
                        else if (rdr.Name.Equals("Element"))
                        {
                            isotopeNum = 0;
                        }
                        else if (rdr.Name.Equals("NumIsotopes"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("Missing information for NumIsotopes in parameter file");
                            }
                        }
                        else if (rdr.Name.Equals("Atomicity"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("Missing information for Atomicity in parameter file");
                            }
                            atomicity = int.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("Symbol"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("Missing information for Symbol in parameter file");
                            }
                            symbol = rdr.Value;
                        }
                        else if (rdr.Name.Equals("Name"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("Missing information for Element Name in parameter file");
                            }
                            name = rdr.Value;
                        }
                        else if (rdr.Name.Equals("Isotope"))
                        {
                            isotopeMass = 0;
                            isotopeProbability = 0;
                        }
                        else if (rdr.Name.Equals("Mass"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("Missing information for Mass in parameter file");
                            }
                            isotopeMass = float.Parse(rdr.Value);
                        }
                        else if (rdr.Name.Equals("Probability"))
                        {
                            rdr.Read();
                            while (rdr.NodeType == XmlNodeType.Whitespace ||
                                   rdr.NodeType == XmlNodeType.SignificantWhitespace)
                            {
                                rdr.Read();
                            }
                            if (rdr.NodeType != XmlNodeType.Text)
                            {
                                throw new Exception("Missing information for Probability in parameter file");
                            }
                            isotopeProbability = float.Parse(rdr.Value);
                        }
                        break;
                    case XmlNodeType.EndElement:
                        if (rdr.Name.Equals("Isotope"))
                        {
                            UpdateElementalIsotope(elementNum, ref atomicity, ref isotopeNum, ref name, ref symbol,
                                ref isotopeMass, ref isotopeProbability);
                            isotopeNum++;
                        }
                        else if (rdr.Name.Equals("Element"))
                        {
                            /*Console.Write(System.Convert.ToString(element_num));
                        Console.WriteLine(" ");
                        Console.WriteLine(name); */
                            elementNum++;
                        }
                        else if (rdr.Name.Equals("ElementIsotopes"))
                        {
                            return;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
#endif
    }
}