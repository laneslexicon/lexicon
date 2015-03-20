#ifndef __THEMEDIALOG_H__
#define __THEMEDIALOG_H__
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QInputDialog>
#include <QMessageBox>
class ThemeDialog : public QDialog {
  Q_OBJECT

 public:
  ThemeDialog(QWidget * parent = 0);
  QString theme() const;
 private:

    QComboBox * m_themes;
    QPushButton * m_editThemeButton;
    QString m_currentTheme;
};
#endif
