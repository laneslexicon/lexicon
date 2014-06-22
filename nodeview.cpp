#include "nodeview.h"
NodeView::NodeView(const QString & html,QWidget * parent)
  : QDialog(parent) {

  QVBoxLayout * layout = new QVBoxLayout;
  m_browser = new QTextBrowser;
  m_browser->setHtml(html);

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    layout->addWidget(m_browser);
    layout->addWidget(buttonBox);
    setLayout(layout);
}
void NodeView::accept() {
  QDialog::accept();
}
void NodeView::reject() {
  QDialog::reject();
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
