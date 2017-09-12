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

#include <QLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

medItkBiasCorrectionProcessPresenter::medItkBiasCorrectionProcessPresenter(medItkBiasCorrectionProcess *parent) : medAbstractBiasCorrectionProcessPresenter(parent)
{
   m_process = parent;

   m_poUIPresenterThreadNb = new medIntParameterPresenter(m_process->getUIThreadNb());
   m_poUIPresenterShrinkFactors = new medIntParameterPresenter(m_process->getUIShrinkFactors());
   m_poUIPresenterSplineOrder = new medIntParameterPresenter(m_process->getUISplineOrder());
   m_poUIPresenterMaxNumbersIterationsVector1 = new medIntParameterPresenter(m_process->getUIMaxNumbersIterationsVector1());
   m_poUIPresenterMaxNumbersIterationsVector2 = new medIntParameterPresenter(m_process->getUIMaxNumbersIterationsVector2());
   m_poUIPresenterMaxNumbersIterationsVector3 = new medIntParameterPresenter(m_process->getUIMaxNumbersIterationsVector3());

   m_poFPresenterWienerFilterNoise = new medDoubleParameterPresenter(m_process->getFWienerFilterNoise());
   m_poFPresenterbfFWHM = new medDoubleParameterPresenter(m_process->getFbfFWHM());
   m_poFPresenterConvergenceThreshold = new medDoubleParameterPresenter(m_process->getFConvergenceThreshold());
   m_poFPresenterSplineDistance = new medDoubleParameterPresenter(m_process->getFSplineDistance());
   m_poFPresenterInitialMeshResolutionVect1 = new medDoubleParameterPresenter(m_process->getFInitialMeshResolutionVect1());
   m_poFPresenterInitialMeshResolutionVect2 = new medDoubleParameterPresenter(m_process->getFInitialMeshResolutionVect2());
   m_poFPresenterInitialMeshResolutionVect3 = new medDoubleParameterPresenter(m_process->getFInitialMeshResolutionVect3());
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

   QLabel *poMaxNumbersIterationsLabel = new QLabel(m_poUIPresenterMaxNumbersIterationsVector1->parameter()->caption(), poResWidget);
   QHBoxLayout *poMaxNumbersIterationsLayout = new QHBoxLayout;
   poMaxNumbersIterationsLayout->addWidget(poMaxNumbersIterationsLabel);
   poMaxNumbersIterationsLayout->addWidget(m_poUIPresenterMaxNumbersIterationsVector1->buildWidget());
   poMaxNumbersIterationsLayout->addWidget(m_poUIPresenterMaxNumbersIterationsVector2->buildWidget());
   poMaxNumbersIterationsLayout->addWidget(m_poUIPresenterMaxNumbersIterationsVector3->buildWidget());
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


   QLabel *poInitialMeshResolutionLabel = new QLabel(m_poFPresenterInitialMeshResolutionVect1->parameter()->caption(), poResWidget);
   QWidget *poWInitialMeshResolution1 = m_poFPresenterInitialMeshResolutionVect1->buildWidget();
   QWidget *poWInitialMeshResolution2 = m_poFPresenterInitialMeshResolutionVect2->buildWidget();
   QWidget *poWInitialMeshResolution3 = m_poFPresenterInitialMeshResolutionVect3->buildWidget();

   connect(m_poFPresenterSplineDistance->parameter(), &medDoubleParameter::valueChanged, [=](double pi_fVal)
   {
      bool bActive = pi_fVal != 0;
      poInitialMeshResolutionLabel->setDisabled(bActive);
      poWInitialMeshResolution1->setDisabled(bActive);
      poWInitialMeshResolution2->setDisabled(bActive);
      poWInitialMeshResolution3->setDisabled(bActive);
   });
   QHBoxLayout *poInitialMeshResolutionLayout = new QHBoxLayout;
   poInitialMeshResolutionLayout->addWidget(poInitialMeshResolutionLabel);
   poInitialMeshResolutionLayout->addWidget(poWInitialMeshResolution1);
   poInitialMeshResolutionLayout->addWidget(poWInitialMeshResolution2);
   poInitialMeshResolutionLayout->addWidget(poWInitialMeshResolution3);
   /*poInitialMeshResolutionLayout->setSizeConstraint(QLayout::SetMaximumSize);
   poInitialMeshResolutionLayout->setSizeConstraint(QLayout::SetMaximumSize);
   poInitialMeshResolutionLayout->setSizeConstraint(QLayout::SetMaximumSize);
   poInitialMeshResolutionLayout->setSizeConstraint(QLayout::SetMaximumSize);*/
   poVLayout->addLayout(poInitialMeshResolutionLayout);

   poVLayout->addWidget(buildRunButton());
   poVLayout->addWidget(buildCancelButton());
   //poVLayout->setSizeConstraint(QLayout::SetMinimumSize);// SetMinimumSize

   return poResWidget;
}
