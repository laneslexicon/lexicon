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

  m_startPosition = -1;
  QFont f;
  if (! fontString.isEmpty())
    f.fromString(fontString);
  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  setSizeGripEnabled(true);
  QVBoxLayout * layout = new QVBoxLayout;
  m_rlabel = new QLabel("");
  m_rlabel->setFont(f);
  m_hlabel = new QLabel("");
  m_hlabel->setFont(f);
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel(tr("Root")));
  hlayout->addWidget(m_rlabel);
  hlayout->addWidget(new QLabel(tr("Entry")));
  hlayout->addWidget(m_hlabel);
  hlayout->addStretch();

  m_browser = new QTextBrowser;
  //  m_browser->setHtml(html);

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);

  m_findFirstButton = new QPushButton(tr("&Find first"));
  m_findNextButton = new QPushButton(tr("Find &next"));

  QPushButton * printButton = new QPushButton(tr("&Print"));
  QPushButton * openButton = new QPushButton(tr("&Open in tab"));

  buttonBox->addButton(m_findFirstButton,QDialogButtonBox::ActionRole);
  buttonBox->addButton(m_findNextButton,QDialogButtonBox::ActionRole);
  buttonBox->addButton(openButton,QDialogButtonBox::ActionRole);
  buttonBox->addButton(printButton,QDialogButtonBox::ActionRole);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(m_findFirstButton,SIGNAL(clicked()),this, SLOT(findFirst()));
  connect(m_findNextButton,SIGNAL(clicked()),this, SLOT(findNext()));
  connect(printButton,SIGNAL(clicked()),this,SLOT(print()));
  connect(openButton,SIGNAL(clicked()),this,SLOT(openEntry()));

  layout->addLayout(hlayout);
  layout->addWidget(m_browser);
  layout->addWidget(buttonBox);
  setLayout(layout);
}
NodeView::~NodeView() {
  qDebug() << Q_FUNC_INFO;
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
  m_findNextButton->setEnabled(false);
  m_findFirstButton->setEnabled(false);
  m_browser->document()->setHtml(html);
  if (m_pattern.isEmpty())
    return;

  findFirst();
  QTextCursor c = m_browser->textCursor();
  if (c.isNull()) {
    return;
  }
  if (m_startPosition == -1) {
    c.setPosition(0);
    m_browser->setTextCursor(c);
    return;
  }
  m_findFirstButton->setEnabled(true);
  c.setPosition(m_startPosition);
  c.movePosition(QTextCursor::PreviousCharacter,QTextCursor::MoveAnchor);
  c.select(QTextCursor::WordUnderCursor);
  m_browser->setTextCursor(c);
  c = m_browser->document()->find(m_pattern,c.position());
  if (! c.isNull())
      m_findNextButton->setEnabled(true);

}
void NodeView::findFirst() {
  QTextCursor c = m_browser->document()->find(m_pattern,0);
  if (c.isNull()) {
    return;
  }
  /// the find positions the cursor at the end, so move back one carh
  /// and select the word
  /// then set m_cursor at the next occurence (if any)
  c.movePosition(QTextCursor::PreviousCharacter,QTextCursor::MoveAnchor);
  c.select(QTextCursor::WordUnderCursor);
  m_browser->setTextCursor(c);
  c.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor);
  m_cursor = m_browser->document()->find(m_pattern,c.position());
  if (m_cursor.isNull())
      m_findNextButton->setEnabled(false);
  else
      m_findNextButton->setEnabled(true);

}
void NodeView::findNext() {
  if (m_cursor.isNull())
    return;

  m_cursor.movePosition(QTextCursor::PreviousCharacter,QTextCursor::MoveAnchor);
  m_cursor.select(QTextCursor::WordUnderCursor);
  m_browser->setTextCursor(m_cursor);
  m_cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor);
  m_cursor = m_browser->document()->find(m_pattern,m_cursor.position());
  if (m_cursor.isNull())
      m_findNextButton->setEnabled(false);
  else
      m_findNextButton->setEnabled(true);
}
void NodeView::setStartPosition(int pos) {
  m_startPosition = pos;
}
