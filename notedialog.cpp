#include "notedialog.h"
#include "laneslexicon.h"
extern LanesLexicon * getApp();
Note::Note() {
  m_id = -1;
}
void Note::save(QSqlDatabase * db) {
  qDebug() << Q_FUNC_INFO;
  if (m_id == -1) {
    QSqlQuery addQuery(*db);

    if (! addQuery.prepare("insert into notes (datasource,word,place,subject,note,created) \
           values (:datasource,:word,:place,:subject,:note,:created)")) {
      QLOG_WARN() << "SQL prepare error" << addQuery.lastError().text();
      return;
    }
    qDebug() << "note:" << m_note;
    addQuery.bindValue(":datasource",m_place.getSource());
    addQuery.bindValue(":word",m_place.getWord());
    addQuery.bindValue(":place",m_place.toString());
    addQuery.bindValue(":subject",m_subject);
    addQuery.bindValue(":note",m_note);
    addQuery.bindValue(":created",QDateTime::currentDateTime().toString());
    if (! addQuery.exec()) {
      QLOG_WARN() << "SQL exec error" << addQuery.lastError().text();
      return;
    }

    //      addQuery.bindValue(
  }
}
NoteDialog::NoteDialog(const Place & p,QWidget * parent) : QDialog(parent) {
  m_data.setPlace(p);
  m_place = p;
  QVBoxLayout * layout = new QVBoxLayout;
  m_subject = new QLineEdit(this);
  m_subject->setText(p.getShortText());
  m_note = new QTextEdit(this);

  layout->addWidget(m_subject);
  layout->addWidget(m_note);
  setLayout(layout);
  if (! m_subject->text().isEmpty()) {
    m_note->setFocus();
  }
  //  setWindowFlags(Qt::CustomizeWindowHint);
  setSizeGripEnabled(true);
  m_changed = false;
  m_autosave = false;
  connect(this,SIGNAL(saveNote(Note *)),getApp(),SLOT(saveNote(Note *)));
}
void NoteDialog::setModified(bool v) {
  m_changed = v;
  m_note->document()->setModified(v);
}
QSize NoteDialog::sizeHint() const {
  QSize sz(300,300);
  return sz;
}
void NoteDialog::closeEvent(QCloseEvent * event) {
  qDebug() << Q_FUNC_INFO << "modified" << this->isModified();
  if (this->isModified()) {
    if (m_autosave) {
        Note * n = new Note();
        n->setSubject(m_subject->text());
        n->setNote(m_note->toPlainText());
        n->setPlace(m_place);
        qDebug() << "emitted signal saveNote";
        qDebug() << "note:" << m_data.m_note;
        emit(saveNote(n));
    }
    else {
      QMessageBox msgBox;
      msgBox.setText("The document has been modified.");
      msgBox.setInformativeText("Do you want to save your changes?");
      msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Save);
      msgBox.setWindowModality(Qt::ApplicationModal);
      int ret = msgBox.exec();
      if (ret == QMessageBox::Save) {
        Note * n = new Note();
        n->setSubject(m_subject->text());
        n->setNote(m_note->toPlainText());
        n->setPlace(m_place);
        qDebug() << "emitted signal saveNote";
        qDebug() << "note:" << n->m_note;
        emit(saveNote(n));
      }
    }
  }
  QDialog::closeEvent(event);
}
NoteDialog::~NoteDialog() {
  qDebug() << Q_FUNC_INFO;
}
void NoteDialog::setSubject(const QString & text) {
  m_subject->setText(text);
  m_data.setSubject(text);
  m_changed = true;
}
bool NoteDialog::isModified() const {
  if (m_changed || m_note->document()->isModified())
    return true;

  return false;
}
