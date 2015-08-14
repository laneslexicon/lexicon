#include "about.h"
#include "version.h"
#include "definedsql.h"
#include "application.h"
#include "laneslexicon.h"
#include "externs.h"
#include "acknowledgements.h"
#define EDIT_WIDTH 300

AboutDialog::AboutDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  setWindowTitle(tr("About Lane's Lexicon"));
  setObjectName("aboutdialog");
  QTabWidget * tabs = new QTabWidget(this);
  tabs->setObjectName("aboutabs");
  tabs->addTab(this->buildAuthorTab(),tr("Author"));
  tabs->addTab(this->buildAckTab(),tr("Acknowledgements"));
  tabs->addTab(this->buildLicenseTab(),tr("License"));
  QWidget * tab;
  tabs->addTab(this->buildSystemTab(),tr("System"));
  tab = this->buildTechTab();
  if (tab) {
    tabs->addTab(tab,tr("Version"));
  }

  QDialogButtonBox *   buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

  layout->addWidget(tabs);
  layout->addWidget(buttonBox);
  setLayout(layout);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  getLexicon()->setCursorPosition(this,0);
  QList<QLineEdit *> edits = this->findChildren<QLineEdit *>();
  int size = 400;
  foreach(QLineEdit *  widget,edits) {
    widget->setReadOnly(true);
#ifdef Q_OS_OSX
    widget->setMinimumWidth(size);
#else
    widget->setMaximumWidth(size);
#endif
  }
}
QSize AboutDialog::sizeHint() const {
  return QSize(600,400);
}
QWidget * AboutDialog::buildAuthorTab() {
  QTextEdit * w = new QTextEdit(this);
  QString html = "<body><p>This program was written by Graeme Andrews. <br/><br/>The author can be contacted at <a href=\"mailto:<graeme@theunwalledcity.com\">graeme@theunwalledcity.com</a></p></body>";
  w->setHtml(html);
  w->setReadOnly(true);
  return w;
}
QWidget * AboutDialog::buildLicenseTab() {
  QFile f("license.txt");
  QString txt;
  if ( f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&f);
    in.setCodec("UTF-8");
    txt = in.readAll();
  }
  else {
    QLOG_WARN() << QString(tr("Missing license file: %1")).arg("license.txt");
    txt =QString(tr("Missing license file: %1")).arg("license.txt");
  }

  QPlainTextEdit * w = new QPlainTextEdit(this);
  w->setPlainText(txt);
  w->setReadOnly(true);
  return w;
  /// read license file
}
QWidget * AboutDialog::buildAckTab() {
  QTextBrowser * w = new QTextBrowser(this);
  w->setOpenExternalLinks(true);
  /*
  QString html =  \
"<ul>                           \
<li><p>The default Arabic font is <a href=\"www.amirifonts.org\">Amiri.</a></p></li> \
<li><p>The <a href=\"http://www.perseus.tufts.edu/hopper/\">Perseus Digital Library</a> at Tufts University, without which none of this would be possible.</p></li> \
<li><p>The logging system, 'QsLog' is by Ravzan Petru.</p></li> \
<li><p>Sotheby's, for permission to use the <a href=\"http://www.sothebys.com/en/auctions/ecatalogue/2014/arts-islamic-world-l14223/lot.59.html\">image</a> of the printers' manuscript copy of Lane's Lexicon</p></li> \
</ul>";
  */
  QString html = getAck();
  w->setHtml("<body><br/>" + html + "<br/><em>(Links will be opened in default browser.)</em></body>");
  w->setReadOnly(true);
  return w;
}
QWidget * AboutDialog::buildSystemTab() {
  QWidget * w = new QWidget(this);
  QFormLayout * layout = new QFormLayout;

  layout->addRow(tr("Database"),new QLineEdit(QSqlDatabase::database().databaseName()));
  layout->addRow(tr("Current theme"),new QLineEdit(getLexicon()->currentTheme()));
  layout->addRow(tr("Virtual keyboard"),new QLineEdit(getLexicon()->getDefaultKeyboard()));
  QCheckBox * keymaps = new QCheckBox;
  keymaps->setChecked(getApp()->keymapsEnabled());
  layout->addRow(tr("Keymaps enabled"),keymaps);
  layout->addRow(tr("Configuration file"),new QLineEdit(QDir::current().relativeFilePath(getLexicon()->settingsFileName())));
  w->setLayout(layout);
  return w;
}
QWidget * AboutDialog::buildTechTab() {
  QString sql(SQL_GET_INFO);

  QSqlQuery query;
  if (! query.prepare(sql)) {
    QLOG_WARN() << "SQL error preparing about info" << query.lastError().text();
    return NULL;
  }
  query.exec();
  if (! query.first()) {
    QLOG_WARN() << "SQL error retrieving about info" << query.lastError().text();
    return NULL;
  }
  QWidget * w = new QWidget(this);
  QFormLayout * layout = new QFormLayout;
  QLineEdit * edit;

  edit = new QLineEdit;
  edit->setText(query.value(1).toString());
  //  edit->setReadOnly(true);
  //  edit->setMinimumWidth(EDIT_WIDTH);
  layout->addRow(tr("Name"),edit);

  edit = new QLineEdit;
  edit->setText(query.value(2).toString());
  //  edit->setReadOnly(true);
  //  edit->setMinimumWidth(EDIT_WIDTH);
  layout->addRow(tr("Parser version"),edit);

  edit = new QLineEdit;
  edit->setText(query.value(3).toString());
  //  edit->setReadOnly(true);
  //  edit->setMinimumWidth(EDIT_WIDTH);
  layout->addRow(tr("Db creation date"),edit);

  edit = new QLineEdit;
  edit->setText(query.value(4).toString());
  //  edit->setReadOnly(true);
  //  edit->setMinimumWidth(EDIT_WIDTH);
  layout->addRow(tr("XML version"),edit);

  edit = new QLineEdit;
  edit->setText(query.value(5).toString());
  //  edit->setReadOnly(true);
  //  edit->setMinimumWidth(EDIT_WIDTH);
  layout->addRow(tr("Db ID"),edit);

  edit = new QLineEdit;
  edit->setText(buildVersion());
  //  edit->setReadOnly(true);
  //  edit->setMinimumWidth(EDIT_WIDTH);
  layout->addRow(tr("Build version"),edit);

   w->setLayout(layout);
  return w;
}
