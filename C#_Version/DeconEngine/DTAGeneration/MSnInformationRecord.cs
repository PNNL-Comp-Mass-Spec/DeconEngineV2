namespace Engine.DTAProcessing
{
    /// <summary>
    /// class to store information that logs into the log file of DeconMSn
    /// </summary>
    internal class MSnInformationRecord
    {
        public const int MaxIsotopes = 16;
        public const int MaxIdLength = 256;

        /// <summary>
        /// scan number of MSn_scan
        /// </summary>
        public int MSnScanNum { get; set; }

        /// <summary>
        /// msNLevel of MSn_scan
        /// </summary>
        public int MSnScanLevel { get; set; }

        /// <summary>
        /// parent scam
        /// </summary>
        public int ParentScanNum { get; set; }

        /// <summary>
        /// msNLevel of parent scan
        /// </summary>
        public int ParentScanLevel { get; set; }

        /// <summary>
        /// m/z value of parent
        /// </summary>
        public double ParentMz { get; set; }

        /// <summary>
        /// mono m/z value of parent
        /// </summary>
        public double MonoMz { get; set; }

        /// <summary>
        /// charge state
        /// </summary>
        public short Charge { get; set; }

        /// <summary>
        /// fit value.
        /// </summary>
        public double FitScore { get; set; }

        /// <summary>
        /// monoisotopic mw of feature.
        /// </summary>
        public double MonoMw { get; set; }

        /// <summary>
        /// intensity of monoisotopic peak observed.
        /// </summary>
        public int MonoIntensity { get; set; }

        /// <summary>
        /// intensity of parent peak observed.
        /// </summary>
        public int ParentIntensity { get; set; }

        /// <summary>
        /// default constructor
        /// </summary>
        public MSnInformationRecord()
        {
            MSnScanNum = 0;
            ParentScanNum = 0;

            MSnScanLevel = 0;
            ParentScanLevel = 0;

            ParentMz = 0;
            MonoMz = 0;

            ParentIntensity = 0;
            MonoIntensity = 0;

            Charge = -1;
            FitScore = 1;
            MonoMw = 0;

        }
    }
}
