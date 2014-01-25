// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_OBJECT_WIDGET_H__
#define __V_OBJECT_WIDGET_H__

#include <VObject>

#ifdef QT_GUI_LIB
#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

// ----------------------------------------------------------------------------
// IVOptionlable
// ----------------------------------------------------------------------------
class IVOptionlable
{
public:
  virtual QDialog* createOptionDlg() = 0;
  virtual void     addOptionWidget(QLayout* layout) = 0;
  virtual bool     showOptionDlg(QDialog* dialog) = 0;
  virtual void     saveOptionDlg(QDialog* dialog) = 0;
};

// ----------------------------------------------------------------------------
// VOptionable
// ----------------------------------------------------------------------------
class VOptionable : public IVOptionlable
{
public:
  virtual QDialog* createOptionDlg();
  virtual void     addOptionWidget(QLayout* layout);
  virtual bool     showOptionDlg(QDialog* dialog);
  virtual void     saveOptionDlg(QDialog* dialog);

public:
  void       addOkCancelButtons(QDialog* dialog);

public:
  QLineEdit* addLineEdit(QLayout* layout, QString objectName, QString text, QString value);
  QCheckBox* addCheckBox(QLayout* layout, QString objectName, QString text, bool value);
  QComboBox* addComboBox(QLayout* layout, QString objectName, QString text, QStringList strList, int index = -1);
};

// ----------------------------------------------------------------------------
// IVShowStatus
// ----------------------------------------------------------------------------
class IVShowStatus
{
public:
  virtual void addStatusWidget(QLayout* layout) = 0;
  virtual bool showStatus(QDialog* parent) = 0;
};

// ----------------------------------------------------------------------------
// VShowStatus
// ----------------------------------------------------------------------------
class VShowStatus : public IVShowStatus
{
public:
  virtual bool showStatus(QDialog* parent);
};

#else // QT_GUI_LIB
class IVOptionlable {};
class VOptionable : public IVOptionlable {};
#endif // QT_GUI_LIB

#endif // __V_OBJECT_WIDGET_H__