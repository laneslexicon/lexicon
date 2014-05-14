#include "notebrowser.h"
#include "laneslexicon.h"
extern LanesLexicon * getApp();
NoteBrowser::NoteBrowser(QWidget * parent) : QWidget(parent) {
  LanesLexicon * app = getApp();
  //  QSettings * settings = app->getSettings();
  //  settings->beginGroup("Notes");
  //  QString f = settings->value("Font",QString()).toString();
//  if (! f.isEmpty()) {
//    m_resultsFont.fromString(f);
//  }
//  delete settings;

  NoteMaster * notes = app->notes();
  QVBoxLayout * layout = new QVBoxLayout;
  m_list = new QTableWidget;
  /// id, word,subject,date
  m_list->setColumnCount(5);
  m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  QStringList headers;
  headers << tr("") << tr("Id") << tr("Word") << tr("Created") << tr("Subject");
  m_list->setHorizontalHeaderLabels(headers);
  m_list->horizontalHeader()->setStretchLastSection(true);
  m_list->setSelectionMode(QAbstractItemView::SingleSelection);
  //  m_list->installEventFilter(this);
  QStyle * style = m_list->style();
  qDebug() << "style hint" << style->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus);
  m_note = new QTextEdit;

  bool ok = false;
  /// TODO replace select *
  QTableWidgetItem * item;
  /// TODO include count in table ?
  QSqlQuery q = notes->getNoteList("select id,word,subject,created from notes");
  while(q.next()) {
    QString word = q.value("word").toString();
    int row = m_list->rowCount();
    m_list->insertRow(row);
    m_list->setCellWidget(row,0,new QCheckBox());
    item = new QTableWidgetItem(q.value("Id").toInt());
    m_list->setItem(row,1,item);

    item = new QTableWidgetItem(word);
    //      item->setFont(m_resultsFont);
    //      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,2,item);
    QString when = q.value("created").toString();
    QDateTime d = QDateTime::fromString(when);
    item = new QTableWidgetItem(d.toString( Qt::SystemLocaleShortDate));
    //      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    m_list->setItem(row,3,item);

    item = new QTableWidgetItem(q.value("subject").toString());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    //      item->setFont(m_resultsFont);
    m_list->setItem(row,4,item);
  }
  QSplitter * splitter = new QSplitter(Qt::Vertical);
  splitter->addWidget(m_list);
  splitter->addWidget(m_note);
  splitter->setStretchFactor(0,0);
  splitter->setStretchFactor(1,1);
  layout->addWidget(splitter);
  m_list->adjustSize();//resizeColumnsToContents();

  setLayout(layout);
  //connect(m_list,SIGNAL(currentItemChanged(QTableWidgetItem * ,QTableWidgetItem * )),
  //      this,SLOT(itemChanged(QTableWidgetItem * ,QTableWidgetItem * )));
//  connect(m_list,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
//          this,SLOT(itemDoubleClicked(QTableWidgetItem * )));
  /// show the first item in the list
  /// TODO decide who gets focus and select the first row
  /// TODO if table loses focus, change the background selection color
  if (m_list->rowCount() > 0) {
    m_list->itemDoubleClicked(m_list->item(0,0));
    m_list->resizeColumnsToContents();
  }

}
