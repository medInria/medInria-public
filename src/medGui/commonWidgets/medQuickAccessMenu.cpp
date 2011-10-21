#include "medQuickAccessMenu.h"

medQuickAccessMenu::medQuickAccessMenu ( QWidget* parent, Qt::WindowFlags f ) : QWidget ( parent, f ) {}

void medQuickAccessMenu::focusOutEvent ( QFocusEvent* event )
{
    QWidget::focusOutEvent ( event );
    emit hideMenu();
}