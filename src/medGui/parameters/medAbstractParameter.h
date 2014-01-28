/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include "medGuiExport.h"

class dtkAbstractObject;
class QLabel;

class medAbstractParameterPrivate;
class MEDGUI_EXPORT medAbstractParameter : public QObject
{
    Q_OBJECT

public:

    medAbstractParameter(QString name = "unknown parameter", QObject* parent = 0);
    virtual ~medAbstractParameter();

    QString name() const;
    QLabel* getLabel();

    /**
     * @brief getWidget Return a default widget.
     * @return QWidget*
     */
    virtual QWidget* getWidget() = 0;

    bool match(medAbstractParameter const *other);
    void show();
    void hide();

    void setToolTip(QString& tooltip);

protected:
    void addToInternWidgets(QWidget * widget);
    void blockInternWidgetsSignals(bool block) const;
protected slots:
    void removeFromInternWidgets(QWidget* widget);

private slots:
    void removeInternLabel();

private:
  medAbstractParameterPrivate *d; 
};

//--------------------------------------------------------------------------
//  medTestParameter

class medTestParameterPrivate;
/**
 * @brief The medTestParameter class is just a dummy implementaion of
 * medAbstractParameter to test its interface.
 */
class medTestParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medTestParameter(QString name = "unknown parameter");
    virtual~medTestParameter();

    /**
     * @brief getWidget Return a default widget.
     * @return QWidget*
     */
    virtual QWidget* getWidget();

private slots:
    void removeInternWidget();

private:
  medTestParameterPrivate *d;
};

//--------------------------------------------------------------------------
//  medAbstractTriggerParameter
class medAbstractTriggerParameter : public medAbstractParameter
{
    Q_OBJECT
public:
    medAbstractTriggerParameter(QString name = "unknown trigger parameter", QObject* parent = 0){}
public slots:
    virtual void trigger();

signals:
    void triggered();
};
//--------------------------------------------------------------------------
//  medAbstractStringParameter
class MEDGUI_EXPORT medAbstractStringParameter : public medAbstractParameter
{
    Q_OBJECT
public:
    medAbstractStringParameter(QString name = "unknown  string parameter", QObject* parent = 0){}
    virtual QString value() const;
public slots:
    virtual void setValue(QString value);
signals:
    void valueChanged(QString);
protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(QString value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;
protected:
    QString m_value;
};
//--------------------------------------------------------------------------
//  medAbstractIntParameter
class MEDGUI_EXPORT medAbstractIntParameter : public medAbstractParameter
{
    Q_OBJECT
public:
    medAbstractIntParameter(QString name = "unknown int parameter", QObject* parent = 0){}
    virtual int value() const;
public slots:
    virtual void setValue(int value);
protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(int value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;
signals:
    void valueChanged(int);

protected:
    int m_value;
};
//--------------------------------------------------------------------------
//  medAbstractDoubleParameter
class MEDGUI_EXPORT medAbstractDoubleParameter : public medAbstractParameter
{
    Q_OBJECT
public:
    medAbstractDoubleParameter(QString name = "unknown double parameter", QObject* parent = 0){}
    virtual double value() const;
public slots:
    virtual void setValue(double value);
protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(double value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;
signals:
    void valueChanged(double);
protected:
    double m_value;
};
//--------------------------------------------------------------------------
//  medAbstractBoolParameter
class MEDGUI_EXPORT medAbstractBoolParameter : public medAbstractParameter
{
    Q_OBJECT
public:
    medAbstractBoolParameter(QString name = "unknown bool parameter", QObject* parent = 0){}
    virtual bool value() const;
public slots:
    virtual void setValue(bool value);
protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(bool value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;

signals:
    void valueChanged(bool);

protected:
    bool m_value;
};

//--------------------------------------------------------------------------
//  medAbstractGroupParameter
class medAbstractGroupParameter : public medAbstractParameter
{
    Q_OBJECT
public:
    medAbstractGroupParameter(QString name = "unknown group parameter", QObject* parent = 0){}

    /**
     * @brief parametersCandidateToPool reimplement this methode to keep correct synchronisation in medParameterPool
     * @return A list of parameters that have to be linked when you had a
     * medAbstractGroupParameter to a medParameterPool.
     */
    virtual QList<medAbstractParameter*> parametersCandidateToPool() const = 0;
};
