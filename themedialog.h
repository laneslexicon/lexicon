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
class OptionsTabWidget;

class ThemeDialog : public QDialog {
  Q_OBJECT

 public:
  ThemeDialog(QWidget * parent = 0);
  private slots:
    void onCopy();
    void onEdit();
    void onDelete();
    void onNew();
    void onThemeChanged(const QString &);
 private:
    bool copyRecursively(const QString &,const QString &);
    bool removeDir(const QString &);
    void addTabs();
    void setButtons();
    QComboBox * m_themes;
    QPushButton * m_editThemeButton;
    QPushButton * m_copyThemeButton;
    QPushButton * m_deleteThemeButton;
    QPushButton * m_newThemeButton;
    OptionsTabWidget  * m_tabs;
    int m_copyCount;
    QString m_currentTheme;
    QString m_startupTheme;
};
#endif
