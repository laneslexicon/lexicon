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
#include "notes.h"
class NoteDialog : public QDialog {
  Q_OBJECT
 public:
  NoteDialog(const Place & p,QWidget * parent = 0);
  NoteDialog(Note *,QWidget * parent = 0);
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
