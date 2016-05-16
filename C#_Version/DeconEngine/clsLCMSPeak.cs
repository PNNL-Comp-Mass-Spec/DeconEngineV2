#if !Disable_Obsolete
using System;

namespace DeconToolsV2.Results
{
    [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files)", false)]
    public class clsLCMSPeak
    {
        public int mint_scan;
        public float mflt_mz;
        public float mflt_intensity;

        public clsLCMSPeak(int scan, float mz, float inten)
        {
            mint_scan = scan;
            mflt_mz = mz;
            mflt_intensity = inten;
        }
    }
}
#endif