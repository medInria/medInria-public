/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <registrationFactory.h>

// /////////////////////////////////////////////////////////////////
// registrationFactoryPrivate
// /////////////////////////////////////////////////////////////////

class registrationFactoryPrivate
{
    public:
        itk::ImageRegistrationFactory<registrationFactory::RegImageType>::Pointer m_Factory;
};

// /////////////////////////////////////////////////////////////////
// registrationFactory
// /////////////////////////////////////////////////////////////////

registrationFactory * registrationFactory::instance(){
    if(!s_instance)
        s_instance = new registrationFactory();

    return s_instance;
}

registrationFactory::registrationFactory( void ): d(new registrationFactoryPrivate()){d->m_Factory = itk::ImageRegistrationFactory<RegImageType>::New();}


registrationFactory::~registrationFactory( void ){
    delete d;
    d = NULL;
}

void registrationFactory::reset()
{
    if (getGeneralTransform()->GetNumberOfTransformsInStack()>0)
    {
        d->m_Factory->Reset();
        emit transformationStackReset();
    }
}

void registrationFactory::setItkRegistrationFactory(itk::ImageRegistrationFactory<RegImageType>::Pointer registrationFactory){
    d->m_Factory = registrationFactory;
}

itk::ImageRegistrationFactory<registrationFactory::RegImageType>::Pointer registrationFactory::getItkRegistrationFactory(){
    return d->m_Factory;
}

itk::GeneralTransform<double,3>::Pointer registrationFactory::getGeneralTransform(){
    return d->m_Factory->GetGeneralTransform();
}
    
unsigned int registrationFactory::addTransformation(itk::Transform<double,3,3>::Pointer arg,QString methodParameters){
    int i= -1;
    i = getGeneralTransform()->InsertTransform(static_cast<itk::Transform<double,3,3>::ConstPointer>(arg));
    if (i!=-1)
        emit transformationAdded(i,methodParameters);
    return i;
}

registrationFactory *registrationFactory::s_instance = NULL;



