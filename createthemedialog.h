#ifndef __CREATETHEMEDIALOG_H__
#define __CREATETHEMEDIALOG_H__
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
class CreateThemeDialog : public QDialog {
  Q_OBJECT

 public:
  CreateThemeDialog(QWidget * parent = 0);
  QPair<QString,QString> getThemes();
  bool activate();
  public slots:
    void onTextChanged(const QString &);
    void onCreate();
 private:
  QComboBox * m_themes;
  QCheckBox * m_activate;
  QLineEdit * m_newTheme;
  QPushButton * m_createButton;
  QString m_currentTheme;
};
#endif
