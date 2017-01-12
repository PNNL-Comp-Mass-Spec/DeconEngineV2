.NET 1.1 version of DeconEngineV2, C++
	Port of DeconEngine from VB to C++
	http://prismsvn.pnl.gov/svn/User_Applications/Jaitly_Utilities/Decon2LS/branches/Decon2LS_Beta_activeBranch
	Used by DeconConsole.exe (11/30/2012) to deisotope spectra
	Compile with Visual Studio 2003, but only works on XP; does not compile with .NET 2003 on Windows 7 (according to Gordon Slysz)

.NET 4.0 version of DeconEngineV2, C++
	Port of DeconEngineV2 to VS 2010
	https://stash.pnnl.gov/projects/OMCS/repos/DeconEnginev2/browse
	http://prismsvn.pnl.gov/svn/User_Applications/Slysz_Utilities/DeconEngineV2/trunk
	Charge determination is 2x slower compared with the .NET 1.1 version

C# DeconTools (with ThrashV2)
	Port of DeconEngine to C#
	https://stash.pnnl.gov/projects/OMCS/repos/DeconTools/browse
	http://prismsvn.pnl.gov/svn/User_Applications/Slysz_Utilities/DeconTools/trunk
	See file DeconTools.Backend\ProcessingTasks\Deconvoluters\HornDeconvolutor\ThrashDeconvolutorV2.cs
	Used by DeconConsole if parameter file has:
		<DeconvolutionType>ThrashV2</DeconvolutionType>
	As of 2016, not used because results do not agree with ThrashV1, C++

C# DeconTools (with ThrashV1)
	2016 Port of ThrashV1 in DeconEngineV2 to C#, .NET 4
	https://stash.pnnl.gov/projects/OMCS/repos/DeconTools/browse/DeconTools.Backend/ProcessingTasks/Deconvoluters/HornDeconvolutor/ThrashV1
	- Part of DeconTools.Backend
	https://stash.pnnl.gov/projects/OMCS/repos/deconenginev2/browse/C%23_Version
	- Standalone, unused
	- Note: File layout has been optimized in DeconTools.Backend/ProcessingTasks/Deconvoluters/HornDeconvolutor/ThrashV1
	  and thus does not match that in DeconEnginev2/C#_Version

	As of July 2016, the 2016 Port of ThrashV1 is used by DeconConsole if the parameter file has:
		<DeconvolutionType>ThrashV1</DeconvolutionType>

DeconMSn code, C#:
	Original C++ repo: https://stash.pnnl.gov/projects/OMCS/repos/deconmsn/browse
	Also contains merged code from DeconMSn, with 2 branches in the Git repository:
	DeconMSn_Match:
		Contains code that will produce files identical to the C++ DeconMSn, with the exception of precursor mass precision (this version has more digits of precision)
		Also contains code to output a _ScanType.txt file, and for outputting accurate centroided masses.

	Master branch:
		Differs from the DeconMSn_Match by using the MathNet.Numerics implementations of Fast Fourier Transform and Cubic Spline Interpolation instead of ported Numerical Recipes in C code
			This affects 1%-5% of the output, with approximately 2/3 of the output changes being better results on the charge state, and sometimes changes the precursor mass without changing the charge.
