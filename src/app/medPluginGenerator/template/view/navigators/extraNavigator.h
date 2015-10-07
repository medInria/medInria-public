/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractNavigator.h>

#include <%1PluginExport.h>

class %1Private;
    
class %2PLUGIN_EXPORT %1 : public medAbstractNavigator
{
    Q_OBJECT
    
public:
    %1(medAbstractView* parent);
    virtual ~%1();
    
public:
    virtual QString identifier() const;
    virtual QString description() const;
    virtual QStringList handled(void) const;

    static bool registered();

    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();

protected:
    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();

private:
    %1Private *d;
};



