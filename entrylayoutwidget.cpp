#include "entrylayoutwidget.h"
#include "laneslexicon.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
EntryLayoutWidget::EntryLayoutWidget(QWidget * parent) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_tabs = new QTabWidget(this);
  m_cssEdit = new QPlainTextEdit(this);
  m_xsltEdit = new QPlainTextEdit(this);

  m_tabs->addTab(m_cssEdit,"CSS");
  m_tabs->addTab(m_xsltEdit,"XSLT");

  readSettings();
  m_cssEdit->setPlainText(m_css);
  m_xsltEdit->setPlainText(m_xslt);


  m_applyButton = new QPushButton("Apply",this);
  m_resetButton = new QPushButton("Reset",this);
  m_saveButton = new QPushButton("Save",this);
  m_restoreButton = new QPushButton("Restore",this);
  m_closeButton = new QPushButton("Close",this);

  m_testCssBox = new QCheckBox(tr("Css"),this);
  m_testXsltBox = new QCheckBox(tr("Xslt"),this);
  m_stateCssBox = new QCheckBox(tr("Css"),this);
  m_stateXsltBox = new QCheckBox(tr("Xslt"),this);

  QGroupBox * groupbox1 = new QGroupBox("Test the Css/Xslt",this);
  QHBoxLayout * group1layout = new QHBoxLayout;

  QVBoxLayout * boxeslayout1 = new QVBoxLayout;
  boxeslayout1->addWidget(m_testCssBox);
  boxeslayout1->addWidget(m_testXsltBox);

  QVBoxLayout * boxeslayout2 = new QVBoxLayout;
  boxeslayout2->addWidget(m_applyButton);
  boxeslayout2->addWidget(m_resetButton);

  group1layout->addLayout(boxeslayout1);
  group1layout->addLayout(boxeslayout2);

  groupbox1->setLayout(group1layout);

  QGroupBox * groupbox2 = new QGroupBox("Save/Restore",this);
  QHBoxLayout * group2layout = new QHBoxLayout;

  QVBoxLayout * boxeslayout3 = new QVBoxLayout;
  boxeslayout3->addWidget(m_stateCssBox);
  boxeslayout3->addWidget(m_stateXsltBox);

  QVBoxLayout * boxeslayout4 = new QVBoxLayout;
  boxeslayout4->addWidget(m_saveButton);
  boxeslayout4->addWidget(m_restoreButton);

  group2layout->addLayout(boxeslayout3);
  group2layout->addLayout(boxeslayout4);

  groupbox2->setLayout(group2layout);

  QVBoxLayout * closelayout = new QVBoxLayout;
  //  closelayout->addStretch();
  closelayout->addWidget(m_closeButton);

  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(groupbox1);
  hlayout->addWidget(groupbox2);
  hlayout->addLayout(closelayout);
  layout->addWidget(m_tabs);
  layout->addLayout(hlayout);


  m_saveButton->setEnabled(false);
  m_restoreButton->setEnabled(false);
  connect(m_applyButton,SIGNAL(clicked()),this,SLOT(onApply()));
  connect(m_resetButton,SIGNAL(clicked()),this,SLOT(onReset()));
  connect(m_saveButton,SIGNAL(clicked()),this,SLOT(onSave()));
  connect(m_restoreButton,SIGNAL(clicked()),this,SLOT(onRestore()));
  connect(m_closeButton,SIGNAL(clicked()),this,SLOT(onClose()));
  connect(m_cssEdit,SIGNAL(textChanged()),this,SLOT(onTextChanged()));
  connect(m_xsltEdit,SIGNAL(textChanged()),this,SLOT(onTextChanged()));
  setLayout(layout);
  //  setModal(false);
  //  readSettings();
}
EntryLayoutWidget::~EntryLayoutWidget()    {
  QLOG_DEBUG() << Q_FUNC_INFO;
  writeSettings();
}
QSize EntryLayoutWidget::sizeHint() const {
  return QSize(800,600);
}
void EntryLayoutWidget::onTextChanged() {
  bool buttonState = false;
  if ((m_css != m_cssEdit->toPlainText()) || (m_xslt != m_xsltEdit->toPlainText())) {// Edit->document()->isModified() || m_xsltEdit->document()->isModified() ) {
    buttonState = true;
  }
  m_saveButton->setEnabled(buttonState);
  m_restoreButton->setEnabled(buttonState);
}
void EntryLayoutWidget::onApply() {
  QString css;
  QString xslt;
  if (m_testCssBox->isChecked()) {
   css = m_cssEdit->toPlainText();
  }
  if (m_testXsltBox->isChecked()) {
    xslt = m_xsltEdit->toPlainText();
  }
  emit(reload(css,xslt));
}
void EntryLayoutWidget::onReset() {
    emit(revert());
}

void EntryLayoutWidget::onRestore() {
  if (m_testCssBox->isChecked()) {
    m_cssEdit->setPlainText(m_css);
  }
  if (m_testXsltBox->isChecked()) {
    m_xsltEdit->setPlainText(m_xslt);
  }
}
void EntryLayoutWidget::onSave() {
  if (m_css != m_cssEdit->toPlainText()) {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Css File"),
                           m_cssFileName,
                           tr("Css files (*.css)"));
    if (! fileName.isEmpty()) {
      QFile f(fileName);
      if (! f.open(QIODevice::WriteOnly)) {
        QLOG_WARN() << QString(tr("Cannot open file %1 for writing: %2")).arg(fileName).arg(qPrintable(f.errorString()));
      }
      else {
        QTextStream out(&f);
        out << m_cssEdit->toPlainText();
        f.close();
        m_css = m_cssEdit->toPlainText();
      }
    }
  }
  if (m_xslt != m_xsltEdit->toPlainText()) {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Xslt File"),
                           m_cssFileName,
                           tr("Xslt files (*.xslt)"));
    if (! fileName.isEmpty()) {
      QFile f(fileName);
      if (! f.open(QIODevice::WriteOnly)) {
        QLOG_WARN() << QString(tr("Cannot open file %1 for writing: %2")).arg(fileName).arg(qPrintable(f.errorString()));
      }
      else {
        QTextStream out(&f);
        out << m_xsltEdit->toPlainText();
        f.close();
        m_xslt = m_xsltEdit->toPlainText();
      }
    }
  }
  onTextChanged();
}
void EntryLayoutWidget::onClose() {
  qDebug() << Q_FUNC_INFO;
  this->hide();
}

void EntryLayoutWidget::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());

  settings->beginGroup("Entry");
  m_cssFileName = settings->value(SID_ENTRY_CSS,QString("entry.css")).toString();
  m_css = loadFromFile(0,m_cssFileName);
  m_xsltFileName = settings->value(SID_ENTRY_XSLT,QString("entry.xslt")).toString();
  m_xslt = loadFromFile(1,m_xsltFileName);
  settings->endGroup();
  settings->beginGroup("EntryLayout");
  this->restoreGeometry(settings->value("Geometry").toByteArray());

}
void EntryLayoutWidget::writeSettings() {
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("EntryLayout");
  settings->setValue("Geometry", saveGeometry());
}
/**
 *
 *
 * @param type 0 for css, 1 for xslt
 * @param name filename
 *
 * @return
 */

QString EntryLayoutWidget::loadFromFile(int type,const QString & name) {
  QString txt;
  QFile f(name);
  if (! f.open(QIODevice::ReadOnly)) {
    QString msg;
    msg = tr("Cannot open");
    if (type == 0) {
      msg = QString(tr("Cannot open CSS file"));
    }
    else {
      msg = QString(tr("Cannot open XSLT file"));
    }
    QLOG_WARN()  << QString("%1 %2: %3").arg(msg).arg(name).arg(f.errorString());
    return txt;
  }
  QTextStream in(&f);
  while( ! in.atEnd()) {
    txt += in.readLine();
    txt += "\n";
  }
  f.close();
  return txt;
}
