#ifndef __KEYDEF_H__
#define __KEYDEF_H__
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QFontMetrics>
#include <QDropEvent>
#include <QMimeData>
#include <QDragMoveEvent>
#include <QTreeWidgetItem>
#include <QTextDocument>
#include <QPair>
class KeyDef {
 public:
  KeyDef();
  void setLevel(int n) { m_currentLevel = n; }
  void setGroup(int n) { m_currentGroup = n; }
  void setValues(const QString &,QSettings &,QSettings &);
  int decorationCount() { return m_decoration.size(); }
  int getRow() { return m_row;}
  int getCol() { return m_col;}
  QString getName() { return m_name; }
  QSize getPos();
  bool isEmpty();
  bool isSticky() { return m_sticky;}
  bool isToggle() { return m_toggle;}
  double getSpan() { return m_span;}
  int  getValue();
  QList<int> getValues();
  void toggle();
  void sticky();
  int getType() { return m_type; }
  int changeTo() { return m_changeTo;}
  int getToggleState() { return m_toggleState;}
  int getStickyState() { return m_stickyState;}
  int levelCount(int);
  int groupCount();
  int getVerticalAdjust(int group,int level);
  bool centerText(int group,int level);
  QString getDecoration(int group,int level);
  enum KeyType { Unknown, Char , LevelShift , GroupShift , Special} ;
  enum ToggleState { KeyUp , KeyDown };
 private:
  QString m_name;
  int m_toggleState;
  int m_stickyState;
  int m_row;
  int m_col;
  int m_currentLevel;
  int m_currentGroup;
  int m_type;    // KeyType
  int m_changeTo;  // level/group to change to
  double m_span;
  bool m_sticky;
  bool m_toggle;
  QMultiMap<QPair<int,int>, int> m_values;
  QMap<QPair<int,int>, int> m_vpos;
  QMap<QPair<int,int>,QString> m_decoration;
  void setGroupValues(int group, QSettings &,QSettings &);
  void setLevelValues(int group, int level,QSettings &,QSettings &);
};
class KeyboardDef {
 public:
  KeyboardDef();
  ~KeyboardDef();
  KeyboardDef(const QString & fileName);
  void clear();
  void load(const QString & fileName);
  void setLevel(int n);
  void setGroup(int n);
  void unstick();
  int rows();
  double cols();
  QMap<QString,int> m_specialKeys;
  QList<KeyDef *> m_keys;
  int keyCount() { return m_keys.size(); }
  KeyDef * getKey(int i) { return m_keys[i]; }
  QString m_name;
  QString m_script;
  int m_levels;
  int m_groups;
};
#endif
