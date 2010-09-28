#using <mscorlib.dll>
#include "Calibrations/Calibrator.h"
#pragma once
namespace DeconToolsV2
{
	public __gc class CalibrationSettings
	{
	public: 


		double ML1;
		double ML2;
		double SW_h;
		int ByteOrder;
		int TD;
		double FRLow;
		int NF;

		CalibrationSettings(void);


	} ; 
}