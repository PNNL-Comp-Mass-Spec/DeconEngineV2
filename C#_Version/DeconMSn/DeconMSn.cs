using System;
using System.Diagnostics;
using System.IO;
using System.Xml;
using DeconToolsV2;
using DeconToolsV2.DTAGeneration;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;

namespace DeconMSn
{
    public class DeconMSn
    {
        public int RunDeconMSn(string[] args)
        {
            string commandLine;

            Stopwatch sw; //initialize Begin and End for the timer

            string strAppFolder;
            string strSVMParamFilePath;
            //string strDllFileAndPath;

            strAppFolder = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            strSVMParamFilePath = Path.Combine(strAppFolder, "svm_params.xml");

            if (args.Length == 0)
            {
                PrintUsage();
                return 0;
            }

            //default options
            int IValue = 25; //ion count - resetting it to 25
            int FValue = 1;
            int LValue = 1000000;
            int CValue = 0;
            double BValue = 200;
            double TValue = 5000;

            string stringIvalue;
            string stringFvalue;
            string stringLvalue;
            string stringBvalue;
            string stringTvalue;
            string stringCvalue;
            //string stringFileName;
            string stringOutputFileFormat;
            string stringSpectraFormat;
            string stringParamFile;

            clsProcRunner obj_proc_runner = new clsProcRunner();
            clsHornTransformParameters obj_transform_parameters = new clsHornTransformParameters();
            clsDTAGenerationParameters obj_dta_generation_parameters = new clsDTAGenerationParameters();
            clsPeakProcessorParameters obj_peak_parameters = new clsPeakProcessorParameters();

            bool error = false;
            obj_dta_generation_parameters.SVMParamFile = strSVMParamFilePath;
            obj_dta_generation_parameters.CentroidMSn = false;

            sw = new Stopwatch();
            sw.Start();

            int argvIndex = 1;

            // First check for a parameter file defined using -P
            if (args.Length > 0)
            {
                for (argvIndex = 0; argvIndex < args.Length; argvIndex++)
                {
                    commandLine = args[argvIndex];
                    if (commandLine.StartsWith("-P") && !commandLine.ToLower().StartsWith("-progress"))
                    {
                        stringParamFile = commandLine.Remove(0, 2);

                        Console.WriteLine("Reading Parameter File " + stringParamFile);

                        XmlTextReader rdr = new XmlTextReader(stringParamFile);
                        //Read each node in the tree.
                        while (rdr.Read())
                        {
                            switch (rdr.NodeType)
                            {
                                case XmlNodeType.Element:
                                    if (string.Compare(rdr.Name, "PeakParameters") == 0)
                                    {
                                        obj_peak_parameters.LoadV1PeakParameters(rdr);
                                    }
                                    else if (string.Compare(rdr.Name, "HornTransformParameters") == 0)
                                    {
                                        obj_transform_parameters.LoadV1HornTransformParameters(rdr);
                                    }
                                    else if (string.Compare(rdr.Name, "DTAGenerationParameters") == 0)
                                    {
                                        obj_dta_generation_parameters.LoadV1DTAGenerationParameters(rdr);
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }

                        if (rdr != null)
                            rdr.Close();
                    }
                }
            }

            bool filenameDefined = false;
            stringSpectraFormat = "ALL";

            if (args.Length > 0)
            {
                // Now read the remaining parameters
                for (argvIndex = 0; argvIndex < args.Length; argvIndex++)
                {
                    commandLine = args[argvIndex];
                    if (!error)
                    {
                        if (commandLine.ToLower().StartsWith("-progress"))
                        {
                            obj_dta_generation_parameters.WriteProgressFile = true;
                            continue;
                        }

                        if (commandLine.ToLower().StartsWith("-centroid"))
                        {
                            // Warning: the masses reported by GetMassListFromScanNum when centroiding are not properly calibrated and thus could be off by 0.3 m/z or more
                            //          For example, in scan 8101 of dataset RAW_Franc_Salm_IMAC_0h_R1A_18Jul13_Frodo_13-04-15, we see these values:
                            //          Profile m/z         Centroid m/z	Delta_PPM
                            //			112.051 			112.077			232
                            //			652.3752			652.4645		137
                            //			1032.56495			1032.6863		118
                            //			1513.7252			1513.9168		127
                            obj_dta_generation_parameters.CentroidMSn = true;
                            continue;
                        }

                        if (!commandLine.StartsWith("-"))
                        {
                            // Treat the first non-switch parameter as the dataset to process
                            if (!filenameDefined)
                                obj_proc_runner.FileName = commandLine;

                            filenameDefined = true;
                        }
                        else if (commandLine.Length > 1)
                        {
                            switch (commandLine[1])
                            {
                                case 'I':
                                    stringIvalue = commandLine.Remove(0, 2);
                                    IValue = Convert.ToInt32(stringIvalue, 10);
                                    obj_dta_generation_parameters.MinIonCount = IValue;
                                    break;
                                case 'F':
                                    stringFvalue = commandLine.Remove(0, 2);
                                    FValue = Convert.ToInt32(stringFvalue, 10);
                                    obj_dta_generation_parameters.MinScan = FValue;
                                    break;
                                case 'L':
                                    stringLvalue = commandLine.Remove(0, 2);
                                    LValue = Convert.ToInt32(stringLvalue, 10);
                                    obj_dta_generation_parameters.MaxScan = LValue;
                                    break;
                                case 'B':
                                    stringBvalue = commandLine.Remove(0, 2);
                                    BValue = Convert.ToDouble(stringBvalue);
                                    obj_dta_generation_parameters.MinMass = BValue;
                                    break;
                                case 'T':
                                    stringTvalue = commandLine.Remove(0, 2);
                                    TValue = Convert.ToDouble(stringTvalue);
                                    obj_dta_generation_parameters.MaxMass = TValue;
                                    break;
                                case 'C':
                                    stringCvalue = commandLine.Remove(0, 2);
                                    CValue = Convert.ToInt16(stringCvalue);
                                    obj_dta_generation_parameters.ConsiderChargeValue = CValue;
                                    break;
                                case 'S':
                                    stringSpectraFormat = commandLine.Remove(0, 2);
                                    if (string.Compare(stringSpectraFormat, "ETD") == 0)
                                    {
                                        obj_dta_generation_parameters.SpectraType = SPECTRA_TYPE.ETD;
                                    }
                                    else if (string.Compare(stringSpectraFormat, "CID") == 0)
                                    {
                                        obj_dta_generation_parameters.SpectraType = SPECTRA_TYPE.CID;
                                    }
                                    else if (string.Compare(stringSpectraFormat, "HCD") == 0)
                                    {
                                        obj_dta_generation_parameters.SpectraType = SPECTRA_TYPE.HCD;
                                    }
                                    else
                                    {
                                        stringSpectraFormat = "ALL";
                                        obj_dta_generation_parameters.SpectraType = SPECTRA_TYPE.ALL;
                                    }
                                    break;
                                case 'X':
                                    stringOutputFileFormat = commandLine.Remove(0, 2);
                                    if (string.Compare(stringOutputFileFormat, "MGF") == 0)
                                        obj_dta_generation_parameters.OutputType = OUTPUT_TYPE.MGF;
                                    else if (string.Compare(stringOutputFileFormat, "LOG") == 0)
                                        obj_dta_generation_parameters.OutputType = OUTPUT_TYPE.LOG;
                                    else if (string.Compare(stringOutputFileFormat, "CDTA") == 0)
                                        obj_dta_generation_parameters.OutputType = OUTPUT_TYPE.CDTA;
                                    else if (string.Compare(stringOutputFileFormat, "MZXML") == 0)
                                        obj_dta_generation_parameters.OutputType = OUTPUT_TYPE.MZXML;
                                    else if (string.Compare(stringOutputFileFormat, "DTA") == 0)
                                        obj_dta_generation_parameters.OutputType = OUTPUT_TYPE.DTA;
                                    else
                                    {
                                        PrintUsage();
                                        error = true;
                                    }
                                    break;
                                case 'D':
                                    string outputFolderPath;
                                    outputFolderPath = commandLine.Remove(0, 2);
                                    obj_proc_runner.OutputPathForDTACreation = outputFolderPath;
                                    break;
                                case 'G': // Used by Extract_msn; ignored by DeconMSn
                                    break;
                                case 'M': // Used by Extract_msn; ignored by DeconMSn
                                    break;
                                case 'P': // Already handled above
                                    break;
                                default:
                                    PrintUsage();
                                    error = true;
                                    break;
                            } // Switch

                        } // If starts with '-'

                    }
                    else
                    {
                        return 0;
                    }
                }
            }

            if (!filenameDefined)
            {
                PrintUsage();
                return -1;
            }

            if (error) //Anoop 05/08
            {
                return -1;
            }

            string filenameToProcess = obj_proc_runner.FileName;

            Console.WriteLine("Processing File {0}", filenameToProcess);
            Console.WriteLine();

            // Display the settings

            Console.WriteLine("Minimum Number of ions for valid MSn scan: {0}", obj_dta_generation_parameters.MinIonCount);

            Console.WriteLine("Scan Start: {0}", obj_dta_generation_parameters.MinScan);
            Console.WriteLine("Scan End: {0}", obj_dta_generation_parameters.MaxScan);

            Console.WriteLine("m/z Start: {0}", obj_dta_generation_parameters.MinMass);
            Console.WriteLine("m/z End: {0}", obj_dta_generation_parameters.MaxMass);

            if (obj_dta_generation_parameters.ConsiderChargeValue > 0)
                Console.WriteLine("-C enabled with {0}", obj_dta_generation_parameters.ConsiderChargeValue);

            Console.WriteLine("Spectra to process: {0}", stringSpectraFormat);

            stringOutputFileFormat = obj_dta_generation_parameters.OutputTypeName;
            Console.WriteLine("Output format: {0}", stringOutputFileFormat);

            Console.WriteLine("Create progress file: {0}", obj_dta_generation_parameters.WriteProgressFile);
            Console.WriteLine("Centroid profile mode MSn spectra: {0}", obj_dta_generation_parameters.CentroidMSn.ToString());
            Console.WriteLine();

            obj_proc_runner.HornTransformParameters = obj_transform_parameters;
            obj_proc_runner.PeakProcessorParameters = obj_peak_parameters;
            obj_proc_runner.DTAGenerationParameters = obj_dta_generation_parameters;

            obj_proc_runner.CreateDTAFile();

            sw.Stop();

            Console.WriteLine();
            Console.WriteLine("Done. Finished processing in {0} seconds.", sw.Elapsed.TotalSeconds);

            return 0;
        }

        private void PrintUsage()
        {
            Console.WriteLine();
            Console.WriteLine("DeconMSn usage : DeconMSn [options] filename");
            Console.WriteLine();
            Console.WriteLine("[options] are");
            Console.WriteLine();
            Console.WriteLine("\t -Istring : string is the minimum Number of ions peaks for a scan to be considered [35]");
            Console.WriteLine("\t -Fstring : string is the first Scan to be considered [1]");
            Console.WriteLine("\t -Lstring : string is the last Scan to be considered [1000000]");
            Console.WriteLine("\t -Bstring : string is the minimum setting for Mass Range [200]");
            Console.WriteLine("\t -Tstring : string is the maximum setting for Mass Range [5000]");
            Console.WriteLine("\t -Cstring : string is the charge to be considered [NULL]");
            Console.WriteLine("\t -Pstring : string is the parameter XML file name to be used for processing [default options are set]");
            Console.WriteLine("\t -Dstring : string is the output directory[default - set to same directory as input file]");
            Console.WriteLine("\t -Centroid: Enables centroiding MSn data (when acquired as profile data); off by default");
            Console.WriteLine("              since the m/z values reported by the centroiding algorithm are typically off by several hundred ppm");
            Console.WriteLine("\t -Progress: Creates a _progress.txt file with a percent complete value every 50 scans");
            Console.WriteLine("\t -Sstring : string is the type of spectra to process, options are");
            Console.WriteLine("\t \t ALL : to process all spectra present in the raw file (default)");
            Console.WriteLine("\t \t CID : to process only CID spectra present in the raw file");
            Console.WriteLine("\t \t ETD : to process only ETD spectra present in the raw file");
            Console.WriteLine("\t \t HCD : to process only HCD spectra present in the raw file");
            Console.WriteLine("\t -Xstring : string is output file format, options are");
            Console.WriteLine("\t \t DTA  : Creates .dta files along with a log file (_log.txt) and profile file (_profile.txt)");
            Console.WriteLine("\t \t LOG  : Creates only the log file (_log.txt) and profile file (_profile.txt)");
            Console.WriteLine("\t \t MGF  : Creates a .mgf file along with the log file and profile file");
            Console.WriteLine("\t \t CDTA : Creates a concatenated dta file (_dta.txt), a log file, and a profile file [default option]");
            Console.WriteLine("filename : input file [mzXML or RAW]");
            Console.WriteLine();
            Console.WriteLine("Written by Anoop Mayampurath and Navdeep Jaitly for the Department of Energy");
            Console.WriteLine("Maintained by Matthew Monroe");
            Console.WriteLine("E-mail: matthew.monroe@pnnl.gov or samuel.payne@pnnl.gov");
            Console.WriteLine("Website: http://omics.pnl.gov or http://panomics.pnnl.gov");
        }
    }
}
