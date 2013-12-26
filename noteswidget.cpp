#include "noteswidget.h"
NotesWidget::NotesWidget(QWidget * parent) : QWidget(parent) {
  if (! createConnection()) {
    return;
  }

  m_model = new QSqlQueryModel;
  m_model->setQuery("select * from notes order by nodeId",m_db);
  m_model->setHeaderData(Notes_NodeId,Qt::Horizontal,tr("Node"));
  m_model->setHeaderData(Notes_Word,Qt::Horizontal,tr("Word"));
  m_model->setHeaderData(Notes_Note,Qt::Horizontal,tr("Note"));
  //  m_model->select();

  m_view = new QTableView;
  m_view->setModel(m_model);
  m_view->show();
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(new QLabel(tr("Show notes for")));

  QHBoxLayout * hlayout1 = new QHBoxLayout;
  hlayout1->addWidget(new QLabel(tr("Word/Node")));
  m_show = new QLineEdit;
  hlayout1->addWidget(m_show);
  layout->addLayout(hlayout1);

  m_showBtn = new QPushButton(tr("Search"));

  connect(m_showBtn,SIGNAL(clicked()),this,SLOT(onShowClicked()));
  layout->addWidget(m_showBtn);
  layout->addWidget(m_view);
  setLayout(layout);
}
NotesWidget::~NotesWidget() {
  if (m_db.isOpen()) {
    m_db.close();
  }
}
void NotesWidget::onShowClicked() {
  QString t = m_show->text();
  if (t.startsWith("n")) {
    m_model->setQuery(QString("select * from notes where nodeId = '%1'").arg(t),m_db);
  }
  else {
    m_model->setQuery(QString("select * from notes where word = '%1'").arg(t),m_db);
  }
  if (m_model->lastError().isValid()) {
    qDebug() << "Last error" << m_model->lastError().text();
  }
}
bool NotesWidget::createConnection() {
  bool dbInitRequired = false;
  QString dbname = "notes.sqlite";

  QFile dbfile(dbname);
  if (! dbfile.exists()) {
    dbInitRequired = true;
  }
  m_db = QSqlDatabase::addDatabase("QSQLITE","notesdb");
  m_db.setDatabaseName(dbname);
  if (! m_db.open()) {
    QMessageBox::critical(0,QObject::tr("Database Error"),
                          m_db.lastError().text());
    return false;
  }
  return true;
}
