#include "task.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc,argv);
  Task task(&a);
  QStringList cmdargs = QCoreApplication::arguments();

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(&task, SIGNAL(finished()), &a, SLOT(quit()));

  // This will run the task from the application event loop.
  QMap<QString,int> optmap;
  int ix;
  optmap.insert("-h",0);
  optmap.insert("-f",1);
  while( ! cmdargs.isEmpty()) {
    QString opt = cmdargs.takeFirst();
    //    qDebug() << "arg" << opt;
    if (optmap.contains(opt)) {
      int argcount = optmap.value(opt);
      QStringList optargs;
      for(int i=0;i < argcount;i++) {
        optargs << cmdargs.takeFirst();
      }
      task.set(opt,optargs);
      if (opt == "-h") {
        qDebug() << "you need help";
      }
    }
  }
  QTimer::singleShot(0, &task, SLOT(run()));

  return a.exec();
}
