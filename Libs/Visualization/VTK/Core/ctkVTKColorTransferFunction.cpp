/*=========================================================================

  Library:   ctk

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

/// Qt includes
#include <QColor>
#include <QDebug>

/// CTK includes
#include "ctkVTKColorTransferFunction.h"

/// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

class ctkVTKColorTransferFunctionPrivate: public ctkPrivate<ctkVTKColorTransferFunction>
{
public:
  vtkSmartPointer<vtkColorTransferFunction> ColorTransferFunction;
};

//-----------------------------------------------------------------------------
ctkVTKColorTransferFunction::ctkVTKColorTransferFunction(vtkColorTransferFunction* colorTransferFunction, 
                                                         QObject* parentObject)
  :ctkTransferFunction(parentObject)
{
  CTK_INIT_PRIVATE(ctkVTKColorTransferFunction);
  this->setColorTransferFunction(colorTransferFunction);
}

//-----------------------------------------------------------------------------
ctkVTKColorTransferFunction::~ctkVTKColorTransferFunction()
{
}

//-----------------------------------------------------------------------------
int ctkVTKColorTransferFunction::count()const
{
  CTK_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->ColorTransferFunction.GetPointer());
    return -1;
    }
  return d->ColorTransferFunction->GetSize();
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::range(qreal& minRange, qreal& maxRange)const
{
  CTK_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->ColorTransferFunction.GetPointer());
    minRange = 1.;
    maxRange = 0.;
    return;
    }
  double rangeValues[2];
  d->ColorTransferFunction->GetRange(rangeValues);
  minRange = rangeValues[0];
  maxRange = rangeValues[1];
}

//-----------------------------------------------------------------------------
QVariant ctkVTKColorTransferFunction::minValue()const
{
  CTK_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->ColorTransferFunction.GetPointer());
    return -1;
    }
  double rgb[3];
  QColor minValue = QColor::fromRgbF(1.,1.,1.);
  for (int i = 0; i < this->count(); ++i)
    {
    d->ColorTransferFunction->GetColor(i, rgb);
    Q_ASSERT(rgb[0] >= 0. && rgb[0] <= 1. &&
             rgb[1] >= 0. && rgb[1] <= 1. &&
             rgb[2] >= 0. && rgb[2] <= 1.);
    QColor color = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
    if ( qGray(color.red(), color.green(), color.blue()) <
         qGray(minValue.red(), minValue.green(), minValue.blue()))
      {
      minValue = color;
      }
    }
  return minValue;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKColorTransferFunction::maxValue()const
{
  CTK_D(const ctkVTKColorTransferFunction);
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    Q_ASSERT(d->ColorTransferFunction.GetPointer());
    return -1;
    }
  double rgb[3];
  QColor minValue = QColor::fromRgbF(0.,0.,0.);
  for (int i = 0; i < this->count(); ++i)
    {
    d->ColorTransferFunction->GetColor(i, rgb);
    Q_ASSERT(rgb[0] >= 0. && rgb[0] <= 1. &&
             rgb[1] >= 0. && rgb[1] <= 1. &&
             rgb[2] >= 0. && rgb[2] <= 1.);
    QColor color = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
    if ( qGray(color.red(), color.green(), color.blue()) >
         qGray(minValue.red(), minValue.green(), minValue.blue()))
      {
      minValue = color;
      }
    }
  return minValue;
}

//-----------------------------------------------------------------------------
ctkControlPoint* ctkVTKColorTransferFunction::controlPoint(int index)const
{
  CTK_D(const ctkVTKColorTransferFunction);
  Q_ASSERT(index >= 0 && index < this->count());
	double values[6];
  d->ColorTransferFunction->GetNodeValue(index, values);
  Q_ASSERT(values[0] >= d->ColorTransferFunction->GetRange()[0] &&
           values[0] <= d->ColorTransferFunction->GetRange()[1] &&
           values[1] >= 0. && values[1] <= 1. &&  // Red
           values[2] >= 0. && values[2] <= 1. &&  // Green
           values[3] >= 0. && values[3] <= 1. &&  // Blue
           values[4] >= 0. && values[4] <= 1. &&  // MidPoint
           values[5] >= 0. && values[5] <= 1.);   // Sharpness
  QColor rgb = QColor::fromRgbF(values[1], values[2], values[3]);
  if (index + 1 >= this->count())
    {
    ctkControlPoint* cp = new ctkControlPoint();
    cp->P.X = values[0];
    cp->P.Value = rgb;
    return cp;
    }
  ctkNonLinearControlPoint* cp = new ctkNonLinearControlPoint();
  cp->P.X = values[0];
  cp->P.Value = rgb;
  d->ColorTransferFunction->GetNodeValue(index + 1, values);
  Q_ASSERT(values[0] >= d->ColorTransferFunction->GetRange()[0] &&
           values[0] <= d->ColorTransferFunction->GetRange()[1] &&
           values[1] >= 0. && values[1] <= 1. &&  // Red
           values[2] >= 0. && values[2] <= 1. &&  // Green
           values[3] >= 0. && values[3] <= 1. &&  // Blue
           values[4] >= 0. && values[4] <= 1. &&  // MidPoint
           values[5] >= 0. && values[5] <= 1.);   // Sharpness
  double subPoints[30];
  d->ColorTransferFunction->GetTable(cp->x(), values[0], 10, subPoints);
  qreal interval = (values[0] - cp->x()) / 9.;
  for(int i = 0; i < 10; ++i)
    {
    
    QColor rgb = QColor::fromRgbF(subPoints[3*i], 
                                  subPoints[3*i+1],
                                  subPoints[3*i+2]);
    cp->SubPoints << ctkPoint(cp->x() + interval*i, rgb);
    }
  return cp;
}

//-----------------------------------------------------------------------------
QVariant ctkVTKColorTransferFunction::value(qreal pos)const
{
  CTK_D(const ctkVTKColorTransferFunction);
  Q_ASSERT(d->ColorTransferFunction.GetPointer());
  double rgb[3];
  d->ColorTransferFunction->GetColor(pos, rgb);
  QColor color = QColor::fromRgbF(rgb[0], rgb[1], rgb[2]);
  return color;
}

//-----------------------------------------------------------------------------
int ctkVTKColorTransferFunction::insertControlPoint(const ctkControlPoint& cp)
{
  CTK_D(ctkVTKColorTransferFunction);
  int index = -1;
  if (d->ColorTransferFunction.GetPointer() == 0)
    {
    return index;
    }
  QColor rgb = cp.value().value<QColor>();
  const ctkNonLinearControlPoint* nonLinearCp = dynamic_cast<const ctkNonLinearControlPoint*>(&cp);
  if (nonLinearCp)
    {
    // TODO retrieve midpoint & sharpness
    index = d->ColorTransferFunction->AddRGBPoint(
      cp.x(), rgb.redF(), rgb.greenF(), rgb.blueF());
    }
  else
    {
    index = d->ColorTransferFunction->AddRGBPoint(
      cp.x(), rgb.redF(), rgb.greenF(), rgb.blueF());
    }
  return index;
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::setControlPointPos(int index, qreal pos)
{
  CTK_D(ctkVTKColorTransferFunction);
  double values[6];
  d->ColorTransferFunction->GetNodeValue(index, values);
  values[0] = pos;
  // warning, a possible new range is not supported
  // SetNodeValue eventually fire the signal changed()
  d->ColorTransferFunction->SetNodeValue(index, values);
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::setControlPointValue(int index, const QVariant& value)
{
  CTK_D(ctkVTKColorTransferFunction);
  Q_ASSERT(value.value<QColor>().isValid());
  QColor rgb = value.value<QColor>();
  double values[6];
  d->ColorTransferFunction->GetNodeValue(index, values);
  values[1] = rgb.redF();
  values[2] = rgb.greenF();
  values[3] = rgb.blueF();
  // setNodeValue should eventually fired the signal changed()
  d->ColorTransferFunction->SetNodeValue(index, values);
}

//-----------------------------------------------------------------------------
void ctkVTKColorTransferFunction::setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction)
{
  CTK_D(ctkVTKColorTransferFunction);
  d->ColorTransferFunction = colorTransferFunction;
  this->qvtkReconnect(d->ColorTransferFunction,vtkCommand::ModifiedEvent,
                      this, SIGNAL(changed()));
  emit changed();
}

//-----------------------------------------------------------------------------
vtkColorTransferFunction* ctkVTKColorTransferFunction::colorTransferFunction()const
{
  CTK_D(const ctkVTKColorTransferFunction);
  return d->ColorTransferFunction;
}