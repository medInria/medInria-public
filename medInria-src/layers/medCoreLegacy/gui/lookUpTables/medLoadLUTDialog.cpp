/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medLoadLUTDialog.h>

medLoadLUTDialog::medLoadLUTDialog(const QStringList & titles, QWidget *parent) :
    QInputDialog(parent)
{
    setModal(Qt::WindowModal);
    setInputMode(QInputDialog::TextInput);
    setLabelText(tr("Choose a transfert Function:"));
    setComboBoxItems(titles);;
    setOkButtonText(tr("Choose"));
}
