#ifndef MEDQUICKACCESSMENU_H
#define MEDQUICKACCESSMENU_H

#include <QtGui>

class medQuickAccessMenu : public QWidget
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
  void hideMenu(void);
  
};

#endif // MEDQUICKACCESSMENU_H
