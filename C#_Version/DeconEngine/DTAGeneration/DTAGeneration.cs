#if !Disable_Obsolete
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using DeconToolsV2;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;
using DeconToolsV2.Readers;
using Engine.HornTransform;
using Engine.PeakProcessing;
using Engine.Utilities;

namespace Engine.DTAProcessing
{
    // Used for DeconMSn
    [Obsolete("Only used by Decon2LS.UI", false)]
    internal class DTAProcessor
    {
        private int mint_minIonCount;
        private int mint_minScan;
        private int mint_maxScan;
        private int mint_window_size;
        private int mint_consider_charge;
        private int mint_isolation_window_size;

        private double mdbl_minMass;
        private double mdbl_maxMass;
        private double mdbl_pkBkgRatio;
        private double mdbl_peptideMinBkgRatio;
        private double mdbl_min_peptide_intensity;
        private double mdbl_parent_Intensity;
        private double mdbl_parent_Mz;
        private double mdbl_cc_mass;
        private double mdbl_min_fit_for_single_spectra;

        private double mdbl_mono_mz_from_header;

        private bool mbln_create_log_file_only;
        private bool mbln_create_composite_dta;
        private bool mbln_consider_multiple_precursors;

        // Warning: the masses reported by GetMassListFromScanNum when centroiding are not properly calibrated and thus could be off by 0.3 m/z or more
        //          For example, in scan 8101 of dataset RAW_Franc_Salm_IMAC_0h_R1A_18Jul13_Frodo_13-04-15, we see these values:
        //          Profile m/z         Centroid m/z	Delta_PPM
        //			112.051 			112.077			232
        //			652.3752			652.4645		137
        //			1032.56495			1032.6863		118
        //			1513.7252			1513.9168		127
        private bool mbln_centroid_msn;

        private bool mbln_is_profile_data_for_mzXML;
        private bool mbln_first_scan_written;

        private List<double> mvect_mzs_parent = new List<double>();
        private List<double> mvect_intensities_parent = new List<double>();
        private List<double> mvect_mzs_msN = new List<double>();
        private List<double> mvect_intensities_msN = new List<double>();
        private List<int> mvect_chargeStateList = new List<int>();

        private clsPeak mobj_parentPeak;
        private clsHornTransformResults mobj_transformRecord = new clsHornTransformResults();
        private Engine.DTAProcessing.MSnInformationRecord mobj_msn_record = new MSnInformationRecord();
        private Engine.DTAProcessing.ProfileRecord mobj_profile_record;
        private Engine.PeakProcessing.PeakProcessor mobj_parent_peak_processor;
        private Engine.PeakProcessing.PeakProcessor mobj_msN_peak_processor;
        private Engine.PeakProcessing.PeakProcessor mobj_summed_peak_processor;
        private Engine.HornTransform.MassTransform mobj_mass_transform;
        private Engine.ChargeDetermination.SVMChargeDetermine mobj_svm_charge_determination;
        private List<clsHornTransformResults> mvect_transformRecords = new List<clsHornTransformResults>();
        private List<Engine.DTAProcessing.MSnInformationRecord> mvect_msn_records = new List<MSnInformationRecord>();
        private List<Engine.DTAProcessing.ProfileRecord> mvect_profile_records = new List<ProfileRecord>();

        public int mint_msNscanIndex;
        public int mint_NumMSnScansProcessed;
        public int mint_NumDTARecords;

        public bool mbln_write_progress_file;

        public Engine.Readers.RawData mobj_raw_data_dta;
        public FileType menm_dataset_type;

        public SortedDictionary<int, int> mmap_msN_parentIndex = new SortedDictionary<int, int>();

        public string mch_log_filename;
        public string mch_progress_filename;
        public string mch_comb_dta_filename;
        public string mch_profile_filename;
        public string mch_mgf_filename;
        public string mch_output_file;
        public string mch_dataset_name;

        public StreamWriter mfile_log;
        public StreamWriter mfile_comb_dta;
        public StreamWriter mfile_mgf;

        // default constructor.
        public DTAProcessor()
        {
            mobj_parent_peak_processor = new Engine.PeakProcessing.PeakProcessor();
            mobj_msN_peak_processor = new Engine.PeakProcessing.PeakProcessor();
            mobj_summed_peak_processor = new Engine.PeakProcessing.PeakProcessor();
            mobj_mass_transform = new Engine.HornTransform.MassTransform();
            mobj_svm_charge_determination = new Engine.ChargeDetermination.SVMChargeDetermine();
            mobj_raw_data_dta = null;
            mdbl_maxMass = 0.0;
            mint_maxScan = 0;
            mint_minIonCount = 0;
            mdbl_minMass = 0.0;
            mdbl_parent_Mz = 0.0;
            mdbl_parent_Intensity = 0.0;
            mint_minScan = 0;
            mint_window_size = 5;
            mint_msNscanIndex = 0;
            mdbl_cc_mass = 1.00727638;
            mint_consider_charge = 0;
            mdbl_min_fit_for_single_spectra = 0.1;
            mint_NumDTARecords = 0;
            mint_NumMSnScansProcessed = 0;
            mint_isolation_window_size = 3;
            mbln_consider_multiple_precursors = false;
            mbln_centroid_msn = false;
                // Warning: the masses reported by GetMassListFromScanNum when centroiding are not properly calibrated and thus could be off by 0.3 m/z or more
            menm_dataset_type = FileType.FINNIGAN;
            mbln_is_profile_data_for_mzXML = false;
            mbln_first_scan_written = false;
        }

        // destructor.
        ~DTAProcessor()
        {
            if (mobj_raw_data_dta != null)
            {
                mobj_raw_data_dta.Close();
            }
        }

        /// <summary>
        /// Sets the Peak Processor Options for peak data
        /// </summary>
        /// <param name="s2n">Signal-To-Noise Ratio</param>
        /// <param name="thresh">Peak-Intensity Threshold</param>
        /// <param name="fit_type">Peak-Fit Type</param>
        public void SetDTAOptions(int minIonCount, int minScan, int maxScan, double minMass, double maxMass,
            bool createLogFileOnly, bool createCompositeDTA, int considerCharge, bool considerMultiplePrecursors,
            int isolationWindowSize, bool isProfileDataForMzXML)
        {
            mint_minIonCount = minIonCount;
            mdbl_maxMass = maxMass;
            mdbl_minMass = minMass;
            mint_maxScan = maxScan;
            mint_minScan = minScan;
            mbln_create_composite_dta = createCompositeDTA;
            mbln_create_log_file_only = createLogFileOnly;
            mint_consider_charge = considerCharge;
            mint_isolation_window_size = isolationWindowSize;
            mbln_consider_multiple_precursors = considerMultiplePrecursors;
            mbln_is_profile_data_for_mzXML = isProfileDataForMzXML;
        }

        /// <summary>
        /// Sets the Peak Processor Options for peak data
        /// </summary>
        /// <param name="s2n">Signal-To-Noise Ratio</param>
        /// <param name="thresh">Peak-Intensity Threshold</param>
        /// <param name="fit_type">Peak-Fit Type</param>
        public void SetDTAOptions(int minIonCount, int minScan, int maxScan, double minMass, double maxMass,
            bool createLogFileOnly, bool createCompositeDTA, int considerCharge, bool considerMultiplePrecursors,
            bool centroid, int isolationWindowSize, bool isProfileDataForMzXML)
        {
            mint_minIonCount = minIonCount;
            mdbl_maxMass = maxMass;
            mdbl_minMass = minMass;
            mint_maxScan = maxScan;
            mint_minScan = minScan;
            mbln_create_composite_dta = createCompositeDTA;
            mbln_create_log_file_only = createLogFileOnly;
            mint_consider_charge = considerCharge;
            mint_isolation_window_size = isolationWindowSize;
            mbln_consider_multiple_precursors = considerMultiplePrecursors;
            mbln_centroid_msn = centroid;
            mbln_is_profile_data_for_mzXML = isProfileDataForMzXML;
            mbln_first_scan_written = false;
        }

        public void SetPeakParameters(double pkBkgRatio, double peptideMinBkgRatio)
        {
            SetPeakParametersLowResolution(pkBkgRatio, peptideMinBkgRatio);
        }

        public void SetPeakParametersLowResolution(double pkBkgRatio, double peptideMinBkgRatio)
        {
            mdbl_pkBkgRatio = pkBkgRatio;
            mdbl_peptideMinBkgRatio = peptideMinBkgRatio;
        }

        public void SetPeakProcessorOptions(double s2n, double thresh, bool thresholded,
            Engine.PeakProcessing.PeakFitType fit_type)
        {
            mobj_parent_peak_processor.SetOptions(s2n, thresh, thresholded, fit_type);
            mobj_msN_peak_processor.SetOptions(s2n, thresh, thresholded, fit_type);
            mobj_summed_peak_processor.SetOptions(s2n, thresh, thresholded, fit_type);
        }

        public void SetMassTransformOptions(short max_charge, double max_mw, double max_fit, double min_s2n,
            double cc_mass, double delete_threshold_intensity, double min_theoretical_intensity_for_score,
            short num_peaks_for_shoulder, bool use_mercury_caching, bool o16_o18_media, string averagine_mf,
            string tag_mf, bool thrash_or_not, bool complete_fit, bool chk_again_ch1, enmIsotopeFitType fit_type,
            clsElementIsotopes atomic_info)
        {
            mobj_mass_transform.SetOptions(max_charge, max_mw, max_fit, min_s2n, cc_mass, delete_threshold_intensity,
                min_theoretical_intensity_for_score, num_peaks_for_shoulder, chk_again_ch1, use_mercury_caching,
                o16_o18_media);

            mobj_mass_transform.SetIsotopeFitOptions(averagine_mf, tag_mf, thrash_or_not, complete_fit);
            mobj_mass_transform.ElementalIsotopeComposition = atomic_info;
            mobj_mass_transform.IsotopeFitType = fit_type;
        }

        public bool FindPrecursorForChargeStates()
        {
            //simple check for charge state
            double mono_mz = 0;
            double mono_intensity = 0;
            double max_intensity = mdbl_parent_Intensity;
            double mz_prev = 0;
            double mz_next = 0;
            int pk_index = 0;
            double fwhm = 0;

            fwhm = mobj_parentPeak.FWHM;

            // This has known to happen
            if (fwhm > 1)
                return false;

            //preference given to higher charge states
            try
            {
                for (int cs = 4; cs > 0; cs--)
                {
                    //Look back first
                    mz_prev = mdbl_parent_Mz - mdbl_cc_mass / cs;
                    clsPeak pk_prev;
                    mobj_parent_peak_processor.PeakData.FindPeakAbsolute(mz_prev - fwhm / cs, mz_prev + fwhm / cs,
                        out pk_prev);
                    if (pk_prev.Mz > 0 && pk_prev.Intensity > max_intensity / 8)
                    {
                        while (pk_prev.Mz > 0 && pk_prev.Intensity > max_intensity / 8)
                        {
                            mono_mz = pk_prev.Mz;
                            mono_intensity = pk_prev.Intensity;
                            mz_prev = pk_prev.Mz - 1.003 / cs;
                            pk_index = pk_prev.PeakIndex;
                            if (pk_prev.Intensity > max_intensity)
                                max_intensity = pk_prev.Intensity;
                            mobj_parent_peak_processor.PeakData.FindPeakAbsolute(mz_prev - fwhm / cs,
                                mz_prev + fwhm / cs, out pk_prev);
                        }

                        double parent_mono = (mdbl_parent_Mz - mdbl_cc_mass) * cs;
                        double mono_mw = (mono_mz - mdbl_cc_mass) * cs;
                        if (Math.Abs(parent_mono - mono_mw) < 4) // to route out co-eluting peptides
                        {
                            mobj_transformRecord.Mz = mono_mz;
                            mobj_transformRecord.ChargeState = (short) cs;
                            mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                          mobj_transformRecord.ChargeState;
                            mobj_transformRecord.Fit = 1;
                            mobj_transformRecord.FitCountBasis = 0;
                            mobj_transformRecord.FitCountBasis = 0;
                            mobj_transformRecord.PeakIndex = pk_index;
                            //mobj_transformRecord.MonoIntensity = (int) pk_prev.Intensity;
                            mobj_transformRecord.MonoIntensity = (int) mono_intensity;
                            mvect_transformRecords.Add(mobj_transformRecord);
                            return true;
                        }
                    }
                    // Look ahead
                    mz_next = mdbl_parent_Mz + 1.003 / cs;
                    clsPeak pk_next;
                    mobj_parent_peak_processor.PeakData.FindPeakAbsolute(mz_next - fwhm / cs, mz_next + fwhm / cs,
                        out pk_next);
                    if (pk_next.Mz > 0 && pk_next.Intensity > max_intensity / 8)
                    {
                        //this is the first isotope
                        mobj_transformRecord.Mz = mdbl_parent_Mz;
                        mobj_transformRecord.ChargeState = (short) cs;
                        mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                      mobj_transformRecord.ChargeState;
                        mobj_transformRecord.Fit = 1;
                        mobj_transformRecord.FitCountBasis = 0;
                        mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                        mobj_transformRecord.PeakIndex = mobj_parentPeak.PeakIndex;
                        mvect_transformRecords.Add(mobj_transformRecord);
                        return true;
                    }
                }
            }
            catch (System.Exception e)
            {
#if DEBUG
                throw e;
#endif
                return false;
            }

            //Not found any peaks
            return false;
        }

        public void WriteSpectrumToFile(int scan_num, List<double> mzs, List<double> intensities)
        {
            //var fileName = "C:\\DeconMsn\\DeconTestFolder\\TestFile" + scan_num + ".spectra";
            var fileName = "C:\\DeconMsn\\DeconTestFolder\\TestFile.spectra";
            using (var stream = new StreamWriter(new FileStream(fileName, FileMode.Create, FileAccess.Write, FileShare.None)))
            {
                for (var i = 0; i < mzs.Count; i++)
                {
                    stream.WriteLine("{0:F10}\t{1:F10}", mzs[i], intensities[i]);
                }
            }
        }

        public bool DeisotopeSummedSpectra(int parent_scan_number, double parent_mz)
        {
            bool found_transform = false;
            double minMZ = parent_mz - mint_window_size;
            double maxMZ = parent_mz + mint_window_size;
            int mono_orig_intensity = 0;

            List<double> vect_mzs_full;
            List<double> vect_intensities_full;

            //Settting to see if found_precursor worked
            bool found_precursor = false;
            if (mvect_transformRecords.Count > 0)
                found_precursor = true;

            //check to see if it is a MS scan i.e. to say perform summing only on MS scans
            if (!mobj_raw_data_dta.IsMSScan(parent_scan_number))
                return false;

            //get raw data first
            try
            {
                mobj_raw_data_dta.GetSummedSpectra(out vect_mzs_full, out vect_intensities_full, parent_scan_number, 2, minMZ, maxMZ);
            }
            catch (System.Exception)
            {
                return false;
            }
            //write output spectra to file
            //WriteSpectrumToFile (parent_scan_number, vect_mzs_full, vect_intensities_full);

            if (vect_intensities_full.Count <= 1)
                return false;

            // Condense regions where adjacent ions all have an intensity of 0
            List<double> vect_mzs = new List<double>();
            List<double> vect_intensities = new List<double>();

            var num_pts = vect_intensities_full.Count;
            int condenseDataThreshold = 10000;

            if (num_pts < condenseDataThreshold)
            {
                // Keep all of the data
                vect_mzs = vect_mzs_full;
                vect_intensities = vect_intensities_full;
            }
            else
            {
                double epsilon = 1E-10;
                double mzMergeTolerancePPM = 0.25;
                double previousMZ = -1;
                double previousIntensity = -1;

                for (var i = 0; i < num_pts; i++)
                {
                    bool addPeak = true;

                    if (i > 0 && i < num_pts - 1)
                    {
                        if (vect_intensities_full[i] > epsilon ||
                            (vect_intensities_full[i] < epsilon && vect_intensities_full[i - 1] > epsilon) ||
                            (vect_intensities_full[i] < epsilon && vect_intensities_full[i + 1] > epsilon))
                        {

                            if (previousMZ > -1)
                            {
                                double deltaMZ = vect_mzs_full[i] - previousMZ;
                                double deltaPPM = deltaMZ / (previousMZ / 1E6);
                                if (deltaPPM < mzMergeTolerancePPM)
                                {
                                    if (vect_intensities_full[i] > previousIntensity)
                                    {
                                        // Replace the previous data point with this data point
                                        vect_mzs.RemoveAt(vect_mzs.Count - 1);
                                        vect_intensities.RemoveAt(vect_intensities.Count - 1);
                                    }
                                    else
                                    {
                                        addPeak = false;
                                    }
                                }
                            }
                        }
                        else
                        {
                            addPeak = false;
                        }
                    }

                    if (addPeak)
                    {
                        vect_mzs.Add(vect_mzs_full[i]);
                        vect_intensities.Add(vect_intensities_full[i]);

                        previousMZ = vect_mzs[vect_mzs.Count - 1];
                        previousIntensity = vect_intensities[vect_intensities.Count - 1];
                    }

                }
            }

            var num_pts2 = vect_intensities.Count ;

            //discover peaks
            //computes the average of all points in the spectrum (all points below FLT_MAX)
            double thres = DeconEngine.Utils.GetAverage(vect_intensities, float.MaxValue);

            //compute the average of all points below that are below 5 times the average of all points in the spectrum
            double background_intensity = DeconEngine.Utils.GetAverage(vect_intensities, (float) (5 * thres));

            //ARS added the correct calculation for noise floor level from a DeconToolsV2 presentation
            //double calc_background_intensity = DeconEngine::Utils::GetBackgroundLevel(vect_intensities, FLT_MAX);

            mobj_summed_peak_processor.SetPeakIntensityThreshold(background_intensity * 5);
            //hard-coded now, need to change it later
            mobj_summed_peak_processor.SetPeaksProfileType(mobj_raw_data_dta.IsProfileScan(parent_scan_number));
            int numPeaks = mobj_summed_peak_processor.DiscoverPeaks(vect_mzs, vect_intensities);
            mobj_summed_peak_processor.PeakData.InitializeUnprocessedPeakData();

            //System.Console.WriteLine("{0}\t{1}\t{2}", parent_scan_number, calc_background_intensity, numPeaks);

            //Get ParentPeak and index in this peak processor
            double parent_Mz_match = mobj_summed_peak_processor.GetClosestPeakMz(parent_mz, out mobj_parentPeak);
            if (mobj_parentPeak.Intensity < mdbl_min_peptide_intensity)
            {
                return false;
            }

            int parent_index = mobj_parentPeak.PeakIndex;

            try
            {
                // Make sure that checking against charge 1 is false
                short maxcs;
                double maxmw;
                double maxfit;
                double mins2n;
                double ccmass;
                double delthintensity;
                double minthscore;
                short numpeaks;
                bool chkcharge1;
                bool newchkcharge1 = false;
                bool usemercury;
                bool o16018media;

                mobj_mass_transform.GetOptions(out maxcs, out maxmw, out maxfit, out mins2n, out ccmass,
                    out delthintensity, out minthscore, out numpeaks, out chkcharge1, out usemercury, out o16018media);
                mobj_mass_transform.SetOptions(maxcs, maxmw, maxfit, mins2n, ccmass, delthintensity, minthscore,
                    numpeaks, newchkcharge1, usemercury, o16018media);

                // now start THRASH all over again
                clsPeak currentPeak;
                clsHornTransformResults transformRecord;
                List<clsHornTransformResults> vectTransformRecord = new List<clsHornTransformResults>();

                bool found_peak = mobj_summed_peak_processor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                double fwhm_SN = currentPeak.FWHM;
                int numDeisotoped = 0;

                while (found_peak)
                {
                    found_transform = mobj_mass_transform.FindTransform(mobj_summed_peak_processor.PeakData,
                        ref currentPeak, out transformRecord);
                    if (found_transform)
                    {
                        numDeisotoped++;
                        vectTransformRecord.Add(transformRecord);
                    }
                    found_peak = mobj_summed_peak_processor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                }

                // not ejecting precusor record here as the indices are going to be off
                // also algo is changed to use only THRASH if both cs are equal
                bool found_transform_record = false;
                for (int recordNum = 0; recordNum < numDeisotoped; recordNum++)
                {
                    transformRecord = vectTransformRecord[recordNum];
                    int num_isotopes = transformRecord.NumIsotopesObserved;
                    for (int isotope_num = 0; isotope_num < num_isotopes && !found_transform_record; isotope_num++)
                    {
                        int isotopeIndex = transformRecord.IsotopePeakIndices[isotope_num];
                        if (isotopeIndex == parent_index)
                        {
                            found_transform_record = true;
                            mobj_transformRecord = transformRecord;
                            break;
                        }
                    }
                }
                if (found_transform_record)
                {
                    // set mono intensity to replace the one that summes spectra would return
                    clsPeak tempPeak;
                    double mono_match = mobj_parent_peak_processor.GetClosestPeakMz(mobj_transformRecord.Mz,
                        out tempPeak);
                    if (tempPeak.Intensity > mdbl_min_peptide_intensity)
                    {
                        mono_orig_intensity = (int) tempPeak.Intensity;
                    }
                    else
                    {
                        // check to see if the mono is the parent here
                        double mono_mz = (mobj_transformRecord.mdbl_mono_mw) / mobj_transformRecord.mshort_cs + mdbl_cc_mass;
                        if (Math.Abs(mono_mz - mdbl_parent_Mz) < 0.01)
                            mono_orig_intensity = (int)mdbl_parent_Intensity;
                        else
                            mono_orig_intensity = (int) mdbl_min_peptide_intensity;
                    }

                    mobj_transformRecord.MonoIntensity = mono_orig_intensity;

                    // store in vector and clear
                    mvect_transformRecords.Add(mobj_transformRecord);
                    mobj_mass_transform.SetOptions(maxcs, maxmw, maxfit, mins2n, ccmass, delthintensity, minthscore,
                        numpeaks, chkcharge1, usemercury, o16018media); //reset chk_charge1
                    return true;
                }

                mobj_mass_transform.SetOptions(maxcs, maxmw, maxfit, mins2n, ccmass, delthintensity, minthscore,
                    numpeaks, chkcharge1, usemercury, o16018media); //reset chk_charge1
                return false;
            }
            catch (System.Exception e)
            {
#if DEBUG
                throw e;
#endif
                return false;
            }
        }

        /**
        Implemented support for zoom scans, the strategy here would be to assume that the peak with the highest intensity
        was selected for fragmentation. Future work would be to be smart about precursor selection in zoom scans
        */
        public bool GenerateDTAZoomScans(int msN_scan_number, int parent_scan_number, int msN_scan_index)
        {
            var found_precursor = true;
            var smoother = new SavGolSmoother();
            smoother.SetOptions(16, 16, 4);
            smoother.Smooth(ref mvect_mzs_parent, ref mvect_intensities_parent);
            WriteSpectrumToFile(parent_scan_number, mvect_mzs_parent, mvect_intensities_parent);

            //mobj_parent_peak_processor->ApplyMovingAverageFilter(&mvect_mzs_parent, &mvect_intensities_parent, 3) ;

            int numPeaks = mobj_parent_peak_processor.DiscoverPeaks(mvect_mzs_parent, mvect_intensities_parent);

            if (numPeaks > 0)
            {
                found_precursor = GenerateDTALowRes(msN_scan_number, parent_scan_number, msN_scan_index);
            }
            else
            {
                found_precursor = false;
            }

            return found_precursor;
        }

        public bool GenerateDTA(int msN_scan_number, int parent_scan_number)
        {
            bool found_transform = false;
            bool found_transform_record = false;
            bool found_precursor = false;
            bool found_transform_thru_summing = false;
            bool found_other_precursors = false;

            // Clear previous and reset objects
            mvect_transformRecords.Clear();
            mdbl_parent_Intensity = 0;
            mdbl_parent_Mz = 0;
            mobj_transformRecord = new clsHornTransformResults();
            mobj_transformRecord.Mz = 0;
            mobj_transformRecord.ChargeState = 0;
            mobj_transformRecord.MonoMw = 0.0;
            mobj_transformRecord.Fit = -1;
            mobj_transformRecord.FitCountBasis = 0;
            mobj_transformRecord.MonoIntensity = 0;
            mobj_msn_record = new MSnInformationRecord();
            mobj_msn_record.mdbl_mono_mw = 0.0;
            mobj_msn_record.mint_mono_intensity = 0;
            mobj_msn_record.mdbl_mono_mz = 0;
            mobj_msn_record.mshort_cs = 0;
            mobj_msn_record.mdbl_fit = -1;

            //check if we have enough values in the ms2
            int numPeaks = mvect_mzs_msN.Count;
            if (numPeaks < mint_minIonCount)
            {
                Console.WriteLine("Skipping scan {0} since too few peaks: {1} < {2}", msN_scan_number, numPeaks, mint_minIonCount);
                return found_transform;
            }

            mint_NumMSnScansProcessed++;

            //get parent
            double parent_mz = mobj_raw_data_dta.GetParentMz(msN_scan_number);

            // if -C option
            /*if (mint_consider_charge > 0)
            {
                // see if we can detect the parent first to get intensity right
                double parent_match = mobj_parent_peak_processor.GetClosestPeakMz(parent_mz, out mobj_parentPeak);
                if (mobj_parentPeak.Intensity > mdbl_min_peptide_intensity)
                {
                    mdbl_parent_Mz = mobj_parentPeak.Mz;
                    mdbl_parent_Intensity = mobj_parentPeak.Intensity;
                }
                else
                {
                    mdbl_parent_Mz = parent_mz;
                    mdbl_parent_Intensity = mdbl_min_peptide_intensity;
                }

                mobj_transformRecord.Mz = mdbl_parent_Mz;
                mobj_transformRecord.ChargeState = (short) mint_consider_charge;
                mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                    mobj_transformRecord.ChargeState;
                mobj_transformRecord.Fit = 1;
                mobj_transformRecord.FitCountBasis = 0;
                mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                mvect_transformRecords.Add(mobj_transformRecord);
                return true;
            }*/

            //start
            double parent_Mz_match = mobj_parent_peak_processor.GetClosestPeakMz(parent_mz, out mobj_parentPeak);
            if (mobj_parentPeak.Intensity > mdbl_min_peptide_intensity)
            {
                mdbl_parent_Mz = mobj_parentPeak.Mz;
                mdbl_parent_Intensity = mobj_parentPeak.Intensity;
                int parent_index = mobj_parentPeak.PeakIndex;

                // Check for CS based detection first
                found_precursor = FindPrecursorForChargeStates();

                double minMZ = parent_Mz_match - mint_window_size;
                double maxMZ = parent_Mz_match + mint_window_size;

                clsPeak currentPeak;
                clsHornTransformResults transformRecord;
                clsHornTransformResults precursorRecord;
                List<clsHornTransformResults> vectTransformRecord = new List<clsHornTransformResults>();

                bool found_peak = mobj_parent_peak_processor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                double fwhm_SN = currentPeak.FWHM;
                int numDeisotoped = 0;

                while (found_peak)
                {
                    if (currentPeak.Intensity < mdbl_min_peptide_intensity)
                        break;
                    found_transform = mobj_mass_transform.FindTransform(mobj_parent_peak_processor.PeakData,
                        ref currentPeak, out transformRecord);
                    if (found_transform)
                    {
                        numDeisotoped++;
                        vectTransformRecord.Add(transformRecord);
                    }
                    found_peak = mobj_parent_peak_processor.PeakData.GetNextPeak(minMZ, maxMZ, out currentPeak);
                }

                if (numDeisotoped == 1)
                {
                    // only  one so get it in
                    found_transform_record = false;
                    transformRecord = new clsHornTransformResults(vectTransformRecord[0]);
                    int num_isotopes = transformRecord.NumIsotopesObserved;
                    if (transformRecord.Fit < mdbl_min_fit_for_single_spectra)
                        // AM: to give slight edge to summing
                    {
                        for (int isotope_num = 0; isotope_num < num_isotopes && !found_transform_record; isotope_num++)
                        {
                            int isotopeIndex = transformRecord.IsotopePeakIndices[isotope_num];
                            if (isotopeIndex == parent_index)
                            {
                                found_transform_record = true;
                                mobj_transformRecord = transformRecord;
                                break;
                            }
                        }
                    }
                    if (found_transform_record)
                    {
                        mvect_transformRecords.Add(mobj_transformRecord);
                        return true;
                    }
                }
                else if (numDeisotoped > 1)
                {
                    if (!mbln_consider_multiple_precursors)
                    {
                        // more than one and consider only one precursor, so might be co-eluting peptides
                        // see if we mave a match first
                        found_transform_record = false;
                        for (int transform_num = 0;
                            transform_num < (int) vectTransformRecord.Count && !found_transform_record;
                            transform_num++)
                        {
                            transformRecord = new clsHornTransformResults(vectTransformRecord[transform_num]);
                            int num_isotopes = transformRecord.NumIsotopesObserved;
                            if (transformRecord.Fit < mdbl_min_fit_for_single_spectra)
                                // AM: to give slight edge to summing
                            {
                                for (int isotope_num = 0; isotope_num < num_isotopes; isotope_num++)
                                {
                                    int isotopeIndex = transformRecord.IsotopePeakIndices[isotope_num];
                                    if (isotopeIndex == parent_index)
                                    {
                                        found_transform_record = true;
                                        mobj_transformRecord = transformRecord;
                                        break;
                                    }
                                }
                            }
                        }
                        if (found_transform_record)
                        {
                            if (found_precursor)
                            {
                                // found both THRASH and precursor, so if precursor is present in any of the
                                // transform records eliminate it
                                bool found_precursor_record = false;
                                precursorRecord = mvect_transformRecords[0];
                                for (int transform_num = 0;
                                    transform_num < (int) vectTransformRecord.Count && !found_precursor_record;
                                    transform_num++)
                                {
                                    transformRecord = new clsHornTransformResults(vectTransformRecord[transform_num]);
                                    int num_isotopes = transformRecord.NumIsotopesObserved;
                                    for (int isotope_num = 0; isotope_num < num_isotopes; isotope_num++)
                                    {
                                        int isotopeIndex = transformRecord.IsotopePeakIndices[isotope_num];
                                        if (isotopeIndex == precursorRecord.PeakIndex)
                                        {
                                            //found it, eject it
                                            found_precursor_record = true;
                                            mvect_transformRecords.Clear();
                                            mvect_transformRecords.Add(new clsHornTransformResults(mobj_transformRecord));
                                            break;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                mvect_transformRecords.Add(new clsHornTransformResults(mobj_transformRecord));
                                return true;
                            }
                        }
                    }
                    else
                    {
                        //consider multiple precursors in on, so spit dtas for all precursors with isolation_window_szie from parent peak
                        for (int transform_num = 0;
                            transform_num < (int) vectTransformRecord.Count && !found_transform_record;
                            transform_num++)
                        {
                            transformRecord = new clsHornTransformResults(vectTransformRecord[transform_num]);
                            if (Math.Abs(transformRecord.Mz - parent_Mz_match) < mint_isolation_window_size)
                            {
                                mvect_transformRecords.Add(transformRecord);
                                found_other_precursors = true;
                            }
                        }

                        if (found_other_precursors)
                            return true;
                    }
                }
            }
            else
            {
                // to set parent intensity find the parent that is closest to the reported parent mz
                clsPeak tempPeak;
                mobj_parent_peak_processor.PeakData.FindPeak(parent_mz - 0.02, parent_mz + 0.02, out tempPeak);
                if (tempPeak.Intensity > 0)
                {
                    mdbl_parent_Mz = tempPeak.Mz;
                    mdbl_parent_Intensity = tempPeak.Intensity;
                }
                else
                {
                    mdbl_parent_Mz = parent_mz;
                    mdbl_parent_Intensity = mdbl_min_peptide_intensity;
                }
            }

            if (!found_transform_record)
            {
                // Either THRASH failed or Peak was below noise floor for that scan so sum spectra
                GetParentScanSpectra(parent_scan_number, mdbl_pkBkgRatio, mdbl_peptideMinBkgRatio);
                found_transform_thru_summing = DeisotopeSummedSpectra(parent_scan_number, parent_mz);
            }

            if (found_transform_thru_summing)
            {
                return true;
            }

            // passed conditions, couldn't THRASH nor FindPeak
            if (!found_precursor)
            {
                // assign default charge states 2 and 3
                mobj_transformRecord.Mz = mdbl_parent_Mz;
                mobj_transformRecord.ChargeState = 2;
                mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                    mobj_transformRecord.ChargeState;
                mobj_transformRecord.Fit = 1;
                mobj_transformRecord.FitCountBasis = 0;
                mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                mvect_transformRecords.Add(new clsHornTransformResults(mobj_transformRecord));
                mobj_transformRecord.Mz = mdbl_parent_Mz;
                mobj_transformRecord.ChargeState = 3;
                mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                    mobj_transformRecord.ChargeState;
                mobj_transformRecord.Fit = 1;
                mobj_transformRecord.FitCountBasis = 0;
                mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                mvect_transformRecords.Add(new clsHornTransformResults(mobj_transformRecord));
                return true;
            }

            return true;
        }

        public bool GenerateDTALowRes(int msN_scan_number, int parent_scan_number, int msN_scan_index)
        {
            mobj_transformRecord = new clsHornTransformResults();
            //Apply a heavy smoothing using savitsky golay filter
            var smoother = new SavGolSmoother();
            smoother.SetOptions(16, 16, 4);
            smoother.Smooth(ref mvect_mzs_parent, ref mvect_intensities_parent);

            int numPeaks = mobj_parent_peak_processor.DiscoverPeaks(mvect_mzs_parent, mvect_intensities_parent) ;

            if ( numPeaks <= 0 ){
                return false;
            }

            mvect_transformRecords.Clear();
            mdbl_parent_Intensity = 0;
            mdbl_parent_Mz = 0;
            bool chargeOne = false;

            //check if we have enough values in the ms2
            numPeaks = mvect_mzs_msN.Count;
            if (numPeaks < mint_minIonCount)
                return false;

            // Get the parent m/z for that msn scan
            double parent_Mz = mobj_raw_data_dta.GetParentMz(msN_scan_number);

            // Now start
            clsPeak parentPeak = new clsPeak();
            double parent_Mz_match = mobj_parent_peak_processor.GetClosestPeakMz(parent_Mz, out mobj_parentPeak);
            if (mobj_parentPeak.Intensity < mdbl_min_peptide_intensity)
            {
                mdbl_parent_Mz = parent_Mz;
                clsPeak tempPeak;
                mobj_parent_peak_processor.PeakData.FindPeak(parent_Mz - 0.1, parent_Mz + 0.1, out tempPeak);
                if (tempPeak.Intensity > 0)
                    mdbl_parent_Intensity = tempPeak.Intensity;
                else
                    mdbl_parent_Intensity = mdbl_min_peptide_intensity;
                parentPeak.Mz = parent_Mz;
                parentPeak.Intensity = mdbl_parent_Intensity;
            }
            else
            {
                mdbl_parent_Mz = parent_Mz_match;
                mdbl_parent_Intensity = mobj_parentPeak.Intensity;
                parentPeak.Mz = parent_Mz_match;
                parentPeak.Intensity = mobj_parentPeak.Intensity;
            }

            mint_NumMSnScansProcessed++;

            // if -C option
            /*if (mint_consider_charge > 0)
            {
                mobj_transformRecord.Mz = mdbl_parent_Mz;
                mobj_transformRecord.ChargeState = (short) mint_consider_charge;
                mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                    mobj_transformRecord.ChargeState;
                mobj_transformRecord.Fit = 1;
                mobj_transformRecord.FitCountBasis = 0;
                mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                mvect_transformRecords.Add(mobj_transformRecord);
                return true;
            }*/

            //bypassed all test, proceed
            //check if +1
            chargeOne = mobj_svm_charge_determination.IdentifyIfChargeOne(mvect_mzs_msN, mvect_intensities_msN,
                parentPeak, parent_scan_number);
            if (chargeOne)
            {
                mobj_transformRecord.Mz = mdbl_parent_Mz;
                mobj_transformRecord.ChargeState = 1;
                mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                    mobj_transformRecord.ChargeState;
                mobj_transformRecord.Fit = 1;
                mobj_transformRecord.FitCountBasis = 0;
                mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                mvect_transformRecords.Add(mobj_transformRecord);
                return true;
            }
            else
            {
                //get features and add to feature space
                mobj_svm_charge_determination.GetFeaturesForSpectra(mvect_mzs_msN, mvect_intensities_msN, parentPeak,
                    msN_scan_number);
                mmap_msN_parentIndex.Add(msN_scan_number, mint_msNscanIndex);
                mint_msNscanIndex++;

                if (IsZoomScan(parent_scan_number))
                {
                    mobj_transformRecord.mdbl_mz = mdbl_parent_Mz;
                    mobj_transformRecord.mshort_cs = 2;
                    mobj_transformRecord.mdbl_mono_mw = (mobj_transformRecord.mdbl_mz - mdbl_cc_mass) * mobj_transformRecord.mshort_cs;
                    mobj_transformRecord.mdbl_fit = 1;
                    mobj_transformRecord.mint_mono_intensity = (int)mdbl_parent_Intensity;
                    mvect_transformRecords.Add(mobj_transformRecord);

                    mobj_transformRecord.mdbl_mz = mdbl_parent_Mz;
                    mobj_transformRecord.mshort_cs = 3;
                    mobj_transformRecord.mdbl_mono_mw = (mobj_transformRecord.mdbl_mz - mdbl_cc_mass) * mobj_transformRecord.mshort_cs;
                    mobj_transformRecord.mdbl_fit = 1;
                    mobj_transformRecord.mint_mono_intensity = (int)mdbl_parent_Intensity;
                    mvect_transformRecords.Add(mobj_transformRecord);
                    //return true;
                }

                return false;
            }
        }

        public void DetermineChargeForEachScan()
        {
            mobj_svm_charge_determination.NormalizeDataSet();
            mobj_svm_charge_determination.DetermineClassForDataSet();
        }

        public void InitializeSVM(string file_name)
        {
            mobj_svm_charge_determination.SetSVMParamFile(file_name);
            mobj_svm_charge_determination.LoadSVMFromXml();
            mobj_svm_charge_determination.InitializeLDA();
        }

        public bool ContainsProfileData(int parent_scan)
        {
            return mobj_raw_data_dta.IsProfileScan(parent_scan);
        }

        public bool IsZoomScan(int parent_scan)
        {
            if (menm_dataset_type == FileType.FINNIGAN)
                return mobj_raw_data_dta.IsZoomScan(parent_scan);

            return false;
        }

        public int GetSpectraType(int msN_scan_number)
        {
            return mobj_raw_data_dta.GetSpectrumType(msN_scan_number);
        }

        public bool IsFTData(int parent_scan)
        {
            if (menm_dataset_type == FileType.FINNIGAN)
                return mobj_raw_data_dta.IsFTScan(parent_scan);
            else if (menm_dataset_type == FileType.MZXMLRAWDATA)
            {
                bool set_FT = false;
                set_FT = mobj_raw_data_dta.IsFTScan(parent_scan);
                if (set_FT)
                    return set_FT;
                else
                {
                    // return user preference
                    return mbln_is_profile_data_for_mzXML;
                }
            }

            return false;
        }

        public void GetMsNSpectra(int msN_scan_number, double peakBkgRatio, double peptideMinBkgRatio)
        {
            mvect_intensities_msN.Clear();
            mvect_mzs_msN.Clear();

            mobj_raw_data_dta.GetRawData(out mvect_mzs_msN, out mvect_intensities_msN, msN_scan_number, mbln_centroid_msn);

            double thres = DeconEngine.Utils.GetAverage(mvect_intensities_msN, float.MaxValue);
            double background_intensity = DeconEngine.Utils.GetAverage(mvect_intensities_msN, (float) (5 * thres));
            mobj_msN_peak_processor.SetPeakIntensityThreshold(background_intensity * peakBkgRatio);
            mobj_msN_peak_processor.SetPeaksProfileType(!mbln_centroid_msn && mobj_raw_data_dta.IsProfileScan(msN_scan_number));

            int numPeaks = mobj_msN_peak_processor.DiscoverPeaks(mvect_mzs_msN, mvect_intensities_msN);
            mobj_msN_peak_processor.PeakData.InitializeUnprocessedPeakData();
        }

        public void GetParentScanSpectra(int parent_scan_number, double peakBkgRatio, double peptideMinBkgRatio)
        {
            mvect_intensities_parent.Clear();
            mvect_mzs_parent.Clear();

            mobj_raw_data_dta.GetRawData(out mvect_mzs_parent, out mvect_intensities_parent, parent_scan_number, false);

            double thres = DeconEngine.Utils.GetAverage(mvect_intensities_parent, float.MaxValue);
            double background_intensity = DeconEngine.Utils.GetAverage(mvect_intensities_parent, (float) (5 * thres));
            mobj_parent_peak_processor.SetPeakIntensityThreshold(background_intensity * peakBkgRatio);
            mobj_parent_peak_processor.SetPeaksProfileType(mobj_raw_data_dta.IsProfileScan(parent_scan_number));

            int numPeaks = mobj_parent_peak_processor.DiscoverPeaks(mvect_mzs_parent, mvect_intensities_parent);
            mdbl_min_peptide_intensity = background_intensity * peptideMinBkgRatio;
            mobj_parent_peak_processor.PeakData.InitializeUnprocessedPeakData();
        }

        public void WriteToMGF(int msN_scan_num, int parent_scan_num)
        {
            //second line
            double massplusH = 0;
            int numTransforms = mvect_transformRecords.Count;

            //check size, else has failed params
            if (numTransforms == 0)
                return;

            try
            {
                mobj_transformRecord = mvect_transformRecords[0];

                // now there can only be two Entries - one through findPeak @[0] and other through THRASH @[1]
                for (int transformNum = 0; transformNum < numTransforms; transformNum++)
                {
                    mobj_transformRecord = mvect_transformRecords[transformNum];
                    if (numTransforms > 1 && !mbln_consider_multiple_precursors)
                    {
                        // if different charges
                        if (mvect_transformRecords[0].ChargeState == mvect_transformRecords[1].ChargeState)
                        {
                            //changed so that THRASH is preferred by default 6/12/07
                            mobj_transformRecord = mvect_transformRecords[1];
                            numTransforms--;
                        }
                    }
                }

                    //stick to range
                    if (mobj_transformRecord.MonoMw < mdbl_minMass ||
                        mobj_transformRecord.MonoMw > mdbl_maxMass)
                    {
                        return;
                        //continue;
                    }

                    mint_NumDTARecords++;

                    System.Console.WriteLine(msN_scan_num + "." + msN_scan_num + "." + mobj_transformRecord.ChargeState);

                    int msN_scan_level = mobj_raw_data_dta.GetMSLevel(msN_scan_num);
                    int parent_scan_level = mobj_raw_data_dta.GetMSLevel(parent_scan_num);
                    CreateMSnRecord(msN_scan_num, msN_scan_level, parent_scan_num, parent_scan_level);
                    CreateProfileRecord(msN_scan_num, parent_scan_num);

                    if (mbln_create_log_file_only)
                        return;

                    //first line
                    mfile_mgf.WriteLine("BEGIN IONS");

                    //second line
                    mfile_mgf.WriteLine("TITLE={0}.{1}.{2}.{3}.dta", mch_dataset_name, msN_scan_num, msN_scan_num,
                        mobj_transformRecord.ChargeState);

                    //third line
                    double monoHmz = (mobj_transformRecord.MonoMw + (mdbl_cc_mass * mobj_transformRecord.ChargeState)) / mobj_transformRecord.ChargeState;
                    mfile_mgf.WriteLine("PEPMASS={0:F12}", monoHmz);

                    //fourth line
                    if (numTransforms > 1)
                        mfile_mgf.WriteLine("CHARGE={0}+ and {1}+", mvect_transformRecords[0].ChargeState,
                            mvect_transformRecords[1].ChargeState);
                    else
                        mfile_mgf.WriteLine("CHARGE={0}+", mobj_transformRecord.ChargeState);

                    //start spectra
                    for (int i = 0; i < (int) mvect_mzs_msN.Count; i++)
                    {
                        double mz = mvect_mzs_msN[i];
                        double intensity = mvect_intensities_msN[i];
                        mfile_mgf.WriteLine("{0:F5} {1:F2}", mz, intensity);
                    }
                    mfile_mgf.WriteLine("END IONS");
                    mfile_mgf.WriteLine();
                //}
            }
            catch (System.Exception e)
            {
#if DEBUG
                throw e;
#endif
                System.Console.Error.WriteLine("Error in creating .MGF");
            }
        }

        public void CreateProfileRecord(int msn_scan_num, int parent_scan_num)
        {
            double agc_acc_time = mobj_raw_data_dta.GetAGCAccumulationTime(parent_scan_num);
            double tic_val = mobj_raw_data_dta.GetTICForScan(parent_scan_num);

            mobj_profile_record = new ProfileRecord();
            mobj_profile_record.mint_msn_scan_num = msn_scan_num;
            mobj_profile_record.mint_parent_scan_num = parent_scan_num;
            mobj_profile_record.mdbl_agc_time = agc_acc_time;
            mobj_profile_record.mdbl_tic_val = tic_val;

            mvect_profile_records.Add(mobj_profile_record);
        }

        public void CreateMSnRecord(int msn_scan_num, int msn_scan_level, int parent_scan, int parent_scan_level)
        {
            mobj_msn_record = new MSnInformationRecord();
            mobj_msn_record.mint_msn_scan_num = msn_scan_num;
            mobj_msn_record.mint_msn_scan_level = msn_scan_level;

            mobj_msn_record.mint_parent_scan_num = parent_scan;
            mobj_msn_record.mint_parent_scan_level = parent_scan_level;

            mobj_msn_record.mdbl_parent_mz = mdbl_parent_Mz;
            mobj_msn_record.mdbl_mono_mz = mobj_transformRecord.Mz;
            mobj_msn_record.mshort_cs = (short)mobj_transformRecord.ChargeState;
            mobj_msn_record.mdbl_mono_mw = mobj_transformRecord.MonoMw;
            mobj_msn_record.mdbl_fit = mobj_transformRecord.Fit;

            mobj_msn_record.mint_parent_intensity = (int)mdbl_parent_Intensity;
            if (mobj_transformRecord.mint_mono_intensity <= 0)
                mobj_msn_record.mint_mono_intensity = (int)mdbl_parent_Intensity;
            else
                mobj_msn_record.mint_mono_intensity = mobj_transformRecord.MonoIntensity;

            mvect_msn_records.Add(mobj_msn_record);
        }

        public void WriteProfileFile()
        {
            using (
                StreamWriter fout =
                    new StreamWriter(new FileStream(mch_profile_filename, FileMode.Create, FileAccess.ReadWrite,
                        FileShare.None)))
            {
                fout.WriteLine("{0}\t{1}\t{2}\t{3}", "MSn_Scan", "Parent_Scan", "AGC_accumulation_time", "TIC");

                // sort all records wrt scan
                mvect_profile_records.Sort((x, y) => x.mint_msn_scan_num.CompareTo(y.mint_msn_scan_num));

                // now sorted output all
                for (int i = 0; i < mvect_profile_records.Count; i++)
                {
                    mobj_profile_record = mvect_profile_records[i];
                    fout.WriteLine("{0}\t{1}\t{2:F4}\t{3:F4}", mobj_profile_record.mint_msn_scan_num,
                        mobj_profile_record.mint_parent_scan_num, mobj_profile_record.mdbl_agc_time,
                        mobj_profile_record.mdbl_tic_val);
                }
            }
        }

        public void WriteLogFile()
        {
            using (
                StreamWriter fout =
                    new StreamWriter(new FileStream(mch_log_filename, FileMode.Create, FileAccess.ReadWrite,
                        FileShare.None)))
            {
                //TODO: Version number is hardcoded and needs to be read off assembly file
                fout.WriteLine("DeconMSn Version:" + "2.3.1.3");
                fout.WriteLine("Dataset:" + mch_dataset_name);
                fout.WriteLine("Number of MSn scans processed:" + mint_NumMSnScansProcessed);
                fout.WriteLine("Number of DTAs generated:" + mint_NumDTARecords);
                fout.WriteLine("Date/Time:\t" + DateTime.Now.ToString("g", CultureInfo.InvariantCulture));
                fout.WriteLine("-----------------------------------------------------------\n\n");

                fout.WriteLine("{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}\t{9}\t{10}", "MSn_Scan", "MSn_Level",
                    "Parent_Scan", "Parent_Scan_Level", "Parent_Mz", "Mono_Mz", "Charge_State", "Monoisotopic_Mass",
                    "Isotopic_Fit", "Parent_Intensity", "Mono_Intensity");

                // sort all records wrt scan
                mvect_msn_records.Sort((x, y) =>
                {
                    int result = x.mint_msn_scan_num.CompareTo(y.mint_msn_scan_num);
                    if (result == 0)
                    {
                        return x.mshort_cs.CompareTo(y.mshort_cs);
                    }
                    return result;
                });

                // now sorted output all
                for (int i = 0; i < mvect_msn_records.Count; i++)
                {
                    mobj_msn_record = mvect_msn_records[i];
                    fout.WriteLine("{0}\t{1}\t{2}\t{3}\t{4:F4}\t{5:F4}\t{6}\t{7:F4}\t{8:F4}\t{9}\t{10}",
                        mobj_msn_record.mint_msn_scan_num, mobj_msn_record.mint_msn_scan_level,
                        mobj_msn_record.mint_parent_scan_num, mobj_msn_record.mint_parent_scan_level,
                        mobj_msn_record.mdbl_parent_mz, mobj_msn_record.mdbl_mono_mz, mobj_msn_record.mshort_cs,
                        mobj_msn_record.mdbl_mono_mw, mobj_msn_record.mdbl_fit, mobj_msn_record.mint_parent_intensity,
                        mobj_msn_record.mint_mono_intensity);
                }
            }
        }

        // percentComplete is a value between 0 and 100
        public void WriteProgressFile(int scansProcessed, int totalScans, int percentComplete)
        {
            // get date and time
            var timeStamp = DateTime.Now.ToString("g", CultureInfo.InvariantCulture);
            try
            {
                using (var stream = new StreamWriter(new FileStream(mch_progress_filename, FileMode.Create, FileAccess.Write, FileShare.Read)))
                {
                    stream.WriteLine("Percent complete:              {0}%", percentComplete);
                    stream.WriteLine("Date/Time:                     {0}", timeStamp);
                    stream.WriteLine("Number of MSn scans processed: {0}", scansProcessed);
                    stream.WriteLine("Total scans:                   {0}", totalScans);
                    stream.WriteLine("Dataset: {0}", mch_dataset_name);
                }
            }
            catch (System.Exception e)
            {
                System.Console.Error.WriteLine("Exception writing progress to {0}: {1}", mch_progress_filename, e.Message);
            }
        }

        public void WriteDTAFile(int msN_scan_num, int parent_scan_num)
        {
            //create file_name
            double massplusH = 0;
            int numTransforms = mvect_transformRecords.Count;

            if (mbln_consider_multiple_precursors)
                throw new System.Exception(
                    "Can only consider multiple precursors for MGF creation. Change param value to false. ");

            //check size, else has failed params
            if (numTransforms == 0)
                return;

            // now there can only be two Entries - one through findPeak @[0] and other through THRASH @[1]
            for (int transformNum = 0; transformNum < numTransforms; transformNum++)
            {
                mobj_transformRecord = mvect_transformRecords[transformNum];
                if (numTransforms > 1)
                {
                    // if same charges
                    if (mvect_transformRecords[0].ChargeState == mvect_transformRecords[1].ChargeState)
                    {
                        //changed so that THRASH is preferred by default 6/12/07
                        mobj_transformRecord = mvect_transformRecords[1];
                        numTransforms--;
                    }
                }

                if (mint_consider_charge != 0 && mobj_transformRecord.mshort_cs != mint_consider_charge)
                {
                    return;
                }

                //stick to range
                if (mobj_transformRecord.MonoMw < mdbl_minMass || mobj_transformRecord.MonoMw > mdbl_maxMass)
                {
                    //return;
                    continue;
                }

                mint_NumDTARecords++;

                // to get mono_mass  + H
                massplusH = mobj_transformRecord.MonoMw + mdbl_cc_mass;

                System.Console.WriteLine(msN_scan_num + "." + msN_scan_num + "." + mobj_transformRecord.ChargeState);

                int msN_scan_level = mobj_raw_data_dta.GetMSLevel(msN_scan_num);
                int parent_scan_level = mobj_raw_data_dta.GetMSLevel(parent_scan_num);
                CreateMSnRecord(msN_scan_num, msN_scan_level, parent_scan_num, parent_scan_level);
                CreateProfileRecord(msN_scan_num, parent_scan_num);

                if (mbln_create_log_file_only)
                    continue;

                string fileName = string.Format("{0}.{1}.{2}.{3}.dta", mch_output_file, msN_scan_num, msN_scan_num,
                    mobj_transformRecord.ChargeState);
                /*// Purely for TomMetz's data
                string metz_mod = "";
                if (mobj_raw_data_dta.IsProfileScan(msN_scan_num))
                    metz_mod = "_FTMS";
                else
                    metz_mod = "_ITMS";
                string fileName = string.Format("{0}.{1}.{2}.{3}.dta", mch_output_file + metz_mod, msN_scan_num, msN_scan_num, mobj_transformRecord.mshort_cs);
                 */

                // for composite dta
                if (mbln_create_composite_dta)
                {
                    if (!mbln_first_scan_written)
                    {
                        mfile_comb_dta.WriteLine();
                        mbln_first_scan_written = true;
                    }

                    //fancy headers
                    mfile_comb_dta.WriteLine(
                        "=================================== \"{0}.{1:D4}.{2:D4}.{3}.dta\" ==================================",
                        mch_dataset_name, msN_scan_num, msN_scan_num, mobj_transformRecord.ChargeState);

                    // massH and cs
                    mfile_comb_dta.WriteLine("{0:F7} {1}   scan={2} cs={3}", massplusH, mobj_transformRecord.ChargeState,
                        msN_scan_num, mobj_transformRecord.ChargeState);

                    for (int i = 0; i < (int) mvect_mzs_msN.Count; i++)
                    {
                        double mz = mvect_mzs_msN[i];
                        double intensity = mvect_intensities_msN[i];

                        if (intensity == 0)
                        {
                            if (i > 0 && mvect_intensities_msN[i - 1].Equals(0))
                            {
                                if (i == mvect_mzs_msN.Count - 1)
                                    continue;
                                else if (mvect_intensities_msN[i + 1].Equals(0))
                                    continue;
                            }
                        }

                        try
                        {
                            mfile_comb_dta.WriteLine("{0:F5} {1:F2}", mz, intensity);
                        }
                        catch (System.Exception e)
                        {
#if DEBUG
                            throw e;
#endif
                            throw new System.Exception("Trouble with writing out Peaks in CDTA");
                        }
                    }
                    mfile_comb_dta.WriteLine();

                    continue;
                }

                using (
                    StreamWriter fout =
                        new StreamWriter(new FileStream(fileName, FileMode.Create, FileAccess.ReadWrite, FileShare.None))
                    )
                {
                    fout.WriteLine("{0:F12} {1}", massplusH, mobj_transformRecord.ChargeState);

                    for (int i = 0; i < (int) mvect_mzs_msN.Count; i++)
                    {
                        double mz = mvect_mzs_msN[i];
                        double intensity = mvect_intensities_msN[i];
                        fout.WriteLine("{0:F5} {1:F2}", mz, intensity);
                    }
                }
            }
        }

        public void WriteLowResolutionMGFFile()
        {
            int msN_scan;
            int parent_scan;
            int msN_scan_index;
            int class_val;
            double parent_Mz_match = 0;
            double parent_mz;

            int numCharges = 0;
            int size = mmap_msN_parentIndex.Count;

            foreach (KeyValuePair<int, int> item in mmap_msN_parentIndex)
            {
                msN_scan = item.Key;
                msN_scan_index = item.Value;
                parent_scan = mobj_raw_data_dta.GetParentScan(msN_scan);

                GetMsNSpectra(msN_scan, mdbl_pkBkgRatio, mdbl_peptideMinBkgRatio);
                parent_mz = mobj_raw_data_dta.GetParentMz(msN_scan);
                GetParentScanSpectra(parent_scan, mdbl_pkBkgRatio, mdbl_peptideMinBkgRatio);
                clsPeak parentPeak;
                parent_Mz_match = mobj_parent_peak_processor.GetClosestPeakMz(parent_mz, out parentPeak);

                class_val = mobj_svm_charge_determination.GetClassAtScanIndex(msN_scan_index);

                mvect_chargeStateList.Clear();
                mvect_transformRecords.Clear();
                switch (class_val)
                {
                    case 0:
                        mvect_chargeStateList.Add(2);
                        mvect_chargeStateList.Add(3);
                        break;
                    case 1:
                        mvect_chargeStateList.Add(1);
                        break;
                    case 2:
                        mvect_chargeStateList.Add(2);
                        break;
                    case 3:
                        mvect_chargeStateList.Add(3);
                        break;
                    case 4:
                        mvect_chargeStateList.Add(4);
                        break;
                    default:
                        break;
                }

                numCharges = (int) mvect_chargeStateList.Count;

                //deconvolute with all charges states in chargeList
                for (int chargeNum = 0; chargeNum < numCharges; chargeNum++)
                {
                    int cs = mvect_chargeStateList[chargeNum];
                    mobj_transformRecord.Mz = parent_Mz_match;
                    mobj_transformRecord.ChargeState = (short) cs;
                    mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                        mobj_transformRecord.ChargeState;
                    mobj_transformRecord.Fit = 1;
                    mobj_transformRecord.FitCountBasis = 1;
                    mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                    mvect_transformRecords.Add(new clsHornTransformResults(mobj_transformRecord));
                }

                WriteToMGF(msN_scan, parent_scan);
            }

            mobj_svm_charge_determination.ClearMemory();
        }

        public void WriteLowResolutionDTAFile()
        {
            int msN_scan;
            int parent_scan;
            int msN_scan_index;
            int class_val;
            double parent_mz;
            double parent_Mz_match = 0;

            int numCharges = 0;
            int size = mmap_msN_parentIndex.Count;
            foreach (KeyValuePair<int, int> item in mmap_msN_parentIndex)
            {
                msN_scan = item.Key;
                msN_scan_index = item.Value;

                GetMsNSpectra(msN_scan, mdbl_pkBkgRatio, mdbl_peptideMinBkgRatio);

                parent_scan = mobj_raw_data_dta.GetParentScan(msN_scan);
                GetParentScanSpectra(parent_scan, mdbl_pkBkgRatio, mdbl_peptideMinBkgRatio);

                parent_mz = mobj_raw_data_dta.GetParentMz(msN_scan);
                parent_Mz_match = mobj_parent_peak_processor.GetClosestPeakMz(parent_mz, out mobj_parentPeak);
                if (mobj_parentPeak.Intensity < mdbl_min_peptide_intensity)
                {
                    mdbl_parent_Mz = parent_mz;
                    clsPeak tempPeak;
                    mobj_parent_peak_processor.PeakData.FindPeak(parent_mz - 0.1, parent_mz + 0.1, out tempPeak);
                    if (tempPeak.Intensity > 0)
                        mdbl_parent_Intensity = tempPeak.Intensity;
                    else
                        mdbl_parent_Intensity = mdbl_min_peptide_intensity;
                }
                else
                {
                    mdbl_parent_Mz = parent_Mz_match;
                    mdbl_parent_Intensity = mobj_parentPeak.Intensity;
                }

                class_val = mobj_svm_charge_determination.GetClassAtScanIndex(msN_scan_index);
                double score = mobj_svm_charge_determination.GetScoreAtScanIndex(msN_scan_index);

                mvect_chargeStateList.Clear();
                mvect_transformRecords.Clear();
                switch (class_val)
                {
                    case 0:
                        mvect_chargeStateList.Add(2);
                        mvect_chargeStateList.Add(3);
                        break;
                    case 1:
                        mvect_chargeStateList.Add(1);
                        break;
                    case 2:
                        mvect_chargeStateList.Add(2);
                        break;
                    case 3:
                        mvect_chargeStateList.Add(3);
                        break;
                    case 4:
                        mvect_chargeStateList.Add(4);
                        break;
                    default:
                        break;
                }

                numCharges = (int) mvect_chargeStateList.Count;

                //deconvolute with all charges states in chargeList
                for (int chargeNum = 0; chargeNum < numCharges; chargeNum++)
                {
                    int cs = mvect_chargeStateList[chargeNum];
                    mobj_transformRecord.Mz = mdbl_parent_Mz;

                    if (mdbl_parent_Mz == 0)
                    {
                        bool debug = true;
                        debug = false;
                    }

                    mobj_transformRecord.ChargeState = (short) cs;
                    mobj_transformRecord.MonoMw = (mobj_transformRecord.Mz - mdbl_cc_mass) *
                                                        mobj_transformRecord.ChargeState;
                    mobj_transformRecord.Fit = 1;
                    mobj_transformRecord.FitCountBasis = 1;
                    mobj_transformRecord.MonoIntensity = (int) mdbl_parent_Intensity;
                    mvect_transformRecords.Add(new clsHornTransformResults(mobj_transformRecord));
                }
                WriteDTAFile(msN_scan, parent_scan);
            }

            //clear up
            mobj_svm_charge_determination.ClearMemory();
        }
    }
}
#endif