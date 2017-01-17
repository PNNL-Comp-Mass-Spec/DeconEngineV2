#if Enable_Obsolete
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using DeconToolsV2.HornTransform;
using DeconToolsV2.Peaks;
using Engine.HornTransform;
using Engine.PeakProcessing;

namespace Engine.Results
{
    /// <summary>
    ///     This class stores information about the results from deisotoping an LC-MS dataset.
    /// </summary>
    /// <remarks>
    ///     Peaks found in the scans of the dataset are stored in a vector and sorted by scan, mz.
    ///     Peaks can be fetched from the entire dataset or for a given range of scans and mz.
    ///     Deisotoped results are also stored and can be accessed for particular scans and mz ranges.
    /// </remarks>
    [Obsolete("Only used by Decon2LS.UI (maybe), and by the PeakImporter (from peak.dat files)", false)]
    internal class LCMSTransformResults
    {
        // Stores all the peaks found in the lc ms experiment in memory blocks of size PEAK_BLOCK_SIZE. This allows large amount of data to be kept in memory without necessarily requiring them to be in contiguous memory spaces, which can often result in memory allocation being unsuccessful. In addition the peaks are written out to temporary files each time AddPeaksForScan function is called. If memory allocation is unsuccessful, all the peaks stored thus far are cleared. And flag is set indicating not all data is stored in memory.
        private readonly List<LcmsPeak> _peaksList = new List<LcmsPeak>();

        /// <summary>
        ///     Keeps the index of the first peak for a scan in mdeque_peaks
        /// </summary>
        /// <remarks>
        ///     The BlockDeque of peaks is to be kept sorted in scans followed by mzs. This map
        ///     keeps track of the first peak seen for a scan. thus mmap_scan_peak_indices[123]
        ///     returns the index of the lowest mz peak for scan 123 in mdeque_peaks.
        ///     All the peaks in this BlockDeque from index mmap_scan_peak_indices[123] to index
        ///     mmap_scan_peak_indices[124]-1 are peaks for scan 123.
        /// </remarks>
        private readonly SortedDictionary<int, int> _scanPeakIndicesDict = new SortedDictionary<int, int>();

        private bool _dataInMemoryIsIncomplete;
        private bool _deisotoped;
        private string _fileName;

        //private int _isoBlockSize;

        private Stream _isoFile;
        private int _lastScanAdded;
        private int _maxScan;
        private int _minScan;
        private int _numIsoStored;
        // Tracks how many peaks were seen thus far. Remember to use this variable
        // for count of number of peaks, because _saveStructsInMemory might be
        // false.
        private int _numPeaksStored;

        //private int _peakBlockSize;
        private Stream _peakFile;

        private bool _saveStructsInMemory;
        //private bool _saveUIMF;
        //private bool _sorted;
        private string _tempIsoFilePath;
        private string _tempPeakFilePath;
        // Stores all the deisotoped patterns found in the lc ms experiment in memory blocks
        // of size ISO_BLOCK_SIZE (see reasoning in explanation for mdeque_peaks). In addition
        // this data is written to temporary file each time AddTransformsForScan is called.
        // If memory allocation is unsuccessful when new isotope patterns are added, all peaks stored
        // are cleared and flag is set.
        //private Engine.Utilities.BlockDeque<Engine.HornTransform.IsotopeFitRecord > mdeque_transforms;
        private readonly List<clsHornTransformResults> _transforms = new List<clsHornTransformResults>();

        public SortedDictionary<int, ScanData> ScanDataDict;

        public LCMSTransformResults()
        {
            _isoFile = null;
            _peakFile = null;
            //_saveUIMF = false;
            Reset();
            _saveStructsInMemory = false;
            _numPeaksStored = 0;
        }

        private void WriteTempPeaksToFile(BinaryWriter fstream)
        {
            _peakFile.Flush();
            _peakFile.Seek(0, SeekOrigin.Begin);
            if (!_peakFile.CanRead || _peakFile.Position != 0)
            {
                _peakFile.Close();
                _peakFile = new FileStream(_tempPeakFilePath, FileMode.Open, FileAccess.ReadWrite, FileShare.None);
            }
            const int blockSize = 1024;
            const int sizeOfLcPeak = 4 + 8 + 8;
            const int bufferSize = blockSize * sizeOfLcPeak;
            var buffer = new byte[bufferSize];
            while (_peakFile.Position < _peakFile.Length)
            {
                // Doing a direct byte-by-byte copy to a new file...
                var read = _peakFile.Read(buffer, 0, bufferSize);
                if (read > 0)
                {
                    fstream.Write(buffer, 0, read);
                }
                else
                {
                    _peakFile.Close();
                }
            }
        }

        private void WriteTempIsosToFile(BinaryWriter fstream)
        {
            if (_saveStructsInMemory && !_dataInMemoryIsIncomplete)
            {
                // write from stored data in mdeque_isotopes
                //Engine.Utilities.BlockDeque<Engine.HornTransform.IsotopeFitRecord >.iterator iter;
                //Engine.Utilities.BlockDeque<Engine.HornTransform.IsotopeFitRecord >.iterator iter_first = mdeque_transforms.begin();
                //Engine.Utilities.BlockDeque<Engine.HornTransform.IsotopeFitRecord >.iterator iter_last = mdeque_transforms.end();

                /*
                    Needs work
                    for (iter = iter_first; iter != iter_last; iter++)
                    {
                        Engine.HornTransform.IsotopeFitRecord record = *iter;
                        fout.precision(2);
                        ptr_fstream.write(record.mdbl_sn,<<",";
                        fout.precision(4);
                        fout<<record.mint_mono_intensity<<",";
                        fout<<record.mint_iplus2_intensity<<"\n";
                    }*/
            }
            else
            {
                _isoFile.Flush();
                _isoFile.Seek(0, SeekOrigin.Begin);
                if (!_isoFile.CanRead || _isoFile.Position != 0)
                {
                    _isoFile.Close();
                    _isoFile = new FileStream(_tempIsoFilePath, FileMode.Open, FileAccess.ReadWrite,
                        FileShare.None);
                }

                const int blockSize = 1024;
                const int sizeOfIso = 92 + 2 + 72;
                const int bufferSize = blockSize * sizeOfIso;
                var buffer = new byte[bufferSize];
                while (_isoFile.Position < _isoFile.Length)
                {
                    // Doing a direct byte-by-byte copy to a new file...
                    var read = _isoFile.Read(buffer, 0, bufferSize);
                    if (read > 0)
                    {
                        fstream.Write(buffer, 0, read);
                    }
                    else
                    {
                        _isoFile.Close();
                    }
                }
            }
        }

        public void SaveScanResults(string scanFile, bool saveSignalRange)
        {
            SaveResultsV1Scan(scanFile, saveSignalRange);
        }

        public string GetFileName()
        {
            return _fileName;
        }

        public int GetNumPeaks()
        {
            return _numPeaksStored;
        }

        public int GetNumTransforms()
        {
            return _numIsoStored;
        }

        public void GetAllPeaks(out List<LcmsPeak> peaks)
        {
            //mdeque_peaks.Get(out vectPeaks, 0, mint_num_peaks_stored);
            peaks = new List<LcmsPeak>(_peaksList.Select(x => new LcmsPeak(x)));
        }

        public LcmsPeak GetPeak(int peakNum)
        {
            //return mdeque_peaks.GetPoint(pk_num);
            return new LcmsPeak(_peaksList[peakNum]);
        }

        public IsotopeFitRecord GetIsoPattern(int isoNum)
        {
            //return mdeque_transforms.GetPoint(iso_num);
            return new IsotopeFitRecord(_transforms[isoNum]);
        }

        ~LCMSTransformResults()
        {
            if (_isoFile != null)
            {
                _isoFile.Close();
                _peakFile.Close();
                _isoFile = null;
                _peakFile = null;
                if (File.Exists(_tempIsoFilePath))
                {
                    File.Delete(_tempIsoFilePath);
                }
                if (File.Exists(_tempPeakFilePath))
                {
                    File.Delete(_tempPeakFilePath);
                }
            }
            ClearPeaksAndIso();
        }

        private void ClearPeaksAndIso()
        {
            _peaksList.Clear();
            _transforms.Clear();
            // mint_num_peaks_stored and mint_num_isos_stored are not set to 0, unless reset is called,
            // because these files might yet be in the temp files.
        }

        private void Reset()
        {
            //_sorted = true;
            _lastScanAdded = -1;
            _deisotoped = false;
            _minScan = int.MaxValue;
            _maxScan = -1;

            _tempIsoFilePath = "tmp.iso.";

            if (_isoFile != null)
            {
                _isoFile.Close();
                _peakFile.Close();
            }
            var time_t = (int) DateTime.Now.Ticks;
            _tempIsoFilePath += time_t;

            _tempPeakFilePath = "tmp.peak.";
            _tempPeakFilePath += time_t;

            _isoFile = new FileStream(_tempIsoFilePath, FileMode.Create, FileAccess.ReadWrite, FileShare.None);
            _peakFile = new FileStream(_tempPeakFilePath, FileMode.Create, FileAccess.ReadWrite,
                FileShare.None);

            _numPeaksStored = 0;
            _numIsoStored = 0;
            ClearPeaksAndIso();
#if Enable_Obsolete
            _dataInMemoryIsIncomplete = false;
#endif
        }

        public void AddInfoForIMSScan(int scan, double bpMz, double bpIntensity, double tic, double signalRange,
            int numPeaks, int numDeisotoped, double time, short msLevel, double driftTime)
        {
            AddInfoForUIMFScan(scan, bpMz, bpIntensity, tic, signalRange, numPeaks, numDeisotoped, time, msLevel, driftTime);
        }

        public void AddInfoForUIMFScan(int scan, double bpMz, double bpIntensity, double tic, double signalRange,
            int numPeaks, int numDeisotoped, double time, short msLevel, double driftTime)
        {
            //_saveUIMF = true;
            var data = new ScanData
            {
                ScanNum = scan,
                ScanBpMz = bpMz,
                ScanBpIntensity = bpIntensity,
                ScanTic = tic,
                ScanNumPeaks = numPeaks,
                ScanNumDeisotoped = numDeisotoped,
                ScanNumMsLevel = msLevel,
                ScanTime = time,
                ScanNumDriftTime = driftTime,
                ScanSignalRange = signalRange
            };
            ScanDataDict.Add(data.ScanNum, data);
        }

        public void AddInfoForScan(int scan, double bpMz, double bpIntensity, double tic, double signalRange,
            int numPeaks, int numDeisotoped, double time, short msLevel)
        {
            var data = new ScanData
            {
                ScanNum = scan,
                ScanBpMz = bpMz,
                ScanBpIntensity = bpIntensity,
                ScanTic = tic,
                ScanNumPeaks = numPeaks,
                ScanNumDeisotoped = numDeisotoped,
                ScanNumMsLevel = msLevel,
                ScanTime = time,
                ScanSignalRange = signalRange
            };
            ScanDataDict.Add(data.ScanNum, data);
        }

        // Peaks for scan are written out to file AND stored in the memory blocks of size
        public void AddPeaksForScan(int scanNum, List<clsPeak> peaks)
        {
            try
            {
                if (scanNum > _maxScan)
                {
                    _maxScan = scanNum;
                }
                if (scanNum < _minScan)
                {
                    _minScan = scanNum;
                }

                var numPeaks = peaks.Count;

                // so we have reached the next scan where peaks were seen.
                // It can be the case that no peaks were seen for intermediate scan.
                for (var scan = _lastScanAdded + 1; scan <= scanNum; scan++)
                {
                    _scanPeakIndicesDict.Add(scan, _numPeaksStored);
                }
                _numPeaksStored += numPeaks;
                // Create LCMSPeaks.
                var lcPeaks = new List<LcmsPeak>();
                lcPeaks.Capacity = peaks.Count;
                lcPeaks.AddRange(peaks.Select(x => new LcmsPeak
                {
                    Mz = x.Mz,
                    Intensity = x.Intensity
                }));

                if (numPeaks != 0)
                {
                    // First thing we do is write the peaks to temporary peaks file.
                    _peakFile.Flush();
                    using (var bWriter = new BinaryWriter(_peakFile, Encoding.ASCII, true))
                    {
                        foreach (var peak in lcPeaks)
                        {
                            peak.WriteToBinaryStream(bWriter);
                        }
                    }
                }

                if (!_saveStructsInMemory)
                    return;
                // Now lets add the new data to our data structures.
                if (_peaksList.Count + lcPeaks.Count > _peaksList.Capacity &&
                    _peaksList.Capacity > int.MaxValue / 2)
                {
                    _peaksList.Capacity = int.MaxValue - 150;
                }
                _peaksList.AddRange(lcPeaks);
            }
            catch (OutOfMemoryException e)
            {
                _peaksList.Clear();
                _transforms.Clear();
                _dataInMemoryIsIncomplete = true;
#if DEBUG
                throw e;
#endif
            }
        }

        public void AddTransforms(List<clsHornTransformResults> fitResults)
        {
            try
            {
                if (fitResults.Count == 0)
                    return;

                _numIsoStored += fitResults.Count;
                if (fitResults.Count != 0)
                {
                    _isoFile.Flush();
                    using (var bWriter = new BinaryWriter(_isoFile, Encoding.ASCII, true))
                    {
                        foreach (var record in fitResults)
                        {
                            new IsotopeFitRecord(record).WriteToBinaryStream(bWriter);
                        }
                    }
                }
                if (!_saveStructsInMemory)
                    return;
                if (_transforms.Count + fitResults.Count > _transforms.Capacity &&
                    _transforms.Capacity > int.MaxValue / 2)
                {
                    _transforms.Capacity = int.MaxValue - 150;
                }
                _transforms.AddRange(fitResults);
            }
            catch (OutOfMemoryException e)
            {
                if (_saveStructsInMemory)
                {
                    _peaksList.Clear();
                    _transforms.Clear();
                }
                _dataInMemoryIsIncomplete = true;
#if DEBUG
                throw e;
#endif
            }
        }

        public int GetMinScan()
        {
            return _minScan;
        }

        public int GetMaxScan()
        {
            return _maxScan;
        }

        public bool IsDeisotoped()
        {
            return _deisotoped;
        }

        private void SaveResultsV1Iso(string isoFileName)
        {
            using (
                var fout =
                    new StreamWriter(new FileStream(isoFileName, FileMode.Create, FileAccess.ReadWrite, FileShare.None))
                )
            {
                //Preserve column headers for VIPER to load it in
                fout.WriteLine("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11}", "scan_num", "charge", "abundance",
                    "mz", "fit", "average_mw", "monoisotopic_mw", "mostabundant_mw", "fwhm", "signal_noise",
                    "mono_abundance", "mono_plus2_abundance");

                if (_saveStructsInMemory && !_dataInMemoryIsIncomplete)
                {
                    // write from stored data in _transforms
                    foreach (var record in _transforms)
                    {
                        fout.WriteLine("{0},{1},{2:F0},{3:F4},{4:F4},{5:F4},{6:F4},{7:F4},{8:F4},{9:F2},{10},{11}",
                            record.ScanNum, record.ChargeState, record.Abundance, record.Mz, record.Fit,
                            record.AverageMw, record.MonoMw, record.MostIntenseMw, record.FWHM, record.SignalToNoise,
                            record.MonoIntensity, record.MonoPlus2Intensity);
                    }
                }
                else
                {
                    // Read in from stored file, little at a time and write it out;
                    _isoFile.Seek(0, SeekOrigin.Begin);
                    _isoFile.Flush();

                    if (!_isoFile.CanRead || _isoFile.Position != 0)
                    {
                        _isoFile.Close();
                        _isoFile = new FileStream(_tempIsoFilePath, FileMode.Create, FileAccess.ReadWrite,
                            FileShare.None);
                    }
                    _isoFile.Flush();
                    using (var bReader = new BinaryReader(_isoFile, Encoding.ASCII, true))
                    {
                        while (_isoFile.Position < _isoFile.Length)
                        {
                            var record = IsotopeFitRecord.ReadFromBinaryStream(bReader);
                            fout.WriteLine("{0},{1},{2},{3:F4},{4:F4},{5:F4},{6:F4},{7:F4},{8:F4},{9:F2},{10},{11}",
                                record.ScanNum, record.ChargeState, record.AbundanceInt, record.Mz, record.Fit,
                                record.AverageMw, record.MonoMw, record.MostIntenseMw, record.FWHM, record.SignalToNoise,
                                record.MonoIntensity, record.MonoPlus2Intensity);
                        }
                    }
                    _isoFile.Close();
                }
            }
        }

        private void SaveResultsV1Scan(string scanFileName, bool saveSignalRange)
        {
            using (
                var fout =
                    new StreamWriter(new FileStream(scanFileName, FileMode.Create, FileAccess.ReadWrite, FileShare.None))
                )
            {
                //Preserve coloumn headers for VIPER to load it in
                var saveSignalHead = "";
                if (saveSignalRange)
                {
                    saveSignalHead = ",time_domain_signal";
                }

                //if (mbln_save_ims)
                //    fout << "scan_num,scan_time,drift_time,type,bpi,bpi_mz,tic,num_peaks,num_deisotoped\n";
                //else
                //    fout << "scan_num,scan_time,type,bpi,bpi_mz,tic,num_peaks,num_deisotoped";
                fout.WriteLine("{0},{1},{2},{3},{4},{5},{6},{7}{8}", "scan_num", "scan_time", "type", "bpi", "bpi_mz",
                    "tic", "num_peaks", "num_deisotoped", saveSignalHead);

                foreach (var item in ScanDataDict)
                {
                    //if (mbln_save_ims)
                    //    fout << scan_num << "," << mmap_scan_time[scan_num] << "," << mmap_scan_num_drift_time[scan_num] << "," << mmap_scan_num_mslevel[scan_num] << "," << mmap_scan_bp_intensity[scan_num] << "," << mmap_scan_bp_mz[scan_num] << ",";
                    //else
                    //    fout << scan_num << "," << mmap_scan_time[scan_num] << "," << mmap_scan_num_mslevel[scan_num] << "," << mmap_scan_bp_intensity[scan_num] << "," << mmap_scan_bp_mz[scan_num] << ",";
                    var data = item.Value;
                    if (saveSignalRange)
                    {
                        fout.WriteLine("{0},{1:F4},{2},{3:F4},{4:F4},{5:F4},{6},{7},{8:F4}", data.ScanNum, data.ScanTime,
                            data.ScanNumMsLevel, data.ScanBpIntensity, data.ScanBpMz, data.ScanTic, data.ScanNumPeaks,
                            data.ScanNumDeisotoped, data.ScanSignalRange);
                    }
                    else
                    {
                        fout.WriteLine("{0},{1:F4},{2},{3:F4},{4:F4},{5:F4},{6},{7}", data.ScanNum, data.ScanTime,
                            data.ScanNumMsLevel, data.ScanBpIntensity, data.ScanBpMz, data.ScanTic, data.ScanNumPeaks,
                            data.ScanNumDeisotoped);
                    }
                }
            }
        }

        private void SaveResultsV1Data(string dataFileName)
        {
            using (
                var fout =
                    new BinaryWriter(new FileStream(dataFileName, FileMode.Create, FileAccess.ReadWrite, FileShare.None))
                )
            {
                //fout.Write(filename_len); // Write(string) auto-prefixes the length
                fout.Write(dataFileName);
                fout.Write("Version: 1.0");

                // now to write the peaks. Write out the number of peaks so that reading program knows how many to read
                fout.Write(_numPeaksStored);
                WriteTempPeaksToFile(fout);

                // write out number of isotopic signatures detected.
                fout.Write(_numIsoStored);
                fout.Flush();
                WriteTempIsosToFile(fout);
            }

            _isoFile.Close();
            _isoFile = null;
            _peakFile.Close();
            _peakFile = null;

            if (File.Exists(_tempIsoFilePath))
            {
                File.Delete(_tempIsoFilePath);
            }
            if (File.Exists(_tempPeakFilePath))
            {
                File.Delete(_tempPeakFilePath);
            }
        }

        //__declspec(align(4)) struct SavePeak
        //{
        //  int min_scan;
        //  double mdbl_mz;
        //  double mdbl_intensity;
        //};
        private void LoadResultsV1Data(string dataFileName)
        {
            _saveStructsInMemory = true;
            _dataInMemoryIsIncomplete = false;

            ClearPeaksAndIso();

            using (
                var fin = new BinaryReader(new FileStream(dataFileName, FileMode.Open, FileAccess.Read, FileShare.Read))
                )
            {
                _fileName = fin.ReadString();
                var versionStr = fin.ReadString();

                // how many peaks do we need to read ?
                _numPeaksStored = fin.ReadInt32();
                //var numRead = 0;

                var totalRead = 0;
                while (totalRead < _numPeaksStored)
                {
                    var peak = LcmsPeak.ReadFromBinaryStream(fin);
                    if (_peaksList.Count + 1 > _peaksList.Capacity && _peaksList.Capacity > int.MaxValue / 2)
                    {
                        _peaksList.Capacity = int.MaxValue - 150;
                    }
                    _peaksList.Add(peak);
                    totalRead++;
                }

                var readTransforms = true;
                if (readTransforms)
                {
                    _numIsoStored = fin.ReadInt32();

                    while (_transforms.Count < _numIsoStored)
                    {
                        var record = new clsHornTransformResults(IsotopeFitRecord.ReadFromBinaryStream(fin));
                        if (_transforms.Count + 1 > _transforms.Capacity &&
                            _transforms.Capacity > int.MaxValue / 2)
                        {
                            _transforms.Capacity = int.MaxValue - 150;
                        }
                        _transforms.Add(record);
                    }
                }

                if (_numPeaksStored != 0)
                {
                    _minScan = _peaksList[0].ScanNum;
                    _maxScan = _peaksList[_peaksList.Count - 1].ScanNum;
                    _lastScanAdded = _maxScan;
                }
                else
                {
                    _minScan = _maxScan = _lastScanAdded = -1;
                }
            }
            CreateIndexesOnData();
        }

        private void SaveResultsV1(string fileName, bool saveSignalRange)
        {
            //need to save three types of files:
            // 1. CSV File
            // 2. Scan File
            // 3. Raw Data file (.dat file).
            var isoFileName = fileName + "_isos.csv";
            SaveResultsV1Iso(isoFileName);

            var scanFileName = fileName + "_scans.csv";
            SaveResultsV1Scan(scanFileName, saveSignalRange);

            var dataFileName = fileName + "_peaks.dat";
            SaveResultsV1Data(dataFileName);
        }

        public void SaveResults(string fileName, bool saveSignalRange)
        {
            SaveResultsV1(fileName, saveSignalRange);
        }

        public void LoadResults(string fileName)
        {
            LoadResultsV1Data(fileName);
        }

        private void CreateIndexesOnData()
        {
            if (_peaksList.Count == 0)
                return;

            _scanPeakIndicesDict.Clear();
            var numPeaks = _peaksList.Count;
            var lastScan = -1;

            //_minScan = int.MaxValue;
            //_maxScan = int.MinValue;
            var peakNum = 0;
            foreach (var currentPeak in _peaksList)
            {
                //_minScan = Math.Min(_minScan, currentPeak.ScanNum);
                if (currentPeak.ScanNum != lastScan)
                {
                    //_maxScan = Math.Max(_maxScan, currentPeak.ScanNum);

                    // so we have reached the next scan where peaks were seen.
                    // It can be the case that no peaks were seen for intermediate peaks.
                    for (var scanNum = lastScan + 1; scanNum <= currentPeak.ScanNum; scanNum++)
                    {
                        _scanPeakIndicesDict.Add(scanNum, peakNum);
                    }
                    lastScan = currentPeak.ScanNum;
                }
                peakNum++;
            }
        }

        public void GetSIC(int minScan, int maxScan, float minMz, float maxMz, out List<double> intensities)
        {
            intensities = new List<double>();
            // go through each scan and find peaks.
            if ((minScan < _minScan && maxScan < _minScan) || (minScan > _maxScan && maxScan > _maxScan))
            {
                for (var scanNum = minScan; scanNum < maxScan; scanNum++)
                    intensities.Add(0);
                return;
            }

            for (var currentScan = minScan; currentScan <= maxScan; currentScan++)
            {
                if (currentScan < _minScan || currentScan > _maxScan)
                {
                    intensities.Add(0);
                }
                else
                {
                    var scanStartIndex = _scanPeakIndicesDict[currentScan];
                    var scanStopIndex = _peaksList.Count;
                    if (currentScan < _maxScan - 1)
                        scanStopIndex = _scanPeakIndicesDict[currentScan + 1];

                    if (scanStartIndex == scanStopIndex)
                    {
                        intensities.Add(0);
                    }
                    else
                    {
                        double maxIntensity = 0;
                        //iter_first = mdeque_peaks.get_iter(scan_start_index);
                        //iter_last = mdeque_peaks.get_iter(scan_stop_index);
                        //for (iter = iter_first; iter != iter_last; iter++)
                        //{
                        //    lc_peak = *iter;
                        //foreach (LCMSPeak lc_peak in mdeque_peaks.Where(x => x.mint_scan_num >= min_scan && x.mint_scan_num <= max_scan))
                        // TODO: Linq can clean this up a bit.
                        for (var i = scanStartIndex; i < scanStopIndex; i++)
                        {
                            var lcPeak = _peaksList[i];
                            var currentIntensity = lcPeak.Intensity;
                            var currentMz = lcPeak.Mz;
                            if (currentMz > maxMz)
                                break;

                            if (currentMz > minMz && currentIntensity > maxIntensity)
                            {
                                maxIntensity = currentIntensity;
                            }
                        }
                        intensities.Add(maxIntensity);
                    }
                }
            }
        }

        public void GetScanPeaks(int scanNum, out List<double> mzs, out List<double> intensities)
        {
            mzs = new List<double>();
            intensities = new List<double>();
            if (scanNum < _minScan || scanNum > _maxScan)
            {
                return;
            }
            var scanStartIndex = _scanPeakIndicesDict[scanNum];
            var scanStopIndex = _peaksList.Count;
            if (scanNum < _maxScan - 1)
                scanStopIndex = _scanPeakIndicesDict[scanNum + 1];

            if (scanStartIndex == scanStopIndex)
                return;
            //Engine.Utilities.BlockDeque<LCMSPeak >.iterator iter;
            //Engine.Utilities.BlockDeque<LCMSPeak >.iterator iter_first = mdeque_peaks.get_iter(scan_start_index);
            //Engine.Utilities.BlockDeque<LCMSPeak >.iterator iter_last = mdeque_peaks.get_iter(scan_stop_index);
            //
            //for (iter = iter_first; iter != iter_last; iter++)
            //{
            //    lc_peak = *iter;

            for (var i = scanStartIndex; i < scanStopIndex; i++)
            {
                var lcPeak = _peaksList[i];
                mzs.Add(lcPeak.Mz);
                intensities.Add(lcPeak.Intensity);
            }
        }

        internal class ScanData
        {
            public double ScanBpIntensity;
            public double ScanBpMz;
            public int ScanNum;
            public int ScanNumDeisotoped;
            public double ScanNumDriftTime;
            public short ScanNumMsLevel;
            public int ScanNumPeaks;
            public double ScanSignalRange;
            public double ScanTic;
            public double ScanTime;
        }
    }
}
#endif