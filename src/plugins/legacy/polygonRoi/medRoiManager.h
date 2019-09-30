/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <polygonRoiPluginExport.h>

#include <medAbstractRoi.h>
#include <medAbstractView.h>

class medRoiManagerPrivate;
class medSeriesOfRoi;

class POLYGONROIPLUGIN_EXPORT medRoiManager : public QObject
{
    Q_OBJECT

public:
    typedef QList<medAbstractRoi*> ListRois;
    typedef QList<medSeriesOfRoi*> ListOfSeriesOfRois;
    typedef QPair<unsigned int,unsigned int> PairInd;

    static medRoiManager *instance();
    static void destroy();
    void addRoi(medAbstractView*, medAbstractRoi*, QString);
    void deleteRoi(medAbstractView*, medAbstractRoi*, QString);
    QHash<medAbstractView*,ListOfSeriesOfRois*> * getSeriesOfRoi();
    QList<PairInd> * getSelectedRois();
    void addToUndoRedoStack(QUndoCommand* command);
    QUndoStack * getUndoStack();
    void undo();
    void redo();
    void copy();
    void paste();
    void setCurrentView(medAbstractView *view);
    void sortListOfRois(QString seriesName);

signals:
    void roiAdded();
    void roiDeleted();
    void allRoisDeleted();

public slots:
    void removeView();

protected:
    medRoiManager();
    ~medRoiManager();

protected:
    static medRoiManager *s_instance;

    friend class AddRoiCommand;
    friend class AddSeveralRoisCommand;
    friend class DeleteRoiCommand;
    friend class DeleteSeveralRoisCommand;

private:
    medRoiManagerPrivate *d;
    static bool sliceIndexLessThan(medAbstractRoi*, medAbstractRoi*);
};

class POLYGONROIPLUGIN_EXPORT medSeriesOfRoi
{
public:
    typedef QList<medAbstractRoi*> ListRois;
    typedef QPair<unsigned int,unsigned int> PairInd;

    medSeriesOfRoi(QString name,ListRois *rois):name(name),rois(rois){}
    virtual ~medSeriesOfRoi(){} // delete everything

    QString getName(){return name;}
    ListRois* getListOfRois(){return rois;}

    void Off();
    void On();

    virtual QString info();

    void select();
    void unselect();
    void computeStatistics();

private:
    QString name;
    ListRois* rois; // indices of roi in Series
};

class POLYGONROIPLUGIN_EXPORT CreationCommand : public QUndoCommand
{
public:
    CreationCommand(medAbstractView * view,medAbstractRoi * roi, QString seriesName, const QString &text)
        : m_view(view), m_roi(roi), m_seriesName(seriesName)
    {
        setText(text);
    }

    // add and remove from roimanager hash
    virtual void undo()
    {
        m_roi->forceInvisibilityOn();
        m_view->render();
    }

    virtual void redo()
    {
        m_roi->forceInvisibilityOff();
        m_view->render();
    }
private:
    medAbstractView *m_view;
    medAbstractRoi *m_roi;
    QString m_seriesName;
};

// The idea is simple
// Define Command for each special interaction with the HASH
// CreationCommand will become AddRoiCommand or AppendRoiCommand
// DeleteRoiCommand for deleting a single roi
// DeleteRoisCommand for example if the user want to get rid of all rois in series
// AddRoisCommand for example interpolate and other interaction that brings several rois at once

class POLYGONROIPLUGIN_EXPORT AddRoiCommand : public QUndoCommand
{
public:
    AddRoiCommand(medAbstractView * view,medAbstractRoi * roi, QString seriesName, const QString &text)
        : m_view(view),m_roi(roi),m_seriesName(seriesName) {setText(text); }

    virtual void undo()
    {
        medRoiManager::instance()->deleteRoi(m_view,m_roi, m_seriesName);
        m_roi->forceInvisibilityOn();
        m_view->render();
        medRoiManager::instance()->roiDeleted();
    };
    virtual void redo()
    {
        medRoiManager::instance()->addRoi(m_view,m_roi,m_seriesName);
        medRoiManager::instance()->sortListOfRois(m_seriesName);
        m_roi->forceInvisibilityOff();
        m_view->render();
        medRoiManager::instance()->roiAdded();
    };
private:
    medAbstractView *m_view;
    medAbstractRoi *m_roi;
    QString m_seriesName;
};

class POLYGONROIPLUGIN_EXPORT AddSeveralRoisCommand : public QUndoCommand
{
public:
    AddSeveralRoisCommand(medAbstractView * view,QList<medAbstractRoi*> list, QString seriesName, const QString &text)
        : m_view(view),m_ListRois(list),m_seriesName(seriesName) {setText(text); }

    virtual void undo()
    {
        for(int i=0; i<m_ListRois.size(); i++)
        {
            medAbstractRoi *m_roi = m_ListRois.at(i);
            medRoiManager::instance()->deleteRoi(m_view, m_roi, m_seriesName);
            m_roi->forceInvisibilityOn();
        }
        m_view->render();
        medRoiManager::instance()->roiDeleted();
    }

    virtual void redo()
    {
        for(int i=0; i<m_ListRois.size(); i++)
        {
            medAbstractRoi *m_roi = m_ListRois.at(i);
            medRoiManager::instance()->addRoi(m_view, m_roi, m_seriesName);
            m_roi->forceInvisibilityOff();
        }
        medRoiManager::instance()->sortListOfRois(m_seriesName);
        m_view->render();
        medRoiManager::instance()->roiAdded();
    }

private:
    medAbstractView *m_view;
    QList<medAbstractRoi*> m_ListRois;
    QString m_seriesName;
};

class POLYGONROIPLUGIN_EXPORT DeleteRoiCommand : public QUndoCommand
{
public:
    DeleteRoiCommand(medAbstractView * view,medAbstractRoi * roi, QString seriesName, const QString &text)
        : m_view(view), m_roi(roi), m_seriesName(seriesName)
    {
        setText(text);
    }

    virtual void undo()
    {
        medRoiManager::instance()->addRoi(m_view,m_roi,m_seriesName);
        medRoiManager::instance()->sortListOfRois(m_seriesName);
        m_roi->forceInvisibilityOff();
        m_view->render();
        medRoiManager::instance()->roiAdded();
    }

    virtual void redo()
    {
        medRoiManager::instance()->deleteRoi(m_view,m_roi, m_seriesName);
        m_roi->forceInvisibilityOn();
        m_view->render();
        medRoiManager::instance()->roiDeleted();
    }

private:
    medAbstractView *m_view;
    medAbstractRoi *m_roi;
    QString m_seriesName;
};

class POLYGONROIPLUGIN_EXPORT DeleteSeveralRoisCommand : public QUndoCommand // TODO : only one type of command needed use QList<medAbstractRoi> for a DeleteRoisCommand and it will solves everything same thing for addRois
{
public:
    DeleteSeveralRoisCommand(medAbstractView * view,QList<medAbstractRoi*> list, QString seriesName, const QString &text)
        : m_view(view),m_ListRois(list),m_seriesName(seriesName)
    {
        setText(text);
    }

    virtual void undo()
    {
        for(int i=0; i<m_ListRois.size(); i++)
        {
            medAbstractRoi *m_roi = m_ListRois.at(i);
            medRoiManager::instance()->addRoi(m_view,m_roi,m_seriesName);
            m_roi->forceInvisibilityOff();
        }
        medRoiManager::instance()->sortListOfRois(m_seriesName);
        m_view->render();
        medRoiManager::instance()->roiAdded();
    }

    virtual void redo()
    {
        for(int i=0; i<m_ListRois.size(); i++)
        {
            medAbstractRoi *m_roi = m_ListRois.at(i);
            medRoiManager::instance()->deleteRoi(m_view, m_roi, m_seriesName);
            m_roi->forceInvisibilityOn();
        }
        m_view->render();
        medRoiManager::instance()->roiDeleted();
    }

private:
    medAbstractView *m_view;
    QList<medAbstractRoi*> m_ListRois;
    QString m_seriesName;
};
