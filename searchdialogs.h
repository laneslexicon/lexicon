#ifndef __SEARCHDIALOG_H__
#define __SEARCHDIALOG_H__
#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QDialog>
#include <QCheckBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include "application.h"
#include "imedit.h"
#include "imlineedit.h"
#include "keyboardwidget.h"
class SearchOptions;
class RootSearchDialog : public QDialog {
  Q_OBJECT

 public:
  RootSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  QString getText();
  void setPrompt(const QString &);
  void setOptions(int);
  public slots:
    //    void keymapChanged();
    virtual void showOptions(bool);
    void showKeyboard();
    void loadKeymap(const QString &);
 protected:
    KeyboardWidget * m_keyboard;
    bool m_attached;
    QLabel * m_prompt;
    QGroupBox * m_group;
    ImLineEdit * m_edit;
    QDialogButtonBox * m_buttonBox;
    QPushButton * m_moreButton;
    QPushButton * m_findButton;
    QPushButton * m_keyboardButton;
    SearchOptions  * m_options;
};
/*
class WordSearchDialog : public SearchDialog {
  Q_OBJECT
 public:
  WordSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  virtual void setup();
  int getOptions();
  public slots:
    virtual void showOptions(bool);
  void searchTargetChanged();
  void searchTypeChanged();
 protected:

  virtual void addOptions(QGridLayout *);

 private:
    QCheckBox * m_ignoreDiacritics;
    QCheckBox * m_wholeWordMatch;
    QCheckBox * m_headButton;
    QCheckBox * m_fullButton;
    QRadioButton * m_normalButton;
    QRadioButton * m_regexButton;
    QRadioButton  * m_arabicTarget;
    QRadioButton * m_buckwalterTarget;
    QGroupBox * m_targetGroup;
    QGroupBox * m_typeGroup;
    void readSettings();
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
  QPushButton * m_findButton;
  QWidget * m_options;
};
*/
#endif
