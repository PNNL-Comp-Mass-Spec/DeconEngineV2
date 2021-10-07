# DeconMSn

DeconMSn creates spectrum files for tandem mass spectrometry data. DeconMSn
can read in Finnigan .RAW files provided that XRawFile2.dll v2.2 is installed
on the computer.  This file is installed with Xcalibur 2.2, but can also be
installed using MSFileReader v2.2, available at:
http://sjsupport.thermofinnigan.com/public/detail.asp?id=703

DeconMSn also supports the mzXML format.  You can use Proteowizard's MSConvert
tool to convert most vendor formats into the mzXML format:
* https://proteowizard.sourceforge.io/

For datasets with high resolution MS1 spectra, DeconMSn calculates the monoisotopic mass
of the parent ion for each MSn spectrum by applying a modified THRASH approach 
to the parent isotopic distribution. For datasets with low-resolution 
MS1 spectra, DeconMSn uses a support-vector machine based charge-detection 
algorithm to determine parent mass. However, this algorithm is not fully
tested and therefore you should process low resolution datasets using
Proteowizard's MSConvert tool.

Results can be saved as .DTA files, .MGF files, or _DTA.txt files.  These files
can then be used for peptide identifications using search engines such as 
SEQUEST, X!Tandem, MASCOT, or MSGF+

## Downloads

Download DeconMSn from the DeconMSn Releases page [on GitHub](https://github.com/PNNL-Comp-Mass-Spec/DeconEngineV2/releases)

## Syntax

DeconMSn usage: `DeconMSn [options] filename`

| Parameter   | Description                                                    | Default  |
| ----------- |----------------------------------------------------------------| ------------|
| `-Ivalue`   | value is the minimum Number of ions peaks for a scan to be considered   | 35 |
| `-Fvalue`   | value is the first Scan to be considered                       | 1           |
| `-Lvalue`   | value is the last Scan to be considered                        | 1000000     |
| `-Bvalue`   | value is the minimum setting for Mass Range                    | 200         |
| `-Tvalue`   | value is the maximum setting for Mass Range                    | 5000        |
| `-Cvalue`   | value is a single charge state to filter the results on        | not-defined |
| `-Pname`    | name is the parameter XML file name to be used for processing  | not-defined |
| `-Dpath`    | path is the output directory                                   | same directory as input file |
| `-Centroid` | Enables centroiding MSn data (when acquired as profile data)   | off by default |
| `-Progress` | Creates a _progress.txt file with a percent complete value every 50 scans | n/a |
| `-Stype`    | type is the type of spectra to process, options are ALL, CID, ETD, or HCD     | ALL |
| `-Xtype`    | type is output file format, options are DTA, LOG, MGF, CDTA    | CDTA |
| filename    | input file (mzXML or RAW)                                      | n/a  |

Output file formats:

| Type     | Description   |
| ---------|---------------|
| DTA      | Creates .dta files along with a log file (_log.txt) and profile file (_profile.txt) |
| LOG      | Creates only log file (_log.txt) and profile file (_profile.txt) |
| MGF      | Creates .mgf file along with log file(_log.txt) and profile file (_profile.txt) |
| CDTA     | Creates a composite dta file (_dta.txt) along with a log file (_log.txt) and profile file (_profile.txt) |

Note that the switch parameters are case-sensitive (for example you must use -P an not -p)

### Examples

Process sample.raw, create a concatenated DTA (_dta.txt) file \
`DeconMSn.exe C:\data\sample.RAW`

Process sample.raw, create .dta files \
`DeconMSn.exe -XDTA C:\data\sample.RAW`

Process sample.raw with scan range of 1000-1200 and mass range of 200-1000, create .dta files \
`DeconMSn.exe -F1000 -L1200 -B200 -T1000 C:\data\sample.RAW`

Process sample.raw using default options; create .MGF as result file \
`DeconMSn.exe -XMGF C:\data\sample.RAW`

Process sample.raw, use processing options specified by parameter file LTQ_FT_Normal.xml \
`DeconMSn.exe -PLTQ_FT_Normal.xml C:\data\sample.RAW`

## Dependencies

.NET 4.7.2 runtime

## Contacts

Written by Anoop Mayampurath and Navdeep Jaitly for the Department of Energy \
Ported to C# by Bryson Gibbons \
(PNNL, Richland, WA) in 2006-2007. \
Copyright 2007, Battelle Memorial Institute.  All Rights Reserved.

E-mail: matthew.monroe@pnnl.gov or proteomics@pnnl.gov \
Website: https://github.com/PNNL-Comp-Mass-Spec/ or https://panomics.pnnl.gov/ or https://www.pnnl.gov/integrative-omics

## License

Licensed under the Apache License, Version 2.0; you may not use this file 
except in compliance with the License.  You may obtain a copy of the License 
at http://www.apache.org/licenses/LICENSE-2.0