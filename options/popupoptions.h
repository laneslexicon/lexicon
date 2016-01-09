#ifndef __POPUPOPTIONS_H__
#define __POPUPOPTIONS_H__
#include <QFontDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>
class DialogOptions : public QDialog {
  Q_OBJECT

 public:
  DialogOptions(QWidget * parent =0);
  enum Which { Tab , Go , Whole, Diacritics, Regex, Force, IgnoreCase };
  void setChecked(int,bool);
  bool isChecked(int);
  void enableOption(int,bool v);
 private:
  QCheckBox * m_newTab;
  QCheckBox * m_goTab;
  QCheckBox * m_wholeWord;
  QCheckBox * m_diacritics;
  QCheckBox * m_regex;
  QCheckBox * m_force;
  QCheckBox * m_ignoreCase;
};
#endif
