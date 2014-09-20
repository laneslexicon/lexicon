#include "entrylayoutdialog.h"
#include "laneslexicon.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
EntryLayoutDialog::EntryLayoutDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_tabs = new QTabWidget;
  m_cssEdit = new QPlainTextEdit;
  m_xsltEdit = new QPlainTextEdit;

  m_tabs->addTab(m_cssEdit,"CSS");
  m_tabs->addTab(m_xsltEdit,"XSLT");

  readSettings();
  m_cssEdit->setPlainText(m_css);
  m_xsltEdit->setPlainText(m_xslt);

  m_testButtons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Reset);
  m_stateButtons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Reset | QDialogButtonBox::Close);

  m_which = new QCheckBox("Both");
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_testButtons);
  hlayout->addStretch();
  hlayout->addWidget(m_stateButtons);
  layout->addWidget(m_tabs);
  layout->addWidget(m_which);
  layout->addLayout(hlayout);

  connect(m_testButtons,SIGNAL(clicked(QAbstractButton *)),this,SLOT(onTest(QAbstractButton *)));
  connect(m_stateButtons,SIGNAL(clicked(QAbstractButton *)),this,SLOT(onState(QAbstractButton *)));
  setLayout(layout);
  setModal(false);
  connect(m_tabs,SIGNAL(currentChanged(int)),this,SLOT(onTabChange(int)));
}
EntryLayoutDialog::~EntryLayoutDialog()    {
  qDebug() << Q_FUNC_INFO;
}
void EntryLayoutDialog::onTabChange(int ix) {
  if (ix == 0) {
    m_which->setText("Use Css only");
  }
  else {
    m_which->setText("Use Xslt only");
  }
}
QSize EntryLayoutDialog::sizeHint() const {
  return QSize(800,600);
}
void EntryLayoutDialog::onTest(QAbstractButton * btn) {
  if (btn == m_testButtons->button(QDialogButtonBox::Apply)) {
    QString css = m_cssEdit->toPlainText();
    QString xslt = m_xsltEdit->toPlainText();
    emit(reload(css,xslt));
  }
  else {
    emit(revert());
  }
}
void EntryLayoutDialog::onState(QAbstractButton * btn) {
  if (btn == m_stateButtons->button(QDialogButtonBox::Reset)) {
    m_cssEdit->setPlainText(m_css);
    m_xsltEdit->setPlainText(m_xslt);
  }
  else if (btn == m_stateButtons->button(QDialogButtonBox::Save)) {
  }
  else {
    ///
    this->hide();
  }
}

void EntryLayoutDialog::readSettings() {
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());

  settings->beginGroup("Entry");
  m_cssFileName = settings->value(SID_ENTRY_CSS,QString("entry.css")).toString();
  m_css = loadFromFile(0,m_cssFileName);
  m_xsltFileName = settings->value(SID_ENTRY_XSLT,QString("entry.xslt")).toString();
  m_xslt = loadFromFile(1,m_xsltFileName);
}
/**
 *
 *
 * @param type 0 for css, 1 for xslt
 * @param name filename
 *
 * @return
 */

QString EntryLayoutDialog::loadFromFile(int type,const QString & name) {
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
  QString line;
  while( ! in.atEnd()) {
    line = in.readLine();
    if (line.startsWith("-")) {
      if (type == 1) {
        txt += line;
        txt += "\n";
      }
    }
    else {
      txt += line;
      txt += "\n";
    }

  }
  f.close();
  return txt;
}
