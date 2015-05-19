#ifndef _IMEDIT_H
#define _IMEDIT_H

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

#ifdef WITH_WRAPPED_EDIT
#include "hermesoptions.h"
#endif
extern QString getScript(int);
class InputMapper;
class ImEdit : public QTextEdit
{
    Q_OBJECT
public:
    ImEdit(QWidget *parent = 0);
    ~ImEdit();
    //    void setMapname(const QString &);
    void setDocFont(const QFont &);
    bool loadMap(const QString & filename,const QString  & mapname = QString());
    QFont & getDocFont() {
      return m_docFont;
    }
    void setDebug(bool v);
    QString getScript(const QString & mapname) const;
    QString currentMap() const;
    void getMapNames(QStringList & m);
    bool isMappingEnabled() const { return m_enabled; }
    void getScriptNames(QStringList &);
    QString currentScript();
    QString convertString(const QString & source);
    QStringList scripts() const;
    InputMapper * mapper;
 private:
    ushort prev_char;
    QFont m_docFont;
    bool m_debug;
    bool m_enabled;
    bool m_discard;
    QString m_activeMap;
    QString m_nullMap;

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void focusInEvent(QFocusEvent *);
    void scrollContentsBy(int,int);

    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
public slots:
  //    void showContextMenu(const QPoint &);
    void enableMapping(bool);
    void actionChangeMap();
    void actionSetFont();
    void actionInsertUnicode();
    void actionDeleteUnicode();
    bool setCurrentMap(const QString &,bool enable = true);
 signals:
  void fontChanged();
  void charInserted(int,int);
  void logMessage(const QString &);
  void mapChanged(const QString &);
  void currentScript(const QString &);
  void scrollby();
  void tabPressed();
  void gotFocus(int);
  void editingFinished(); // added for QItemDelegate subclass
};

#ifdef WITH_WRAPPED_EDIT
class WrappedEdit : public QWidget {
  Q_OBJECT
 public:
  WrappedEdit(QWidget * parent = 0);
  ImEdit * editor() {
    return m_edit;
  }
  ~WrappedEdit();
  void setSz(QSize sz) {
    m_sz = sz;
  }
  QString getText();
  public slots:
    void shortcutActivated();
    void setScriptFont(const QString &);
    void showContextMenu(const QPoint &);
    void actionChangeMap();
    void actionChangeFont();
 protected:
  void toggleMap(const QString &);
  virtual void buildContextMenu(QMenu * menu);
  virtual QSize sizeHint() const { return QSize(300,30);};
  HermesOptions m_options;
  QString m_currentMap;
  QString  m_docFontString;
  ImEdit * m_edit;
  QSize m_sz;

 signals:
  void dataChanged();
  void fontChanged();
  void mapChanged();
  void editingFinished();
};
#endif
#ifdef WITH_HUD
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

#endif
