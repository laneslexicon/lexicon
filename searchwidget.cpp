#include "searchwidget.h"
SearchWidget::SearchWidget(QWidget * parent) : QWidget(parent) {
  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new WrappedEdit;

  m_cancelBtn = new QPushButton(tr("Cancel"));
  layout->addWidget(m_edit);
  layout->addWidget(m_cancelBtn);

  connect(m_cancelBtn,SIGNAL(clicked()),this,SLOT(onCancel()));
  setLayout(layout);
}
void SearchWidget::onCancel() {
  close();
}

RootSearchDialog::RootSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for root"));

  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new WrappedEdit;
  m_edit->setSz(QSize(300,30));
  m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


  layout->addWidget(m_edit,0);
  layout->addStretch(1);
  layout->addWidget(m_buttonBox);

  setModal(true);
  setLayout(layout);
}
void RootSearchDialog::onCancel() {
  //  reject();
}
QString RootSearchDialog::getText() {
  return m_edit->getText();
}
NodeSearchDialog::NodeSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for node"));
  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new WrappedEdit;
  m_edit->setSz(QSize(300,30));
  m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


  layout->addWidget(m_edit,0);
  layout->addStretch(1);
  layout->addWidget(m_buttonBox);

  setModal(true);
  setLayout(layout);
}
QString NodeSearchDialog::getText() {
  return m_edit->editor()->toPlainText();
}
/**
 *
 *
 * @param parent
 * @param f
 */
WordSearchDialog::WordSearchDialog(QWidget * parent,Qt::WindowFlags f) :
  QDialog(parent,f) {
  setWindowTitle(tr("Search for word"));
  QVBoxLayout * layout = new QVBoxLayout;
  m_edit = new WrappedEdit;
  m_edit->setSz(QSize(300,30));
  m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

  connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


  layout->addWidget(m_edit,0);
  layout->addStretch(1);
  layout->addWidget(m_buttonBox);

  setModal(true);
  setLayout(layout);
}
QString WordSearchDialog::getText() {
  /// getText() converts buckwalter to arabic
  return m_edit->getText();
}
