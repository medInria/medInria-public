#ifndef MEDQUICKACCESSMENU_H
#define MEDQUICKACCESSMENU_H

#include "medGuiExport.h"

#include <QtGui>

class MEDGUI_EXPORT medQuickAccessMenu : public QWidget
{
  Q_OBJECT
public:
    medQuickAccessMenu ( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    
protected:
  /**
   *  emit the hideMenu() signal when the widget lost the focus
   */
  void focusOutEvent(QFocusEvent * event);

signals:
  void hideMenu();
  
};

#endif // MEDQUICKACCESSMENU_H
