/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QWidget>

class medSettingsWidgetPrivate;

/**
 * @brief AbstractClass for all Settings widgets.
 *
 * All the pages in the medSettingsEditor must inherit from this class.
 *
*/
class MEDCORE_EXPORT medSettingsWidget : public QWidget
{
    Q_OBJECT
public:

    explicit medSettingsWidget(QWidget *parent = 0);

    virtual QString identifier() const = 0;
    virtual QString name() const = 0;
    /**
    * description - mandatory method to describe the plug-in
    * Should be used as the section name (QSettings) when using the setttingsmanager
    * @return   QString
    */
    virtual QString description() const = 0;


    QString tabName() const;

    /**
     * @brief Save the form into the QSettings.
     * First calls validate() and if validate() fails, emit a showError signal.
     *
     * @param void
     * @return bool true if saving worked, false otherwise.
    */
    virtual bool write() = 0;

public slots:
    /**
     * @brief Reads its data from the QSettings.
     *
    */
    virtual void read() = 0;

    virtual bool validate();

protected:
    void setTabName(QString section);

private:
    medSettingsWidgetPrivate *d; /**< TODO */
};

#define MED_SETTINGS_INTERFACE(_name,_desc) \
public:\
    static QString staticIdentifier() {return QString(staticMetaObject.className());}\
    static QString staticName() {return QString::fromUtf8(_name);}\
    static QString staticDescription() {return QString::fromUtf8(_desc);}\
    virtual QString identifier() const {return staticIdentifier();}\
    virtual QString name() const {return staticName();}\
    virtual QString description() const {return staticDescription();}
