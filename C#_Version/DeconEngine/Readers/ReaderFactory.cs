using System;
using DeconToolsV2.Readers;

namespace Engine.Readers
{
    /// <summary>
    /// Reader Factory
    /// </summary>
    /// <remarks>Used by DeconMSn</remarks>
    internal static class ReaderFactory
    {
        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static RawData GetMSDataReader(FileType fileType)
        {
            return GetRawData(fileType);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static RawData GetMSDataReader(FileType fileType, string fileName)
        {
            return GetRawData(fileType, fileName);
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static void GetMSDataReader(out RawData rawData, FileType fileType)
        {
            GetRawData(out rawData, fileType);
        }

        public static RawData GetRawData(FileType fileType, string fileName)
        {
            RawData rawData = null;
            switch (fileType)
            {
#pragma warning disable 618
                case FileType.BRUKER:
                    rawData = new BrukerRawData();
                    rawData.Load(fileName);
                    break;
                case FileType.BRUKER_ASCII:
                    rawData = new BrukerAsciiRawData();
                    rawData.Load(fileName);
                    break;
                case FileType.ASCII:
                    rawData = new AsciiRawData();
                    rawData.Load(fileName);
                    break;
                case FileType.AGILENT_TOF:
#if ANALYST_INSTALLED
                    rawData = new AgilentRawData();
                    rawData.Load(fileName);
#endif
                    break;
                case FileType.SUNEXTREL:
                    rawData = new SunExtrelRawData();
                    rawData.Load(fileName);
                    break;
                case FileType.ICR2LSRAWDATA:
                    rawData = new Icr2lsRawData();
                    rawData.Load(fileName);
                    break;
#pragma warning restore 618
                case FileType.THERMORAW:
                    rawData = new ThermoRawData();
                    rawData.Load(fileName);
                    break;

#pragma warning disable 618
                case FileType.MICROMASSRAWDATA:
#if MASSLYNX_4_INSTALLED
                    rawData = new MicromassRawData();
                    rawData.Load(fileName);

                    break;
#pragma warning restore 618
#endif
                case FileType.MZXMLRAWDATA:
                    rawData = new MZXmlRawData();
                    rawData.Load(fileName);
                    break;

#pragma warning disable 618
                case FileType.PNNL_IMS:
                    rawData = new IMSRawData();
                    rawData.Load(fileName);
                    break;
#pragma warning restore 618
                //case FileType.PNNL_UIMF:
                //    rawData = new UIMFRawData();
                //    rawData.Load(file_name);
                //    break;
                /*case FileType.YAFMS:
                    rawData = new YafmsRawData();
                    rawData.Load(file_name);
                    break; */
                default:
                    break;
            }
            return rawData;
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static void GetRawData(out RawData rawData, FileType fileType)
        {
            rawData = null;
            switch (fileType)
            {
                case FileType.BRUKER:
                    rawData = new BrukerRawData();
                    break;
                case FileType.AGILENT_TOF:
#if ANALYST_INSTALLED
                    rawData = new AgilentRawData();
#endif
                    break;
                case FileType.THERMORAW:
                    rawData = new ThermoRawData();
                    break;
                case FileType.SUNEXTREL:
                    rawData = new SunExtrelRawData();
                    break;
                case FileType.ICR2LSRAWDATA:
                    rawData = new Icr2lsRawData();
                    break;
                case FileType.MICROMASSRAWDATA:
#if MASSLYNX_4_INSTALLED
                    rawData = new MicromassRawData();
#endif
                    break;
                case FileType.MZXMLRAWDATA:
                    rawData = new MZXmlRawData();
                    break;
                case FileType.PNNL_IMS:
                    rawData = new IMSRawData();
                    break;
                /*case FileType.PNNL_UIMF:
                    rawData = new UIMFRawData();
                    break; */
                /*case FileType.YAFMS:
                    rawData = new YafmsRawData();
                    break; */
                case FileType.BRUKER_ASCII:
                    rawData = new BrukerAsciiRawData();
                    break;
                case FileType.ASCII:
                    rawData = new AsciiRawData();
                    break;
                default:
                    break;
            }
        }

        [Obsolete("Only used by DeconTools for ICR2LSRun and IMFRun; BrukerV2 exists, but has no use path", false)]
        public static RawData GetRawData(FileType fileType)
        {
            RawData rawData = null;
            switch (fileType)
            {
                case FileType.BRUKER:
                    rawData = new BrukerRawData();
                    break;
                case FileType.AGILENT_TOF:
#if ANALYST_INSTALLED
                    rawData = new AgilentRawData();
#endif
                    break;
                case FileType.THERMORAW:
                    rawData = new ThermoRawData();
                    break;
                case FileType.SUNEXTREL:
                    rawData = new SunExtrelRawData();
                    break;
                case FileType.ICR2LSRAWDATA:
                    rawData = new Icr2lsRawData();
                    break;
                case FileType.MICROMASSRAWDATA:
#if MASSLYNX_4_INSTALLED
                    rawData = new MicromassRawData();
#endif
                    break;
                case FileType.MZXMLRAWDATA:
                    rawData = new MZXmlRawData();
                    break;
                case FileType.PNNL_IMS:
                    rawData = new IMSRawData();
                    break;
                /*case FileType.PNNL_UIMF:
                    rawData = new UIMFRawData();
                    break; */
                /*case FileType.YAFMS:
                    rawData = new YafmsRawData();
                    break; */
                case FileType.BRUKER_ASCII:
                    rawData = new BrukerAsciiRawData();
                    break;
                case FileType.ASCII:
                    rawData = new AsciiRawData();
                    break;
                default:
                    break;
            }
            return rawData;
        }
    }
}