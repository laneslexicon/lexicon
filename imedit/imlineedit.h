#ifndef _IMLINE_EDIT_H
#define _IMLINE_EDIT_H
#include "inputmapper.h"
#include <iomanip>
#include <QLineEdit>
#include <QTextStream>
#include <QDebug>
#include <QKeyEvent>
#include <QAction>
#include <QMenu>
#include <QFont>
#include <QFontDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPaintEvent>
#include <QFile>
#include <QVBoxLayout>
#include <QShortcut>
#include "scripts.h"
class ImLineEdit : public QLineEdit {
    Q_OBJECT
  public:
    ImLineEdit(QWidget *parent = 0);
    ~ImLineEdit();
  public slots:
    bool loadMap(const QString & filename,const QString & mapname = QString());
    void activateMap(const QString &,bool enable = true);
  private:
    InputMapper * m_mapper;
    ushort m_prev_char;
    void readSettings();
    QString m_activeMap;
    bool m_debug;
  protected:
    virtual void keyPressEvent(QKeyEvent *e);

};
#endif
