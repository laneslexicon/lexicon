#ifndef __SEARCHOPTIONSWIDGET_H__
#define __SEARCHOPTIONSWIDGET_H__
#include <QtCore>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include "searchoptions.h"
#ifndef qStrip
#define qStrip qDebug()
#endif
class FullSearchWidget;
class SearchOptionsWidget : public QWidget {
  Q_OBJECT

 public:
  SearchOptionsWidget(SearchOptions::SearchScope_t,QWidget * parent = 0);
  SearchOptionsWidget(SearchOptions & opts,QWidget * parent = 0);
  ~SearchOptionsWidget();

  void getOptions(SearchOptions &) const;
  void setOptions(const SearchOptions &);
  void setOptions(int type);
  void hideTabOptions(bool visible = false);
  //  int getOptions();
  //  void setOptions(int);
  void showMore(bool);
  void addKeymaps(const QString &activeMap,const QStringList & maps);
  void setKeymapsEnabled(bool v);
  bool getForceLTR();
  bool isTextSearch() const;
  void setTextSearch(bool);
 public slots:
  void searchTypeChanged();
  void keymapChanged();
  void onForceLeftToRight(int);
  void onNewTab(int);
 private:
  void setup(QWidget * parent);
  bool m_textSearch;
  bool m_hasMaps;
  bool m_more;                   // false basic, 1 show more
  bool m_keymapsEnabled;
  SearchOptions m_options;
  QSpacerItem * m_spacer;
  QGroupBox *  m_targetGroup;
  QGroupBox * m_typeGroup;
  QGroupBox * m_contextGroup;
  QGroupBox * m_keymapGroup;
  QCheckBox * m_ignoreDiacritics;
  QCheckBox * m_wholeWordMatch;
  QCheckBox * m_includeHeads;
  QCheckBox * m_ignoreCase;
  QCheckBox * m_showAllSearch;
  QCheckBox * m_newTab;
  QCheckBox * m_makeActive;
  QCheckBox * m_headPhrase;
  QRadioButton * m_headWord;
  QRadioButton * m_fullText;
  QCheckBox * m_forceLTR;
  QRadioButton * m_normalSearch;
  QRadioButton * m_regexSearch;
  QRadioButton * m_arabicTarget;
  QRadioButton * m_buckwalterTarget;
  QLabel * m_diacriticsText;
  QLabel * m_caseText;
  QLabel * m_wholeText;
 signals:
  void loadKeymap(const QString &);
  void force(bool);
};
#endif
