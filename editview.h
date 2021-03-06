#ifndef __EDITVIEW_H__
#define __EDITVIEW_H__
#include <QDialog>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QScopedPointer>
#include <QSettings>
#include <QMap>
#include <QFile>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QTextStream>
#include <QGroupBox>
#include <QDebug>
#include <QMessageBox>
class EditPage : public QWidget {
  Q_OBJECT

 public:
  EditPage(int type,QWidget * parent = 0);
  enum EditType { Css, Xslt };
  QString getText() const;
  QString getOriginalText() const;
  bool writeFile();
  public slots:
    void onClicked(QAbstractButton *);
    void onTextChanged();
    void restore();
 private:
    void readSettings();

    void readFile(const QString &);
    void apply();
    void reset();

    int m_type;
    QString m_fileName;
    QStringList m_lines;
    QPlainTextEdit * m_text;
    QCheckBox * m_useOther;
    QDialogButtonBox * m_buttons;
    QLineEdit * m_infoName;
 signals:
    void apply(int,bool);
    void modified(int,bool);


};
class EditView : public QWidget {
  Q_OBJECT
 public:
  EditView(QWidget * parent = 0);
  ~EditView();
  public slots:
    void accept();
    void reject();
    void apply(int,bool);
    void modified(int,bool);
    void onHelp();
 protected:
  QSize sizeHint() const;
 private:
  EditPage * m_cssEditor;
  EditPage * m_xsltEditor;
  void readSettings();
  void writeSettings();
  QTabWidget * m_tabs;
  QDialogButtonBox * m_buttons;
  bool m_cssModified;
  bool m_xsltModified;
 signals:
      void reload(const QString &,const QString &);
      void showHelp(const QString &);
};
#endif
