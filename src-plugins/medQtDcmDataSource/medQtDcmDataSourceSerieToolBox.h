/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class QtDcmPreviewWidget;
class QtDcmSerieInfoWidget;
class QtDcmImportWidget;

class medQtDcmDataSourceSerieToolBoxPrivate;

class medQtDcmDataSourceSerieToolBox : public medToolBox
{
    Q_OBJECT
public:
    medQtDcmDataSourceSerieToolBox ( QWidget* parent = 0 );
    ~medQtDcmDataSourceSerieToolBox();

    QtDcmPreviewWidget * getPreviewWidget();
    QtDcmSerieInfoWidget * getSerieInfoWidget();
    QtDcmImportWidget * getImportWidget();

private:
    medQtDcmDataSourceSerieToolBoxPrivate * d;
};


