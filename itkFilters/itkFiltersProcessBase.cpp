#include "itkFiltersProcessBase.h"

#include "itkFiltersProcessBase_p.h"

itkFiltersProcessBase::itkFiltersProcessBase(itkFiltersProcessBase *parent) 
    : dtkAbstractProcess(*new itkFiltersProcessBasePrivate(this), parent)
{
    
}


itkFiltersProcessBase::itkFiltersProcessBase(const itkFiltersProcessBase& other) 
    : dtkAbstractProcess(*new itkFiltersProcessBasePrivate(*other.d_func()), other)
{
    
}

itkFiltersProcessBase::~itkFiltersProcessBase()
{
    
}

void itkFiltersProcessBase::emitProgress(int progress)
{
    emit progressed(progress);
}

//int itkFiltersProcessBase::update ( void )
//{
//    DTK_D(itkFiltersProcessBase);
    
//    if ( !d->input )
//        return -1;

//    QString id = d->input->identifier();

//    qDebug() << "itkFilters, update : " << id;

//    if ( id == "itkDataImageChar3" )
//    {
//        d->update<char>();
//    }
//    else if ( id == "itkDataImageUChar3" )
//    {
//        d->update<unsigned char>();
//    }
//    else if ( id == "itkDataImageShort3" )
//    {
//        d->update<short>();
//    }
//    else if ( id == "itkDataImageUShort3" )
//    {
//        d->update<unsigned short>();
//    }
//    else if ( id == "itkDataImageInt3" )
//    {
//        d->update<int>();
//    }
//    else if ( id == "itkDataImageUInt3" )
//    {
//        d->update<unsigned int>();
//    }
//    else if ( id == "itkDataImageLong3" )
//    {
//        d->update<long>();
//    }
//    else if ( id== "itkDataImageULong3" )
//    {
//        d->update<unsigned long>();
//    }
//    else if ( id == "itkDataImageFloat3" )
//    {
//        d->update<float>();
//    }
//    else if ( id == "itkDataImageDouble3" )
//    {
//        d->update<double>();
//    }
//    else
//    {
//        qDebug() << "Error : pixel type not yet implemented ("
//        << id
//        << ")";
//        return -1;
//    }

//    return EXIT_SUCCESS;
//}
