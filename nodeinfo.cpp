#include "nodeinfo.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
/**
 * This is used to show details of the linked-to node in a cross reference
 * and by HeadSearch to view an entry's details
 * The same settings are used by NodeView
 *
 * @param parent
 */
NodeInfo::NodeInfo(QWidget * parent)
  : QDialog(parent) {

  setObjectName("nodeinfo");

  SETTINGS
  settings.beginGroup("Node");
  QString sz = settings.value(SID_NODE_VIEWER_SIZE,QString()).toString();
  if (! sz.isEmpty())
    this->setPreferredSize(sz);

  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  setSizeGripEnabled(true);
  QVBoxLayout * layout = new QVBoxLayout;
  m_rlabel = new QLabel("",this);
  m_hlabel = new QLabel("",this);
  m_vlabel = new QLabel("",this);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(tr("Root"),this));
  hlayout->addWidget(m_rlabel);
  hlayout->addWidget(new QLabel(tr("Entry"),this));
  hlayout->addWidget(m_hlabel);
  hlayout->addSpacing(10);
  hlayout->addWidget(m_vlabel);
  hlayout->addStretch();


  m_browser = new QTextBrowser;

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);

  QPushButton * button = new QPushButton(tr("&Show in tab"));


  buttonBox->addButton(button,QDialogButtonBox::ActionRole);
  connect(button,SIGNAL(clicked()),this,SLOT(openEntry()));

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QHBoxLayout * boxlayout = new QHBoxLayout;
  m_closeOnTabLoad = new QCheckBox(tr("Close after load"));
  m_alwaysLoad = new QCheckBox(tr("Load in new tab"));
  boxlayout->addWidget(m_closeOnTabLoad);
  boxlayout->addWidget(m_alwaysLoad);
  boxlayout->addWidget(buttonBox);

  layout->addLayout(hlayout);
  layout->addWidget(m_browser);
  layout->addLayout(boxlayout);
  setLayout(layout);

}
NodeInfo::~NodeInfo() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void NodeInfo::setCloseOnLoad(bool v) {
  m_closeOnTabLoad->setChecked(v);
}
void NodeInfo::setAlwaysLoad(bool v) {
  m_alwaysLoad->setChecked(v);
}
bool NodeInfo::alwaysLoad() const {
  return m_alwaysLoad->isChecked();
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
void NodeInfo::showForce(bool v) {
  m_alwaysLoad->setVisible(v);
}
void NodeInfo::openEntry() {
  emit(openNode(m_node,m_alwaysLoad->isChecked()));
  if (m_closeOnTabLoad->isChecked()) {
    this->accept();
  }
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
void NodeInfo::setPlace(const Place & p) {
  m_place = p;
  this->setHeader(p);
}
void NodeInfo::setCss(const QString & css) {
  m_css = css;
  m_browser->document()->setDefaultStyleSheet(css);
}
void NodeInfo::setHtml(const QString & html) {
  m_browser->document()->setHtml(html);
  QTextCursor cursor = m_browser->textCursor();
  cursor.setPosition(0);
  m_browser->setTextCursor(cursor);
}
void NodeInfo::setHeader(const QString & root,const QString & head,const QString & node,int page) {
  m_rlabel->setText(getLexicon()->spanArabic(root,"nodeview"));
  m_hlabel->setText(getLexicon()->spanArabic(head,"nodeview"));
  m_node = node;
  if (page > 0) {
    m_vlabel->setText(QString("(Vol v%1/%2)").arg(Place::volume(page)).arg(page));
  }
}
void NodeInfo::setHeader(const Place & p) {
  m_rlabel->setText(getLexicon()->spanArabic(p.root(),"nodeview"));
  m_hlabel->setText(getLexicon()->spanArabic(p.word(),"nodeview"));
  m_node = p.node();
  QString head;
  if (p.head() != p.word()) {
    head = QString(tr(", Head word: %1")).arg(p.head());
  }
  QString str = QString("%1%2").arg(p.format("Vol %V/%P")).arg(head);
  m_vlabel->setText(getLexicon()->scanAndStyle(str,"nodeview"));
}
