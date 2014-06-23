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

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addLayout(hlayout);
    layout->addWidget(m_browser);
    layout->addWidget(buttonBox);
    setLayout(layout);
}
NodeView::~NodeView() {
  qDebug() << Q_FUNC_INFO;
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
  m_browser->document()->setHtml(html);
  if (m_pattern.isEmpty())
    return;
  QTextCursor c = m_browser->document()->find(m_pattern,0);
  if (c.isNull()) {
    return;
  }
  c.select(QTextCursor::WordUnderCursor);
  m_browser->setTextCursor(c);
}
/*
QString NodeView::transform(const QString & xml) {
  int ok = compileStylesheet(1,m_xsltSource);
  if (ok == 0) {
    QString html = xsltTransform(1,xml);
    if (! html.isEmpty()) {
      return html;
    }
  }
  /// could be errors in stylesheet or in the xml
  QStringList errors = getParseErrors();
  if (ok != 0) {
    errors.prepend("Errors when processing stylesheet:");
  }
  else {
    errors.prepend("Errors when processing entry:");
  }
  QMessageBox msgBox;
  msgBox.setText(errors.join("\n"));
  msgBox.exec();
  clearParseErrors();
  return QString();
}
QString NodeView::getXml(const QString & node) {

}
*/
