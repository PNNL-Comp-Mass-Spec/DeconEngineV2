# __<span style="color:#D57500">DeconMSn C#/.NET version</span>__
DeconMSn creates spectrum files for tandem mass spectrometry data.

### Description
DeconMSn can read in Finnigan .RAW files.

DeconMSn also supports the mzXML format. You can use Proteowizard's MSConvert tool to convert most vendor formats into the mzXML format.

For datasets with high resolution MS1 spectra (e.g. LTQ-FT or LTQ-Orbitrap), DeconMSn calculates the monoisotopic mass of the parent ion for each MSn spectrum by applying a modified THRASH approach (see the DeconMSn tutorial below and the DeconLS tutorial) to the parent isotopic distribution. For datasets with low-resolution MS1 spectra, DeconMSn uses a support-vector machine based charge-detection algorithm to determine parent mass. However, this algorithm is not fully tested and therefore you should process low resolution datasets using Proteowizard's MSConvert tool.

Results can be saved as .DTA files, .MGF files, or _DTA.txt files. These files can then be used for peptide identifications using search engines such as SEQUEST, X!Tandem, MASCOT, or MSGF+ (see Matrix Science's Data File Format page for additional descriptions of these file types). Additional details on the DeconMSn software are available in the 2007 ASMS poster and in a Tutorial Powerpoint file (available below).

The "Added Functionality" versions properly handle retrieving centroided peak data from Thermo .RAW files, and also automatically create a "_ScanType.txt" file when creating a "_DTA.txt" file, which MSGF+ will use to automatically determine collision mode and also output retention time to the mzid results.

Please note that the nature of the distribution of correct identifications to wrong identifications can be substantially affected by the use of DeconMSn. Thus, caution must be exercised while using tools such as Peptide Prophet that are trained on differing distributions.

### Downloads
* [Latest version: C#/.NET, no additional libraries needed](https://github.com/PNNL-Comp-Mass-Spec/DeconEngineV2/releases)
* [Source code on GitHub](https://github.com/PNNL-Comp-Mass-Spec/DeconEngineV2)
* [DeconMSn MatLab, SVM Scripts](https://pnnl-comp-mass-spec.github.io/DeconMSn/DeconMSn_SVM_ChargeDetermination.zip)
* [Supplementary Information](https://pnnl-comp-mass-spec.github.io/DeconMSn/DeconMSn_SupplementaryInfo.ppt)

### Acknowledgment

All publications that utilize this software should provide appropriate acknowledgement to PNNL and the DeconMSn GitHub repository. However, if the software is extended or modified, then any subsequent publications should include a more extensive statement, as shown in the Readme file for the given application or on the website that more fully describes the application.

### Disclaimer

These programs are primarily designed to run on Windows machines. Please use them at your own risk. This material was prepared as an account of work sponsored by an agency of the United States Government. Neither the United States Government nor the United States Department of Energy, nor Battelle, nor any of their employees, makes any warranty, express or implied, or assumes any legal liability or responsibility for the accuracy, completeness, or usefulness or any information, apparatus, product, or process disclosed, or represents that its use would not infringe privately owned rights.

Portions of this research were supported by the NIH National Center for Research Resources (Grant RR018522), the W.R. Wiley Environmental Molecular Science Laboratory (a national scientific user facility sponsored by the U.S. Department of Energy's Office of Biological and Environmental Research and located at PNNL), and the National Institute of Allergy and Infectious Diseases (NIH/DHHS through interagency agreement Y1-AI-4894-01). PNNL is operated by Battelle Memorial Institute for the U.S. Department of Energy under contract DE-AC05-76RL0 1830.

We would like your feedback about the usefulness of the tools and information provided by the Resource. Your suggestions on how to increase their value to you will be appreciated. Please e-mail any comments to proteomics@pnl.gov
