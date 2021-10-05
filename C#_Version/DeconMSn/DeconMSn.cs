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
        // Ignore Spelling: Bryson, Anoop Mayampurath, Navdeep Jaitly

        public const string PROGRAM_DATE = "2021-10-05";

        public int RunDeconMSn(string[] args)
        {
            try
            {

                var strAppFolder = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
                var strSVMParamFilePath = Path.Combine(strAppFolder, "svm_params.xml");

                if (args.Length == 0)
                {
                    PrintUsage();
                    return 0;
                }

                var procRunner = new clsProcRunner();
                var transformParameters = new clsHornTransformParameters();
                var dtaGenParameters = new clsDTAGenerationParameters();
                var peakParameters = new clsPeakProcessorParameters();

                var error = false;
                dtaGenParameters.SVMParamFile = strSVMParamFilePath;
                dtaGenParameters.CentroidMSn = false;

                var sw = new Stopwatch();
                sw.Start();

                // First check for a parameter file defined using -P
                if (args.Length > 0)
                {
                    foreach (var currentArg in args)
                    {
                        if (!currentArg.StartsWith("-P") || currentArg.ToLower().StartsWith("-progress"))
                            continue;

                        var stringParamFile = currentArg.Substring(2);

                        Console.WriteLine("Reading Parameter File " + stringParamFile);

                        var rdr = new XmlTextReader(stringParamFile);

                        //Read each node in the tree.
                        while (rdr.Read())
                        {
                            switch (rdr.NodeType)
                            {
                                case XmlNodeType.Element:
                                    if (string.Equals(rdr.Name, "PeakParameters", StringComparison.OrdinalIgnoreCase))
                                    {
                                        peakParameters.LoadV1PeakParameters(rdr);
                                    }
                                    else if (string.Equals(rdr.Name, "HornTransformParameters", StringComparison.OrdinalIgnoreCase))
                                    {
#pragma warning disable 618
                                        transformParameters.LoadV1HornTransformParameters(rdr);
#pragma warning restore 618
                                    }
                                    else if (string.Equals(rdr.Name, "DTAGenerationParameters", StringComparison.OrdinalIgnoreCase))
                                    {
                                        dtaGenParameters.LoadV1DTAGenerationParameters(rdr);
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }

                        rdr.Close();
                    }
                }

                var filenameDefined = false;
                var stringSpectraFormat = "ALL";

                if (args.Length == 0)
                {
                    PrintUsage();
                    return 0;
                }

                // Read the remaining parameters
                foreach (var arg in args)
                {
                    var currentArg = GetCommandLineArg(arg);

                    if (currentArg == "-?" || currentArg == "/?" || currentArg == "-help" || currentArg == "/help")
                    {
                        PrintUsage();
                        return 0;
                    }

                    if (currentArg.ToLower().StartsWith("-progress"))
                    {
                        dtaGenParameters.WriteProgressFile = true;
                        continue;
                    }

                    if (currentArg.ToLower().StartsWith("-centroid"))
                    {
                        dtaGenParameters.CentroidMSn = true;
                        continue;
                    }

                    if (!currentArg.StartsWith("-"))
                    {
                        // Treat the first non-switch parameter as the dataset to process
                        if (!filenameDefined)
                        {
                            procRunner.FileName = currentArg;
                            filenameDefined = true;
                        }
                        else
                        {
                            Console.WriteLine("Ignoring extra argument " + currentArg);
                        }

                        continue;
                    }

                    if (currentArg.Length <= 1)
                    {
                        Console.WriteLine("Ignoring invalid argument " + currentArg);
                        continue;
                    }

                    switch (currentArg[1])
                    {
                        case 'I':
                            if (GetParamInt(currentArg, "Minimum ion count", out var minIonCount))
                                dtaGenParameters.MinIonCount = minIonCount;
                            else
                                error = true;
                            break;
                        case 'F':
                            if (GetParamInt(currentArg, "First scan", out var scanFirst))
                                dtaGenParameters.MinScan = scanFirst;
                            else
                                error = true;
                            break;
                        case 'L':
                            if (GetParamInt(currentArg, "Last scan", out var scanLast))
                                dtaGenParameters.MaxScan = scanLast;
                            else
                                error = true;
                            break;
                        case 'B':
                            if (GetParamDbl(currentArg, "Min mass", out var minMass))
                                dtaGenParameters.MinMass = minMass;
                            else
                                error = true;
                            break;
                        case 'T':
                            if (GetParamDbl(currentArg, "Max mass", out var maxMass))
                                dtaGenParameters.MaxMass = maxMass;
                            else
                                error = true;
                            break;
                        case 'C':
                            if (GetParamInt(currentArg, "Consider charge", out var considerCharge))
                                dtaGenParameters.ConsiderChargeValue = considerCharge;
                            else
                                error = true;
                            dtaGenParameters.ConsiderChargeValue = considerCharge;
                            break;
                        case 'S':
                            stringSpectraFormat = currentArg.Substring(2);
                            if (string.Equals(stringSpectraFormat, "ETD", StringComparison.OrdinalIgnoreCase))
                            {
                                dtaGenParameters.SpectraType = SPECTRA_TYPE.ETD;
                            }
                            else if (string.Equals(stringSpectraFormat, "CID", StringComparison.OrdinalIgnoreCase))
                            {
                                dtaGenParameters.SpectraType = SPECTRA_TYPE.CID;
                            }
                            else if (string.Equals(stringSpectraFormat, "HCD", StringComparison.OrdinalIgnoreCase))
                            {
                                dtaGenParameters.SpectraType = SPECTRA_TYPE.HCD;
                            }
                            else
                            {
                                stringSpectraFormat = "ALL";
                                dtaGenParameters.SpectraType = SPECTRA_TYPE.ALL;
                            }
                            break;
                        case 'X':
                            var outputFileFormat = currentArg.Substring(2);
                            if (string.Equals(outputFileFormat, "MGF", StringComparison.OrdinalIgnoreCase))
                                dtaGenParameters.OutputType = OUTPUT_TYPE.MGF;
                            else if (string.Equals(outputFileFormat, "LOG", StringComparison.OrdinalIgnoreCase))
                                dtaGenParameters.OutputType = OUTPUT_TYPE.LOG;
                            else if (string.Equals(outputFileFormat, "CDTA", StringComparison.OrdinalIgnoreCase))
                                dtaGenParameters.OutputType = OUTPUT_TYPE.CDTA;
                            else if (string.Equals(outputFileFormat, "MZXML", StringComparison.OrdinalIgnoreCase))
                                dtaGenParameters.OutputType = OUTPUT_TYPE.MZXML;
                            else if (string.Equals(outputFileFormat, "DTA", StringComparison.OrdinalIgnoreCase))
                                dtaGenParameters.OutputType = OUTPUT_TYPE.DTA;
                            else
                            {
                                PrintUsage();
                                error = true;
                            }
                            break;
                        case 'D':
                            var outputFolderPath = currentArg.Substring(2);
                            procRunner.OutputPathForDTACreation = outputFolderPath;
                            break;
                        case 'G': // Used by Extract_msn; ignored by DeconMSn
                            break;
                        case 'M': // Used by Extract_msn; ignored by DeconMSn
                            break;
                        case 'P': // Already handled above
                            break;
                        default:
                            Console.WriteLine("Unrecognized argument: " + currentArg);
                            error = true;
                            break;
                    } // Switch

                    if (!error)
                        continue;

                    Console.WriteLine();
                    Console.WriteLine("For program syntax, run " + GetExeName() + " without any parameters");
                    System.Threading.Thread.Sleep(1500);
                    return -1;
                }

                if (!filenameDefined)
                {
                    PrintUsage();
                    return -1;
                }

                var inputFile = new FileInfo(procRunner.FileName);

                Console.WriteLine("DeconMSn Version: {0}", GetExeVersion());
                Console.WriteLine("Processing File:  {0}", PRISM.PathUtils.CompactPathString(inputFile.FullName, 120));
                Console.WriteLine();

                // Display the settings

                Console.WriteLine("Minimum Number of ions for valid MSn scan: {0}", dtaGenParameters.MinIonCount);

                Console.WriteLine("Scan Start: {0}", dtaGenParameters.MinScan);
                Console.WriteLine("Scan End:   {0}", dtaGenParameters.MaxScan);

                Console.WriteLine("m/z Start:  {0}", dtaGenParameters.MinMass);
                Console.WriteLine("m/z End:    {0}", dtaGenParameters.MaxMass);
                Console.WriteLine();

                if (dtaGenParameters.ConsiderChargeValue > 0)
                {
                    Console.WriteLine("-C enabled with {0}", dtaGenParameters.ConsiderChargeValue);
                    Console.WriteLine();
                }

                Console.WriteLine("Spectra to process:   {0}", stringSpectraFormat);

                var outputTypeName = dtaGenParameters.OutputTypeName;
                Console.WriteLine("Output format:        {0}", outputTypeName);

                Console.WriteLine("Create progress file: {0}", dtaGenParameters.WriteProgressFile);
                Console.WriteLine("Centroid profile mode MSn spectra: {0}", dtaGenParameters.CentroidMSn.ToString());
                Console.WriteLine();

                procRunner.HornTransformParameters = transformParameters;
                procRunner.PeakProcessorParameters = peakParameters;
                procRunner.DTAGenerationParameters = dtaGenParameters;

                var deconMSnVersion = GetExeVersion();
                procRunner.CreateDTAFile(deconMSnVersion);

                sw.Stop();

                Console.WriteLine();
                Console.WriteLine("Done. Finished processing in {0:F1} seconds.", sw.Elapsed.TotalSeconds);

                System.Threading.Thread.Sleep(1500);
                return 0;
            }
            catch (Exception ex)
            {
                Console.WriteLine();
                Console.WriteLine("Error processing: " + ex.Message);
                Console.WriteLine(PRISM.StackTraceFormatter.GetExceptionStackTraceMultiLine(ex));
                System.Threading.Thread.Sleep(2000);
                return -1;
            }
        }

        private string GetCommandLineArg(string currentArg)
        {
            if (currentArg.StartsWith("/"))
            {
                // DeconMSn arguments start with a dash; auto-switch
                return "-" + currentArg.Substring(1);
            }

            return currentArg;
        }

        private string GetExeName()
        {
            return Path.GetFileName(System.Reflection.Assembly.GetEntryAssembly()?.Location);
        }

        public static string GetExeVersion()
        {
            return PRISM.FileProcessor.ProcessFilesOrDirectoriesBase.GetAppVersion(PROGRAM_DATE);
        }

        private bool GetParamDbl(string currentArg, string argName, out double argValue)
        {
            var argValueText = currentArg.Substring(2).TrimStart(':');

            if (double.TryParse(argValueText, out argValue))
            {
                return true;
            }

            argValue = 0;
            Console.WriteLine("{0} for -{1} must be a number", argName, currentArg[0]);
            return false;
        }

        private bool GetParamInt(string currentArg, string argName, out int argValue)
        {
            var argValueText = currentArg.Substring(2).TrimStart(':');

            if (int.TryParse(argValueText, out argValue))
            {
                return true;
            }

            argValue = 0;
            Console.WriteLine("{0} for -{1} must be an integer", argName, currentArg[0]);
            return false;
        }

        private void PrintUsage()
        {
            Console.WriteLine();
            Console.WriteLine("DeconMSn Version: " + GetExeVersion());
            Console.WriteLine();
            Console.WriteLine("Syntax: " + GetExeName() + " [options] filename");
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
            Console.WriteLine("\t \t CDTA : Creates a concatenated DTA file (_dta.txt), a log file, and a profile file [default option]");
            Console.WriteLine("filename : input file [mzXML or RAW]");
            Console.WriteLine();
            Console.WriteLine("Written by Anoop Mayampurath and Navdeep Jaitly for the Department of Energy");
            Console.WriteLine("Maintained by Bryson Gibbons and Matthew Monroe");
            Console.WriteLine("E-mail: matthew.monroe@pnnl.gov or proteomics@pnnl.gov");
            Console.WriteLine("Website: https://github.com/PNNL-Comp-Mass-Spec/ or https://panomics.pnnl.gov/ or https://www.pnnl.gov/integrative-omics");

            System.Threading.Thread.Sleep(2500);
        }
    }
}
