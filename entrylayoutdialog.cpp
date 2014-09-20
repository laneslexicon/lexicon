#include "entrylayoutdialog.h"
#include "laneslexicon.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
EntryLayoutDialog::EntryLayoutDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_tabs = new QTabWidget;
  qDebug() << Q_FUNC_INFO << "start setup";
  m_cssEdit = new QPlainTextEdit;
  m_xsltEdit = new QPlainTextEdit;

  m_tabs->addTab(m_cssEdit,"CSS");
  m_tabs->addTab(m_xsltEdit,"XSLT");

  readSettings();
  m_cssEdit->setPlainText(m_css);
  m_xsltEdit->setPlainText(m_xslt);

  m_testButtons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Reset);
  m_stateButtons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Discard | QDialogButtonBox::Close);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_testButtons);
  hlayout->addStretch();
  hlayout->addWidget(m_stateButtons);
  layout->addWidget(m_tabs);
  layout->addLayout(hlayout);

  connect(m_testButtons,SIGNAL(clicked(QAbstractButton *)),this,SLOT(onTest(QAbstractButton *)));
  setLayout(layout);
  qDebug() << Q_FUNC_INFO << "done setup";
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
    qDebug() << "reset";
  }
}

void EntryLayoutDialog::readSettings() {
  QString txt;
  QString v;
  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QMap<QString,QString> cmdOptions = app->getOptions();
  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("Entry");

  v = settings->value(SID_ENTRY_CSS,QString("entry.css")).toString();
  txt = loadFromFile(0,v);
  m_css = txt;
  v = settings->value(SID_ENTRY_XSLT,QString("entry.xslt")).toString();
  m_xslt = loadFromFile(1,v);
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
