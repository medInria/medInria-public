#include "medDatabaseDataSource.h"

#include <medCore/medDataManager.h>

#include <medSql/medDatabasePreview.h>
#include <medSql/medDatabaseModel.h>
#include <medSql/medDatabaseView.h>
#include <medSql/medDatabaseExporter.h>

class medDatabaseDataSourcePrivate
{
public:
    QWidget* database_widget;
    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;
   
};

medDatabaseDataSource::medDatabaseDataSource( QWidget* parent /*= 0*/ ): medAbstractDataSource(parent), d(new medDatabaseDataSourcePrivate)
{
    d->preview = new medDatabasePreview(parent);

    d->model = new medDatabaseModel;

    d->view = new medDatabaseView(parent);
    d->view->setModel(d->model);


    d->database_widget = new QWidget(parent);

    QVBoxLayout *database_layout = new QVBoxLayout(d->database_widget);
    database_layout->setContentsMargins(0, 0, 0, 0);
    database_layout->setSpacing(0);
    database_layout->addWidget(d->view);
    database_layout->addWidget(d->preview);

    connect(d->view, SIGNAL(patientClicked(int)), d->preview, SLOT(onPatientClicked(int)));
    connect(d->view, SIGNAL(seriesClicked(int)), d->preview, SLOT(onSeriesClicked(int)));
    connect(d->view, SIGNAL(open(const medDataIndex&)), this, SIGNAL(open(const medDataIndex&)));
    connect(d->view, SIGNAL(exportData(const medDataIndex&)), this, SIGNAL(exportData(const medDataIndex&)));


}

medDatabaseDataSource::~medDatabaseDataSource()
{
    delete d;
    d = NULL;
}

QWidget* medDatabaseDataSource::mainViewWidget()
{
    return d->database_widget;
}

QWidget* medDatabaseDataSource::sourceSelectorWidget()
{
    return new QWidget();
}

QString medDatabaseDataSource::tabName()
{
    return tr("Db");
}

QList<medToolBox*> medDatabaseDataSource::getToolboxes()
{
    //nothing at the moment
    return QList<medToolBox*>(); 
}

void medDatabaseDataSource::update()
{
    d->preview->reset();
    d->preview->init();
    d->preview->update();
}

