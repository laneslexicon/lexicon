#ifndef __TEXTSEARCHDIALOG_H__
#define __TEXTSEARCHDIALOG_H__
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
#include "searchoptionswidget.h"
class TextSearchDialog : public QDialog {
  Q_OBJECT

 public:
  TextSearchDialog(SearchOptions::SearchScope_t type,QWidget * parent = 0, Qt::WindowFlags f = 0);
  ~TextSearchDialog();
  QString getText() const;
  void setPrompt(const QString &);
  void getOptions(SearchOptions &) const;
  void setOptions(SearchOptions &);
  bool getForceLTR() const ;
  void setText(const QString &);
  void setArabic(bool);
  void hideKeyboard();
  public slots:
    virtual void showOptions(bool);
    void showKeyboard();
    void loadKeymap(const QString &);
    void keyboardClosed();
    void onKeyboardShortcut(const QString &);
    void onHelp();
    void languageSwitch(int);
    void onTextChanged(const QString &);
 protected:
    QString showText(const QString &);
    SearchOptions::SearchScope_t m_searchType;
    int m_count;
    KeyboardWidget * m_keyboard;
    QLabel * m_text;
    bool m_attached;
    bool m_mapEnabled;
    QLabel * m_prompt;
    QGroupBox * m_group;
    ImLineEdit * m_edit;
    QDialogButtonBox * m_buttonBox;
    QPushButton * m_moreButton;
    QPushButton * m_findButton;
    QPushButton * m_keyboardButton;
    SearchOptionsWidget  * m_options;
 signals:
    void showHelp(const  QString &);
};
#endif
