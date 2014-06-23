#include "nodeview.h"
#include "application.h"

NodeView::NodeView(QWidget * parent)
  : QDialog(parent) {

  Lexicon * app = qobject_cast<Lexicon *>(qApp);
  QSettings * settings = app->getSettings();
  settings->beginGroup("System");
  QString fontString = settings->value("Arabic font").toString();
  delete settings;
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

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                      | QDialogButtonBox::Cancel);

  QPushButton * findFirst = new QPushButton(tr("Find first"));
  m_findNextButton = new QPushButton(tr("Find next"));

  buttonBox->addButton(findFirst,QDialogButtonBox::ActionRole);
  buttonBox->addButton(m_findNextButton,QDialogButtonBox::ActionRole);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(findFirst,SIGNAL(clicked()),this, SLOT(findFirst()));
  connect(m_findNextButton,SIGNAL(clicked()),this, SLOT(findNext()));
  layout->addLayout(hlayout);
  layout->addWidget(m_browser);
  layout->addWidget(buttonBox);
  setLayout(layout);
}
NodeView::~NodeView() {
  qDebug() << Q_FUNC_INFO;
}
void NodeView::setPattern(const QRegExp & rx) {
  m_pattern = rx;
}
QSize NodeView::sizeHint() const {
  return QSize(400,300);
}
void NodeView::accept() {
  QDialog::accept();
}
void NodeView::reject() {
  QDialog::reject();
}
void NodeView::setHeader(const QString & root,const QString & head) {
  m_rlabel->setText(root);
  m_hlabel->setText(head);
}
void NodeView::setCSS(const QString & css) {
  m_css = css;
  m_browser->document()->setDefaultStyleSheet(css);
}
void NodeView::setHtml(const QString & html) {
  m_findNextButton->setEnabled(false);
  m_browser->document()->setHtml(html);
  if (m_pattern.isEmpty())
    return;

  findFirst();
  QTextCursor c = m_browser->textCursor();
  if (c.isNull()) {
    return;
  }
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
