/*=========================================================================
 
 medInria
 
 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.
 
 =========================================================================*/

#pragma once

#include "v3dViewPluginExport.h"

#include <v3dView.h>
#include <medVtkView.h>

#include <medAbstractViewInteractor.h>
#include <medParameter.h>

class V3DVIEWPLUGIN_EXPORT medAbstractVtkViewInteractor: public medAbstractViewInteractor
{
    Q_OBJECT
    
public:
    medAbstractVtkViewInteractor(){}
    virtual ~medAbstractVtkViewInteractor(){}
    
    virtual QStringList handled() const
    {
        return QStringList () << v3dView::s_identifier() << medVtkView::s_identifier();
    }

    virtual QList<medAbstractParameter *> getParameters(dtkAbstractData *data) const{return parameters.values(data);}


protected:
    QMultiHash<dtkAbstractData*, medAbstractParameter*> parameters;


public slots:
    virtual void setOpacity(dtkAbstractData * data, double opacity) = 0;
    virtual double opacity(dtkAbstractData * data) const = 0;

    virtual void setVisible(dtkAbstractData * data, bool visible) = 0;
    virtual bool isVisible(dtkAbstractData * data) const = 0;
};
