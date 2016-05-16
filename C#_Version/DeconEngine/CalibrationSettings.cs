#if !Disable_Obsolete
using System;

namespace DeconToolsV2
{
    [Obsolete("Only used by Decon2LS.UI", false)]
    public class CalibrationSettings
    {
        public double ML1;
        public double ML2;
        public double SW_h;
        public int ByteOrder;
        public int TD;
        public double FRLow;
        public int NF;

        public CalibrationSettings()
        {
            ML1 = -1;
            ML2 = -1;
            SW_h = -1;
            TD = -1;
            FRLow = -1;
            NF = -1;
        }
    }
}
#endif