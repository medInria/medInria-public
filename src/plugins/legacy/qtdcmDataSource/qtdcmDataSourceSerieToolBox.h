#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medToolBox.h>

class QtDcmPreviewWidget;
class QtDcmSerieInfoWidget;
class QtDcmImportWidget;

class qtdcmDataSourceSerieToolBoxPrivate;

class qtdcmDataSourceSerieToolBox : public medToolBox
{
    Q_OBJECT

public:
    qtdcmDataSourceSerieToolBox ( QWidget* parent = nullptr );
    ~qtdcmDataSourceSerieToolBox();

    QtDcmPreviewWidget * getPreviewWidget();
    QtDcmSerieInfoWidget * getSerieInfoWidget();
    QtDcmImportWidget * getImportWidget();

private:
    qtdcmDataSourceSerieToolBoxPrivate * d;
};
