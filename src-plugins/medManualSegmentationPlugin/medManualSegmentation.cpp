#include "medManualSegmentation.h"
#include <iostream>

#include <QEventLoop>
#include <QTimer>
#include <QGridLayout>
#include <QPushButton>

#include "medViewFactory.h"
#include "medAbstractImageView.h"


#include "../legacy/medSegmentation/medAlgorithmPaintToolbox.h"
#include "../legacy/medSegmentation/medAnnotationInteractor.h"

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

medManualSegmentation::medManualSegmentation(void)
{
    m_view=NULL;
}

medManualSegmentation::~medManualSegmentation(void)
{

}

void medManualSegmentation::setImage(medAbstractImageData* image)
{
    this->m_image=image;
}

void medManualSegmentation::run(void)
{
    if(this->m_image==NULL)
    {
        qDebug()<<"no image";
        return;
    }

    QEventLoop ev;

    QWidget *w =new QWidget();
    w->setGeometry(w->x(),w->y(),800,800);
    w->setWindowFlags( Qt::CustomizeWindowHint );

    QGridLayout *layout=new QGridLayout();

    m_view=new medViewContainer();
    m_view->addData(this->m_image);

    layout->addWidget(m_view,0,0);

    QPushButton* doneButton=new QPushButton("done");
    layout->addWidget(doneButton,1,0,2,1);

    medAlgorithmPaintToolbox* paintToolBox=new medAlgorithmPaintToolbox(w);
    layout->addWidget(paintToolBox,0,1);
    paintToolBox->setMaximumWidth(200);

    w->setLayout(layout);
    connect(paintToolBox,SIGNAL(installEventFilterRequest(medViewEventFilter*)),this, SLOT(installFilter(medViewEventFilter*)));
    connect(doneButton,SIGNAL(clicked(bool)),&ev,SLOT(quit()));
    w->show();

    ev.exec();
    w->close();
    delete w;
    qDebug()<<"segmentation done";

}

void medManualSegmentation::installFilter(medViewEventFilter *filter)
{
    if(!m_view)
        return;
    filter->installOnView(m_view->view());
    qDebug()<<Q_FUNC_INFO<<__LINE__;


}
