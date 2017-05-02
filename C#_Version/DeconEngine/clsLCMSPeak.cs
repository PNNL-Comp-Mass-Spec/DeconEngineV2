#if !Disable_Obsolete
using System;

namespace DeconToolsV2.Results
{
    [Obsolete("Superseded by Results\\LcmsPeak.cs")]
    public class clsLCMSPeakOld
    {
        public int mint_scan;
        public float mflt_mz;
        public float mflt_intensity;

        public clsLCMSPeakOld(int scan, float mz, float inten)
        {
            mint_scan = scan;
            mflt_mz = mz;
            mflt_intensity = inten;
        }

        public clsLCMSPeakOld(int scan, double mz, double inten)
        {
            mint_scan = scan;

            if (mz > float.MaxValue)
                mflt_mz = float.MaxValue;
            else
            {
                mflt_mz = (float)mz;
            }

            if (inten > float.MaxValue)
                mflt_intensity = float.MaxValue;
            else
            {
                mflt_intensity = (float)inten;
            }

        }

    }
}
#endif