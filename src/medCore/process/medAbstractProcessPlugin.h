/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractPlugin.h>

class medAbstractProcess;
class medAsbtractProcessPluginPrivate;

class medAsbtractProcessPlugin : public medAbstractPlugin
{

public:
    medAsbtractProcessPlugin(QObject *parent = 0);
    ~medAsbtractProcessPlugin();

    virtual medAbstractProcess *process() const = 0;
    virtual QWidget toolbox() const = 0;

    void addTags(QStringList tags);
    QStringList tags() const;

private:
    medAsbtractProcessPluginPrivate *d;
};
