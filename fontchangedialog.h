#ifndef __FONTCHANGEDIALOG_H__
#define __FONTCHANGEDIALOG_H__
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QFontDialog>
#include <QFontDatabase>
#include <QCheckBox>
#include <QListWidget>
#include <QSettings>
#include <QLabel>
#include <QPushButton>
#include <QLocale>
#include "QsLog.h"
#include <QComboBox>
#include <QGroupBox>
class FontChangeWidget : public QWidget {
  Q_OBJECT
 public:
  FontChangeWidget(QLocale::Script script,QWidget * parent = 0);
  bool isModified() const;
  //  QSize sizeHint() const ;
 public slots:
   void onFontChanged(const QString &);
   void onShowAllChanged(int);
   void onApply();
 private:
   QLocale::Script m_script;
   void setCurrentFontText();
   //  QStringList m_arFonts;
  void readSettings();
  void writeSettings();
  QLabel * m_currentFont;
  QCheckBox * m_allFonts;
  QCheckBox * m_changeIni;
  QCheckBox * m_changeApplicationCss;
  QCheckBox * m_changeEntryCss;
  QComboBox * m_arabicFont;
  QLineEdit * m_fontSize;
  QListWidget * m_changes;
  QPushButton * m_applyButton;
  QLabel * m_fontLabel;
  bool m_modified;
};
class FontChangeDialog : public QDialog {
  Q_OBJECT

 public:
  FontChangeDialog(QWidget * parent = 0);
  bool isModified() const;
 public slots:
   void onHelp();
 private:
   bool m_modified;
   FontChangeWidget * m_arabicFont;
   FontChangeWidget * m_latinFont;
 signals:
  void showHelp(const QString &);
};
#endif
