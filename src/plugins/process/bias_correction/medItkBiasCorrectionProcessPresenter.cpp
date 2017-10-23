/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2017. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkBiasCorrectionProcessPresenter.h>

#include <medIntParameterPresenter.h>
#include <medDoubleParameterPresenter.h>
#include <medStringParameterPresenter.h>

#include <QLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

medItkBiasCorrectionProcessPresenter::medItkBiasCorrectionProcessPresenter(medItkBiasCorrectionProcess *parent) : medAbstractBiasCorrectionProcessPresenter(parent)
{
   m_process = parent;

   m_poUIPresenterThreadNb = new medIntParameterPresenter(m_process->getUIThreadNb());
   m_poUIPresenterShrinkFactors = new medIntParameterPresenter(m_process->getUIShrinkFactors());
   m_poUIPresenterSplineOrder = new medIntParameterPresenter(m_process->getUISplineOrder());
   m_poSPresenterMaxIterations = new medStringParameterPresenter(m_process->getSMaxIterations());

   m_poFPresenterWienerFilterNoise = new medDoubleParameterPresenter(m_process->getFWienerFilterNoise());
   m_poFPresenterbfFWHM = new medDoubleParameterPresenter(m_process->getFbfFWHM());
   m_poFPresenterConvergenceThreshold = new medDoubleParameterPresenter(m_process->getFConvergenceThreshold());
   m_poFPresenterSplineDistance = new medDoubleParameterPresenter(m_process->getFSplineDistance());
   m_poFPresenterInitialMeshResolutionVect1 = new medDoubleParameterPresenter(m_process->getFInitialMeshResolutionVect1());
   m_poFPresenterInitialMeshResolutionVect2 = new medDoubleParameterPresenter(m_process->getFInitialMeshResolutionVect2());
   m_poFPresenterInitialMeshResolutionVect3 = new medDoubleParameterPresenter(m_process->getFInitialMeshResolutionVect3());

   m_poUIPresenterProgression = new medIntParameterPresenter(m_process->progression());
}

QWidget * medItkBiasCorrectionProcessPresenter::buildToolBoxWidget()
{
   QWidget *poResWidget = new QWidget;
   QVBoxLayout *poVLayout = new QVBoxLayout;
   poResWidget->setLayout(poVLayout);

   QLabel *poThredNbLabel = new QLabel(m_poUIPresenterThreadNb->parameter()->caption(), poResWidget);
   QHBoxLayout *poThredNbLayout = new QHBoxLayout;
   poThredNbLayout->addWidget(poThredNbLabel);
   poThredNbLayout->addWidget(m_poUIPresenterThreadNb->buildWidget());
   poVLayout->addLayout(poThredNbLayout);

   QLabel *poShrinkFactorsLabel = new QLabel(m_poUIPresenterShrinkFactors->parameter()->caption(), poResWidget);
   QHBoxLayout *poShrinkFactorsLayout = new QHBoxLayout;
   poShrinkFactorsLayout->addWidget(poShrinkFactorsLabel);
   poShrinkFactorsLayout->addWidget(m_poUIPresenterShrinkFactors->buildWidget());
   poVLayout->addLayout(poShrinkFactorsLayout);

   QLabel *poSplineOrderLabel = new QLabel(m_poUIPresenterSplineOrder->parameter()->caption(), poResWidget);
   QHBoxLayout *poSplineOrderLayout = new QHBoxLayout;
   poSplineOrderLayout->addWidget(poSplineOrderLabel);
   poSplineOrderLayout->addWidget(m_poUIPresenterSplineOrder->buildWidget());
   poVLayout->addLayout(poSplineOrderLayout);

   QLabel *poMaxNumbersIterationsLabel = new QLabel(m_poSPresenterMaxIterations->parameter()->caption(), poResWidget);
   QHBoxLayout *poMaxNumbersIterationsLayout = new QHBoxLayout;
   poMaxNumbersIterationsLayout->addWidget(poMaxNumbersIterationsLabel);
   poMaxNumbersIterationsLayout->addWidget(m_poSPresenterMaxIterations->buildWidget());

   poVLayout->addLayout(poMaxNumbersIterationsLayout);
   

   QLabel *poWienerFilterNoiseLabel = new QLabel(m_poFPresenterWienerFilterNoise->parameter()->caption(), poResWidget);
   QHBoxLayout *poWienerFilterNoiseLayout = new QHBoxLayout;
   poWienerFilterNoiseLayout->addWidget(poWienerFilterNoiseLabel);
   poWienerFilterNoiseLayout->addWidget(m_poFPresenterWienerFilterNoise->buildWidget());
   poVLayout->addLayout(poWienerFilterNoiseLayout);

   QLabel *pobfFWHMLabel = new QLabel(m_poFPresenterbfFWHM->parameter()->caption(), poResWidget);
   QHBoxLayout *pobfFWHMLayout = new QHBoxLayout;
   pobfFWHMLayout->addWidget(pobfFWHMLabel);
   pobfFWHMLayout->addWidget(m_poFPresenterbfFWHM->buildWidget());
   poVLayout->addLayout(pobfFWHMLayout);

   QLabel *poConvergenceThresholdLabel = new QLabel(m_poFPresenterConvergenceThreshold->parameter()->caption(), poResWidget);
   QHBoxLayout *poConvergenceThresholdLayout = new QHBoxLayout;
   poConvergenceThresholdLayout->addWidget(poConvergenceThresholdLabel);
   poConvergenceThresholdLayout->addWidget(m_poFPresenterConvergenceThreshold->buildWidget());
   poVLayout->addLayout(poConvergenceThresholdLayout);

   QLabel *poSplineDistanceLabel = new QLabel(m_poFPresenterSplineDistance->parameter()->caption(), poResWidget);
   QHBoxLayout *poSplineDistanceLayout = new QHBoxLayout;
   poSplineDistanceLayout->addWidget(poSplineDistanceLabel);
   poSplineDistanceLayout->addWidget(m_poFPresenterSplineDistance->buildWidget());
   poVLayout->addLayout(poSplineDistanceLayout);


   QLabel *poInitialMeshResolutionLabelX = new QLabel(m_poFPresenterInitialMeshResolutionVect1->parameter()->caption(), poResWidget);
   QLabel *poInitialMeshResolutionLabelY = new QLabel(m_poFPresenterInitialMeshResolutionVect2->parameter()->caption(), poResWidget);
   QLabel *poInitialMeshResolutionLabelZ = new QLabel(m_poFPresenterInitialMeshResolutionVect3->parameter()->caption(), poResWidget);
   QWidget *poWInitialMeshResolution1 = m_poFPresenterInitialMeshResolutionVect1->buildWidget();
   QWidget *poWInitialMeshResolution2 = m_poFPresenterInitialMeshResolutionVect2->buildWidget();
   QWidget *poWInitialMeshResolution3 = m_poFPresenterInitialMeshResolutionVect3->buildWidget();

   connect(m_poFPresenterSplineDistance->parameter(), &medDoubleParameter::valueChanged, [=](double pi_fVal)
   {
      bool bActive = pi_fVal != 0;
      poInitialMeshResolutionLabelX->setDisabled(bActive);
      poInitialMeshResolutionLabelY->setDisabled(bActive);
      poInitialMeshResolutionLabelZ->setDisabled(bActive);
      poWInitialMeshResolution1->setDisabled(bActive);
      poWInitialMeshResolution2->setDisabled(bActive);
      poWInitialMeshResolution3->setDisabled(bActive);
   });

   QHBoxLayout *poInitialMeshResolutionLayoutX = new QHBoxLayout;
   poInitialMeshResolutionLayoutX->addWidget(poInitialMeshResolutionLabelX);
   poInitialMeshResolutionLayoutX->addWidget(poWInitialMeshResolution1);
   poVLayout->addLayout(poInitialMeshResolutionLayoutX);
   QHBoxLayout *poInitialMeshResolutionLayoutY = new QHBoxLayout;
   poInitialMeshResolutionLayoutY->addWidget(poInitialMeshResolutionLabelY);
   poInitialMeshResolutionLayoutY->addWidget(poWInitialMeshResolution2);
   poVLayout->addLayout(poInitialMeshResolutionLayoutY);
   QHBoxLayout *poInitialMeshResolutionLayoutZ = new QHBoxLayout;
   poInitialMeshResolutionLayoutZ->addWidget(poInitialMeshResolutionLabelZ);
   poInitialMeshResolutionLayoutZ->addWidget(poWInitialMeshResolution3);
   poVLayout->addLayout(poInitialMeshResolutionLayoutZ);

   QProgressBar *poProgessBar = m_poUIPresenterProgression->buildProgressBar();
   poVLayout->addWidget(poProgessBar);

   poVLayout->addWidget(buildRunButton());
   poVLayout->addWidget(buildCancelButton());

   return poResWidget;
}
