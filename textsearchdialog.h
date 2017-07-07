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
  void setText(const QString &);
  void hideKeyboard();
  void readSettings();
  void setForRoot();
  void setForHead();
  void setForLocal();
  void showTabOptions(bool v);
  bool headPhrase() const;
  TextOption options() const;
  static QStringList failureActions();
  SearchOptions searchOptions() const; // this returns everything including headphrase and highlight all
  void fromOptions(SearchOptions &);   // set widgets from options
  QPair<bool,bool> tabOptions() const;
  enum NotFound_t { SearchAgainYes , SearchAgainNo, SearchDialog, Nothing };
    public slots:
    void showKeyboard();
    void loadKeymap(const QString &);
    void keyboardClosed();
    void onKeyboardShortcut(const QString &);
    void onHelp();
    void onTextChanged(const QString &);
    void searchTypeChanged(bool);
    void newTabChanged(int);
    void keymapChanged();

 protected:
    QString showText(const QString &);
 private:
    QMap<int,bool>  m_showKeymap;
    QGroupBox   * m_typeGroup;
    QGridLayout * m_form;
    int m_count;
    int m_type;

    KeyboardWidget * m_keyboard;
    QLabel * m_text;
    bool m_attached;
    bool m_mapEnabled;
    QLabel * m_prompt;
    QGroupBox * m_group;
    ImLineEdit * m_edit;
    QDialogButtonBox * m_buttonBox;
    QPushButton * m_findButton;
    QPushButton * m_keyboardButton;
    QPushButton * m_keymapButton;
    QCheckBox * m_wholeWord;
    QCheckBox * m_ignoreCase;
    QCheckBox * m_ignoreDiacritics;
    QCheckBox * m_goTab;
    QCheckBox * m_newTab;
    QCheckBox * m_headPhrase;             // for headword search
    QCheckBox * m_highlightAll;           // for local search;
    QRadioButton * m_normalSearch;
    QRadioButton * m_regexSearch;

 signals:
    void showHelp(const  QString &);
};
#endif
