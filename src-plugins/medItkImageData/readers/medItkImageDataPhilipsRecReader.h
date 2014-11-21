/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkImageDataReaderBase.h>
#include <medItkImageExport.h>
#include <itkImage.h>

class MEDITKIMAGEPLUGIN_EXPORT medItkImageDataPhilipsRecReader: public medItkImageDataReaderBase {
public:
    medItkImageDataPhilipsRecReader();
    virtual ~medItkImageDataPhilipsRecReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    QStringList handled() const;

    static QStringList s_handled();

    static bool registered();

    /**
     * @brief Reads File.
     *
     * Overrides parent method. Calls the parent method and then applies
     * a fix for the directions and origin of the image.
     * Itk is doing it wrong here.
     *
     * @param path
     */
    virtual bool read (const QString& path);

private:

    static const char ID[];
    static dtkAbstractDataReader* create();

    typedef itk::Image<float,3> FloatImageType;
    template <unsigned dim,typename T>
    /**
     * @brief Corrects the origin and directions on a loaded file.
     *
     * Requires the data to be already loaded using the io and the base class
     * read() method.
     *
     * @param origin
     * @param direction
     */
    void applyCorrection(FloatImageType::PointType origin,
                    FloatImageType::DirectionType direction);
    FloatImageType::PointType ExtractPARRECImageOrigin (
            FloatImageType::DirectionType direction);
    FloatImageType::DirectionType ExtractPARRECImageOrientation ();
};


