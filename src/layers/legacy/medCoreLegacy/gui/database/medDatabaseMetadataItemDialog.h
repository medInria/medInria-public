/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QDialog>

class medAbstractDatabaseItem;
class medDatabaseMetadataItemDialogPrivate;

/**
 * @brief Dialog displaying metadata from the selected data
 */
class medDatabaseMetadataItemDialog: public QDialog
{
    Q_OBJECT

public:
    medDatabaseMetadataItemDialog(QList<QString> keyList, QList<QVariant> metadataList, QWidget* parent);

    virtual ~medDatabaseMetadataItemDialog();

private:
    medDatabaseMetadataItemDialogPrivate *d;
};


