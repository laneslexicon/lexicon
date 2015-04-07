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
#include "QsLog.h"
#include <QComboBox>
class FontChangeDialog : public QDialog {
  Q_OBJECT

 public:
  FontChangeDialog(QWidget * parent = 0);
  QSize sizeHint() const;
 public slots:
   void onFontChanged(const QString &);
   void onShowAllChanged(int);
   void onApply();
 private:
   //  QStringList m_arFonts;
  void readSettings();
  void writeSettings();
  QLabel * m_currentFont;
  QCheckBox * m_allFonts;
  QCheckBox * m_changeIni;
  QCheckBox * m_changeApplicationCss;
  QCheckBox * m_changeEntryCss;
  QComboBox * m_arabicFont;
  QListWidget * m_changes;
  QPushButton * m_applyButton;
  QLabel * m_fontLabel;
};
#endif
