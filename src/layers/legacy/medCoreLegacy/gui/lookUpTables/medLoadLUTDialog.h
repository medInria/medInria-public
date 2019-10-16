/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QInputDialog>

#include <medCoreLegacyExport.h>

class MEDCORELEGACY_EXPORT medLoadLUTDialog : public QInputDialog
{
    Q_OBJECT

public:
    medLoadLUTDialog(const QStringList & titles, QWidget *parent = nullptr);
};


