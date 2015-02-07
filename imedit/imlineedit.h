#ifndef _IMLINE_EDIT_H
#define _IMLINE_EDIT_H
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
class InputMapper;
class ImLineEdit : public QLineEdit {
    Q_OBJECT
  public:
    ImLineEdit(QWidget *parent = 0);
    ~ImLineEdit();
    QStringList getMaps() const;
    QString getActiveMap() const;
    QString getNullMap() const;
    void setDebug(bool v) { m_debug = v;}
    void readSettings(const QString & filePath = QString());
    public slots:
    bool loadMap(const QString & filename,const QString & mapname = QString());
    void activateMap(const QString &,bool enable = true);
    void shortcutActivated();
    void setForceLTR(bool);
    void onTextChanged(const QString &);
    public slots:
      void setEnabled(bool);
  private:
    InputMapper * m_mapper;
    ushort m_prev_char;

    QString m_activeMap;
    QString m_nullMap;
    bool m_debug;
    bool m_forceLTR;
    bool m_keymapsEnabled;
  protected:
    virtual void keyPressEvent(QKeyEvent *e);

};
#endif
