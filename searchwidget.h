#ifndef __SEARCHWIDGET_H__
#define __SEARCHWIDGET_H__
#include <QWidget>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include "imedit.h"
class SearchWidget : public QWidget {
  Q_OBJECT
 public:
  SearchWidget(QWidget * parent = 0);
  public slots:
    void onCancel();
 private:
  WrappedEdit * m_edit;
  QPushButton * m_cancelBtn;
};

class RootSearchDialog : public QDialog {
  Q_OBJECT

 public:
  RootSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  QString getText();
  public slots:
    void onCancel();
 private:
  WrappedEdit * m_edit;
  QDialogButtonBox * m_buttonBox;
};
class NodeSearchDialog : public QDialog {
  Q_OBJECT

 public:
  NodeSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  QString getText();
 private:
  WrappedEdit * m_edit;
  QDialogButtonBox * m_buttonBox;
};
#endif
