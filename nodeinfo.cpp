#include "nodeinfo.h"
#include "application.h"

NodeInfo::NodeInfo(QWidget * parent)
  : QDialog(parent) {

  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("System");
  QString fontString = settings->value("Arabic font").toString();
  settings->endGroup();
  settings->beginGroup("FullSearch");
  QString sz = settings->value("Viewer size",QString()).toString();
  if (! sz.isEmpty())
    this->setPreferredSize(sz);

  QFont f;
  if (! fontString.isEmpty())
    f.fromString(fontString);
  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  setSizeGripEnabled(true);
  QVBoxLayout * layout = new QVBoxLayout;
  m_rlabel = new QLabel("",this);
  m_rlabel->setFont(f);
  m_hlabel = new QLabel("",this);
  m_hlabel->setFont(f);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(tr("Root"),this));
  hlayout->addWidget(m_rlabel);
  hlayout->addWidget(new QLabel(tr("Entry"),this));
  hlayout->addWidget(m_hlabel);
  hlayout->addStretch();

  m_browser = new QTextBrowser;
  //  m_browser->setHtml(html);

  QDialogButtonBox * buttonBox = new QDialogButtonBox();

  QPushButton * button = new QPushButton(tr("&Cancel"));
  buttonBox->addButton(button,QDialogButtonBox::RejectRole);

  /*
  button = new QPushButton(tr("&Print"));
  connect(button,SIGNAL(clicked()),this,SLOT(print()));
  buttonBox->addButton(button,QDialogButtonBox::ActionRole);
  */
  button = new QPushButton(tr("&Go"));
  buttonBox->addButton(button,QDialogButtonBox::ActionRole);
  connect(button,SIGNAL(clicked()),this,SLOT(openEntry()));

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  layout->addLayout(hlayout);
  layout->addWidget(m_browser);
  layout->addWidget(buttonBox);
  setLayout(layout);
}
NodeInfo::~NodeInfo() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void NodeInfo::setPreferredSize(const QString & szStr) {
  m_size.setWidth(400);
  m_size.setHeight(400);
  QRegExp rx("(\\d+)x(\\d+)");
  if ((rx.indexIn(szStr) != -1) && (rx.captureCount() == 2)) {
    bool ok;
    int w = rx.cap(1).toInt(&ok);
    if (ok) {
      int h = rx.cap(2).toInt(&ok);
      if (ok) {
        m_size.setWidth(w);
        m_size.setHeight(h);
      }
    }
  }
}
/*
void NodeInfo::print() {
  qDebug() << Q_FUNC_INFO;
}
*/
void NodeInfo::openEntry() {
  emit(openNode(m_node));
}
QSize NodeInfo::sizeHint() const {
  return m_size;
}
void NodeInfo::accept() {
  QDialog::accept();
}
void NodeInfo::reject() {
  QDialog::reject();
}
 void NodeInfo::setHeader(const QString & root,const QString & head,const QString & node) {
  m_rlabel->setText(root);
  m_hlabel->setText(head);
  m_node = node;
}
void NodeInfo::setCss(const QString & css) {
  m_css = css;
  m_browser->document()->setDefaultStyleSheet(css);
}
void NodeInfo::setHtml(const QString & html) {
  m_browser->document()->setHtml(html);
}
