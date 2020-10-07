#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>
#include <QtWidgets>

#include <medCoreLegacyExport.h>


class MEDCORELEGACY_EXPORT medToolBoxTab : public QTabWidget
{
    Q_OBJECT
    Q_PROPERTY(int height READ height WRITE setFixedHeight)

public:
     medToolBoxTab(QWidget *parent = nullptr);
    ~medToolBoxTab();
};
