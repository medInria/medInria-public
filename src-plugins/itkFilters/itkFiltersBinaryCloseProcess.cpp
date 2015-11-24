/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersBinaryCloseProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>

#include <medMetaDataKeys.h>

#include <itkFiltersBinaryCloseProcess_p.h>

//-------------------------------------------------------------------------------------------

itkFiltersBinaryCloseProcess::itkFiltersBinaryCloseProcess(itkFiltersBinaryCloseProcess *parent)
    : itkMorphologicalFiltersProcessBase(*new itkFiltersBinaryCloseProcessPrivate(this), parent)
{
    DTK_D(itkFiltersBinaryCloseProcess);

    d->filter = this;
    d->description = tr("ITK Binary Close filter");
}


itkFiltersBinaryCloseProcess::itkFiltersBinaryCloseProcess(const itkFiltersBinaryCloseProcess& other)
    : itkMorphologicalFiltersProcessBase(*new itkFiltersBinaryCloseProcessPrivate(*other.d_func()), other)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersBinaryCloseProcess::~itkFiltersBinaryCloseProcess( void )
{
}

//-------------------------------------------------------------------------------------------

bool itkFiltersBinaryCloseProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkFiltersBinaryCloseProcess", createitkFiltersBinaryCloseProcess);
}

//-------------------------------------------------------------------------------------------

int itkFiltersBinaryCloseProcess::update()
{
    DTK_D(itkFiltersBinaryCloseProcess);

    if (d->input)
    {
        QString id = d->input->identifier();

        if ( id == "itkDataImageChar3" )
        {
            d->update<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            d->update<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            d->update<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            d->update<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            d->update<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            d->update<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            d->update<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            d->update<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            d->update<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            d->update<double>();
        }
        else
        {
            qDebug() << "Error : pixel type not yet implemented ("
                     << id
                     << ")";
            return DTK_FAILURE;
        }
        return DTK_SUCCEED;
    }
    return DTK_FAILURE;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createitkFiltersBinaryCloseProcess ( void )
{
    return new itkFiltersBinaryCloseProcess;
}
