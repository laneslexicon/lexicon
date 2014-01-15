#ifndef _IMEDIT_H
#define _IMEDIT_H
#include "inputmapper.h"
#include <iomanip>
#include <QTextEdit>
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
#include "hermesoptions.h"
extern QString getScript(int);
class ImEdit : public QTextEdit
{
    Q_OBJECT
public:
    ImEdit(QWidget *parent = 0);
    ~ImEdit();
    //    void setMapname(const QString &);
    void setDocFont(const QFont &);
    bool loadMap(const QString & filename,const QString  & mapname);
    QFont & getDocFont() {
      return m_docFont;
    }
    void setDebug(bool v) {
      m_debug = v;
    }
    void getMapNames(QStringList & m) {
      mapper->getMapNames(m);
    }
    void getScriptNames(QStringList &);
    QString currentScript();
 private:
    InputMapper * mapper;
    QString mapname;
    bool mapEnabled;
    ushort prev_char;
    QFont m_docFont;
    bool m_debug;

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void focusInEvent(QFocusEvent *);
    void scrollContentsBy(int,int);
    void enableMapping(bool);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
public slots:
  //    void showContextMenu(const QPoint &);
    void actionChangeMap();
    void actionSetFont();
    void actionInsertUnicode();
    void actionDeleteUnicode();
    void setMapname(const QString &);
 signals:
  void fontChanged();
  void logMessage(const QString &);
  void mapChanged(const QString &);
  void currentScript(const QString &);
  void scrollby();
  void tabPressed();
  void editingFinished(); // added for QItemDelegate subclass
};
class WrappedEdit : public QWidget {
  Q_OBJECT
 public:
  WrappedEdit(QWidget * parent = 0);
  ImEdit * editor() {
    return m_edit;
  }
  ~WrappedEdit();
  public slots:
    void shortcutActivated();
    void setScriptFont(const QString &);
    void showContextMenu(const QPoint &);
    void actionChangeMap();
    void actionChangeFont();
 protected:
  void toggleMap(const QString &);
  virtual void buildContextMenu(QMenu * menu);
  HermesOptions m_options;
  QString m_currentMap;
  QString  m_docFontString;
  ImEdit * m_edit;
 signals:
  void dataChanged();
  void fontChanged();
  void mapChanged();
  void editingFinished();
};
class HudEdit : public WrappedEdit {
  Q_OBJECT
 public:
  HudEdit(QWidget * parent = 0);
  ~HudEdit();
  void setUnicode(const UnicodeSettings &);
 private:
    UniSettings m_unicode;
    bool m_isHudChar;
    int m_hudCharCount;
    QString m_hudChar;
    int m_hudPos;
    void setHudChar(const QString & c,int pos) {
      m_hudChar = c;
      m_isHudChar = true;
      m_hudPos = pos;
    }
    void clearHudChar() {
      m_isHudChar = false;
    }
    void setHudCharCount(int c) {
      m_hudCharCount = c;
    }
    int getHudCharCount() {
      return m_hudCharCount;
    }
  virtual void buildContextMenu(QMenu * menu);
};
#endif
