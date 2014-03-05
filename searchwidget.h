#ifndef __SEARCHWIDGET_H__
#define __SEARCHWIDGET_H__
#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QDialog>
#include <QCheckBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include "imedit.h"
#include "imlineedit.h"
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
  bool getNewTab();
  void setNewTab(bool v);
  bool getSwitchFocus();
  void setSwitchFocus(bool v);
  public slots:
    void keymapChanged();
    void showOptions(bool);
 private:
    QLabel * m_prompt;
    QGroupBox * m_group;
    ImLineEdit * m_edit;
    QCheckBox * m_newTab;
    QCheckBox * m_switchFocus;
    QDialogButtonBox * m_buttonBox;
    QPushButton * m_moreButton;
    QPushButton * m_findButton;
    QWidget * m_options;
};
class NodeSearchDialog : public QDialog {
  Q_OBJECT

 public:
  NodeSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  QString getText() const;
  bool getNewTab() const;
  void setNewTab(bool v);
 private:
  QLabel * m_prompt;
  QLineEdit * m_edit;
  QCheckBox * m_newTab;
  QCheckBox * m_switchFocus;
  QDialogButtonBox * m_buttonBox;
  QPushButton * m_moreButton;
  QPushButton * m_findButton;
  QWidget * m_options;
};
class WordSearchDialog : public QDialog {
  Q_OBJECT

 public:
  WordSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  QString getText();
  public slots:
    void keymapChanged();
 private:
  //  WrappedEdit * m_edit;
    ImLineEdit * m_edit;
  QDialogButtonBox * m_buttonBox;
};
#endif
