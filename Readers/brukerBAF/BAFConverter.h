/*---------------------------------------------------------------------------
##  File:
##      @@(#) BAFConverter.h
##  Author:
##      Robert M. Hubley   rhubley@systemsbiology.org
##         - based on the example application supplied by Bruker
##           in the CDAL library.
##  Description:
##      A program to convert Bruker MicroTOF LC raw data files (analysis.baf)
##      into mzXML files.
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
##  $Log: BAFConverter.h,v $
##  Revision 1.1  2004/09/01 23:30:31  rhubley
##    Initial checkin
##
##
##
##---------------------------------------------------------------------------*/
#ifndef MZBRUKER_BAFCONVERTER_H_
#define MZBRUKER_BAFCONVERTER_H_

#include <ostream>
#include <sstream>
#include <CDALInclude/AnalysisInterfaces.h>


/**
*/
class CBAFConverter
{
public:
	/**
	The destructor takes the analysis Directory and the display target (stream)
	as input parameter.
	*/
	CBAFConverter(const std::string& sAnalysisDirectory, std::ostream& output);

	/**
	destructor
	*/
	~CBAFConverter();

	/**
	*/
	void convertFile() const;

        template <typename T>
        std::ostream& operator<<(const T& t)
        {
             return (m_display << t);
        }


private:

        template <class T>
        std::string string_fmt(const T& t) const 
        {
          std::ostringstream oss;
          oss << t;
          return( oss.str() );
        }

  void ByteSwap( unsigned char * b, int n ) const;

  void encode_group (unsigned char output[],
                            const unsigned char input[],
                            int n) const;
  int b64_encode (char *dest,
                  const char *src,
                  int len) const;

	/// helper function to handle exceptions
	void HandleException() const;
	void do_convertFile() const;
        std::string   m_sAnalysisDirectory;
	std::ostream&  m_display;
};


#endif // MZBRUKER_BAFCONVERTER_H_
