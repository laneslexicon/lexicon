#ifndef __SEARCHOPTIONS_H__
#define __SEARCHOPTIONS_H__
#include <QtCore>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
class SearchOptions : public QWidget {
  Q_OBJECT

 public:
  SearchOptions(int searchType,QWidget * parent = 0);
  ~SearchOptions();
  int getOptions();
  void setOptions(int);
  void showMore(bool);
  void addKeymaps(const QString &activeMap,const QStringList & maps);
 public slots:
  void searchTypeChanged();
  void searchTargetChanged();
  void keymapChanged();
 private:
  bool m_hasMaps;
  int m_searchType;
  QSpacerItem * m_spacer;
  QGroupBox *  m_targetGroup;
  QGroupBox * m_typeGroup;
  QGroupBox * m_contextGroup;
  QGroupBox * m_keymapGroup;
  QCheckBox * m_ignoreDiacritics;
  QCheckBox * m_wholeWordMatch;
  QRadioButton * m_headWord;
  QRadioButton * m_fullText;
  QCheckBox * m_forceLTR;
  QRadioButton * m_normalSearch;
  QRadioButton * m_regexSearch;
  QRadioButton * m_arabicTarget;
  QRadioButton * m_buckwalterTarget;

 signals:
  void loadKeymap(const QString &);

};
#endif
