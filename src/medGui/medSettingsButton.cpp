#include "medSettingsButton.h"

#include <QtGui>

medSettingsButton::medSettingsButton(QWidget *parent) : QWidget(parent)
{
    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/icons/settings.png"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(icon);
    setToolTip(tr("Edit Application Settings"));
}

medSettingsButton::~medSettingsButton(void)
{

}

QSize medSettingsButton::sizeHint(void) const
{
    return QSize(16, 16);
}

void medSettingsButton::mousePressEvent(QMouseEvent *event)
{
    emit editSettings();
}
