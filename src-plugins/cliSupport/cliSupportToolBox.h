/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class medDataIndex;
class medAbstractView;
class medAbstractWorkspace;

class cliSupportToolBoxPrivate;

class cliSupportToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("CLI Modules",
                          "CommandLine plugin support using CTK implementation",
                          <<"CLI"<<"modules")
public:
     cliSupportToolBox(QWidget *parent, medAbstractWorkspace * workspace = 0);
     virtual ~cliSupportToolBox();

protected:
     void init();

public slots:
     virtual void update (medAbstractView *view);

protected slots:

     virtual void clear (void);

     void moduleSelected(int index);
     void runCurrentModule();

     void moduleFinished();
     void progressTextChanged(const QString & progressText);
     void progressValueChanged(int progressValue);

private:
    cliSupportToolBoxPrivate *d;
};
