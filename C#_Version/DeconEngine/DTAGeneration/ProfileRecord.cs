namespace Engine.DTAProcessing
{
    /// <summary>
    /// class to store information that logs into the log file of DeconMSn
    /// </summary>
    internal class ProfileRecord
    {
        /// <summary>
        /// scan number of MSn_scan
        /// </summary>
        public int MSnScanNum { get; set; }

        /// <summary>
        /// parent scan
        /// </summary>
        public int ParentScanNum { get; set; }

        /// <summary>
        /// ion injection value
        /// </summary>
        public double AgcTime { get; set; }

        /// <summary>
        /// tic
        /// </summary>
        public double TotalIonCurrent { get; set; }

        /// <summary>
        /// default constructor
        /// </summary>
        public ProfileRecord()
        {
            MSnScanNum = 0;
            ParentScanNum = 0;
            AgcTime = -1.0;
            TotalIonCurrent = -1.0;
        }
    }
}
