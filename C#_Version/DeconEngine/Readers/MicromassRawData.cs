#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DeconToolsV2.Readers;

//set preprocessor declaration MASSLYNX_4_INSTALLED in project properties if
// MassLynx 4 is installed on computer (Masslynx 3 not supported currently).
// Also make sure that DacServer.dll exists in c:\Masslynx or change the path
// below accordingly.
#if MASSLYNX_4_INSTALLED
//#import "..\Library\DacServer.dll"
#endif

namespace Engine.Readers
{
    [Obsolete("Only used by MicromassRawData, which is obsolete", false)]
    internal enum enmMassLynxVersion
    {
        NONE = 0,
        V_3 = 3,
        V_4 = 4
    };

    [Obsolete("Not used by DeconTools at all", false)]
    internal class MicromassRawData : RawData
    {
        internal class ScanInfo
        {
            public short mshort_function_num;
            public int mint_scan_num;
        }

        private bool mbln_get_ms_data_only;
        private const int MAX_FUNCTIONS = 16;
        private string marr_filename;
        private int[] marr_num_scans_per_function = new int[MAX_FUNCTIONS];
        private List<ScanInfo> mvectScanInfo = new List<ScanInfo>();

        private enmMassLynxVersion menm_masslynx_version;
        private short mshort_num_functions;
        private int mint_num_points;

#if MASSLYNX_4_INSTALLED
        private DACSERVERLib.IDACHeader *mptr_dac_header;
        private DACSERVERLib.IDACSpectrum *mptr_dac_spectrum;
        private DACSERVERLib.IDACScanStats *mptr_dac_scan_stat;
        private DACSERVERLib.IDACFunctionInfo *mptr_dac_func_info;
#endif

        public override FileType GetFileType()
        {
            return FileType.MICROMASSRAWDATA;
        }

        public void SetMassLynxVersion(enmMassLynxVersion version)
        {
            menm_masslynx_version = version;
        }

        public override int GetFirstScanNum()
        {
            return 1;
        }

        public override int GetLastScanNum()
        {
            return GetNumScans() - 1;
        }

        public override int GetNumScansLoaded()
        {
            return GetNumScans();
        }

        public override int GetParentScan(int scan_num)
        {
            //future work
            return 0;
        }

        public override bool IsMSScan(int scan_num)
        {
            //future work
            return true;
        }

        public override double GetParentMz(int scan_num)
        {
            //future work
            return 0;
        }

        public override bool IsProfileScan(int scan_num)
        {
            //future work
            return false;
        }

        public override int GetMSLevel(int scan_num)
        {
            //future work
            return 1;
        }

        public bool IsMicroMassV4Installed()
        {
#if MASSLYNX_4_INSTALLED
            CoInitialize( null );
            CLSID clsid;
            HRESULT res =  CLSIDFromString(L"{111A3111-8A5C-11D5-809C-00508B5FFEC8}", &clsid );

            if (res != S_OK)
                return false;

            return true;
    #endif
            return false;
        }

        public bool IsMicroMassV3Installed()
        {
            return false;
        }

        public MicromassRawData()
        {
            mbln_get_ms_data_only = false;
            menm_masslynx_version = GetLatestInstalledVersion();
            for (int funcNum = 0; funcNum < MAX_FUNCTIONS; funcNum++)
                marr_num_scans_per_function[funcNum] = 0;

            mint_num_points = 0;

            if (menm_masslynx_version == enmMassLynxVersion.V_4)
                InitializeForV4();
            else if (menm_masslynx_version == enmMassLynxVersion.V_3)
                InitializeForV3();

        }

        private void InitializeForV4()
        {
#if MASSLYNX_4_INSTALLED
            CLSID clsid;
            IID riid;
            HRESULT res;

            // DACHeader
            res =  CLSIDFromString(L"{111A3111-8A5C-11D5-809C-00508B5FFEC8}", &clsid );
            if (res ==REGDB_E_WRITEREGDB)
            {
                throw "Unable to instantiate MassLynx objects: IDACHeader from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = IIDFromString(L"{111A3110-8A5C-11D5-809C-00508B5FFEC8}", &riid);
            if (res == E_INVALIDARG)
            {
                throw "Unable to instantiate MassLynx objects: IDACHeader from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = CoCreateInstance(clsid, null,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_header);
            if(res != S_OK)
            {
                throw "Unable to instantiate MassLynx objects: IDACHeader from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }

            // DACSpectrum
            res =  CLSIDFromString(L"{42BAE6E4-3D52-11D5-8043-00508B5FFEC8}", &clsid );
            if (res ==REGDB_E_WRITEREGDB)
            {
                throw "Unable to instantiate MassLynx objects: IDACSpectrum from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = IIDFromString(L"{42BAE6E3-3D52-11D5-8043-00508B5FFEC8}", &riid);
            if (res == E_INVALIDARG)
            {
                throw "Unable to instantiate MassLynx objects: IDACSpectrum from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = CoCreateInstance(clsid, null,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_spectrum);
            if(res != S_OK)
            {
                throw "Unable to instantiate MassLynx objects: IDACSpectrum from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }

            // DACScanStats
            res =  CLSIDFromString(L"{0D0678C2-3A1B-11D5-8040-00508B5FFEC8}", &clsid );
            if (res ==REGDB_E_WRITEREGDB)
            {
                throw "Unable to instantiate MassLynx objects: IDACScanStats from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = IIDFromString(L"{0D0678C1-3A1B-11D5-8040-00508B5FFEC8}", &riid);
            if (res == E_INVALIDARG)
            {
                throw "Unable to instantiate MassLynx objects: IDACScanStats from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = CoCreateInstance(clsid, null,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_scan_stat);
            if(res != S_OK)
            {
                throw "Unable to instantiate MassLynx objects: IDACScanStats from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }

            // DACFunctionInfo
            res =  CLSIDFromString(L"{63E4A0C2-5684-11D5-8063-00508B5FFEC8}", &clsid );
            if (res ==REGDB_E_WRITEREGDB)
            {
                throw "Unable to instantiate MassLynx objects: IDACFunctionInfo from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = IIDFromString(L"{63E4A0C1-5684-11D5-8063-00508B5FFEC8}", &riid);
            if (res == E_INVALIDARG)
            {
                throw "Unable to instantiate MassLynx objects: IDACFunctionInfo from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
            res = CoCreateInstance(clsid, null,CLSCTX_INPROC_SERVER, riid, (void **) &mptr_dac_func_info);
            if(res != S_OK)
            {
                throw "Unable to instantiate MassLynx objects: IDACFunctionInfo from DacServer.dll . Please install MassLynxV4 on this computer or convert file to MZXml file format using MassWolf on http://sashimi.sourceforge.net/software_glossolalia.html.";
            }
    #endif
        }

        private void InitializeForV3()
        {
        }

        ~MicromassRawData()
        {
        }

        public override string GetFileName()
        {
            return null;
        }

        public override void Load(string file)
        {
            if (file.Length < 4)
            {
                // should end in .dat or .raw
                string message = "Invalid Masslynx 4 file name: " + file + " Should have a .raw in the path name";
                throw new System.Exception(message);
            }
            string ext = Path.GetExtension(file);
            if (ext.ToLower() == ".raw")
            {
                marr_filename = file;
            }
            else if (ext.ToLower() == ".dat")
            {
                //find index of .raw the file name or complain.
                int ptr = file.ToLower().IndexOf(".raw");
                if (ptr == -1)
                {
                    // should end in .dat or .raw
                    string message = "Invalid Masslynx 4 file name: " + file + " Should have a .raw in the path name";
                    throw new System.Exception(message);
                }
                marr_filename = file.Substring(ptr + 4);
            }

            LoadHeader();
        }

        private bool LoadHeaderV4()
        {
#if MASSLYNX_4_INSTALLED
            _bstr_t bstr_file = marr_filename;
            // initialize dDACScanStats object
            HRESULT res = mptr_dac_scan_stat.GetScanStats(bstr_file, 1, 0, 1);
            if (res != S_OK)
            {
                char message[512];
                strcpy(message, "Unable to open Masslynx 4 file: ");
                strcat(message, marr_filename);
                throw message;
            }
            mvectScanInfo.Clear();
            if (mbln_get_ms_data_only)
                mshort_num_functions = 1;
            else
                mshort_num_functions = (short) mptr_dac_func_info.GetNumFunctions(bstr_file);

            SortedDictionary<double, ScanInfo> mapScanStats;
            for (short func_num = 1; func_num <= mshort_num_functions; func_num++)
            {
                ScanInfo info;
                mptr_dac_func_info.GetFunctionInfo(bstr_file, func_num);
                marr_num_scans_per_function[func_num] = mptr_dac_func_info.NumScans;
                for (int scan_num = 1; scan_num <= marr_num_scans_per_function[func_num];
                    scan_num++)
                {
                    mptr_dac_scan_stat.GetScanStats(bstr_file, func_num, 0, scan_num);
                    float rt = mptr_dac_scan_stat.RetnTime;
                    info.mshort_function_num = func_num;
                    info.mint_scan_num = scan_num;
                    mapScanStats.insert(std.pair<double, ScanInfo>(rt, info));
                }
            }
            foreach (KeyValuePair<double, ScanInfo> item in mapScanStats)
            {
                mvectScanInfo.Add(item.Value);
            }
    #endif
            return false;
        }

        private bool LoadHeaderV3()
        {
            return false;
        }

        private bool LoadHeader()
        {
            if (menm_masslynx_version == enmMassLynxVersion.V_4)
                return LoadHeaderV4();
            else if (menm_masslynx_version == enmMassLynxVersion.V_3)
                return LoadHeaderV3();
            return false;
        }

        public override bool IsZoomScan(int scan_num)
        {
            return false;
        }

        private bool GetRawDataV3(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            // Not implemented
            return false;
        }

        private bool GetRawDataV3(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            // Not implemented
            return false;
        }

        // Note that Centroid is ignored by this class
        private bool GetRawDataV4(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid)
        {
            mzs = new List<double>();
            intensities = new List<double>();
#if MASSLYNX_4_INSTALLED
            if (scan_num < 1)
                throw "Scan Number needs to be at least 1";
            _bstr_t bstr = marr_filename;

            ScanInfo info = mvectScanInfo[scan_num-1];
            System.Console.Error.WriteLine(bstr<<" Scan = "<<info.mint_scan_num<<" Func # = "<<info.mshort_function_num);
            mptr_dac_spectrum.GetSpectrum(bstr, info.mshort_function_num, 0, info.mint_scan_num);

            long num_pts_spectrum;
            mptr_dac_spectrum.get_NumPeaks(&num_pts_spectrum);
            mint_num_points = num_pts_spectrum;

            if (num_pts_spectrum > (int) mzs.Capacity)
            {
                mzs.Capacity = num_pts_spectrum);
                intensities.Capacity = num_pts_spectrum);
            }

            _variant_t var_mzs;
            _variant_t var_intensities;
            var_mzs = mptr_dac_spectrum.Masses;
            var_intensities = mptr_dac_spectrum.Intensities;

            float *mz_arr, *intensity_arr;

            SafeArrayAccessData(var_mzs.parray, (void **) &mz_arr);
            SafeArrayAccessData(var_intensities.parray, (void **) &intensity_arr);

            for (int i = 0; i < num_pts_spectrum; i++)
            {
                mzs.Add(mz_arr[i]);
                intensities.Add(intensity_arr[i]);
            }

            SafeArrayUnaccessData(var_mzs.parray);
            SafeArrayUnaccessData(var_intensities.parray);
            return true;
    #endif
            return false;
        }

        // Note that Centroid is ignored by this class
        private bool GetRawDataV4(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts)
        {
            mzs = new List<double>();
            intensities = new List<double>();
#if MASSLYNX_4_INSTALLED
            _bstr_t bstr = marr_filename;
            if (scan_num < 1)
                throw "Scan Number needs to be at least 1";

            ScanInfo info = mvectScanInfo[scan_num-1];
            mptr_dac_spectrum.GetSpectrum(bstr, info.mshort_function_num, 0, info.mint_scan_num);

            long num_pts_spectrum;
            mptr_dac_spectrum.get_NumPeaks(&num_pts_spectrum);
            num_pts_spectrum = min(num_pts_spectrum, num_pts);

            if (num_pts_spectrum > (int) mzs.Capacity)
            {
                mzs.Capacity = num_pts_spectrum);
                intensities.Capacity = num_pts_spectrum);
            }

            _variant_t var_mzs;
            _variant_t var_intensities;
            var_mzs = mptr_dac_spectrum.Masses;
            var_intensities = mptr_dac_spectrum.Intensities;

            float *mz_arr = new float[num_pts_spectrum];
            float *intensity_arr = new float[num_pts_spectrum];

            SafeArrayAccessData(var_mzs.parray, (void **) &mz_arr);
            SafeArrayAccessData(var_intensities.parray, (void **) &intensity_arr);

            for (int i = 0; i < num_pts_spectrum; i++)
            {
                mzs.Add(mz_arr[i]);
                intensities.Add(intensity_arr[i]);
            }

            delete [] mz_arr;
            delete [] intensity_arr;
            return true;
    #endif
            return false;
        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid)
        {
            mzs = null;
            intensities = null;
            switch (menm_masslynx_version)
            {
                case enmMassLynxVersion.V_3:
                    return GetRawDataV3(out mzs, out intensities, scan_num, centroid);
                    break;
                case enmMassLynxVersion.V_4:
                    return GetRawDataV4(out mzs, out intensities, scan_num, centroid);
                    break;
                default:
                    break;
            }
            return false;
        }

        public override bool GetRawData(out List<double> mzs, out List<double> intensities, int scan_num, bool centroid, int num_pts)
        {
            mzs = null;
            intensities = null;
            switch (menm_masslynx_version)
            {
                case enmMassLynxVersion.V_3:
                    return GetRawDataV3(out mzs, out intensities, scan_num, centroid, num_pts);
                    break;
                case enmMassLynxVersion.V_4:
                    return GetRawDataV4(out mzs, out intensities, scan_num, centroid, num_pts);
                    break;
                default:
                    break;
            }
            return false;
        }

        private double GetScanTimeV4(int scan_num)
        {
#if MASSLYNX_4_INSTALLED
            _bstr_t bstr_file_name = marr_filename;
            if (scan_num < 1)
                throw "Scan Number needs to be at least 1";

            ScanInfo info = mvectScanInfo[scan_num-1];
            mptr_dac_scan_stat.GetScanStats(bstr_file_name, info.mshort_function_num, 0, info.mint_scan_num);
            return mptr_dac_scan_stat.RetnTime;
    #endif
            return 0;
        }

        private double GetScanTimeV3(int scan_num)
        {
            return 0;
        }

        public override double GetScanTime(int scan_num)
        {
            switch (menm_masslynx_version)
            {
                case enmMassLynxVersion.V_3:
                    return GetScanTimeV3(scan_num);
                    break;
                case enmMassLynxVersion.V_4:
                    return GetScanTimeV4(scan_num);
                    break;
                default:
                    break;
            }
            return 0;
        }

        public override int GetScanSize()
        {
            return mint_num_points;
        }

        public override int GetNumScans()
        {
            return (int) mvectScanInfo.Count;
        }

        public override double GetSignalRange(int scan_num, bool centroid)
        {
            return 0;
        }

#if MASSLYNX_4_INSTALLED
        private void GetStringRepresentation(char *buffer, double value)
        {
            int precision = 4;
            int intLen = (int)(precision*1.0 + log10(value));
            _gcvt(value, intLen, buffer);
            if (strlen(buffer) > 0 && buffer[strlen(buffer)-1] == '.')
            {
                buffer[strlen(buffer)-1] = '\0';
            }
        }
#endif

        public virtual void GetScanDescription(int scan, string description)
        {
#if MASSLYNX_4_INSTALLED
            _bstr_t bstr_file_name = marr_filename;

            if (scan < 1)
                throw "Scan Number needs to be at least 1";
            ScanInfo info = mvectScanInfo[scan-1];
            mptr_dac_scan_stat.GetScanStats(bstr_file_name, info.mshort_function_num, 0, info.mint_scan_num);

            float rt = mptr_dac_scan_stat.RetnTime;
            float bpi = mptr_dac_scan_stat.BPI;
            float bpim = mptr_dac_scan_stat.BPM;
            float lowMass = mptr_dac_scan_stat.LoMass;
            float hiMass = mptr_dac_scan_stat.HiMass;

            if (info.mshort_function_num > 1)
            {
                strcpy(description, "MSMS (fnc ");
                _itoa((int)info.mshort_function_num, &description[strlen(description)], 10);
                strcat(description, " scan: ");
                _itoa((int)info.mint_scan_num, &description[strlen(description)], 10);
                strcat(description, " )");
            }
            else
            {
                strcpy(description, "MS");
            }
            int a = 0 , b= 0;
            strcat(description, " Scan ");
            _itoa(scan, &description[strlen(description)], 10);
            char buffer[64];

            strcat(description, "RT: ");
            GetStringRepresentation(buffer, (double)rt);
            strcat(description, buffer);

            strcat(description, " BPI Mass: ");
            GetStringRepresentation(buffer, (double)bpim);
            strcat(description, buffer);
            strcat(description, "[");
            GetStringRepresentation(buffer, (double)lowMass);
            strcat(description, buffer);
            strcat(description, ",");
            GetStringRepresentation(buffer, (double)hiMass);
            strcat(description, buffer);
            strcat(description, "]");
#endif
        }

        private void GetTicFromFileV4(out List<double> intensities, out List<double> scan_times, bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();
#if MASSLYNX_4_INSTALLED
    // Tics only come from MS data.

            _bstr_t bstr_file_name = marr_filename;

            int num_scans = GetNumScans();

            for (int scan_num = 0; scan_num < num_scans; scan_num++)
            {
                ScanInfo info = mvectScanInfo[scan_num];
                if (info.mshort_function_num != 1)
                    continue;
                mptr_dac_scan_stat.GetScanStats(bstr_file_name, info.mshort_function_num, 0, info.mint_scan_num);
                if (base_peak_tic)
                {
                    intensities.Add(mptr_dac_scan_stat.BPI);
                }
                else
                {
                    intensities.Add(mptr_dac_scan_stat.TIC);
                }
                scan_times.Add(mptr_dac_scan_stat.RetnTime);
            }
    #endif
            return;
        }

        private void GetTicFromFileV3(out List<double> intensities, out List<double> scan_times, bool base_peak_tic)
        {
            intensities = new List<double>();
            scan_times = new List<double>();
            return;
        }

        public override void GetTicFromFile(out List<double> intensities, out List<double> scan_times,
            bool base_peak_tic)
        {
            switch (menm_masslynx_version)
            {
                case enmMassLynxVersion.V_3:
                    GetTicFromFileV3(out intensities, out scan_times, base_peak_tic);
                    return;
                    break;
                case enmMassLynxVersion.V_4:
                    GetTicFromFileV4(out intensities, out scan_times, base_peak_tic);
                    return;
                    break;
                default:
                    intensities = new List<double>();
                    scan_times = new List<double>();
                    break;
            }
        }

        public enmMassLynxVersion GetLatestInstalledVersion()
        {
            if (IsMicroMassV4Installed())
            {
                return enmMassLynxVersion.V_4;
            }
            else if (IsMicroMassV3Installed())
            {
                // check for version 3.
                return enmMassLynxVersion.V_3;
            }

            return enmMassLynxVersion.NONE;
        }
    }
}
#endif