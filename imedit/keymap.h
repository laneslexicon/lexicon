#ifndef __KEYMAP_H__
#define __KEYMAP_H__
#include <stdio.h>
#include <stdlib.h>
#include <QHash>
#include <QStringList>
#include <QList>

class KeyInput {
 public:
  QString source;
  QString target;
  int type;
};
KeyInput * koi_new();
/*

void koi_free(KeyInput *);
void koi_set_source(KeyInput *,char *);
void koi_set_target(KeyInput *,char *);
*/
//---------------------------
class  KeyEntry {
 public:
  KeyEntry();
  QString base;
  QList<int> combined;
  int parent;
  QStringList diacritics;
};
KeyEntry * ke_new();
void ke_free();
void ke_add_diacritic(KeyEntry *,QString);
void ke_add_combined(KeyEntry *,int);

/*
void ke_set_base(KeyEntry *, char *);
void ke_set_parent(KeyEntry *,int);
void ke_add_diacritic(KeyEntry *,const char *);
void ke_add_combined(KeyEntry *,int);
void ke_print(KeyEntry *);
void ke_free(KeyEntry *);
*/
//---------------------------------------
class  KeyMap {
 public:
  KeyMap();
  ~KeyMap();
  //  GPtrArray * codes;

  QHash<int,KeyEntry *> combinations;
  QHash<int,KeyInput *> koi;
  QHash<QString,int> unicode;
  QString language;
  QString name;
  QString parseDate;
  QString script;
  QString type;
  QString version;
  friend class InputMapper;
};

KeyMap * km_new();
void km_free(KeyMap *);
void km_add_keyentry(KeyMap *,int,KeyEntry *);
void km_add_koi(KeyMap * map,int key,KeyInput * koi);
void km_add_unicode(KeyMap * map,QString key,int value);
KeyEntry * km_get_combination_entry(KeyMap * map,int v);
/*
void km_print(KeyMap *);
char * km_free_gstring(QString * );
void km_add_code(KeyMap *,char *);
void km_add_unicode(KeyMap *,char * ,int);
int km_get_unicode_value(KeyMap *,char *);
void km_add_keyentry(KeyMap *,char *,KeyEntry *);
// these are "private"
void km_unicode_key_destroy(QPointer *);
void km_free_unicode_value(QPointer *);
void km_add_koi(KeyMap * map,char * key,KeyInput * koi);
KeyEntry * km_get_combination_entry(KeyMap * map,int v);
*/
#endif
