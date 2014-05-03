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
#include "application.h"
#include "imedit.h"
#include "imlineedit.h"
#include "keyboardwidget.h"

class SearchDialog : public QDialog {
  Q_OBJECT

 public:
  SearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  QString getText();
  void setPrompt(const QString &);
  bool getNewTab();
  void setNewTab(bool v);
  bool getSwitchFocus();
  void setSwitchFocus(bool v);
  virtual void setup();
    public slots:
    void keymapChanged();
    virtual void showOptions(bool);
    void showKeyboard();
 protected:

    virtual void addOptions(QGridLayout *);
    KeyboardWidget * m_keyboard;
    bool m_attached;
    QLabel * m_prompt;
    QGroupBox * m_group;
    ImLineEdit * m_edit;
    QCheckBox * m_newTab;
    QCheckBox * m_switchFocus;
    QDialogButtonBox * m_buttonBox;
    QPushButton * m_moreButton;
    QPushButton * m_findButton;
    QPushButton * m_keyboardButton;
    QWidget * m_options;
};
class WordSearchDialog : public SearchDialog {
  Q_OBJECT
 public:
  WordSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  virtual void setup();
  public slots:
    virtual void showOptions(bool);
 protected:

  virtual void addOptions(QGridLayout *);

 private:
    QCheckBox * m_ignoreDiacritics;
    QCheckBox * m_wholeWordMatch;

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
#endif
