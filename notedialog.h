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
#include "imedit.h"
#include "keyboardwidget.h"

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
  QString getNote() { return m_note->toPlainText();}
  void setAutosave(bool v) { m_autosave = v;}
  public slots:
    void showOptions(bool);
    void showKeyboard();
    void cancel();
    void save();
    void print();
 protected:
  void closeEvent(QCloseEvent *);
 private:
  void setup();
  bool m_attached;
  bool m_changed;
  bool m_autosave;

  Place m_place;
  KeyboardWidget * m_keyboard;
  QLineEdit * m_subject;
  QTextEdit * m_note;
  QLineEdit * m_tags;
  QPushButton * m_keyboardButton;
  QPushButton * m_moreButton;
  QPushButton * m_printButton;
  QDialogButtonBox * m_moreButtonBox;
  QDialogButtonBox * m_buttonBox;
  Note * m_noteItem;
  QString m_subjectText;
  QString m_noteText;
  QString m_word;
  int m_id;
 signals:
  void saveNote(Note *);
};
#endif
