#include "bookmarkwidget.h"
#include "place.h"
#include "QsLog.h"
#include "htmldelegate.h"
#include "application.h"
#include "externs.h"
#define KEY_COLUMN 0
#define ROOT_COLUMN 1
#define WORD_COLUMN 2
#define VOL_COLUMN 3
#define NODE_COLUMN 4
BookmarkWidget::BookmarkWidget(const QMap<QString,Place> & marks,QWidget * parent)
  : QDialog(parent) {
  readSettings();
  setWindowTitle(tr("Current Bookmarks"));
  QStringList keys = marks.keys();
  keys.removeOne("-here-");
  m_list = new QTableWidget(keys.size(),5);
  m_list->setObjectName("bookmarklist");
  //  HtmlDelegate * d = new HtmlDelegate(m_list);
  //  d->setStyleSheet(".ar { font-family : Amiri;font-size : 16px}");
  //  m_list->setItemDelegate(d);

  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);

  m_list->setHorizontalHeaderLabels(QStringList() << tr("Id") << tr("Root") << tr("Entry") << tr("Vol/Page") << tr("Node"));
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QTableWidgetItem * item;
  for(int i=0;i < keys.size();i++) {
    item = new QTableWidgetItem(QString("%1").arg(keys[i]));
      m_list->setItem(i,KEY_COLUMN,item);
      Place p = marks.value(keys[i]);
      //      QString html = qobject_cast<Lexicon *>(qApp)->scanAndSpan(p.getText());
      item = new QTableWidgetItem(p.getRoot());
      item->setFont(m_arFont);
      m_list->setItem(i,ROOT_COLUMN,item);
      item = new QTableWidgetItem(p.getWord());
      item->setFont(m_arFont);
      m_list->setItem(i,WORD_COLUMN,item);
      item = new QTableWidgetItem(QString(tr("V%1/%2")).arg(p.getVol()).arg(p.getPage()));
      m_list->setItem(i,VOL_COLUMN,item);
      item = new QTableWidgetItem(p.getNode());
      m_list->setItem(i,NODE_COLUMN,item);
  }
  if (keys.size() > 0) {
    m_list->selectRow(0);
  }
  if (! m_debug) {
    m_list->hideColumn(NODE_COLUMN);
  }
  m_newTab = new QCheckBox(tr("Open in new tab"));
  m_switchTab = new QCheckBox(tr("Switch to new tab"));
  SETTINGS
  settings.beginGroup("Bookmark");
  m_newTab->setChecked(settings.value("New tab",false).toBool());
  m_switchTab->setChecked(settings.value("Switch tab",true).toBool());
  m_switchTab->setEnabled(m_newTab->isChecked());
  connect(m_newTab,SIGNAL(stateChanged(int)),this,SLOT(onStateChanged(int)));

  QPushButton * jumpButton = new QPushButton(tr("&Jump to"));
  QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
  if (keys.size() == 0) {
    jumpButton->setEnabled(false);
  }
  QDialogButtonBox *   buttonBox = new QDialogButtonBox();//QDialogButtonBox::Cancel);
  buttonBox->addButton(jumpButton,QDialogButtonBox::AcceptRole);
  buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(setPlace()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(m_list,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  //  connect(m_list,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  //  connect(m_list,SIGNAL(itemPressed(QTableWidgetItem *)),this,SLOT(jump(QTableWidgetItem *)));
  QVBoxLayout * layout = new QVBoxLayout;
  QHBoxLayout * hlayout = new QHBoxLayout;
  hlayout->addWidget(m_newTab);
  hlayout->addWidget(m_switchTab);
  hlayout->addStretch();
  hlayout->addWidget(buttonBox);

  layout->addWidget(m_list);
  layout->addLayout(hlayout);
  setLayout(layout);
  m_list->installEventFilter(this);
}
void BookmarkWidget::setPlace() {
  int row = m_list->currentRow();
  QLOG_DEBUG() << Q_FUNC_INFO << row;
  QTableWidgetItem * item = m_list->item(row,KEY_COLUMN);
  if (item) {
    QString t = item->text();
    m_mark = item->text();
  }
  this->accept();
}
void BookmarkWidget::jump(QTableWidgetItem * /* item */) {
  //  QLOG_DEBUG() << Q_FUNC_INFO << item->row();
  this->setPlace();
}
bool BookmarkWidget::eventFilter(QObject * target ,QEvent * event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space: {
      this->setPlace();
        break;
      }
    default:
      break;
    }
  }
  return QWidget::eventFilter(target,event);
}
QSize BookmarkWidget::sizeHint() const {
  return QSize(600,300);
}
void BookmarkWidget::readSettings() {
  SETTINGS
  settings.beginGroup("Bookmark");
  settings.beginGroup("List");
  QString fontString = settings.value("Arabic font").toString();
  if ( ! fontString.isEmpty()) {
    m_arFont.fromString(fontString);
  }
  settings.endGroup();
  m_debug = settings.value("Debug",false).toBool();
  settings.endGroup();
}
bool BookmarkWidget::getNewTab() {
  return m_newTab->isChecked();
}
bool BookmarkWidget::getSwitchTab() {
  return m_switchTab->isChecked();
}
void BookmarkWidget::onStateChanged(int /* state */) {
  m_switchTab->setEnabled(m_newTab->isChecked());
}
