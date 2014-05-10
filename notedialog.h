#ifndef __NOTEDIALOG_H__
#define __NOTEDIALOG_H__
#include <QGridLayout>
#include <QFormLayout>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDebug>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QLabel>
#include <QToolButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "QsLog.h"
#include "place.h"

class Note  {
 public:
  Note();
  void setSubject(const QString & text) { m_subject = text;};
  void setPlace(const Place & p) { m_place = p;}
  void setNote(const QString & n) { m_note = n;}
  QString m_note;
  QString m_subject;
  Place m_place;
  QString m_created;
  void save(QSqlDatabase *);
 private:
  int m_id;
};
class NoteDialog : public QDialog {
  Q_OBJECT
 public:
  NoteDialog(const Place & p,QWidget * parent = 0);
  ~NoteDialog();
  QSize sizeHint() const;
  void setSubject(const QString &);
  void setModified(bool);
  bool isModified() const;
  Note getNote() { return m_data;}
 protected:
  void closeEvent(QCloseEvent *);
 private:
  Note m_data;
  bool m_changed;
  bool m_autosave;
  int m_id;
  Place m_place;
  QLineEdit * m_subject;
  QTextEdit * m_note;
  QLineEdit * m_tags;
  QPushButton * m_keyboardButton;
  QPushButton * m_moreButton;
  QDialogButtonBox * m_buttonBox;
 signals:
  void saveNote(Note *);
};
#endif
