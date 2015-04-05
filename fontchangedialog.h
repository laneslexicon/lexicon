#ifndef __FONTCHANGEDIALOG_H__
#define __FONTCHANGEDIALOG_H__
#include <QDialog>
#include <QFormLayout>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QFontDialog>
#include <QFontDatabase>
#include <QSettings>
#include <QTextStream>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include "QsLog.h"
class FontChangeDialog : public QDialog {
  Q_OBJECT

 public:
  FontChangeDialog(QWidget * parent = 0);
  QSize sizeHint() const;
 public slots:
  void lexiconArabicFontDialog();
  void lexiconEnglishFontDialog();
  void appArabicFontDialog();
  void appNativeFontDialog();
 private:
  QStringList m_arFonts;
  QStringList readFile(const QString &);
  void readSettings();
  void writeSettings();

  QLineEdit * m_lexiconArabicFontName;
  QLineEdit * m_lexiconArabicFontSize;
  QLineEdit * m_lexiconEnglishFontName;
  QLineEdit * m_lexiconEnglishFontSize;
  QLineEdit * m_appArabicFontName;
  QLineEdit * m_appArabicFontSize;
  QLineEdit * m_appNativeFontName;
  QLineEdit * m_appNativeFontSize;
};
#endif
