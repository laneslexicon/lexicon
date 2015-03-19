#ifndef __DELETETHEMEDIALOG_H__
#define __DELETETHEMEDIALOG_H__
#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
class DeleteThemeDialog : public QDialog {
  Q_OBJECT

 public:
  DeleteThemeDialog(QWidget * parent = 0);
  QPair<QString,QString> getThemes();
  public slots:
    void onChangeTheme(int);
 private:
  QComboBox * m_themes;
  QComboBox * m_switch;
  QCheckBox * m_delete;
  QPushButton * m_deleteButton;
  QString m_currentTheme;
};
#endif
