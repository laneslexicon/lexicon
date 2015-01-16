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
  void setText(const QString &);
  public slots:
    void onClicked(QAbstractButton *);
    void onTextChanged();
 private:
    void readSettings();
    void readFile(const QString &);
    void apply();
    void reset();
    void restore();
    int m_type;
    QStringList m_lines;
    QPlainTextEdit * m_text;
    QCheckBox * m_useOther;
    QDialogButtonBox * m_buttons;


};
class EditView : public QWidget {
  Q_OBJECT
 public:
  EditView(QWidget * parent = 0);
 protected:
  QSize sizeHint() const;
 private:
  EditPage * m_cssEditor;
  EditPage * m_xsltEditor;
  void readSettings();
  QStringList readFile(const QString &);
  QTabWidget * m_tabs;
  QString m_cssFileName;
  QString m_xsltFileName;
  QStringList m_css;
  QStringList m_xslt;
};
#endif
