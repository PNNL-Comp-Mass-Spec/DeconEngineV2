#if Enable_Obsolete
using System;

namespace Engine.DTAProcessing
{
    // class to store information that logs into the log file of DeconMSn
    [Obsolete("Only used by Decon2LS.UI", false)]
    internal class MSnInformationRecord
    {
        public const int MAX_ISOTOPES = 16;
        public const int MAX_ID_LEN = 256;
        // scan number of MSn_scan
        public int mint_msn_scan_num;
        // msNLevel of MSn_scan
        public int mint_msn_scan_level;
        // parent scam
        public int mint_parent_scan_num;
        // msNLevel of parent scan
        public int mint_parent_scan_level;
        // m/z value of parent
        public double mdbl_parent_mz;
        // mono m/z value of parent
        public double mdbl_mono_mz;
        // charge state
        public short mshort_cs;
        // fit value .
        public double mdbl_fit;
        // monoisotopic mw of feature.
        public double mdbl_mono_mw;
        // intensity of monoisotopic peak observed.
        public int mint_mono_intensity;
        // intensity of parent peak observed.
        public int mint_parent_intensity;

        // default constructor
        public MSnInformationRecord()
        {
            mint_msn_scan_num = 0;
            mint_parent_scan_num = 0;

            mint_msn_scan_level = 0;
            mint_parent_scan_level = 0;

            mdbl_parent_mz = 0;
            mdbl_mono_mz = 0;

            mint_parent_intensity = 0;
            mint_mono_intensity = 0;

            mshort_cs = -1;
            mdbl_fit = 1;
            mdbl_mono_mw = 0;

        }
    }
}
#endif