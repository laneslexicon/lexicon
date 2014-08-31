#include "task.h"
#include "notes.h"
void Task::run() {
  qDebug() << "opts" << m_opts.keys();


  NoteMaster notes;
  bool ok;
  ok = notes.openDb();
  qDebug() << "Open db" << ok;

  QSqlQuery query = notes.getNoteList("select id,word from notes");
  while(query.next()) {
    qDebug() << query.value(0) << query.value(1);
  }
  //  query.finish();
  Note * n = new Note();
  n->setWord("word1");
  n->setNote("the first note");
  notes.save(n);

  notes.find("word1");

  Note * m = new Note();
  m->setWord("word2");
  m->setNote("the second note");
  notes.save(m);


  emit(finished());
}
