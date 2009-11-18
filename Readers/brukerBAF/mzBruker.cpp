/*---------------------------------------------------------------------------
##  File:
##      @@(#) mzBruker.cpp
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
##  $Log: mzBruker.cpp,v $
##  Revision 1.1  2004/09/01 23:30:31  rhubley
##    Initial checkin
##
##
##
##---------------------------------------------------------------------------*/


#include "StdAfx.h"
#include "mzBruker.h"
#include <iostream>
#include <sstream>
#include "MyLibraryRegistry.h"
#include "BAFConverter.h"


void FillCommandLineVector(VectorString& vecCommandLine, 
			   int argc, TCHAR* argv[] );
bool CheckNoFlag(const std::string& toCheck);
void Parse(const VectorString& vecCommandLine);
void Usage();
void ParseDirs(const char *szPath);

const std::string s_sAnalysisPathFlag("-analysispath");
const std::string s_sQueueDirFlag("-queuepath");
const std::string s_sOutFileFlag("-outfile");
const std::string s_sVersionFlag("-v");
const std::string s_sProgramVersion("1.2");

//std::ostream& operator<<(std::ostream& os, UINT64 i ) {
//  char buf[20];
//  sprintf(buf,"%I64d", i);
//  os << buf;
//  return os;
//}



/**
 * The only purpose of this instantiation is library registration before 
 * entering main.
 */
CDALLibrayRegistry justForRegistration;

std::string m_sBafFile;
std::ofstream m_fOutFile;
std::string m_sQueueDir;


CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
  int nRetCode = 0;

  // initialize MFC and print and error on failure
  if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
  {
    cerr << _T("Fatal Error: MFC initialization failed") << endl;
    nRetCode = 1;
  }else
  {
    
   //UINT64 i = 0;
   //for ( i = 0; i < 10000000000; i+=100000 ) {
   //  std::cout << "i = " << i << endl;
   //} 


    if ( argc == 1 )
    {
      Usage();
    }else
    {
      // instantiate a command line object
      VectorString vecCommandLine;
      FillCommandLineVector( vecCommandLine, argc, argv );
      Parse( vecCommandLine );

      if ( m_sQueueDir != "" ) {
        cout << "Recursing through directories starting at: " << m_sQueueDir << endl;
        ParseDirs( m_sQueueDir.c_str() );
      }else {
        if ( !m_fOutFile.is_open() ) {
          // instantiate converter object
          CBAFConverter converter( m_sBafFile, std::cout );
          converter.convertFile();
        }else {
          CBAFConverter converter( m_sBafFile, m_fOutFile );
          converter.convertFile();
        }
      }
    }	
  }
  return nRetCode;
}


void ParseDirs(const char *szPath)
{
  TCHAR szCurDirPath[MAX_PATH + 1];
  TCHAR szNewFileName[MAX_PATH + 1];
  TCHAR szNewDirPath[MAX_PATH + 1];
  WIN32_FIND_DATA data;
  HANDLE hFind;
  BOOL bContinue = TRUE;

  sprintf(szCurDirPath, "%s*.*", szPath); // Build current path

  hFind = FindFirstFile(szCurDirPath, &data);

  if (INVALID_HANDLE_VALUE == hFind) {
  	cerr << "Invalid handle! Aborting";
  	exit(0);
  }

  // If we have no error, loop thru the files in this dir
  while (hFind && bContinue) {
    // Check if this entry is a directory
    string s = data.cFileName;    
    std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) tolower);
    //std::cout << "Considering file = " << s << endl;
    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      // Make sure this dir is not . or ..
      if (!(strcmp(data.cFileName, ".") == 0) && 
          !(strcmp(data.cFileName, "..") == 0)) {
        // Build the new dir path
	sprintf(szNewDirPath, "%s%s\\", szPath, data.cFileName);
	// *** Add dir to the listbox (or to your tree structure) ***
	// *** Modify as needed ***
	//m_lstDirs->AddString(szNewDirPath); 
	ParseDirs(szNewDirPath); // Recurse into the function
      }
    }else {
      if ( s == "analysis.baf" ){
        sprintf(szNewFileName, "%s%s", szPath, "analysis.xml");
        std::cout << "Opening output file = " << szNewFileName << endl;
        m_fOutFile.open( szNewFileName, std::ofstream::binary );
        if ( m_fOutFile.fail() ) {
          std::cout << "error: could not open " << szNewFileName << " for output" << endl;
          exit(1);
        }
        CBAFConverter converter( szPath, m_fOutFile );
        converter.convertFile();
        m_fOutFile.close();
      }
    }	
    bContinue = FindNextFile(hFind, &data);
  }
  FindClose(hFind); // Free the dir structure
}



void Usage()
{
  std::cout << "mzBruker [-v] [-analysispath path [-outfile outputfile]] | [-queuepath path]" << std::endl;
  std::cout << "  A converter from the Bruker MicroTOFLC file format to " << std::endl;
  std::cout << "  the mzXML format." << std::endl << std::endl;
  std::cout << "\t" << "-v                  : Print out the program version number." << std::endl;
  std::cout << "\t" << "-queuepath          : The top level directory of a work queue." << std::endl;
  std::cout << "\t" << "                        All files named analysis.baf in *all* " << std::endl;
  std::cout << "\t" << "                        subdirectories will be converted and " << std::endl;
  std::cout << "\t" << "                        the results will be placed in a " << std::endl;
  std::cout << "\t" << "                        corresponding analysis.xml file." << std::endl;
  std::cout << std::endl;
  std::cout << "\t" << " or" << std::endl;
  std::cout << std::endl;
  std::cout << "\t" << "-analysispath path  : The path to a directory containing an " << std::endl;
  std::cout << "\t" << "                        \"analysis.baf\" file." << std::endl;
  std::cout << "\t" << "-outfile outputfile : Optional file to write the mzXML to." << std::endl;
  std::cout << std::endl;
}





void Parse(const VectorString& vecCommandLine)
{
  VectorString::const_iterator it( vecCommandLine.begin() );

  if ( vecCommandLine.size() )
  {
    for ( ; it != vecCommandLine.end(); ++it )
    {
      if ( *it == s_sQueueDirFlag && CheckNoFlag(*++it) ) 
      {
        m_sQueueDir = *it;
        if ( m_sQueueDir.substr( m_sQueueDir.size()-1,1) != "\\" ) {
          m_sQueueDir += "\\";
        }
      }else if ( *it == s_sAnalysisPathFlag && CheckNoFlag(*++it) )
      {
        m_sBafFile = *it;
      }else if ( *it == s_sOutFileFlag && CheckNoFlag(*++it) )
      {
        m_fOutFile.open( it->c_str(), std::ofstream::binary );
      }else if ( *it == s_sVersionFlag )
      {
        std::cout << "mzBruker - " << s_sProgramVersion << std::endl;
        exit(0);
      }else {
        Usage();
        exit(0);
      }
    }
  }else
  {	
    Usage();
    exit(0);
  }
  if ( !( m_sQueueDir != "" || m_sBafFile  != "" ) || ( m_sQueueDir != "" && m_sBafFile != "" ) ) {
    cout << "here" << endl;
    Usage();
    exit(0);
  }

}





bool CheckNoFlag(const std::string& toCheck)
{
  bool bRet(false);

  if ( toCheck.size() > 0 && toCheck[0] != '-' )
  {
    bRet = true;
  }else
  {
    Usage();
  }

  return bRet;
}




void FillCommandLineVector(VectorString& vecCommandLine, 
						   int argc, TCHAR* argv[] )
{
  vecCommandLine.clear();
  vecCommandLine.reserve(argc-1);
  std::string argument;
  for ( int iCount = 1; iCount < argc; ++iCount )
  {
    argument = std::string( argv[iCount] );
    vecCommandLine.push_back( argument );
  }
}

