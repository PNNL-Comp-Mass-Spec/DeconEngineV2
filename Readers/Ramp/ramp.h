/***************************************************************************
                             RAMP


Non sequential parser for mzXML files

                             -------------------
    begin                : Wed Oct 10
    copyright            : (C) 2003 by Pedrioli Patrick, ISB, Proteomics
    email                : ppatrick@student.ethz.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "winsock.h"
//#include <netinet/in.h>
#include <string.h>
#include "base64.h"

namespace Engine
{
	namespace Readers
	{
		const int INSTRUMENT_LENGTH = 2000 ;

		struct ScanHeaderStruct
		{
		int  msLevel;
		int  peaksCount;
		double retentionTime;        /* in seconds */
		double lowMZ;
		double highMZ;
		double precursorMZ;  /* only if MS level > 1 */
		double basePeakMz;
		double basePeakIntensity;
		double totIonCurrent;
		char msTypeString[256] ;  /* AM: to read out the filter string FTMS or ITMS*/ 
		char profileType ; /* AM : to recognize if the peaks are in profile or centroided */
		};

		struct RunHeaderStruct
		{
		double startMZ;
		double endMZ;
		double lowMZ;
		double highMZ;
		double dStartTime;
		double dEndTime;
		};

		typedef struct InstrumentStruct
		{
		char manufacturer[INSTRUMENT_LENGTH];
		char model[INSTRUMENT_LENGTH];
		char ionisation[INSTRUMENT_LENGTH];
		char analyzer[INSTRUMENT_LENGTH];
		char detector[INSTRUMENT_LENGTH];
		//char msType[INSTRUMENT_LENGTH];
		} InstrumentStruct;


		off_t getIndexOffset(FILE * pFI);
		off_t *readIndex(FILE * pFI,
						off_t indexOffset,
						int *iLastScan, off_t* &pScanIndex);
		void readHeader(FILE * pFI,
						off_t lScanIndex,
						struct ScanHeaderStruct *scanHeader);
		int  readMsLevel(FILE * pFI,
						off_t lScanIndex);
		double readStartMz(FILE * pFI,
				off_t lScanIndex);
		double readEndMz(FILE * pFI,
				off_t lScanIndex);
		char *readRT(FILE * pFI,
					off_t lScanIndex);
		float *readPeaks(FILE * pFI,
						off_t lScanIndex);
		void readRunHeader(FILE * pFI,
						off_t *pScanIndex,
						struct RunHeaderStruct *runHeader,
						int iLastScan);

		int setTagValue(char* text,
			char* storage,
			int maxlen,
			char* lead,
			char* tail);

		InstrumentStruct* getInstrumentStruct(FILE* pFI);
	}
}