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

 private:
    bool copyRecursively(const QString &,const QString &);
    void addTabs();
    QComboBox * m_themes;
    QPushButton * m_editThemeButton;
    QPushButton * m_copyThemeButton;
    QPushButton * m_deleteThemeButton;
    QPushButton * m_newThemeButton;
    OptionsTabWidget  * m_tabs;
    int m_copyCount;
};
#endif
