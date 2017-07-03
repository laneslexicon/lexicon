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
#include <QTextBrowser>
#include <QGridLayout>
#include <QDesktopWidget>
#include "application.h"
#include "imedit.h"
#include "imlineedit.h"
#include "keyboardwidget.h"
#include "textsearch.h"
class SearchOptions;
//#include "searchoptionswidget.h"
class TextSearchDialog : public QDialog {
  Q_OBJECT

 public:
  TextSearchDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  ~TextSearchDialog();
  QString getText() const;
  void setPrompt(const QString &);
  //  void getOptions(SearchOptions &) const;
  //  void setOptions(SearchOptions &);
  //  bool getForceLTR() const ;
  void setText(const QString &);
  void hideKeyboard();
  void readSettings();
  void showHeadOption(bool v);
  void showHighlightAll(bool v);
  bool headPhrase() const;
  TextOption options() const;
  SearchOptions searchOptions() const; // this returns everything including headphrase and highlight all
  void fromOptions(SearchOptions &);   // set widgets from options
  QPair<bool,bool> tabOptions() const;
  public slots:
    virtual void showOptions(bool);
    void showKeyboard();
    void loadKeymap(const QString &);
    void keyboardClosed();
    void onKeyboardShortcut(const QString &);
    void onHelp();
    void onTextChanged(const QString &);
    void searchTypeChanged(bool);
 protected:
    QString showText(const QString &);
    //    SearchOptions::SearchScope_t m_searchType;
 private:
    QGridLayout * m_form;
    QWidget * m_optionsWidget;
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
    QCheckBox * m_wholeWord;
    QCheckBox * m_ignoreCase;
    QCheckBox * m_ignoreDiacritics;
    QCheckBox * m_goTab;
    QCheckBox * m_newTab;
    QCheckBox * m_headPhrase;             // for headword search
    QCheckBox * m_highlightAll;           // for local search;
    QRadioButton * m_normalSearch;
    QRadioButton * m_regexSearch;
//    SearchOptionsWidget  * m_options;

 signals:
    void showHelp(const  QString &);
};
#endif
