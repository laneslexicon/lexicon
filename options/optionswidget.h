#ifndef __OPTIONWIDGET_H__
#define __OPTIONWIDGET_H__
#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QColorDialog>
#include <QFontDialog>
#include <QKeySequence>
#include <QDateTimeEdit>
#define SMALL_EDIT 30
#define MEDIUM_EDIT 50
#define LARGE_EDIT 60


class OptionsWidget : public QWidget {
  Q_OBJECT
 public:
  OptionsWidget(QSettings * settings = 0,QWidget * parent = 0);
  OptionsWidget(const QString & theme,QWidget * parent = 0);
  OptionsWidget(QWidget * parent = 0);
  ~OptionsWidget();
  virtual bool isModified() = 0;
  virtual void setupConnections();
  virtual void addButtons();
  void setDebug(bool);
  void setFileName(const QString &);
  void blockAllSignals(bool block);
  virtual bool compare(const QSettings *,const QString & key, QWidget * p);
  public slots:
    virtual void onSave();
    virtual void onReset();
    virtual void readSettings() = 0;
    virtual void stateChanged(int);
    virtual void writeSettings(const QString & fileName = QString()) = 0;
    virtual void setButtons(bool);
    virtual void onHelp();
  //  virtual void reset();
  virtual void editingFinished();
  virtual void valueChanged(int);
  virtual void valueChanged(double);
  virtual void keySequenceChanged(const QKeySequence &);
  virtual void dateTimeChanged(const QDateTime &);
   void textChanged(const QString &);
 protected:
  bool m_dirty;
  QDialogButtonBox * m_btns;
  QSettings * m_settings;
  QString m_section;
  QString m_settingsFileName;
  QLabel * m_info;
  void notifyChange();
  bool m_debug;
 signals:
  void valueChanged();
  void modified(bool);
  void showHelp(const QString &);
};
#endif
