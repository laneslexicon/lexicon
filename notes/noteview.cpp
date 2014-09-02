#include "noteview.h"
#include "notebrowser.h"
#ifndef TEST_FRAMEWORK
#include "application.h"
#endif
#include "QsLog.h"
#include "notes.h"
extern NoteMaster * getNotes();
extern QSettings * getSettings();
NoteView::NoteView(QWidget * parent)
  : QDialog(parent) {
  QSettings * settings = ::getSettings();
  settings->setIniCodec("UTF-8");
  settings->beginGroup("System");
  QString fontString = settings->value("Arabic font").toString();
  settings->endGroup();
  //TODO different setting
  settings->beginGroup("FullSearch");
  QString sz = settings->value("Viewer size",QString()).toString();
  if (! sz.isEmpty())
    this->setPreferredSize(sz);

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

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);

  //  m_findFirstButton = new QPushButton(tr("&Find first"));
  //  m_findNextButton = new QPushButton(tr("Find &next"));

  QPushButton * printButton = new QPushButton(tr("&Print"));
  QPushButton * openButton = new QPushButton(tr("&Open in tab"));

  //  buttonBox->addButton(m_findFirstButton,QDialogButtonBox::ActionRole);
  //  buttonBox->addButton(m_findNextButton,QDialogButtonBox::ActionRole);
  buttonBox->addButton(openButton,QDialogButtonBox::ActionRole);
  buttonBox->addButton(printButton,QDialogButtonBox::ActionRole);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  //  connect(m_findFirstButton,SIGNAL(clicked()),this, SLOT(findFirst()));
  //  connect(m_findNextButton,SIGNAL(clicked()),this, SLOT(findNext()));
  connect(printButton,SIGNAL(clicked()),this,SLOT(print()));
  connect(openButton,SIGNAL(clicked()),this,SLOT(openEntry()));

  layout->addLayout(hlayout);
  layout->addWidget(m_browser);
  layout->addWidget(buttonBox);
  setLayout(layout);
}
NoteView::~NoteView() {
  qDebug() << Q_FUNC_INFO;
}
void NoteView::setPreferredSize(const QString & szStr) {
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

void NoteView::print() {

}
void NoteView::openEntry() {
  emit(openNode(m_node));
}
QSize NoteView::sizeHint() const {
  return m_size;
}
void NoteView::accept() {
  QDialog::accept();
}
void NoteView::reject() {
  QDialog::reject();
}
void NoteView::setCSS(const QString & css) {
  m_css = css;
  m_browser->document()->setDefaultStyleSheet(css);
}
void NoteView::setHtml(const QString & html) {
  m_browser->document()->setHtml(html);

}
void NoteView::setHeader(const QString & root,const QString & head,const QString & node) {
  m_rlabel->setText(root);
  m_hlabel->setText(head);
  m_node = node;
}
