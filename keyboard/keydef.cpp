#include "keydef.h"
KeyDef::KeyDef() {
  m_currentLevel = 1;
  m_currentGroup = 1;
  m_sticky = false;
  m_toggle = false;
  m_toggleState = KeyDef::KeyUp;
  m_stickyState = KeyDef::KeyUp;

}
bool KeyDef::isEmpty() {
  if (m_toggle || m_sticky)
    return false;
  if (m_values.size() == 0) {
    return true;
  }
  return false;
}
int KeyDef::levelCount(int group) {
  int count = 0;
  QList<QPair<int,int> > keys;
  keys = m_decoration.keys();
  for(int i=0;i < keys.size();i++) {
    if (keys[i].first == group) {
      count++;
    }
  }
  return count;
}
/**
 * Calculates the number of diffent groups on this key
 * NOTE: this is not the number of goups on the keyboard
 *
 * @return group count
 */
int KeyDef::groupCount() {
  QList<QPair<int,int> > keys;
  QList<int> groups;
  keys = m_decoration.keys();
  for(int i=0;i < keys.size();i++) {
    if ( ! groups.contains(keys[i].first))
      groups << keys[i].first;
  }
  return groups.size();
}
int KeyDef::getVerticalAdjust(int group,int level) {
  return  m_vpos.value(qMakePair(group,level));
}

/**
 * if no vertical adjustment specified, return true
 *
 * @param group
 * @param level
 *
 * @return
 */
bool KeyDef::centerText(int group,int level) {
  return ! m_vpos.contains(qMakePair(group,level));
}
void KeyDef::toggle() {
  if (m_toggleState == KeyDef::KeyUp) {
    m_toggleState = KeyDef::KeyDown;
  }
  else {
    m_toggleState = KeyDef::KeyUp;
  }
}
void KeyDef::sticky() {
  if (m_stickyState == KeyDef::KeyUp) {
    m_stickyState = KeyDef::KeyDown;
  }
  else {
    m_stickyState = KeyDef::KeyUp;
  }
}
QSize KeyDef::getPos() {
  return QSize(m_row,m_col);
}
QString KeyDef::getDecoration(int group,int level) {
  QPair<int,int> ix(group,level);
  if (m_decoration.contains(ix)) {
    return m_decoration.value(ix);
  }
  /// for single declaration with multiple keys
  if ((m_decoration.size() == 1) && (m_values.size() > 1)) {
    return m_decoration.first();
  }
  return QString();
}
int KeyDef::getValue() {
  QPair<int,int> ix(m_currentGroup,m_currentLevel);
  if (m_values.contains(ix)) {
    return m_values.value(ix);
  }
  return -1;
}
QList<int> KeyDef::getValues() {
  QList<int> v;
  QPair<int,int> ix(m_currentGroup,m_currentLevel);
  if (!m_values.contains(ix)) {
    return v;
  }
  v = m_values.values(ix);
  for(int k = 0; k < (v.size()/2); k++) {
    v.swap(k,v.size()-(1+k));
  }
  return v;
}
void KeyDef::setValues(const QString & /* name */,QSettings & settings,QSettings & specials) {
  QRegExp lx("level(\\d+)");
  QRegExp gx("group(\\d+)");

  m_row = settings.value("row",-1).toInt();
  m_col = settings.value("col",-1).toInt();
  m_sticky = settings.value("sticky",0).toBool();
  m_toggle = settings.value("toggle",0).toBool();
  m_name = settings.value("name",QString()).toString();
  m_span = settings.value("span",1.0).toDouble();
  m_type = KeyDef::Unknown;

  QString v = settings.value("type",QString()).toString();
  if (lx.indexIn(v) != -1) {
    m_type = KeyDef::LevelShift;
    m_changeTo = lx.cap(1).toInt();
  }
  else if (gx.indexIn(v) != -1) {
    m_type = KeyDef::GroupShift;
    m_changeTo = gx.cap(1).toInt();
  }
  else if (v == "special") {
    m_type = KeyDef::Special;
  }
  else if (v == "char") {
    m_type = KeyDef::Char;
  }
  QString decoration = settings.value("decoration",QString()).toString();
  ///
  QStringList keys = settings.childGroups();
  for(int i=0; i < keys.size();i++) {
    int group = -1;
    /// if no group is specified then assume it
    /// is group 1
    if (lx.indexIn(keys[i]) != -1) {
        int level = lx.cap(1).toInt();
        settings.beginGroup(keys[i]);
        setLevelValues(1,level,settings,specials);
        settings.endGroup();
    }
    else  if (gx.indexIn(keys[i]) != -1) {
        group = gx.cap(1).toInt();
        settings.beginGroup(keys[i]);
        setGroupValues(group,settings,specials);
        settings.endGroup();
      }
    }
  if (decoration.isEmpty())
    return;

  /// If decoration is set then we have a single decoration on a key with
  /// multiple values
  int group = 1;
  int level = 1;
  if (gx.indexIn(decoration) != -1) {
    group = gx.cap(1).toInt();
  }
  if (lx.indexIn(decoration) != -1) {
    level = lx.cap(1).toInt();
  }
  /// group,level index is the only decoration we want so remove the others
  QPair<int,int> ix = qMakePair(group,level);
  decoration = m_decoration.value(ix);
  if ( ! decoration.isEmpty()) {
    m_decoration.clear();
    m_decoration.insert(ix,decoration);
  }

}

void KeyDef::setGroupValues(int group, QSettings & settings,QSettings & specials) {

  QStringList keys = settings.childGroups();
  QRegExp rx("level(\\d+)");
  for(int i=0;i < keys.size();i++) {
    if (rx.indexIn(keys[i]) != -1) {
      int level = rx.cap(1).toInt();
      settings.beginGroup(keys[i]);
      setLevelValues(group,level,settings,specials);
      settings.endGroup();
    }
  }
}
/************************************************

  as a short hand we can have
     value=A
  or
     value=65
  or
     value=0x
*************************************************/
void KeyDef::setLevelValues(int group,int level,QSettings & settings,QSettings & specials) {
  bool ok;
  //int base;
  QRegExp sp("<(.+)>");
  QRegExp nx("^\\d+$");
  QRegExp hx("^0x[0-9,a-f,A-F]+");
  QPair<int,int> k(group,level);
  QStringList keys  = settings.allKeys();
  if (m_type == KeyDef::Char) {
    if (keys.contains("value")) {
      //      base = 10;
      int x = -1;
      QString s = settings.value("value").toString();
      if (! s.isEmpty()) {
        /// if it is a special key, get the value
        if (sp.indexIn(s) != -1) {
          s = sp.cap(1);
          if (specials.contains(s)) {
            QString t = specials.value(s).toString();
            int v = t.toInt(&ok,0);       // leading 0x will convert base 16
            if (ok) {
              x = v;
            }
          }
          else {
            qDebug() << "warning: no value for special key" << s;
          }
        }

        else if (s.indexOf(QChar(',')) != -1) {
          QStringList chars = s.split(QChar(','),QString::SkipEmptyParts);
          for(int  i=0;i < chars.size();i++) {
            if (hx.indexIn(chars[i]) != -1) {
              m_values.insert(k,chars[i].toInt(&ok,0));
            }
          }
        }
        else if (nx.indexIn(s) != -1) {
          x = s.toInt(&ok);
        }
        else if (hx.indexIn(s) != -1) {
          x = s.toInt(&ok,0);
        }
        else {
          QChar c = s.at(0);
          x = c.unicode();
        }
        m_values.insert(k,x);
      }
    }
    if (keys.contains("decoration")) {
      QString d = settings.value("decoration").toString();
      if (hx.indexIn(d) != -1) {
        d = QString(QChar(d.toInt(&ok,0)));
      }
      m_decoration.insert(k,d);
    }
    /// if value supplied, but no decoration, convert the value to a string
    if (m_values.contains(k) && ! m_decoration.contains(k)) {
      QChar c(m_values.value(k));
      QString s = QString("%1").arg(c);
      m_decoration.insert(k,s);
    }
    /// if decoratin but no value, get the value of the first character of the decoration
    else if (! m_values.contains(k) &&  m_decoration.contains(k)) {
      QChar c = m_decoration.value(k).at(0);
      m_values.insert(k,c.unicode());
    }
    if (keys.contains("vpos")) {
      m_vpos.insert(k,settings.value("vpos").toInt());
    }
  }
  if (m_type == KeyDef::LevelShift) {
    m_decoration.insert(k,settings.value("decoration").toString());
  }
}
/**
 * This is the keyboard, made up of rows/cols of  KeyDef
 *
 */
KeyboardDef::KeyboardDef() {
}
KeyboardDef::~KeyboardDef() {
  this->clear();
}
void KeyboardDef::clear() {
  while(m_keys.size() > 0) {
    KeyDef * k = m_keys.takeFirst();
    delete k;
  }
}
/**
 * rows are all the same height
 *
 *
 * @return
 */
int KeyboardDef::rows() {
  int maxRow = 0;
  for(int i=0;i < m_keys.size();i++) {
    int row = m_keys[i]->getRow();
    if (row > maxRow)
      maxRow = row;
  }
  return maxRow;
}
/**
 * a column can be different size, so we return the number
 * of columns in the longest row
 *
 * @return
 */
double KeyboardDef::cols() {
  double maxCol = 0;
  int rows = this->rows();
/********
  for(int i=0;i < m_keys.size();i++) {
    row = m_keys[i]->getRow();
    if (! rows.contains(row))
      rows << row;
  }
*/
  for(int i=0;i < rows;i++) {
    for(int j=0;j < m_keys.size();j++) {
      if (m_keys[j]->getRow() == i) {
        double col = m_keys[j]->getCol() + (m_keys[j]->getSpan() - 1.0);
        if (col > maxCol)
          maxCol = col;
      }
    }
  }
  return maxCol;
}
/**
 *
 * Extact the header information and then load each key as it is in
 * a section [Key-<row>-<col>]
 *
 * we pass two QSettings to the load routine but they both point to the
 * same ini file. The first contains the key definitions and the other
 * the special character codes like <Backspace>,<Tab>
 *
 * It's not pretty.
 *
 * @param name absolute path to settings file
 */
void KeyboardDef::load(const QString & name) {
  QSettings sp(name,QSettings::IniFormat);
  sp.setIniCodec("UTF-8");
  sp.beginGroup("CharacterCodes");
  QSettings settings(name,QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  QStringList groups = settings.childGroups();
  settings.beginGroup("Header");
  m_name = settings.value("name",QString()).toString();
  m_script = settings.value("script",QString()).toString();
  m_levels = settings.value("levels",2).toInt();
  m_groups = settings.value("groups",1).toInt();
  settings.endGroup();

  for(int i=0;i < groups.size();i++) {
    if (groups[i].startsWith("Key")) {
      settings.beginGroup(groups[i]);
      KeyDef * k = new KeyDef();
      k->setValues(groups[i],settings,sp);
      m_keys << k;
      settings.endGroup();
    }
  }

}
void KeyboardDef::setLevel(int n) {
  for(int i=0;i < m_keys.size();i++) {
    m_keys[i]->setLevel(n);
  }

}
void KeyboardDef::setGroup(int n) {
  for(int i=0;i < m_keys.size();i++) {
    m_keys[i]->setGroup(n);
  }
}
void KeyboardDef::unstick() {
  int count = 0;
  for(int i=0;i < m_keys.size();i++) {
    if ( m_keys[i]->isSticky() && (m_keys[i]->getStickyState() == KeyDef::KeyDown)) {
      m_keys[i]->sticky();
      count++;
    }
  }
  if (count > 0) {
    setLevel(1);
  }
}
