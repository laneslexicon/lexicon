#include "noteswidget.h"
NotesWidget::NotesWidget(QWidget * parent) : QWidget(parent) {
  if (! createConnection()) {
    return;
  }
  m_model = new QSqlQueryModel(this);
  m_model->setQuery("select * from notes order by nodeId");
  m_model->setHeaderData(Notes_NodeId,Qt::Horizontal,tr("Node"));
  m_model->setHeaderData(Notes_Word,Qt::Horizontal,tr("Word"));
  m_model->setHeaderData(Notes_Note,Qt::Horizontal,tr("Note"));
  //  m_model->select();

  m_view = new QTableView;
  m_view->setModel(m_model);
  m_view->show();
  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(m_view);
  setLayout(layout);
}
NotesWidget::~NotesWidget() {
  if (m_db.isOpen()) {
    m_db.close();
  }
}
bool NotesWidget::createConnection() {
  bool dbInitRequired = false;
  QString dbname = "notes.sqlite";

  QFile dbfile(dbname);
  if (! dbfile.exists()) {
    dbInitRequired = true;
  }
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(dbname);
  if (! m_db.open()) {
    QMessageBox::critical(0,QObject::tr("Database Error"),
                          m_db.lastError().text());
    return false;
  }
  return true;
}
