#include "nodeview.h"
#include "application.h"
#include "definedsettings.h"
#include "externs.h"
#include "place.h"
#include "textsearch.h"
NodeView::NodeView(QWidget * parent)
  : QDialog(parent) {

  this->setup();
}
NodeView::NodeView(const SearchParams & p,QWidget * parent)
  : QDialog(parent) {
  m_params = p;
  this->setup();
  this->setHtml(p.html);
  m_positionIndex = 0;
  m_positions.clear();
  /// build index of positions
  ///
  QTextCursor c;
  QRegularExpression rx;
  if (p.regex) {
    rx.setPattern(p.pattern);
    c = m_browser->document()->find(rx,0,p.flags);
  }
  else {
    c = m_browser->document()->find(p.pattern,0,p.flags);
  }
  int position;
  while(!c.isNull()) {
    position = c.position();
    if (position == p.pos) {
      m_startPosition = m_positions.size();
    }
    m_positions << (position - c.selectedText().size());
    if (p.regex) {
      c = m_browser->document()->find(rx,position,p.flags);
    }
    else {
      c = m_browser->document()->find(p.pattern,position,p.flags);
    }

  }
  qDebug() << m_positions;
  if (m_positions.size() > 0) {
    m_findFirstButton->setEnabled(true);
    m_findNextButton->setEnabled(true);
  }
}
void NodeView::setup() {
  SETTINGS

  settings.beginGroup("Entry");
  QString css = settings.value(SID_ENTRY_CSS,QString("entry.css")).toString();
#ifdef LANE
  css = getLexicon()->getResourceFilePath(Lexicon::Stylesheet,css);
#else
  css = getSupport()->getResourceFilePath(Lexicon::Stylesheet,css);
#endif
  m_css = getSupport()->readTextFile(css);

  settings.endGroup();
  settings.beginGroup("Node");
  //  QString fontString = settings.value(SID_NODE_ARABIC_FONT).toString();
  QString sz = settings.value(SID_NODE_VIEWER_SIZE,QString()).toString();
  if (! sz.isEmpty())
    this->setPreferredSize(sz);

  m_startPosition = 0;
  setObjectName("nodeview");
  //  QFont f;
  //  if (! fontString.isEmpty())
  //    f.fromString(fontString);
  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  setSizeGripEnabled(true);
  QVBoxLayout * layout = new QVBoxLayout;
  m_rlabel = new QLabel("",this);
  //  m_rlabel->setFont(f);
  m_hlabel = new QLabel("",this);
  //  m_hlabel->setFont(f);
  m_pageLabel = new QLabel("",this);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(tr("Root"),this));
  hlayout->addWidget(m_rlabel);
  hlayout->addWidget(new QLabel(tr("Entry"),this));
  hlayout->addWidget(m_hlabel);
  hlayout->addWidget(m_pageLabel);
  hlayout->addStretch();

  m_browser = new QTextBrowser;
  m_browser->document()->setDefaultStyleSheet(m_css);
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
  emit(printNode(m_node));
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
void NodeView::setHeader(const QString & root,const QString & head,const QString & node,int page) {
#ifdef LANE
  m_rlabel->setText(getLexicon()->spanArabic(root,"nodeview"));
  m_hlabel->setText(getLexicon()->spanArabic(head,"nodeview"));
#else
  m_rlabel->setText(getSupport()->spanArabic(root,"nodeview"));
  m_hlabel->setText(getSupport()->spanArabic(head,"nodeview"));
#endif
  m_node = node;
  if (page > 0) {
    m_pageLabel->setText(QString("(v%1/%2)").arg(Place::volume(page)).arg(page));
  }
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
  /// build index of positions
  ///
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

}
/**
 * First time through we use startPosition so that the initial display is at the right occurrence
 * for when we are showing one occurrence per line in the results table
 */
void NodeView::findFirst() {
  if (m_positions.size() == 0) {
    return;
  }
  if ((m_startPosition >= 0) && (m_startPosition < m_positions.size())) {
    m_positionIndex = m_startPosition;
  }
  else {
  m_positionIndex = 0;
  }
  QTextCursor c;// = m_browser->document()->find(m_pattern,m_positions[m_positionIndex]);
  if (m_params.regex) {
    QRegularExpression rx(m_params.pattern);
    c = m_browser->document()->find(rx,m_positions[m_positionIndex],m_params.flags);
  }
  else {
    c = m_browser->document()->find(m_params.pattern,m_positions[m_positionIndex],m_params.flags);
  }
  m_browser->setTextCursor(c);
  if (m_positionIndex == (m_positions.size() -1 )) {
    m_findNextButton->setEnabled(false);
  }
  else {
    m_findNextButton->setEnabled(true);
  }
  m_startPosition = -1;
}
void NodeView::findNext() {
  m_positionIndex++;
  if (m_positionIndex < m_positions.size()) {
    QTextCursor c;// = m_browser->document()->find(m_pattern,m_positions[m_positionIndex]);
    if (m_params.regex) {
      QRegularExpression rx(m_params.pattern);
      c = m_browser->document()->find(rx,m_positions[m_positionIndex],m_params.flags);
    }
    else {
      c = m_browser->document()->find(m_params.pattern,m_positions[m_positionIndex],m_params.flags);
    }
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
