/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QDialog>

class medEmptyDbWarningPrivate;


/**
 * @brief Shows a warning dialog box telling the user there is no data in the DB.
 *
 * Also writes in the settings whether the user wants to see this message again.
 *
 * The setting is in the section "system", key: "showEmptyDbWarning".
 *
 * The class doesn't check for this value before showing,
 * this has to be done before trying to call this dialog.
 *
 */
class medEmptyDbWarning : public QDialog
{
    Q_OBJECT
public:

    explicit medEmptyDbWarning(QWidget *parent = 0);

    virtual ~medEmptyDbWarning();
signals:

public slots:
    /**
     * @brief accepts and sets the show setting. the dialog as the parent method
     *
     */
    void accept();
    void reject();

private:
    medEmptyDbWarningPrivate* d; /**< TODO */
};


