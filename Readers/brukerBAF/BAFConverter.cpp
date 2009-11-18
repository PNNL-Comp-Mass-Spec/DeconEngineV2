/*---------------------------------------------------------------------------
##  File:
##      @@(#) BAFConverter.cpp
##  Author:
##      Robert M. Hubley   rhubley@systemsbiology.org
##         - based on the example application supplied by Bruker
##           in the CDAL library.
##  Description:
##      Convert a file from the Bruker MicroTOF LC raw format into
##      an mzXML file.
##
##*****************************************************************************
##
##   This program is free software; you can redistribute it and/or modify  
##   it under the terms of the GNU Library or "Lesser" General Public      
##   License (LGPL) as published by the Free Software Foundation;          
##   either version 2 of the License, or (at your option) any later        
##   version.                                                              
##
##*****************************************************************************
##
## Changes
##
##  $Log: BAFConverter.cpp,v $
##  Revision 1.1  2004/09/01 23:30:31  rhubley
##    Initial checkin
##
##
##
##---------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "sha1.h"
#include "BAFConverter.h"
#include <CDALInclude/Exceptions.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm> // required for std::swap


std::ostream& operator<<(std::ostream& os, __int64 i )
{
  char buf[20];
  sprintf(buf,"%I64d", i);
  os << buf;
  return os;
}

std::ostream& operator<<(std::ostream& os, UINT64 i )
{
  char buf[20];
  sprintf(buf,"%I64u", i);
  os << buf;
  return os;
}


////////////////// constructor and destructor /////////////////////////////////
CBAFConverter::CBAFConverter(const std::string& sAnalysisDirectory, std::ostream& out) :
				   m_sAnalysisDirectory(sAnalysisDirectory), m_display(out)
{
  if ( ! m_sAnalysisDirectory.empty() ) {
    if ( m_sAnalysisDirectory[m_sAnalysisDirectory.size()-1] == '/' ||
         m_sAnalysisDirectory[m_sAnalysisDirectory.size()-1] == '\\' ) {
      m_sAnalysisDirectory.erase( m_sAnalysisDirectory.size()-1 );
    }
  }
}


CBAFConverter::~CBAFConverter()
{
}


void CBAFConverter::do_convertFile() const
{

  UINT64 scanCount = 0; 
  UINT64 indexElementOffset = 0;
  UINT64 byteCounter = 0;
  CSHA1 sha1;
  char szRawSHA1Hash[50];
  std::string rawFile( m_sAnalysisDirectory );
  std::string outputLine;
  std::stringstream outLine;

   
  rawFile += "\\analysis.baf";
  std::cout << "Opening ";
  std::cout << rawFile;
  std::cout << std::endl;
  if ( !sha1.HashFile( (char *)rawFile.c_str() ) ) {
    std::cout << "Error...could not hash raw file analysis.baf ";
    exit( 0 );
  }
  std::cout << "Opened file!" << std::endl;
  sha1.Final();
  szRawSHA1Hash[0] = '\0';
  sha1.ReportHash( szRawSHA1Hash, CSHA1::REPORT_HEX );

  sha1.Reset();

  outputLine = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
  sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
  m_display << outputLine;
  byteCounter += outputLine.size();

  
  //
  // Open the analysis with the factory function
  //
  CDAL::IAnalysisPtr pAnalysis( CDAL::IAnalysis::Open(m_sAnalysisDirectory) );

  // do we have a valid (smart) analysis-pointer?
  if ( pAnalysis.get() ) {


    //
    // Get analysis META data
    //
    CDAL::IAnalysisMetaInfoPtr pAnalysisMeta( pAnalysis->GetMetaInfo() );
    if ( pAnalysisMeta ) {
      //m_display << "Analysis Meta Info: " << std::endl;
      std::string sName;
      pAnalysisMeta->GetOperatorName( sName );
      //m_display << "  Analysis Name: " << sName << std::endl;
      //TODO MORE TO GET HERE
    }
        

    // let's pass a chromatographic spectrum collection to the interface
    //CDAL::ChromSpectrumCollection specCollection;
    CDAL::SpectrumCollection specCollection;
    pAnalysis->GetSpectrumCollection(specCollection);


    // Allocate a vector to hold the file offsets
    std::vector<unsigned int> offsets( specCollection.size() + 1 );


    if ( !specCollection.empty() ) {
      outputLine = "<msRun xmlns=\"http://sashimi.sourceforge.net/schema/\" ";
      outputLine += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
      outputLine += "xsi:schemaLocation=\"http://sashimi.sourceforge.net/schema/ ";
      outputLine += "http://sashimi.sourceforge.net/schema/MsXML.xsd\" ";
      outputLine += "scanCount=\"" + string_fmt( specCollection.size() ) + "\" ";
      outputLine += ">\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "<parentFile fileName=\"" + rawFile + "\" ";
      outputLine += "fileType=\"RAWData\" ";
      outputLine += "fileSha1=\"" + string_fmt( szRawSHA1Hash ) + "\"/>\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "<instrument manufacturer=\"Bruker\" ";
      outputLine += "model=\"microTOFLC\" ";
      outputLine += "ionisation=\"ESI\" ";
      outputLine += "msType=\"TOF\">\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "<software type=\"acquisition\" ";
      outputLine += "name=\"CDAL\" ";
      outputLine += "version=\"1.0.7\"/>\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "</instrument>\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "<dataProcessing intensityCutoff=\"0\">\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "<software type=\"conversion\" ";
      outputLine += "name=\"mzBruker\" ";
      outputLine += "version=\"1.0\"/>\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "<processingOperation ";
      outputLine += "name=\"min_peaks_per_spectra\" ";
      outputLine += "value=\"1\"/>\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();

      outputLine = "</dataProcessing>\n";
      sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
      m_display << outputLine;
      byteCounter += outputLine.size();


      CDAL::RetentionTime retentionTime;
      CDAL::eION_POLARITY polarity;

      CDAL::Real64_t min; 
      CDAL::Real64_t max;
      CDAL::Real64_t basePeak;
      CDAL::VectorReal64 vecMZ;
      CDAL::VectorReal64 vecIntensity;
      CDAL::Real64_t totalIONCurrent;
  

      // SCAN/SPECTRUM LOOP
      //CDAL::ChromSpectrumCollection::const_iterator it( specCollection.begin() );
      CDAL::SpectrumCollection::const_iterator it( specCollection.begin() );
      for ( ; it != specCollection.end(); ++it ) {


        // Grab out the spectrum pointer
        //CDAL::ISpectrumPtr pSpectrum( it->second );
        CDAL::ISpectrumPtr pSpectrum( *it );

        // 
        // Get the spectrum parameters
        //
        CDAL::ISpectrumParameterPtr pSpectrumParam( pSpectrum->GetSpectrumParameter() );
        pSpectrumParam->GetRetentionTime( retentionTime );
        pSpectrumParam->GetIonPolarity( polarity );


        // access and display/dump the raw data of the spectrum
        if ( pSpectrum->HasRawData() ) {

          CDAL::ISpectrumRawDataPtr pRawData( pSpectrum->GetSpectrumRawData() );

          // Get raw mz/int pairs as two vectors
          pRawData->GetMZ( vecMZ );
          pRawData->GetIntensity( vecIntensity );

          // Get min/max/sum intensities
          pRawData->GetIntensityMinMax( min, max );
          pRawData->GetSumIntensity( totalIONCurrent ); 
 

          // Now build an array of UINT64 using the mz/int pairs.  Make sure we
          // don't exceed the size of this type by converting.

          
          UINT32 *pEncoded;
          CDAL::Real32_t *pData;
          unsigned int n_values_to_encode = 2 * vecMZ.size();

          // If we have zero peaks we still need to store the scan meta data.
          // For some strange reason the mzXML schema requires that a peaks
          // tag be included with all scans.  So...we create a single zero
          // mass, zero intensity peak and store this in the scan.
          if ( !n_values_to_encode ) {
            n_values_to_encode = 2;
          }

          // Malloc an array to hold the 32 bit real mz/int pairs for byte swapping.
          if ( !(pData = (CDAL::Real32_t *) malloc( n_values_to_encode * sizeof(CDAL::Real32_t) ))  ) {
            std::cout << "Cannot allocate memory for pData buffer!";
          }

          // Convert the 64 bit real mz/int pairs to 32bit reals.  Also look for
          // the peak with the max intensity and grab it's mass.     
          // TODO: Consider what happens when we have more than one peak
          //       with the max mass.
          // TODO: Check for overflows during conversion to 32bit and
          //       warn the user.
          CDAL::VectorReal64::const_iterator it_x( vecMZ.begin() );
          CDAL::VectorReal64::const_iterator it_y( vecIntensity.begin() );
          CDAL::Real32_t *pDataPtr = pData;
          UINT64 pDataIndex = 0;
          basePeak = 0;
          for ( ; it_x != vecMZ.end(); ++it_x, ++it_y ) {
            if ( *it_y > 0.0 ) {
              pDataPtr[pDataIndex++] = *it_x; 
              pDataPtr[pDataIndex++] = *it_y; 
            }
            if ( basePeak == 0 && *it_y == max ) {
              basePeak = *it_x;
            }
          }

          // Create an zero peak pair if we need to
          if ( ! pDataIndex ) {
            pDataPtr[0] = 0;
            pDataPtr[1] = 0;
            n_values_to_encode = 2;
            pDataIndex = 2;
          }else {
            n_values_to_encode = pDataIndex;
          }

          // Convert to network byte order (assuming little-endian architecture here)
          UINT32 n = 0;
          for ( n = 0; n < n_values_to_encode; n++ ) {
            ByteSwap( (unsigned char * )&(pData[n]), sizeof(UINT32) );
          }

          // Malloc an array to hold the output ( b64 encoded ) peaks
          if ( !(pEncoded = (UINT32 *) malloc( (((n_values_to_encode) * sizeof(UINT32)) / 3) * 4 + 5))  ) {
            std::cout << "Cannot allocate memory for encode buffer!";
          }
 
          // Base 64 encode.
          int encode_length = 0;
          encode_length = b64_encode( (char *)pEncoded, (const char *)pData, n_values_to_encode * sizeof(UINT32) );
          ((char *)pEncoded)[encode_length] = '\0';
  
          // Save the byte offset
          offsets.push_back( byteCounter );

          // Start the scan line
          std::cout << "  - Writing scan #";
          char foo[2000];
          std::cout << scanCount;
          std::cout << std::endl;
          outputLine =  "<scan num=\"" + string_fmt( (scanCount++) + 1 ) + "\" msLevel=\"1\" ";
          outputLine += "peaksCount=\"" + string_fmt( pDataIndex  / 2 ) + "\" ";
          outputLine += "polarity=\"" + string_fmt( (polarity==CDAL::ION_POL_POSITIVE ? "+" : "-") ) + "\" ";
          //outputLine += "retentionTime=\"PT" + string_fmt( it->first ) + "S\" ";
          outputLine += "retentionTime=\"PT" + string_fmt( retentionTime ) + "S\" ";
          outputLine += "lowMz=\"" + string_fmt( vecMZ[0] ) + "\" ";
          outputLine += "highMz=\"" + string_fmt( vecMZ[vecMZ.size()-1] ) + "\" ";
          outputLine += "basePeakMz=\"" + string_fmt( basePeak ) + "\" ";
          outputLine += "basePeakIntensity=\"" + string_fmt( max ) + "\" ";
          outputLine += "totIonCurrent=\"" + string_fmt( totalIONCurrent ) + "\">\n";
          outputLine += "<peaks precision=\"32\">";
          outputLine += std::string( (char *)pEncoded ) + "</peaks>\n";
          outputLine += "</scan>\n";
          sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
          m_display << outputLine;
          byteCounter += outputLine.size();

          free( pData );
          free( pEncoded );

        } // if ( HasRawData()... 

      } // Scan Iterator

    }else {  // If specCollection
      std::cout << "Error: analysis.baf file doesn't contain any spectrum!\n";
      exit( 0 );
    }

    indexElementOffset = byteCounter;
    outputLine = "<index name=\"scan\">\n";
    sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
    m_display << outputLine;
    std::vector<unsigned int>::iterator it_off( offsets.begin() );
    UINT64 index = 1;
    for ( ; it_off != offsets.end(); ++it_off ) {
      if ( *it_off > 0 ) {
        outputLine = "<offset id=\"" + string_fmt( index++ ) + "\">";
        outputLine += string_fmt( *it_off );
        outputLine += "</offset>\n";
        sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
        m_display << outputLine;
      }
    }
    outputLine = "</index>\n";
    outputLine += "<indexOffset>" + string_fmt( indexElementOffset ) + "</indexOffset>\n";
    sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
    m_display << outputLine;

    outputLine = "<sha1>";
    sha1.Update( (unsigned char *)outputLine.c_str(), outputLine.size() );
    m_display << outputLine;

    // Finalize SHA1
    sha1.Final();
    szRawSHA1Hash[0] = '\0';
    sha1.ReportHash( szRawSHA1Hash, CSHA1::REPORT_HEX );

    outputLine = string_fmt( szRawSHA1Hash ) + "</sha1>\n";
    outputLine += "</msRun>\n";
    m_display << outputLine;
  }

}


void CBAFConverter::ByteSwap( unsigned char * b, int n ) const
{
  register int i = 0;
  register int j = n - 1;
  while ( i < j ) 
  {
    std::swap( b[i], b[j] );
    i++, j--;
  }

}



void CBAFConverter::convertFile() const
{
	try
	{
		do_convertFile();
	}
	catch(...)
	{
		HandleException();
	}
}



void CBAFConverter::HandleException() const
{
	try
	{
		throw;
	}
	catch(const CDAL::CDALContentsException& e)
	{
		std::cout << "currupt content was detected. Error ID : " << e.GetErrorID() << std::endl << std::endl;
	}
	catch(const CDAL::CDALFileException& e)
	{
                if ( e.GetErrorID() == CDAL::FILE_ERROR_DIRECTORY_NOT_EXISTS ) {
		  std::cout << "a file error occured. Directory does not exist" << std::endl << std::endl;
                }    
                if ( e.GetErrorID() == CDAL::FILE_ERROR_FILE_NOT_EXISTS ) {
		  std::cout << "a file error occured. File does not exist" << std::endl << std::endl;
                }    
                if ( e.GetErrorID() == CDAL::FILE_ERROR_FILE_ACCESS_VIOLATION ) {
		  std::cout << "a file error occured. File access violation"  << std::endl << std::endl;
                }    

	}
	catch(const CDAL::CDALBaseException& e)
	{
		std::cout << "a CDAL base error occurred. Error ID : " << e.GetErrorID() << std::endl << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << "a standard error occurred : " << e.what() << std::endl << std::endl;
	}	
	catch(...)
	{
		std::cout << "an unexpected error occurred" << std::endl << std::endl;
	}
}


static const unsigned char *b64_tbl = (const unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned char b64_pad = '=';

/* base64 encode a group of between 1 and 3 input chars into a group of  4 output chars */
void CBAFConverter::encode_group (unsigned char *output,
                          const unsigned char *input,
                          int n) const
{
   unsigned char ingrp[3];

   ingrp[0] = n > 0 ? input[0] : 0;
   ingrp[1] = n > 1 ? input[1] : 0;
   ingrp[2] = n > 2 ? input[2] : 0;

   /* upper 6 bits of ingrp[0] */
   output[0] = n > 0 ? b64_tbl[ingrp[0] >> 2] : b64_pad;

   /* lower 2 bits of ingrp[0] | upper 4 bits of ingrp[1] */
   output[1] = n > 0 ? b64_tbl[((ingrp[0] & 0x3) << 4) | (ingrp[1] >> 4)] : b64_pad;

   /* lower 4 bits of ingrp[1] | upper 2 bits of ingrp[2] */
   output[2] = n > 1 ? b64_tbl[((ingrp[1] & 0xf) << 2) | (ingrp[2] >> 6)] : b64_pad;

   /* lower 6 bits of ingrp[2] */
   output[3] = n > 2 ? b64_tbl[ingrp[2] & 0x3f] : b64_pad;

}


int CBAFConverter::b64_encode (char *dest,
                const char *src,
                int len) const
{
   int outsz = 0;

   while (len > 0)
   {
      encode_group ((unsigned char *)(dest + outsz), (const unsigned char *)src, len > 3 ? 3 : len);
      len -= 3;
      src += 3;
      outsz += 4;
   }

   return outsz;
}

