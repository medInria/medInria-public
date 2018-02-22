/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1.h>

#include <dtkCore/dtkAbstractDataFactory.h>

// /////////////////////////////////////////////////////////////////
// %1Private
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

static QString s_identifier() {
    return "%1";
}

static QStringList s_handled() {
    return QStringList();

    /* TODO: return the types handled by your writer, e.g.
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
                          << "itkDataImageUChar3" << "itkDataImageUChar4"
                          << "itkDataImageShort3" << "itkDataImageShort4"
                          << "itkDataImageUShort3" << "itkDataImageUShort4"
                          << "itkDataImageInt3" << "itkDataImageInt4"
                          << "itkDataImageUInt3" << "itkDataImageUInt4"
                          << "itkDataImageLong3" << "itkDataImageLong4"
                          << "itkDataImageULong3" << "itkDataImageULong4"
                          << "itkDataImageFloat3" << "itkDataImageFloat4"
                          << "itkDataImageDouble3" << "itkDataImageDouble4"
                          << "itkDataImageRGB3" << "itkDataImageRGBA3"; */
}

%1::%1() : dtkAbstractDataWriter(), d(new %1Private)
{
    
}

%1::~%1()
{
    
}

bool %1::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataWriterType(s_identifier(), s_handled(), create);
}

QString %1::description() const
{
    return "%1";
}

QString %1::identifier() const
{
    return s_identifier();
}

QStringList %1::handled() const
{
    return s_handled();
}

QStringList %1::supportedFileExtensions() const
{
    // TODO: add your supported files extensions here
    return QStringList();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *create()
{
    return new %1;
}
