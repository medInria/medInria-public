#pragma once

#include <dtkCore>
#include <medCore.h>
#include <QMatrix4x4>

#include <medAbstractRigidRegistration.h>
#include "registration.h"

class meditkRigidRegistration : public medAbstractRigidRegistration
{
    Q_OBJECT

public:
     meditkRigidRegistration(void);
    ~meditkRigidRegistration(void);

public:
    virtual void setFixedImage (medAbstractImageData* image);
    virtual void setMovingImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* transformedImage(void) const;
    virtual QMatrix4x4 transform(void) const;

public:
    void run();
    
private:
    medAbstractImageData* m_fixedImage,*m_movingImage,*res;
    QMatrix4x4 m_transfo;
};


inline medAbstractRigidRegistration *meditkRigidRegistrationCreator(void)
{
    return new meditkRigidRegistration();
}
