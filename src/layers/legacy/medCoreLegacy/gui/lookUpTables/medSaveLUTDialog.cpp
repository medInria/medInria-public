/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSaveLUTDialog.h>
#include <QWidget>

medSaveLUTDialog::medSaveLUTDialog(const QString & name, QWidget *parent) :
    QInputDialog(parent)
{
    setModal(Qt::WindowModal);
    setInputMode(QInputDialog::TextInput);
    setLabelText(tr("Enter a name for this table, or keep existing one"));
    setTextValue(name);
    setOkButtonText(tr("Save"));
}
