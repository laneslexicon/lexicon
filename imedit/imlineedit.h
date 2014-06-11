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
#include <QSettings>
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
    QStringList getMaps() { return m_mapper->getMaps();};
    QString getActiveMap() { return m_activeMap;};
    QString getNullMap() { return m_nullMap;}
    void setDebug(bool v) { m_debug = v;}
    void readSettings(QSettings * settings = 0);
    public slots:
    bool loadMap(const QString & filename,const QString & mapname = QString());
    void activateMap(const QString &,bool enable = true);
    void shortcutActivated();
    void setForceLTR(bool);
  private:
    InputMapper * m_mapper;
    ushort m_prev_char;

    QString m_activeMap;
    QString m_nullMap;
    bool m_debug;
    bool m_forceLTR;
  protected:
    virtual void keyPressEvent(QKeyEvent *e);

};
#endif
