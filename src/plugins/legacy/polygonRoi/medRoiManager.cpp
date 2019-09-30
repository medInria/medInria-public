/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medRoiManager.h>

#include <QTreeWidget>

class medRoiManagerPrivate
{
public:

    medRoiManagerPrivate(){}

    typedef QList<medAbstractRoi*> ListRois;
    typedef QList<medSeriesOfRoi*> ListOfSeriesOfRois;
    typedef QPair<unsigned int,unsigned int> PairInd;

    medAbstractView *currentView;

    QList<QTreeWidget*> listOfPages;

    QHash<medAbstractView*,ListOfSeriesOfRois*> *viewsRoisSeries;
    QPair<medAbstractView*,ListRois*> *copyList;

    QHash<medAbstractView*,QUndoStack*> *hashViewUndoStack;
};

medRoiManager *medRoiManager::instance()
{
    if(!s_instance)
    {
        s_instance = new medRoiManager;
    }
    return s_instance;
}

medRoiManager::medRoiManager() : d(new medRoiManagerPrivate)
{
    d->viewsRoisSeries = new QHash<medAbstractView*, ListOfSeriesOfRois *>();
    d->hashViewUndoStack = new QHash<medAbstractView*, QUndoStack*>();
    d->copyList = new QPair<medAbstractView*, ListRois*>();
    d->copyList->second = new ListRois();
    d->copyList->first = nullptr;
}

medRoiManager::~medRoiManager()
{
    delete d;
    d = nullptr;
}
void medRoiManager::destroy()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

void medRoiManager::setCurrentView(medAbstractView *view)
{
    d->currentView = view;
}

void medRoiManager::addRoi(medAbstractView *view, medAbstractRoi *roi, QString seriesName)
{
    if (view)
    {
        d->currentView = view;
    }
    bool added = false;
    if (!d->viewsRoisSeries->contains(view))
    {
        ListOfSeriesOfRois *series = new ListOfSeriesOfRois();
        d->viewsRoisSeries->insert(view, series);
        connect(view, SIGNAL(closed()), this, SLOT(removeView()));
    }
    else
    {
        ListOfSeriesOfRois *series = d->viewsRoisSeries->value(view);

        for(int i = 0; i < series->size(); i++)
        {
            if (series->at(i)->getName() == seriesName) //same type of ROI
            {
                added = true;
                series->at(i)->getListOfRois()->append(roi);
                break;
            }
        }
    }
    if (!added)
    {
        ListRois *listRois = new ListRois();
        listRois->append(roi);
        d->viewsRoisSeries->value(view)->append(new medSeriesOfRoi(seriesName,listRois));
    }
}

void medRoiManager::sortListOfRois(QString seriesName)
{
    ListOfSeriesOfRois *series = d->viewsRoisSeries->value(d->currentView);
    if(series->size() > 0)
    {
        ListRois *listOfRois = nullptr;
        for(int i = 0; i < series->size(); i++)
        {
            if (series->at(i)->getName() == seriesName) //same type of ROI
            {
                listOfRois = series->at(i)->getListOfRois();
                break;
            }
        }
        qSort(listOfRois->begin(), listOfRois->end(), sliceIndexLessThan);
    }
}

bool medRoiManager::sliceIndexLessThan(medAbstractRoi *a, medAbstractRoi *b)
{
    return a->getIdSlice() < b->getIdSlice();
}

void medRoiManager::deleteRoi(medAbstractView *view, medAbstractRoi *roi, QString seriesName)
{
    if (view)
    {
        d->currentView = view;
    }
    if (!d->viewsRoisSeries->contains(view))
    {
        return;
    }
    ListOfSeriesOfRois *series = d->viewsRoisSeries->value(view);
    for(int i = 0; i < series->size(); i++)
    {
        if (series->at(i)->getName() == seriesName) //same type of ROI
        {
            series->at(i)->getListOfRois()->removeAll(roi);
        }
    }

    // if no more ROI in the serie
    if (series->at(0)->getListOfRois()->count() == 0)
    {
        emit medRoiManager::instance()->allRoisDeleted();
    }
}

void medRoiManager::addToUndoRedoStack(QUndoCommand *command)
{
    if (!d->hashViewUndoStack->contains(d->currentView))
    {
        d->hashViewUndoStack->insert(d->currentView, new QUndoStack());
    }
    d->hashViewUndoStack->value(d->currentView)->push(command);
}

QUndoStack * medRoiManager::getUndoStack()
{
     if (d->hashViewUndoStack->contains(d->currentView))
     {
        return d->hashViewUndoStack->value(d->currentView);
     }

     return nullptr;
}

void medRoiManager::undo()
{
    if (d->hashViewUndoStack->contains(d->currentView))
    {
        d->hashViewUndoStack->value(d->currentView)->undo();
    }
}

void medRoiManager::redo()
{
    if (d->hashViewUndoStack->contains(d->currentView))
    {
        d->hashViewUndoStack->value(d->currentView)->redo();
    }
}

void medRoiManager::copy() // copy everything for the time being dont take into consideration selection
{
    if (!d->currentView || !d->viewsRoisSeries->contains(d->currentView))
    {
        return;
    }

    d->copyList->second->clear();
    d->copyList->first = d->currentView;

    ListOfSeriesOfRois *series = d->viewsRoisSeries->value(d->currentView);
    for(int i = 0; i < series->size(); i++)
    {
        ListRois *list = series->at(i)->getListOfRois();
        for(int j = 0; j < list->size(); j++)
            if (list->at(j)->isVisible())
            {
                if (list->at(j)->copyROI(d->currentView))
                    d->copyList->second->append(list->at(j));
            }
    }
}

void medRoiManager::paste()
{
    if (!d->currentView || d->copyList->first != d->currentView) // we cant copy roi from different view for the time being
    {
        return;
    }
    for(int i = 0; i < d->copyList->second->size(); i++)
    {
        AddRoiCommand *command = new AddRoiCommand(d->currentView,d->copyList->second->at(i)->getCopy(d->currentView),"Polygon rois","NewRoi");
        addToUndoRedoStack(command);
    }
}

QHash<medAbstractView*,medRoiManager::ListOfSeriesOfRois*> * medRoiManager::getSeriesOfRoi()
{
    return d->viewsRoisSeries;
}

QList<QPair<unsigned int,unsigned int> > * medRoiManager::getSelectedRois()
{
    return nullptr;
}

void medRoiManager::removeView()
{
    medAbstractView *view = qobject_cast<medAbstractView*>(QObject::sender());
    if (view)
    {
        delete d->viewsRoisSeries->take(view);
        delete d->hashViewUndoStack->take(view);
    }
}

medRoiManager *medRoiManager::s_instance = nullptr;


/*******************************MEDSERIESOFROI*/

void medSeriesOfRoi::Off()
{
    for(int i = 0; i < rois->size(); i++)
    {
        rois->at(i)->Off();
    }
}

void medSeriesOfRoi::On()
{
    for(int i = 0; i < rois->size(); i++)
    {
        rois->at(i)->On();
    }
}

QString medSeriesOfRoi::info()
{
    return QString();
}

void medSeriesOfRoi::select()
{
    for(int i = 0; i < rois->size(); i++)
    {
        rois->at(i)->select();
    }
}
void medSeriesOfRoi::unselect()
{
    for(int i = 0; i < rois->size(); i++)
    {
        rois->at(i)->unselect();
    }
}

void medSeriesOfRoi::computeStatistics()
{
    // TODO
}
