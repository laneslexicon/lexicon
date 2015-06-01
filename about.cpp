#include "about.h"
#include "version.h"
#include "definedsql.h"
#include "application.h"
#include "laneslexicon.h"
#include "externs.h"
#define EDIT_WIDTH 300

AboutDialog::AboutDialog(QWidget * parent) : QDialog(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  setWindowTitle(tr("About Lane's Lexicon"));
  setObjectName("aboutdialog");
  QTabWidget * tabs = new QTabWidget(this);
  tabs->setObjectName("aboutabs");
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
  foreach(QLineEdit *  widget,edits) {
    widget->setReadOnly(true);
  }
}
QSize AboutDialog::sizeHint() const {
  return QSize(600,400);
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
  layout->addRow(tr("Configuration file"),new QLineEdit(getLexicon()->settingsFileName()));
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
