#include "columnselectdialog.h"
ColumnSelectDialog::ColumnSelectDialog(QMap<int,QString> cols,QWidget * parent) : QDialog(parent) {
  this->setup();
  QList<int> c = cols.keys();
  for(int i=0;i < c.size();i++) {
    m_grid->addWidget(new QLabel(cols.value(c[i])),i+1,0);
    m_cols << new QCheckBox;
    m_grid->addWidget(m_cols[i],i+1,1);
  }
}
ColumnSelectDialog::ColumnSelectDialog(const QStringList & cols,QWidget * parent) : QDialog(parent) {
  this->setup();
  for(int i=0;i < cols.size();i++) {
    m_grid->addWidget(new QLabel(cols[i]),i+1,0);
    m_cols << new QCheckBox;
    m_grid->addWidget(m_cols[i],i+1,1);
  }
}
void ColumnSelectDialog::setup() {
  QVBoxLayout * layout = new QVBoxLayout;
  m_grid = new QGridLayout;


  m_grid->addWidget(new QLabel(tr("Column")),0,0);
  m_grid->addWidget(new QLabel(tr("Visible")),0,1);

  QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  connect(buttons,SIGNAL(accepted()),this,SLOT(accept()));
  connect(buttons,SIGNAL(rejected()),this,SLOT(reject()));

  setWindowTitle(tr("Select columns to show"));

  layout->addLayout(m_grid);
  layout->addStretch();
  layout->addWidget(buttons);
  setLayout(layout);
}
void ColumnSelectDialog::setState(QList<bool> v) {
  for(int i=0;i < v.size();i++) {
    m_cols[i]->setChecked(v[i]);
  }
}
QList<bool> ColumnSelectDialog::state() const {
  QList<bool> c;
  for(int i=0;i < m_cols.size();i++) {
    //    QCheckBox * b = qobject_cast<QCheckBox *>(m_grid->itemAtPosition(i,1));
    c << m_cols[i]->isChecked();
  }
  return c;
}
