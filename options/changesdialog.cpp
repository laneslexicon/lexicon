#include "changesdialog.h"
#include "QsLog.h"
ChangesDialog::ChangesDialog(bool cancel,QWidget * parent) : QDialog(parent) {
    QVBoxLayout * layout = new QVBoxLayout;
    setWindowTitle(tr("Save changes"));
    setWindowFlags(Qt::WindowStaysOnTopHint);
    layout->addWidget(new QLabel(tr("Some settings have changed. Do you want to save your changes?")));
    //    layout->addSpacing(20);
    //    layout->addWidget(new QLabel(tr("Do you want to save your changes?")));
    /*
    QGroupBox * group = new QGroupBox(tr("Set default and not show this dialog again"));
    QHBoxLayout * boxlayout = new QHBoxLayout;

    m_alwaysSave = new QCheckBox(tr("Always save"));
    m_alwaysDiscard = new QCheckBox(tr("Always discard"));

    boxlayout->addWidget(m_alwaysSave);
    boxlayout->addWidget(m_alwaysDiscard);
    group->setLayout(boxlayout);
    layout->addWidget(group);
    */
    m_always = new QCheckBox(tr("Make save/discard choice as permanent and not show this again"));
    layout->addWidget(m_always);
    QCheckBox * details = new QCheckBox("Show details");
    layout->addWidget(details);
    m_details = new QTableWidget;
    m_details->horizontalHeader()->setStretchLastSection(true);
    m_details->setColumnCount(4);
    m_details->setHorizontalHeaderLabels(QStringList() << tr("Section") << tr("Item") << tr("Old value") << tr("New value"));
    //    m_details->setRowCount(10);
    m_details->setVisible(false);
    layout->addWidget(m_details);
    connect(details,SIGNAL(stateChanged(int)),this,SLOT(onDetails(int)));

    QDialogButtonBox * box;
    QPushButton * savebtn = new QPushButton(tr("Save"));
    QPushButton * discardbtn = new QPushButton(tr("Discard"));

    if (cancel) {
      box = new QDialogButtonBox(QDialogButtonBox::Cancel);
    }
    else {
      box = new QDialogButtonBox();
    }
    box->addButton(savebtn,QDialogButtonBox::ActionRole);
    box->addButton(discardbtn,QDialogButtonBox::ActionRole);
    layout->addWidget(box);
    layout->addStretch();
    connect(savebtn,SIGNAL(clicked()),this,SLOT(onSave()));
    connect(discardbtn,SIGNAL(clicked()),this,SLOT(onDiscard()));
    connect(box,SIGNAL(rejected()),this,SLOT(onCancel()));
    setLayout(layout);
}
void ChangesDialog::setChanges(const QStringList & changes) {
  m_changes = changes;
  m_details->clearContents();
  m_details->setRowCount(0);
  int row = 0;
  int col;
  QStringList line;
  for(int i=0;i < changes.size();i++) {
    m_details->insertRow(row);
    line = changes[i].split("|",QString::KeepEmptyParts);
    if (line.size() > 1) {
      col = 1;
    }
    else {
      col = 0;
    }
    for(int j=0;j < line.size();j++) {
      m_details->setItem(row,j+col,new QTableWidgetItem(line[j]));
    }
    row++;
  }
  m_details->resizeColumnsToContents();
}
int ChangesDialog::choice() const {
  return m_ret;
}
bool ChangesDialog::always() const {
  return m_always->isChecked();
}
void ChangesDialog::onSave() {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_ret = QDialogButtonBox::Save;
 this->close();
}
void ChangesDialog::onCancel() {
  QLOG_DEBUG() << Q_FUNC_INFO;
 m_ret = QDialogButtonBox::Cancel;
 this->close();
}
void ChangesDialog::onDiscard() {
  QLOG_DEBUG() << Q_FUNC_INFO;
 m_ret = QDialogButtonBox::Discard;
 this->close();
}
void ChangesDialog::onDetails(int state) {
  QLOG_DEBUG() << Q_FUNC_INFO;
  m_details->setVisible(state == Qt::Checked);
  this->adjustSize();
}
QSize ChangesDialog::sizeHint() const {
  return QSize(400,100);
}
