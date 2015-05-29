#include "nodeinfo.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
/**
 * This is used to show details of the linked-to node in a cross reference
 * The some settings are used by NodeView
 *
 * @param parent
 */
NodeInfo::NodeInfo(QWidget * parent)
  : QDialog(parent) {

  SETTINGS
  settings.beginGroup("Node");
  QString fontString = settings.value(SID_NODE_ARABIC_FONT).toString();
  QString sz = settings.value(SID_NODE_VIEWER_SIZE,QString()).toString();
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
void NodeInfo::setPlace(const Place & p) {
  m_place = p;
  m_rlabel->setText(p.getRoot());
  m_hlabel->setText(p.getWord());
  m_vlabel->setText(QString("(Vol %1,Page %2)").arg(p.getVol()).arg(p.getPage()));

  m_node = p.getNode();
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
