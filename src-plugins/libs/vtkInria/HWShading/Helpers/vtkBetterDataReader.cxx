/*============================================================================

The Hardware Shading (HWShading) module is protected by the
following copyright:
Copyright (c) 2007 Biomedical Image Analysis (BMIA) - Department of
Biomedical Engineering - Eindhoven University of Technology.
All rights reserved. See Copyright.txt for details.
The HWShading implementation was originally written by Tim Peeters (BMIA - TUe)
and published at the "11th International Fall Workshop on Vision, Modeling,
and Visualization 2006" (VMV'06):
"Visualization of the Fibrous Structure of the Heart", by T. Peeters et al.
See http://timpeeters.com/research/vmv2006 for more information.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
============================================================================*/
/**
 * vtkBetterDataReader.cxx
 *
 * 2003-02-26	Tim Peeters
 * - Initial version (in DNAVis)
 *
 * 2003-05-08	Tim Peeters
 * - New version.
 *
 * 2003-07-23	Tim Peeters
 * - Added functions with a spacers vector as an argument.
 *
 * 2005-02-11	Tim Peeters
 * - Ported from DNAVis to VTK (see comments in header file).
 *
 * 2005-05-17	Tim Peeters
 * - Replaced asserts by vtk{Debug,Warning,Error}Macro for non-static methods.
 *
 * 2005-06-03	Tim Peeters
 * - Use bmia namespace and cleaned up included headers.
 */

#include "vtkBetterDataReader.h"

#include <vtkObjectFactory.h>
#include <vtkErrorCode.h>
#include <vtkIOStream.h>

#include <assert.h>

vtkStandardNewMacro(vtkBetterDataReader);

vtkBetterDataReader::vtkBetterDataReader()
{
  this->ResetCurrentLineNumber();
  this->FileName = NULL;
  //this->CurrentLineNumber = -1;
  //this->CurrentLine = "";
}

void vtkBetterDataReader::ResetCurrentLineNumber()
{
  this->CurrentLineNumber = 0;
}

vtkBetterDataReader::~vtkBetterDataReader()
{
  if (this->FileName)
    {
    delete [] this->FileName;
    this->FileName = NULL;
    }
}

bool vtkBetterDataReader::NextLine()
{
  string nextLine; 
  // read the next line into nextLine. \n is used as delimiter:
  if (this->IS == NULL)
    {
    vtkErrorMacro("IS is NULL!");
    return false;
    }

  if (!std::getline(*this->IS, nextLine))
    { // eof?
    return false;
    }

  this->CurrentLineNumber++;

  // to avoid problems caused by Windows systems that have \r\n at the end of
  // a line.
  if (nextLine[nextLine.size() - 1] == '\r')
    {
    this->CurrentLine = nextLine.substr(0, nextLine.size() - 1);
    }
  else
    {
    this->CurrentLine = nextLine;
    }
  return true;
}

vector<char> vtkBetterDataReader::GetStandardSpacers()
{
  vector<char> spacers;
  spacers.push_back(' ');
  spacers.push_back('\t');
  return spacers;
}

bool vtkBetterDataReader::IsSpacer(char ch)
{
  // return true if ch is a space or a tab.
  return ((ch == ' ') || (ch == '\t'));
}

bool vtkBetterDataReader::IsSpacer(char ch, vector<char> spacers)
{
  bool result = false;
  for (unsigned int i=0; i < spacers.size(); i++)
    {
    if (ch == spacers[i]) result = true;
    }
  return result;
}

string vtkBetterDataReader::ReadWord(string line, unsigned int pos, unsigned int& endpos)
{
  vtkBetterDataReader::StripSpacers(line, pos, endpos);
  unsigned int startpos = endpos;
  while ( (endpos < line.size()) &&
          ( !(vtkBetterDataReader::IsSpacer(line[endpos])) ) )
    {
    endpos++;
    }
  return line.substr(startpos, endpos-startpos);
}

string vtkBetterDataReader::ReadWord(string line, unsigned int pos,
                                     unsigned int& endpos, vector<char> spacers)
{
  vtkBetterDataReader::StripSpacers(line, pos, endpos, spacers);
  unsigned int startpos = endpos;
  while ( (endpos < line.size()) &&
        ( !(vtkBetterDataReader::IsSpacer(line[endpos], spacers)) ) )
    {
    endpos++;
    }
  return line.substr(startpos, endpos-startpos);
}

int vtkBetterDataReader::ReadInt(string line, unsigned int pos, unsigned int& endpos)
{
  vtkBetterDataReader::StripSpacers(line, pos, endpos);
  int result = 0;
  while (	( endpos < line.size() ) 	&&
		( line[endpos] - '0' < 10  )	&&
		( line[endpos] - '0' >= 0  )	)
    {
    result = result * 10 + (line[endpos] - '0');
    endpos++;
    } // while
  return result;
}

float vtkBetterDataReader::ReadFloat(string line, unsigned int pos, unsigned int& endpos)
{
  string word = vtkBetterDataReader::ReadWord(line, pos, endpos);
  //return strtof(word.c_str(), 0);
  return (float) strtod(word.c_str(), 0); // MS VC++ does not have strtof.
}

double vtkBetterDataReader::ReadDouble(string line, unsigned int pos, unsigned int& endpos)
{
  string word = vtkBetterDataReader::ReadWord(line, pos, endpos);
  //return strtof(word.c_str(), 0);
  return strtod(word.c_str(), 0); // MS VC++ does not have strtof.
}


string vtkBetterDataReader::ReadQuotedString(string line, unsigned int pos, unsigned int& endpos)
{
  vtkBetterDataReader::StripSpacers(line, pos, endpos);
  vtkBetterDataReader::ReadChar('\"', line, pos, endpos);
  unsigned int startpos = endpos;
  while ( (endpos < line.size()) && ( line[endpos] != '\"' ) )
    {
    endpos++;
    }
  vtkBetterDataReader::ReadChar('\"', line, endpos, endpos);
  return line.substr(startpos, endpos-startpos-1);
}

unsigned int vtkBetterDataReader::StripSpacers(string line, unsigned int pos, unsigned int& endpos)
{
  endpos = pos;
  // strip spaces and tabs.
  while ((endpos < line.size()) && vtkBetterDataReader::IsSpacer(line[endpos]))
    {
    endpos++;
    }

  // return the number of spaces and tabs that were stripped.
  return endpos - pos;
}

unsigned int vtkBetterDataReader::StripSpacers(	string line, unsigned int pos,
						unsigned int& endpos,
						vector<char> spacers)
{
  endpos = pos;
  while ((endpos < line.size()) && vtkBetterDataReader::IsSpacer(line[endpos], spacers))
    {
    endpos++;
    }
  return endpos - pos;
}

void vtkBetterDataReader::ReadChar(char ch, string line, unsigned int pos, unsigned int& endpos)
{
  if (!vtkBetterDataReader::IsSpacer(ch))
    {
    vtkBetterDataReader::StripSpacers(line, pos, endpos);
    }
  else
    {
    endpos = pos;
    }

  assert( endpos < line.size() );
  //if ( endpos >= line.size() )
  //  {
  //  vtkErrorMacro(<<"endpos must be smaller than line.size()!");
  //  return;
  //  }

  assert( line[endpos] == ch );
  //if ( line[endpos] != ch )
  //  {
  //  vtkWarningMacro(<<"line[endpos] != ch.  Incorrect file!");
  //  }

  endpos++;
}

// Open a vtk data file. Returns zero if error.
int vtkBetterDataReader::OpenFile()
{ // copied from vtkDataReader::OpenVTKFile()
  // some code for supporting input strings was removed.

  vtkDebugMacro(<< "Opening file");

  if ( !this->FileName || (strlen(this->FileName) == 0))
    {
    vtkErrorMacro(<< "No file specified!");
    this->SetErrorCode( vtkErrorCode::NoFileNameError );
    return 0;
    }

  // first make sure the file exists, this prevents an empty file from
  // being created on older compilers
  //struct stat fs;
  //if (stat(this->FileName, &fs) != 0) 
  //  {
  //  vtkErrorMacro(<< "Unable to open file: "<< this->FileName);
  //  this->SetErrorCode( vtkErrorCode::CannotOpenFileError );
  //  return 0;
  //  }
  this->IS = new ifstream(this->FileName, ios::in);
  if (this->IS->fail())
    {
    vtkErrorMacro(<< "Unable to open file: "<< this->FileName);
    delete this->IS;
    this->IS = NULL;
    this->SetErrorCode( vtkErrorCode::CannotOpenFileError );
    return 0;
    }
  return 1;
  }

// Close a file.
void vtkBetterDataReader::CloseFile()
{ // copied from vtkDataReader::CloseVTKFile().
  vtkDebugMacro(<<"Closing file");
  if ( this->IS != NULL )
    {
    delete this->IS;
    }
  this->IS = NULL;
}

