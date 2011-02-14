#ifndef MEDSETTINGSWIDGETFACTORY_H
#define MEDSETTINGSWIDGETFACTORY_H

#include "medGuiExport.h"
#include <dtkCore/dtkAbstractFactory.h>
#include <QtCore>

class medSettingsWidget;
class medSettingsWidgetFactoryPrivate;

/**
 * @brief
 *
*/
class medSettingsWidgetFactory : public dtkAbstractFactory
{
  Q_OBJECT

public:
  /**
   * @brief
   *
  */
  typedef medSettingsWidget*(*medSettingsWidgetCreator)(void);

  /**
   * @brief
   *
  */
  typedef QHash<QString, medSettingsWidgetCreator> medSettingsWidgetCreatorHash;

public:
  /**
   * @brief
   *
   * @param void
   * @return medSettingsWidgetFactory *
  */
  static medSettingsWidgetFactory * instance(void);
  /**
   * @brief
   *
   * @param type
   * @param func
   * @return bool
  */
  bool registerSettingsWidget (QString type,medSettingsWidgetCreator func);
  /**
   * @brief
   *
   * @return QList<QString>
  */
  QList<QString> widgets();

public slots:
  /**
   * @brief
   *
   * @param type
   * @return medSettingsWidget *
  */
  medSettingsWidget * createSettingsWidget(QString type);

protected:
  /**
   * @brief
   *
   * @param void
  */
  medSettingsWidgetFactory(void);

  /**
   * @brief
   *
   * @param void
  */
  ~medSettingsWidgetFactory(void);

private:
  static medSettingsWidgetFactory * s_instance; /**< TODO */

private:
  medSettingsWidgetFactoryPrivate * d; /**< TODO */
};

#endif // MEDSETTINGSWIDGETFACTORY_H
