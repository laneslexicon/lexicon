#include "notedialog.h"
NoteDialog::NoteDialog(const Place & p,QWidget * parent) : QDialog(parent) {
  m_data.setPlace(p);
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
      m_data.setSubject(m_subject->text());
      m_data.setNote(m_note->toHtml());
      emit(saveNote(m_data));
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
        m_data.setSubject(m_subject->text());
        m_data.setNote(m_note->toHtml());
        qDebug() << "emitted signal saveNote";
        emit(saveNote(m_data));
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
