#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <QDomDocument>
#include <QDomElement>
#include <QObject>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <medCoreExport.h>

class QLabel;

class medAbstractParameterLPrivate;
class MEDCORE_EXPORT medAbstractParameterL : public QObject
{
    Q_OBJECT

public:

    medAbstractParameterL(QString name = "Unknown parameter", QObject* parent = nullptr);
    virtual ~medAbstractParameterL();

    QString name() const;
    void setName(QString &name);
    QLabel* getLabel();

    /**
     * @brief getWidget Return a default widget.
     * @return QWidget*
     */
    virtual QWidget* getWidget() = 0;

    virtual void trigger() = 0;

    bool match(medAbstractParameterL const *other);
    void show();
    void hide();

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);

    void setToolTip(QString tooltip);

protected:
    void addToInternWidgets(QWidget * widget);
    void blockInternWidgetsSignals(bool block) const;

protected slots:
    void removeFromInternWidgets(QWidget* widget);

private slots:
    void removeInternLabel();

signals:
    void aboutToBeDestroyed();

private:
  medAbstractParameterLPrivate *d;
};

//--------------------------------------------------------------------------
//  medAbstractTriggerParameterL
class MEDCORE_EXPORT medAbstractTriggerParameterL : public medAbstractParameterL
{
    Q_OBJECT
public:
    medAbstractTriggerParameterL(QString name = "Unknown trigger parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
public slots:
    virtual void trigger();

signals:
    void triggered();
};

//--------------------------------------------------------------------------
//  medAbstractStringParameterL
class MEDCORE_EXPORT medAbstractStringParameterL : public medAbstractParameterL
{
    Q_OBJECT
public:
    medAbstractStringParameterL(QString name = "Unknown string parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
    virtual QString value() const;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static QString fromString(QString value);

public slots:
    virtual void setValue(QString value);
    virtual void trigger();

signals:
    void valueChanged(QString);

protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(QString value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;

    QString m_value;
};

//--------------------------------------------------------------------------
//  medAbstractIntParameterL
class MEDCORE_EXPORT medAbstractIntParameterL : public medAbstractParameterL
{
    Q_OBJECT
public:
    medAbstractIntParameterL(QString name = "Unknown int parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
    virtual int value() const;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static int fromString(QString value);

public slots:
    virtual void setValue(int value);
    virtual void trigger();

signals:
    void valueChanged(int);

protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(int value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;

    int m_value;
};

//--------------------------------------------------------------------------
//  medAbstractDoubleParameterL
class MEDCORE_EXPORT medAbstractDoubleParameterL : public medAbstractParameterL
{
    Q_OBJECT
public:
    medAbstractDoubleParameterL(QString name = "Unknown double parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
    virtual double value() const;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static double fromString(QString value);

public slots:
    virtual void setValue(double value);
    virtual void trigger();

signals:
    void valueChanged(double);

protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(double value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;

    double m_value;
};

//--------------------------------------------------------------------------
//  medAbstractBoolParameterL
class MEDCORE_EXPORT medAbstractBoolParameterL : public medAbstractParameterL
{
    Q_OBJECT
public:
    medAbstractBoolParameterL(QString name = "Unknown bool parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
    virtual bool value() const;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static bool fromString(QString value);

public slots:
    virtual void setValue(bool value);
    virtual void trigger();

signals:
    void valueChanged(bool);

protected:
    /**
     * @brief updateInternWigets
     * this methode is called by setValue(bool value) update the intern widget
     * of your medParameter implementaion
     */
    virtual void updateInternWigets() = 0;

    bool m_value;
};

//--------------------------------------------------------------------------
//  medAbstractGroupParameterL
class MEDCORE_EXPORT medAbstractGroupParameterL : public medAbstractParameterL
{
    Q_OBJECT
public:
    medAbstractGroupParameterL(QString name = "Unknown group parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}

    /**
     * @brief parametersCandidateToPool reimplement this methode to keep correct synchronisation in medParameterPoolL
     * @return A list of parameters that have to be linked when you had a
     * medAbstractGroupParameterL to a medParameterPoolL.
     */
    virtual QList<medAbstractParameterL*> parametersCandidateToPool() const = 0;
};

//--------------------------------------------------------------------------
//  medAbstractVector2DParameterL

class QVector2D;

class MEDCORE_EXPORT medAbstractVector2DParameterL : public medAbstractParameterL
{
    Q_OBJECT

public:
    medAbstractVector2DParameterL(QString name = "Unknown Vector 2D parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
    virtual ~medAbstractVector2DParameterL(){}

    virtual QVector2D value() const;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static QVector2D fromString(QString value);

public slots:
    virtual void setValue(const QVector2D &);
    virtual void trigger();

protected:
    virtual void updateInternWigets() = 0;

signals:
    void valueChanged(const QVector2D & );

protected:
    QVector2D m_value;

};

//--------------------------------------------------------------------------
//  medAbstractVector3DParameterL

class QVector3D;

class MEDCORE_EXPORT medAbstractVector3DParameterL : public medAbstractParameterL
{
    Q_OBJECT

public:
    medAbstractVector3DParameterL(QString name = "Unknown Vector 3D parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
    virtual ~medAbstractVector3DParameterL(){}

    virtual QVector3D value() const;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static QVector3D fromString(QString value);

public slots:
    virtual void setValue(const QVector3D &);
    virtual void trigger();

protected:
    virtual void updateInternWigets() = 0;

signals:
    void valueChanged(const QVector3D & );

protected:
    QVector3D m_value;

};

//--------------------------------------------------------------------------
//  medAbstractVector4DParameterL

class QVector4D;

class MEDCORE_EXPORT medAbstractVector4DParameterL : public medAbstractParameterL
{
    Q_OBJECT

public:
    medAbstractVector4DParameterL(QString name = "Unknown Vector 4D parameter", QObject* parent = nullptr):
    medAbstractParameterL(name, parent){}
    virtual ~medAbstractVector4DParameterL(){}

    virtual QVector4D value() const;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);
    static QVector4D fromString(QString value);

public slots:
    virtual void setValue(const QVector4D &);
    virtual void trigger();

protected:
    virtual void updateInternWigets() = 0;

signals:
    void valueChanged(const QVector4D & );

protected:
    QVector4D m_value;

};
