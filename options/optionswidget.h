#ifndef __OPTIONWIDGET_H__
#define __OPTIONWIDGET_H__
#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
class OptionsWidget : public QWidget {
  Q_OBJECT
 public:
  OptionsWidget(QSettings * settings = 0,QWidget * parent = 0);
  OptionsWidget(QWidget * parent = 0);
  ~OptionsWidget();
  virtual bool isModified();
  virtual void setupConnections();
  public slots:
    virtual void readSettings();
  virtual void stateChanged(int);
  virtual void writeSettings();
  //  virtual void reset();
  virtual void editingFinished();
   void textChanged(const QString &);
 protected:
  bool m_dirty;
  QSettings * m_settings;
  QString m_section;
  QLabel * m_info;
  void notifyChange();
 signals:
  void valueChanged();
  void modified(bool);
};
#endif
