#pragma once
/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: mipsInrimageImageIO.h,v $
Language:  C++
Date:      $Date: 2005/03/23 16:33:27 $
Version:   $Revision: 1.6 $

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/**
 * \file   mipsInrimageImageIO.h
 *         Much of the code for this file reader/writer was taken from
 *         the University of Iowa Imaging Group library with the
 *         permission of the authors, Milan Sonka, Joseph Reinhardt,
 *         Ryan Long, Hans Johnson, Gary Christensen, and others.
 *         The specification for this file format is taken from the
 *         web site http://www.mayo.edu/bir/PDF/ANALYZE75.pdf.
 * \author Gregoire Malandain
 *         INRIA 2003
 * \brief TO BE CAHNGED
 *        This file was written as a modification to the itkMetaImageIO
 *        as a new method for reading in files from the Inrimage 7.5 specification.
 *        An Inrimage 7.5 specification consists of 2 files.  The first file is has a .hdr extension
 *        that is the header information containing specific information about the
 *        image dimensions, and other information concerning the data.
 *        The second file has a .img file extenstion and it contains the raw image data.
 */

#include "itkImageIOBase.h"
#include <medImageIOExport.h>

#include <itkImageIOBase.h>
#include <itkMetaDataObject.h>
#include <itk_zlib.h>

namespace itk //TODO no namespace please ! (and no class name prifixed with other librarie pefix)
{
    /**
     * \author Gregoire Malandain
     * \brief Class that defines how to read Inrimage 4 file format.
     * */
    class MEDIMAGEIO_EXPORT InrimageImageIO : public ImageIOBase
    {
    public:
        /** Standard class typedefs. */
        typedef InrimageImageIO    Self;
        typedef ImageIOBase        Superclass;
        typedef SmartPointer<Self> Pointer;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro(InrimageImageIO, Superclass);

        /*-------- This part of the interfaces deals with reading data. ----- */

        /** Determine if the file can be read with this ImageIO implementation.
         * \author Gregoire Malandain
         * \param FileNameToRead The name of the file to test for reading.
         * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
         * \return Returns true if this ImageIO can read the file specified.
         */
        virtual bool CanReadFile(const char* FileNameToRead);

        /** Set the spacing and dimension information for the set filename. */
        virtual void ReadImageInformation();

        /** Convert to type_info */
        const std::type_info& ConvertToTypeInfo(IOPixelType) const;

        /** Reads the data from disk into the memory buffer provided. */
        virtual void Read(void* buffer);

        /** Compute the size (in bytes) of the components of a pixel. For
         * example, and RGB pixel of unsigned char would have a
         * component size of 1 byte. NO MORE USEFUL FOR ITK > 1.8*/
         //virtual unsigned int GetComponentSize() const;

         /*-------- This part of the interfaces deals with writing data. ----- */

         /** Determine if the file can be written with this ImageIO implementation.
          * \param FileNameToWrite The name of the file to test for writing.
          * \author Gregoire Malandain
          * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
          * \return Returns true if this ImageIO can write the file specified.
          */
        virtual bool CanWriteFile(const char * FileNameToWrite);

        /** Set the spacing and dimension information for the set filename. */
        virtual void WriteImageInformation();

        /** Writes the data to disk from the memory buffer provided. Make sure
         * that the IORegions has been set properly. */
        virtual void Write(const void* buffer);


    protected:
        InrimageImageIO();
        ~InrimageImageIO();
        void PrintSelf(std::ostream& os, Indent indent) const;

    private:
        InrimageImageIO(const Self&); //purposely not implemented
        void operator=(const Self&); //purposely not implemented
        void SwapBytesIfNecessary(void * buffer, unsigned long numberOfPixels);

        gzFile m_file;

        /**  All of the information read in from the header file */
        unsigned int m_NumberBlocksInHeader;
        std::string m_header;
    };
}