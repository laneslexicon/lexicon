#ifndef __EDITVIEW_H__
#define __EDITVIEW_H__
#include <QDialog>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QPushButton>
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
class EditPage : public QWidget {
  Q_OBJECT

 public:
  EditPage(int type,QWidget * parent = 0);
  QString getText() const;
  QString getOriginalText() const;
  void writeFile();
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
 signals:
      void apply(int,bool);


};
class EditView : public QWidget {
  Q_OBJECT
 public:
  EditView(QWidget * parent = 0);
  public slots:
    void accept();
    void reject();
    void apply(int,bool);
 protected:
  QSize sizeHint() const;
 private:
  EditPage * m_cssEditor;
  EditPage * m_xsltEditor;
  void readSettings();
  QTabWidget * m_tabs;
  QDialogButtonBox * m_buttons;
 signals:
      void reload(const QString &,const QString &);
};
#endif
