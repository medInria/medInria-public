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
 * vtkBetterDataReader.h
 *
 * 2003-02-26	Tim Peeters
 * - Initial version (in DNAVis)
 *
 * 2003-05-08	Tim Peeters
 * - Rewritten and changed some functions.
 * - Added functionality that used to be in Loader
 *
 * 2003-07-22	Tim Peeters
 * - Added functions with a spacers vector as an argument.
 *
 * 2005-02-10	Tim Peeters
 * - Ported from DNAVis to be subclassed as a VTK class.
 * - Renamed class from FileReader to vtkBetterDataReader.
 * - Removed functionality already in vtkDataReader.
 * - Changed coding standards to VTK coding standards.
 *
 * 2005-05-09	Tim Peeters
 * - Made some previously protected methods public in order to be able to use
 *   this class in vtkBMIAShaderProgram::ReadShaderFromFile(char* filename) without
 *   subclassing it.
 * - Removed some old comments.
 *
 * 2005-05-17	Tim Peeters
 * - Made it a subclass of vtkSource instead of vtkStructuredPointsSource to be
 *   more general.
 * - Changed included header files.
 *
 * 2007-07-21	Paulo Rodrigues
 * - added ReadDouble
 */

#pragma once

#include "vtkHelpersExport.h"

#include <vtkSource.h>

#include <string>
#include <vector>

using namespace std;

/**
 * Subclass of vtkDataReader that adds extra functionality for the handling
 * of ASCII-formatted input files. Note: the name may be misleading. This class
 * is perhaps not really better ;) Just more convenient for me at the time.
 */
class VTK_HELPERS_EXPORT vtkBetterDataReader : public vtkSource {
public:
    static vtkBetterDataReader *New();
    
    /**
     * Reads a word/int/float/quoted string on line starting at pos.
     * Spacers before the word/int/float/quoted string are automatically stripped.
     */
    static string ReadWord(string line, unsigned int pos, unsigned int& endpos);

    /**
     * Same as ReadWord(string, unsigned int, unsigned int&), but with all elements of
     * spacers as spacers instead of just space and tab.
     */
    static string ReadWord(string line, unsigned int pos, unsigned int& endpos, vector<char> spacers);

    static int ReadInt(string line, unsigned int pos, unsigned int& endpos);
    static float ReadFloat(string line, unsigned int pos, unsigned int& endpos);
	static double ReadDouble(string line, unsigned int pos, unsigned int& endpos);
    static string ReadQuotedString(string line, unsigned int pos, unsigned int& endpos);

    /**
     * Reads the line until ch is encountered.
     */
    static void ReadChar(char ch, string line, unsigned int pos, unsigned int& endpos);

    /**
     * Strips spacers (spaces and tabs) from a line.
     */
    static unsigned int StripSpacers(string line, unsigned int pos, unsigned int& endpos);
    static unsigned int StripSpacers(string line, unsigned int pos, unsigned int& endpos,
					vector<char> spacers);

    static bool IsSpacer(char ch);
    static vector<char> GetStandardSpacers();

    /**
     * Same as IsSpacer(char), but with spacers not space and tab but the elements of spacer.
     *
     * @return ch is an element of the spacer vector.
     */
    static bool IsSpacer(char ch, vector<char> spacer);

    // Description:
    // Specify file name of vtk data file to read.
    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);

    /**
     * Open or close the file.
     */
    int OpenFile();
    void CloseFile();

    /**
     * Read the next line from the input file into CurrentLine.
     * Returns false if something goes wrong (e.g. EOF).
     */
    bool NextLine();

    /**
     * Returns the current line in the file as a string that was read
     * by nextLine(). Don't use before calling nextLine().
     */
    string CurrentLine;

    /**
     * Returns the number of the last read line, where 0 means no line has
     * been read yet and the first line has number 1.
     *
     * @return the number of the last read line.
     */
    vtkGetMacro(CurrentLineNumber, int);

    /**
     * Set CurrentLineNumber to 0.
     */
    void ResetCurrentLineNumber();

protected:
    vtkBetterDataReader();
    ~vtkBetterDataReader();

    char* FileName;

    istream *IS;

private:

    /**
     * The number of lines read.
     */
    int CurrentLineNumber;
};


