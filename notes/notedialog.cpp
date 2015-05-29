#include "notedialog.h"
#include "notes.h"
#include "imeditor.h"
#include "imlineedit.h"
#ifndef TEST_FRAMEWORK
#include "application.h"
#include "laneslexicon.h"
#include "externs.h"
#include "definedsettings.h"
#endif
extern NoteMaster * getNotes();
/**
 * This constructor is used when adding a new note
 *
 * @param p
 * @param parent
 */
NoteDialog::NoteDialog(const Place & p,QWidget * parent) : QDialog(parent) {
  m_id = -1;
  m_place = p;
  m_word = p.getWord();
  this->setup();
  m_noteType = Note::User;
  setObjectName("notedialog");
  /// set values from place
  if (m_noteType == Note::User) {
    m_typeUser->setChecked(true);
  }
  else {
    m_typeSystem->setChecked(true);
  }
  m_subject->setText(p.getShortText());
  m_note->setFocus();
  if (m_subject->text().isEmpty()) {
    setWindowTitle(tr("Add Note"));
  }
  else {
    setWindowTitle(m_subject->text());
  }
}
/**
 * This constructor is used when updating a note
 *
 * @param note
 * @param parent
 */
NoteDialog::NoteDialog(Note * note ,QWidget * parent) : QDialog(parent) {
  m_id = note->getId();
  m_place = note->getPlace();
  m_word = note->getWord();
  m_subjectText = note->getSubject();
  m_noteText = note->getNote();
  m_noteType = note->getType();

  this->setup();
  /// set values from note
  m_subject->setText(m_subjectText);
  m_note->edit()->setText(m_noteText);
  if (m_noteType == Note::User) {
    m_typeUser->setChecked(true);
  }
  else {
    m_typeSystem->setChecked(true);
  }
  m_note->setFocus();
  if (m_subject->text().isEmpty()) {
    setWindowTitle(tr("Add Note"));
  }
  else {
    setWindowTitle(m_subject->text());
  }
}
void NoteDialog::setup() {
  QVBoxLayout * mainlayout = new QVBoxLayout;
  //  QFormLayout * layout = new QFormLayout;
  m_subject = new ImLineEdit(this);

  m_note = new ImEditor(this);
  m_moreButton = new QPushButton(tr("&More"));
  m_moreButton->setCheckable(true);
  m_moreButton->setAutoDefault(false);

  m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel,Qt::Horizontal);
  //  m_buttonBox->addButton(new QPushButton("&Save"),QDialogButtonBox::AcceptRole);
  //  m_buttonBox->addButton(new QPushButton("&Cancel"),QDialogButtonBox::RejectRole);
  m_buttonBox->addButton(m_moreButton, QDialogButtonBox::ActionRole);

  m_printButton = new QPushButton(tr("&Print"));
  m_keyboardButton  = new QPushButton(tr("Show &keyboard"));
  m_keyboardButton->setAutoDefault(false);

  m_moreButtonBox = new QDialogButtonBox(Qt::Horizontal);
  m_moreButtonBox->addButton(m_keyboardButton, QDialogButtonBox::ActionRole);
  m_moreButtonBox->addButton(m_printButton, QDialogButtonBox::ActionRole);

  m_moreButtonBox->hide();
  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(showOptions(bool)));
  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(save()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  connect(m_keyboardButton, SIGNAL(clicked()),this,SLOT(showKeyboard()));
  connect(m_printButton,SIGNAL(clicked()),this,SLOT(print()));

  m_typeSystem = new QRadioButton(tr("System"),this);
  m_typeUser  = new QRadioButton(tr("User"),this);

  QHBoxLayout * typelayout = new QHBoxLayout;
  typelayout->addWidget(new QLabel(tr("Subject")));
  typelayout->addWidget(m_subject);
  typelayout->addWidget(new QLabel(tr("Note type")));
  typelayout->addWidget(m_typeUser);
  typelayout->addWidget(m_typeSystem);

  mainlayout->addLayout(typelayout);
  mainlayout->addWidget(m_note);
  mainlayout->addWidget(m_buttonBox);
  mainlayout->addWidget(m_moreButtonBox);

  setLayout(mainlayout);
  //  setWindowFlags(Qt::CustomizeWindowHint);
  setSizeGripEnabled(true);
  m_changed = false;
  m_autosave = false;
  setWindowTitle(m_subject->text());
  m_attached = false;
#ifndef TEST_FRAMEWORK
  SETTINGS
  settings.beginGroup("Keyboards");
  QString keyboardConfig = settings.value(SID_KEYBOARDS_CONFIG,"keyboard.ini").toString();
  m_keyboard = new KeyboardWidget(getLexicon()->getResourceFilePath(Lexicon::Keyboard),keyboardConfig);

  m_note->hideMaps();
  m_note->hidePrint();
  settings.endGroup();
  settings.beginGroup("Notes");
  m_note->hideMaps(settings.value(SID_NOTES_SHOW_KEYMAPS,false).toBool());
  m_note->hidePrint(settings.value(SID_NOTES_SHOW_PRINT,false).toBool());
  settings.endGroup();
#else
  m_keyboard = new KeyboardWidget(this);
#endif
  QPoint p = this->pos();

  int h = this->frameGeometry().height();

  m_keyboard->move(p.x(),p.y() + h);
  /* tried the following but it didn't work
  bool ok = disconnect(m_note->saveAction(), &QAction::triggered,
             m_note, &ImEditor::onSave );

  */
  disconnect(m_note->saveAction(), SIGNAL(triggered()),
                  m_note, SLOT(onSave()));
  connect(m_note->saveAction(),SIGNAL(triggered()),this,SLOT(save()));
  connect(m_note,SIGNAL(editFocus(int)),this,SLOT(focusChanged(int)));
  connect(m_subject,SIGNAL(gotFocus(int)),this,SLOT(focusChanged(int)));
  m_attachedEdit = NoteDialog::AttachNote;
}
void NoteDialog::showOptions(bool v) {
  if (v)
    m_moreButtonBox->show();
  else
    m_moreButtonBox->hide();
}
void NoteDialog::setModified(bool v) {
  m_changed = v;
  m_note->edit()->document()->setModified(v);
}
QSize NoteDialog::sizeHint() const {
  QSize sz(600,300);
  return sz;
}
void NoteDialog::closeEvent(QCloseEvent * event) {
  QLOG_DEBUG() << Q_FUNC_INFO << "modified" << this->isModified();
  if (this->isModified()) {
    if (m_autosave) {
      this->save();
    }
    else {
      QMessageBox msgBox;
      msgBox.setText("The note has been modified.");
      msgBox.setInformativeText("Do you want to save your changes?");
      msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Save);
      msgBox.setWindowModality(Qt::ApplicationModal);
      int ret = msgBox.exec();
      if (ret == QMessageBox::Save) {
        this->save();
      }
    }
  }

  QDialog::closeEvent(event);
}
NoteDialog::~NoteDialog() {
  QLOG_DEBUG() << Q_FUNC_INFO;

}
void NoteDialog::setSubject(const QString & text) {
  m_subject->setText(text);
  //  m_data.setSubject(text);
  m_changed = true;
}
bool NoteDialog::isModified() const {
  if (m_changed || m_note->edit()->document()->isModified())
    return true;

  return false;
}
void NoteDialog::showKeyboard() {
  if (! m_attached) {
    if (m_attachedEdit == NoteDialog::AttachNote) {
      m_keyboard->attach(m_note->edit());
    }
    else {
      m_keyboard->attach(m_subject);
    }
    m_keyboardButton->setText(tr("Hide &keyboard"));
    m_attached = true;
    this->positionKeyboard();
  }
  else {
    m_keyboard->detach();
    m_keyboardButton->setText(tr("Show &keyboard"));
    m_attached = false;
  }

}
void NoteDialog::positionKeyboard() {
    QPoint p;
    p = m_keyboard->currentPosition();
    if (p.isNull()) {
      p = this->pos();
      int h = this->frameGeometry().height();
      p.setX(p.x() - 50);
      p.setY(p.y() + h);
    }
    m_keyboard->move(p);
}
void NoteDialog::cancel() {
  m_note->edit()->setText(m_noteText);
  m_subject->setText(m_subjectText);
  m_changed = false;
  if (m_attached)
    showKeyboard();

  m_note->hideHelp();
  this->reject();
}
void NoteDialog::save() {
  Note * n = new Note();
  n->setPlace(m_place);
  n->setId(m_id);
  n->setWord(m_word);
  n->setSubject(m_subject->text());
  n->setNote(m_note->edit()->toPlainText());
  if (m_typeUser->isChecked()) {
      n->setType(Note::User);
  }
  else {
      n->setType(Note::System);
  }
  if (m_attached) {
    showKeyboard();
  }
  NoteMaster * notes = ::getNotes();
  bool ok = notes->save(n);
  delete n;
  if (ok) {
    m_changed = false;
    m_note->edit()->document()->setModified(false);
    m_subjectText = m_subject->text();
    m_noteText = m_note->edit()->toPlainText();
    emit(noteSaved(ok));
  }
  m_note->hideHelp();
  this->accept();
}
void NoteDialog::print() {
  QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();

  QPrinter * printer = new QPrinter();
  if (printers.size() == 0) {
    printer->setOutputFormat(QPrinter::PdfFormat);
    m_note->edit()->print(printer);
    delete printer;
    return;
  }

  QPrintDialog * d = new QPrintDialog(printer,this);
  d->setWindowModality(Qt::WindowModal);
  d->setWindowTitle("Set and Configure Printer");
  d->open(this,SLOT(onPrinterSetup()));
}
void NoteDialog::onPrinterSetup() {
  QPrintDialog * d = qobject_cast<QPrintDialog *>(sender());
  if (! d ) {
    return;
  }
  if (d->result() == QDialog::Rejected) {
    delete d;
    return;
  }
  QPrinter * printer = d->printer();
  m_note->edit()->print(printer);
  delete printer;

}
/*
void NoteDialog::onTypeChange(int ix) {
  if (ix == -1)
    return;

  int type = m_type->currentData().toInt();
  if (type != m_noteType) {
    m_changed = true;
  }
}
*/
void NoteDialog::focusChanged(int reason) {
  ImLineEdit * e = qobject_cast<ImLineEdit *>(sender());
  if (e){
    m_attachedEdit = NoteDialog::AttachSubject;
  }
  ImEditor * d = qobject_cast<ImEditor *>(sender());
  if (d) {
    m_attachedEdit = NoteDialog::AttachNote;
  }
  if ( m_attached) {
    if (m_attachedEdit == NoteDialog::AttachNote) {
      m_keyboard->attach(m_note->edit());
    }
    else {
      m_keyboard->attach(m_subject);
    }
  }
}
