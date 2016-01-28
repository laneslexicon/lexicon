#ifndef __OPTIONSDIALOG_H__
#define __OPTIONSDIALOG_H__
#include <QtCore>
#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
class OptionsDialog : public QDialog {
  Q_OBJECT

 public:
  //  OptionsDialog(QWidget * parent = 0);
  OptionsDialog(const QString & theme,QWidget * parent = 0);
  ~OptionsDialog();
  bool isModified() const;
  QStringList getChanges() const;
  public slots:
    void valueChanged(bool);
    void saveChanges();
    void resetChanges();
    void onClose();
    void currentChanged(int);
 private:
    void setup();
    void writeSettings();
    void enableButtons();
    //    void setApplyReset(bool);
    QDialogButtonBox * m_buttons;
    QTabWidget * m_tabs;
    QString m_theme;
    bool m_hasChanges;
    bool m_changed;
    bool m_debug;
    QString m_tempFileName;
  signals:
    void showHelp(const QString &);
    void hasChanges(bool);
};
#endif
