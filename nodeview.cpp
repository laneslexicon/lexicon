#include "nodeview.h"
#include "application.h"

NodeView::NodeView(QWidget * parent)
  : QDialog(parent) {

  QScopedPointer<QSettings> settings((qobject_cast<Lexicon *>(qApp))->getSettings());
  settings->beginGroup("System");
  QString fontString = settings->value("Arabic font").toString();
  settings->endGroup();
  settings->beginGroup("FullSearch");
  QString sz = settings->value("Viewer size",QString()).toString();
  if (! sz.isEmpty())
    this->setPreferredSize(sz);

  m_startPosition = 0;
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

  m_findFirstButton = new QPushButton(tr("&Find first"));
  buttonBox->addButton(m_findFirstButton,QDialogButtonBox::ActionRole);
  connect(m_findFirstButton,SIGNAL(clicked()),this, SLOT(findFirst()));

  m_findNextButton = new QPushButton(tr("Find &next"));
  buttonBox->addButton(m_findNextButton,QDialogButtonBox::ActionRole);
  connect(m_findNextButton,SIGNAL(clicked()),this, SLOT(findNext()));

  button = new QPushButton(tr("&Print"));
  connect(button,SIGNAL(clicked()),this,SLOT(print()));
  buttonBox->addButton(button,QDialogButtonBox::ActionRole);

  button = new QPushButton(tr("&Open in tab"));
  buttonBox->addButton(button,QDialogButtonBox::ActionRole);
  connect(button,SIGNAL(clicked()),this,SLOT(openEntry()));

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));






  layout->addLayout(hlayout);
  layout->addWidget(m_browser);
  layout->addWidget(buttonBox);
  setLayout(layout);
}
NodeView::~NodeView() {
  QLOG_DEBUG() << Q_FUNC_INFO;
}
void NodeView::setPreferredSize(const QString & szStr) {
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

void NodeView::print() {
  qDebug() << Q_FUNC_INFO;
}
void NodeView::openEntry() {
  emit(openNode(m_node));
}
void NodeView::setPattern(const QRegExp & rx) {
  m_pattern = rx;
}
QSize NodeView::sizeHint() const {
  return m_size;
}
void NodeView::accept() {
  QDialog::accept();
}
void NodeView::reject() {
  QDialog::reject();
}
 void NodeView::setHeader(const QString & root,const QString & head,const QString & node) {
  m_rlabel->setText(root);
  m_hlabel->setText(head);
  m_node = node;
}
void NodeView::setCSS(const QString & css) {
  m_css = css;
  m_browser->document()->setDefaultStyleSheet(css);
}
void NodeView::setHtml(const QString & html) {
  int position;
  m_findNextButton->setEnabled(false);
  m_findFirstButton->setEnabled(false);
  m_browser->document()->setHtml(html);
  if (m_pattern.isEmpty())
    return;
  m_positionIndex = 0;
  m_positions.clear();
  QTextCursor c = m_browser->document()->find(m_pattern,0);
  while(!c.isNull()) {
    position = c.position();
    m_positions << (position - c.selectedText().size());
    c = m_browser->document()->find(m_pattern,position);
  }
  if (m_positions.size() > 0) {
    m_findFirstButton->setEnabled(true);
    m_findNextButton->setEnabled(true);
  }

  //  c = m_browser->document()->find(m_pattern,m_positions[0]);
  //  m_browser->setTextCursor(c);
  //  qDebug() << Q_FUNC_INFO << m_positions;
}
void NodeView::findFirst() {
  if (m_positions.size() == 0) {
    return;
  }
  if ((m_startPosition >= 0) && (m_startPosition < m_positions.size())) {
    m_positionIndex = m_startPosition;
  }
  m_positionIndex = 0;
  QTextCursor c = m_browser->document()->find(m_pattern,m_positions[m_positionIndex]);
  m_browser->setTextCursor(c);
  if (m_positionIndex == (m_positions.size() -1 )) {
    m_findNextButton->setEnabled(false);
  }
  else {
    m_findNextButton->setEnabled(true);
  }
}
void NodeView::findNext() {
  m_positionIndex++;
  if (m_positionIndex < m_positions.size()) {
    QTextCursor c = m_browser->document()->find(m_pattern,m_positions[m_positionIndex]);
    m_browser->setTextCursor(c);
  }
  if (m_positionIndex == (m_positions.size() -1 )) {
    m_findNextButton->setEnabled(false);
  }
  return;
}
void NodeView::setStartPosition(int pos) {
  m_startPosition = pos;
  m_positionIndex = 0;
}
