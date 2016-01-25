#include "editview.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
EditPage::EditPage(int type,QWidget * parent) : QWidget(parent) {
  m_type = type;
  m_infoName = new QLineEdit;
  m_infoName->setReadOnly(true);
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_text = new QPlainTextEdit;
  if (type == EditPage::Css) {
    m_useOther = new QCheckBox(tr("Use edited XSLT"));
    setObjectName("editcss");
  }
  else {
    m_useOther = new QCheckBox(tr("Use edited CSS"));
    setObjectName("editxslt");
  }

  m_buttons =   new QDialogButtonBox(QDialogButtonBox::Apply|QDialogButtonBox::RestoreDefaults,this);
  connect(m_buttons,SIGNAL(clicked(QAbstractButton *)),this,SLOT(onClicked(QAbstractButton *)));
  connect(m_text,SIGNAL(textChanged()),this,SLOT(onTextChanged()));
  QPushButton * btn = m_buttons->button(QDialogButtonBox::Apply);
  if (btn) {
    btn->setText(tr("Apply changes to current tabs"));
  }
  btn = m_buttons->button(QDialogButtonBox::RestoreDefaults);
  if (btn) {
    btn->setText(tr("Reload original contents"));
  }
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(tr("File")));
  hlayout->addWidget(m_infoName);
  layout->addLayout(hlayout);
  layout->addWidget(m_text);
  layout->addWidget(m_useOther);
  layout->addWidget(m_buttons);
  setLayout(layout);
  this->restore();
}
QString EditPage::getText() const {
  return m_text->toPlainText();

}
QString EditPage::getOriginalText() const {
  return m_lines.join("\n");

}
void EditPage::apply() {
  emit(apply(m_type,m_useOther->isChecked()));
}
void EditPage::reset() {
  m_text->setPlainText(m_lines.join("\n"));
}
void EditPage::restore() {
  m_lines.clear();
  this->readFile(m_fileName);
  m_text->setPlainText(m_lines.join("\n"));
  emit(modified(m_type,false));
}

void EditPage::onClicked(QAbstractButton * btn) {
  QPushButton * button = qobject_cast<QPushButton *>(btn);
  if (! button )
    return;

  if (button == m_buttons->button(QDialogButtonBox::Apply)) {
    this->apply();
    return;
  }
  if (button == m_buttons->button(QDialogButtonBox::RestoreDefaults)) {
    this->restore();
    return;
  }
  if (button == m_buttons->button(QDialogButtonBox::Reset)) {
    this->reset();
    return;
  }
}
void EditPage::onTextChanged() {
  emit(modified(m_type,true));
}
void EditPage::readFile(const QString & name) {
  QLOG_DEBUG() << Q_FUNC_INFO << m_type << name;
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QLOG_WARN()  << QString(tr("Cannot open file %1:  %2")).arg(QDir::current().relativeFilePath(name)).arg(f.errorString());
    return;
  }
  m_infoName->setText(QString(tr("F(ile:%1")).arg(name));
  // TODO remove this when screenshot done
  /*
  if (m_type == EditPage::Css) {
    m_infoName->setText("/usr/home/andrewsg/Lexicon/Resources/themes/default/css/entry.css");
  }
  else {
    m_infoName->setText("/usr/home/andrewsg/Lexicon/Resources/themes/default/xslt/entry.xslt");
  }
  */
  QTextStream in(&f);
  while(! in.atEnd()) {
    m_lines << in.readLine();
  }
  f.close();
}
bool EditPage::writeFile() {
  QFile f(m_fileName);
  if (! f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QString msg = QString(tr("<p>Cannot open file %1 for writing: %2</p>")).arg(QDir::current().relativeFilePath(m_fileName)).arg(qPrintable(f.errorString()));
    QString title;
    if (m_type == EditPage::Css) {
      title = tr("Edit CSS");
    }
    else {
      title = tr("Edit XSLT");
    }
    QMessageBox::warning(this,
                         title,
                         msg +  tr("<p>Unable to save changes.<br/>You may wish to copy the text and save outside the application.</p>"),
                         QMessageBox::Ok);
    return false;
  }
  QTextStream out(&f);
  out << m_text->toPlainText();
  f.close();
  return true;
}

void EditPage::readSettings() {
  SETTINGS

  settings.beginGroup("EditView");
  QString style;
  style = settings.value(SID_EDITVIEW_FILE_STYLESHEET,QString()).toString();
  if (! style.isEmpty()) {
    m_infoName->setStyleSheet(style);
  }
  settings.endGroup();
  if (m_type == EditPage::Css) {
    settings.beginGroup("Entry");
    m_fileName = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();
    m_fileName = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,m_fileName);
  }
  else {
    settings.beginGroup("XSLT");
    m_fileName = settings.value(SID_XSLT_ENTRY,QString("entry.xslt")).toString();
    m_fileName = getLexicon()->getResourceFilePath(Lexicon::XSLT,m_fileName);

  }
  //  readFile(m_fileName);
}
/**
 * EditView
 *
 * @param parent
 */
EditView::EditView(QWidget * parent) : QWidget(parent) {
  setWindowTitle(tr("CSS/XSLT Editor"));
  setObjectName("editview");
  readSettings();
  QVBoxLayout * layout = new QVBoxLayout;
  m_tabs = new QTabWidget;
  m_tabs->setObjectName("edittabs");
  m_cssEditor = new EditPage(EditPage::Css);
  m_tabs->addTab(m_cssEditor,tr("CSS"));

  m_xsltEditor = new EditPage(EditPage::Xslt);
  m_tabs->addTab(m_xsltEditor,tr("XSLT"));
  m_buttons = new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel);

  QPushButton * helpbutton = m_buttons->addButton(QDialogButtonBox::Help);
  connect(helpbutton,SIGNAL(clicked()),this,SLOT(onHelp()));

  layout->addWidget(m_tabs);
  layout->addWidget(m_buttons);
  connect(m_buttons,SIGNAL(accepted()),this,SLOT(accept()));
  connect(m_buttons,SIGNAL(rejected()),this,SLOT(reject()));

  connect(m_cssEditor,SIGNAL(apply(int,bool)),this,SLOT(apply(int,bool)));
  connect(m_xsltEditor,SIGNAL(apply(int,bool)),this,SLOT(apply(int,bool)));
  connect(m_cssEditor,SIGNAL(modified(int,bool)),this,SLOT(modified(int,bool)));
  connect(m_xsltEditor,SIGNAL(modified(int,bool)),this,SLOT(modified(int,bool)));
  m_cssModified = false;
  m_xsltModified = false;
  m_buttons->button(QDialogButtonBox::Save)->setEnabled(m_cssModified || m_xsltModified);

  setLayout(layout);
}
EditView::~EditView() {
  writeSettings();
}
void EditView::accept() {
  if (m_cssEditor->writeFile() &&
      m_xsltEditor->writeFile()) {
    m_buttons->button(QDialogButtonBox::Save)->setEnabled(false);
    return;
  }

}
void EditView::reject() {
  this->hide();
}
void EditView::modified(int type,bool isDirty) {
  if (type == EditPage::Css) {
    m_cssModified = isDirty;
  }
  if (type == EditPage::Xslt) {
    m_xsltModified = isDirty;
  }
  m_buttons->button(QDialogButtonBox::Save)->setEnabled(m_cssModified || m_xsltModified);
}


QSize EditView::sizeHint() const {
  return QSize(800,600);
}
void EditView::writeSettings() {
  SETTINGS
  settings.beginGroup("EntryLayout");
  settings.setValue("Size", size());
  settings.setValue("Pos", pos());
  settings.endGroup();
  settings.sync();
}
void EditView::readSettings() {
  SETTINGS

  settings.beginGroup("EditView");
  move(settings.value(SID_EDITVIEW_POS,QPoint(400,150)).toPoint());
  resize(settings.value(SID_EDITVIEW_SIZE,QSize(800,60)).toSize());
  settings.endGroup();
}
void EditView::apply(int type,bool useEdited) {
  QString css;
  QString xslt;
  if (type == EditPage::Css) {
    css = m_cssEditor->getText();
    if (useEdited) {
      xslt = m_xsltEditor->getText();
    }
    else {
      xslt = m_xsltEditor->getOriginalText();
    }
  }
  if (type == EditPage::Xslt) {
    xslt = m_xsltEditor->getText();
    if (useEdited) {
      css = m_cssEditor->getText();
    }
    else {
      css = m_cssEditor->getOriginalText();
    }
  }
  if (! css.isEmpty() && ! xslt.isEmpty()) {
    emit(reload(css,xslt));
  }
}
void EditView::onHelp() {
  emit(showHelp(m_tabs->currentWidget()->metaObject()->className()));
}
