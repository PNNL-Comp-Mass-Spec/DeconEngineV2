using System.IO;

namespace Engine.DTAProcessing
{
    internal class DTAScanTypeGeneration
    {
        public string DTAScanTypeFilename;
        public StreamWriter DTAScanTypeFileWriter;

        public Readers.RawData RawDataReader;

        /// <summary>
        /// default constructor.
        /// </summary>
        public DTAScanTypeGeneration()
        {
            RawDataReader = null;
        }

        public void GenerateScanTypeFile()
        {
            var min = RawDataReader.GetFirstScanNum();
            var max = RawDataReader.GetLastScanNum();

            DTAScanTypeFileWriter.WriteLine("ScanNumber\tScanTypeName\tScanType\tScanStartTime");

            for (var scan = min; scan <= max; scan++)
            {
                var isFT = RawDataReader.IsFTScan(scan);
                var actType = RawDataReader.GetSpectrumType(scan);
                var msLevel = RawDataReader.GetMSLevel(scan);
                var time = RawDataReader.GetScanTime(scan);

                var scanTypeName = "MS";
                if (isFT)
                {
                    scanTypeName = "HMS";
                }

                if (msLevel > 1)
                {
                    scanTypeName += "n";

                    switch (actType)
                    {
                        case 2:
                            //ETD, or ETciD or EThcD
                            scanTypeName = "ETD-" + scanTypeName;
                            break;
                        case 3:
                            //HCD
                            scanTypeName = "HCD-" + scanTypeName;
                            break;
                        case 1:
                            // CID
                            scanTypeName = "CID-" + scanTypeName;
                            break;
                        default:
                            // We don't handle it, don't make assumptions
                            break;
                    }
                }

                DTAScanTypeFileWriter.WriteLine("{0}\t{1}\t{2}\t{3:F4}", scan, scanTypeName, msLevel, time);
            }
        }
    }
}
