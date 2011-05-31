#include "medToolBoxiLogRegistration.h"
#include <qdebug.h>
#include <iomanip>
#include <QListWidget>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkLog.h>
#include <medGui/medToolBoxTab.h>
#include <QList>
#include <string>
#include <sstream>
#include <QMouseEvent>
#include <cmath>

class medToolBoxiLogRegistrationPrivate
{
public:
    QComboBox *frameNumber;
    dtkAbstractView *view;
};

medToolBoxiLogRegistration::medToolBoxiLogRegistration(QWidget *parent) : medToolBox(parent), d(new medToolBoxiLogRegistrationPrivate)
{
    d->view = 0;

    d->frameNumber = new QComboBox(this);
    d->frameNumber->setFocusPolicy(Qt::NoFocus);
    d->frameNumber->addItem("0");
    d->frameNumber->addItem("1");
    d->frameNumber->addItem("2");
    d->frameNumber->addItem("3");
    d->frameNumber->addItem("4");
    d->frameNumber->addItem("5");

    connect(d->frameNumber, SIGNAL(currentIndexChanged(QString)), this, SLOT(onFrameNumberChanged(QString)));


    QWidget *framesWidget = new QWidget;
    QFormLayout *framesWidgetLayout = new QFormLayout(framesWidget);
    //framesWidgetLayout->addRow("Select Reference Frame:", d->frameNumber);
    framesWidgetLayout->addWidget(framesWidget);
    framesWidgetLayout->setFormAlignment(Qt::AlignHCenter);

    this->setTitle("iLog Demons Registration");
    
}

medToolBoxiLogRegistration::~medToolBoxiLogRegistration(void)
{
    delete d;

    d = NULL;
}