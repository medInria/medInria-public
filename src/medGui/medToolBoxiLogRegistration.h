#ifndef MEDTOOLBOXILOGREGISTRATION_H
#define MEDTOOLBOXILOGREGISTRATION_H

#include <medGui/toolboxes/medToolBox.h>

class dtkAbstractView;
class dtkAbstractData;
class medToolBoxiLogRegistrationPrivate;

class MEDGUI_EXPORT medToolBoxiLogRegistration : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxiLogRegistration(QWidget *parent = 0);
    ~medToolBoxiLogRegistration(void);

    
	
public slots:


    
 protected:

    
    
 private:
    medToolBoxiLogRegistrationPrivate *d;
    
};

#endif MEDTOOLBOXILOGREGISTRATION_H