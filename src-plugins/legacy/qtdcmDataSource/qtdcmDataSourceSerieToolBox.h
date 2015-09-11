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

class qtdcmDataSourceSerieToolBoxPrivate;

class qtdcmDataSourceSerieToolBox : public medToolBox
{
    Q_OBJECT
public:
    qtdcmDataSourceSerieToolBox ( QWidget* parent = 0 );
    ~qtdcmDataSourceSerieToolBox();

    QtDcmPreviewWidget * getPreviewWidget();
    QtDcmSerieInfoWidget * getSerieInfoWidget();
    QtDcmImportWidget * getImportWidget();

private:
    qtdcmDataSourceSerieToolBoxPrivate * d;
};


