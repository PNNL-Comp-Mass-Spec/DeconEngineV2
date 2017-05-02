#if Enable_Obsolete
using System;
using System.IO;
using System.Text;

namespace DeconToolsV2
{
    [Obsolete("Not used within the DeconTools big solution", false)]
    public class clsTransformFile
    {
        public void ReadFile(string file_name, DeconToolsV2.Results.clsTransformResults results,
            Peaks.clsPeakProcessorParameters peak_parameters,
            HornTransform.clsHornTransformParameters transform_parameters)
        {
        }

        private void WritePeakParameters(TextWriter file, Peaks.clsPeakProcessorParameters peak_parameters)
        {
            // In a wierd pseudo xml/raw file format, we will write out tags in text
            // but keep all the data in binary.
            // We will not use a specialized SAX parser because our vocabulary on the XML file
            // will not be increasing to a point where we actually want to go through the effort
            // of incorporating an XML parser with our binary data.
            if (peak_parameters != null)
            {
                file.Write("<PeakParameters>");
                file.Write("<MinBackgroundRatio>" + peak_parameters.PeakBackgroundRatio + "</MinBackgroundRatio>");

                file.Write("<PeakFitType>");
                switch (peak_parameters.PeakFitType)
                {
                    case Peaks.PEAK_FIT_TYPE.APEX:
                        file.Write("APEX");
                        break;
                    case Peaks.PEAK_FIT_TYPE.QUADRATIC:
                        file.Write("QUADRATIC");
                        break;
                    case Peaks.PEAK_FIT_TYPE.LORENTZIAN:
                        file.Write("LORENTZIAN");
                        break;
                    default:
                        break;
                }
                file.Write("</PeakFitType>");

                file.Write("<S2NThreshold>" + peak_parameters.SignalToNoiseThreshold + "</S2NThreshold>");
                file.Write("</PeakParameters>");
            }
        }

        private void WriteTransformParameters(TextWriter file,
            HornTransform.clsHornTransformParameters transform_parameters)
        {
            if (transform_parameters != null)
            {
                file.Write("<HornTransformParameters>");
                file.Write("<MaxCharge>" + transform_parameters.MaxCharge + "</MaxCharge>");
                file.Write("<NumPeaksForShoulder>" + transform_parameters.NumPeaksForShoulder + "</NumPeaksForShoulder>");
                file.Write("<MaxMW>" + transform_parameters.MaxMW + "</MaxMW>");
                file.Write("<MaxFit>" + transform_parameters.MaxFit + "</MaxFit>");
                file.Write("<CCMass>" + transform_parameters.CCMass + "</CCMass>");
                file.Write("<DeleteIntensityThreshold>" + transform_parameters.DeleteIntensityThreshold +
                           "</DeleteIntensityThreshold>");
                file.Write("<MinTheoreticalIntensityForScore>" + transform_parameters.MinIntensityForScore +
                           "</MinTheoreticalIntensityForScore>");
                file.Write("<O16O18Media>" + transform_parameters.O16O18Media + "</O16O18Media>");
                file.Write("</HornTransformParameters>");
            }
        }

        private void WritePeaks(StreamWriter file, DeconToolsV2.Results.clsTransformResults results)
        {
            var num_peaks = results.GetNumPeaks();
            file.Write("<PeakResults><NumPeaks>" + num_peaks + "</NumPeaks>");
            file.Write("<Peaks>");

            //const int PK_SIZE = sizeof (Engine.Results.LCMSPeak);
            //char[] peaks = new char[num_peaks * PK_SIZE];
            //for (int pk_num = 0; pk_num < num_peaks; pk_num++)
            //{
            //    *((Engine.Results.LCMSPeak*) &peaks[pk_num]) = results.mobj_lcms_results.GetPeak(pk_num);
            //}
            //file.Write(peaks);
            file.Flush();
            using (var bWriter = new BinaryWriter(file.BaseStream, Encoding.ASCII, true))
            {
                for (var pk_num = 0; pk_num < num_peaks; pk_num++)
                {
                    results.mobj_lcms_results.GetPeak(pk_num).WriteToBinaryStream(bWriter);
                }
            }
            file.Write("</Peaks>");
        }

        public void WriteFile(string file_name, DeconToolsV2.Results.clsTransformResults results,
            Peaks.clsPeakProcessorParameters peak_parameters,
            HornTransform.clsHornTransformParameters transform_parameters)
        {
            using (var file = new StreamWriter(new FileStream(file_name, FileMode.Create, FileAccess.Write)))
            {
                // Write out descriptors. Some information is unavailable for now.
                // i.e. what is the start position of the peaks data block and
                // the transform data block. Fill in place holders for now.
                // Once the data is actually written out, we can return to the stream and
                // write it.
                float version_num = 1;
                file.Write("<Version>" + version_num + "</version>");
                WritePeakParameters(file, peak_parameters);
                WriteTransformParameters(file, transform_parameters);
                WritePeaks(file, results);
            }
        }
    }
}
#endif