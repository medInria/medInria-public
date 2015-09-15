#include "meditkRigidRegistration.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkRigidRegistration::meditkRigidRegistration(void)
{
    m_movingImage=0;
    m_fixedImage=0;
    m_transfo.setToIdentity();
}

meditkRigidRegistration::~meditkRigidRegistration(void)
{

}

void meditkRigidRegistration::setFixedImage(medAbstractImageData* image)
{
    m_fixedImage=image;
}

void meditkRigidRegistration::setMovingImage(medAbstractImageData* image)
{
    m_movingImage=image;
}

medAbstractImageData* meditkRigidRegistration::transformedImage(void) const
{
    return res;
}

QMatrix4x4 meditkRigidRegistration::transform() const
{
    return m_transfo;
}

void meditkRigidRegistration::run(void)
{
    if(!m_movingImage || !m_fixedImage)
    {
        qDebug()<<"images not set";
        return;
    }
    if( m_movingImage->identifier()!=m_fixedImage->identifier())
    {
        qDebug()<<"images don't match" << m_movingImage->identifier()<<" "<<m_fixedImage->identifier();
        return;
    }
    QString id=m_movingImage->identifier();
    qDebug()<<id;

    if ( id == "itkDataImageChar3" )
    {
        registration<char>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageUChar3" )
    {
        registration<unsigned char>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageShort3" )
    {
        registration<short>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageUShort3" )
    {
        registration<unsigned short>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageInt3" )
    {
        registration<int>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageUInt3" )
    {
        registration<unsigned int>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageLong3" )
    {
        registration<long>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id== "itkDataImageULong3" )
    {
        registration<unsigned long>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageFloat3" )
    {
        registration<float>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else if ( id == "itkDataImageDouble3" )
    {
        registration<double>(m_fixedImage,m_movingImage,&m_transfo);
    }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
        << id
        << ")";
        return;
    }

    res=m_movingImage;

}
