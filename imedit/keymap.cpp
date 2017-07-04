#include "keymap.h"
/*
  sample koi:
       "47": {
            "s": "SOLIDUS",
            "t": "OXIA",
            "type": 2
        },
          "97": {
            "s": "LATIN SMALL LETTER A",
            "t": "GREEK SMALL LETTER ALPHA",
            "type": 1
        },

  sample combination table:
        "945": {
            "base": "GREEK SMALL LETTER ALPHA",
            "combined": [
                8067,                7939,                940,                8115,
                8113,                7942,                7941,                8064,
                8066,                8114,                7936,                8118,
                7937,                8112,                7943,                8049,
                8065,                8068,                8069,                8119,
                8116,                7938,                8048,                8070,
                8071,                7940
                ],
            "diacritics": []
        },
   ...
       "8049": {
            "base": "GREEK SMALL LETTER ALPHA",
            "diacritics": [
                "OXIA"
            ],
            "parent": 945
        },
 excerpt from unicode table:

     "GREEK CAPITAL LETTER ZETA": 918,
     "GREEK SMALL LETTER ALPHA": 945,
     "GREEK SMALL LETTER BETA": 946,

  Suppose we convert "a/".

  -  Search koi table looking a matching numeric key i.e 97
  -  if not found, return the key unchanged
  -  if koi entry is type 1, then search the unicode table
      for target value i.e "GREEK SMALL LETTER ALPHA" and return
      the associated numeric value i.e 945
  - Processing '/', search koi table for 47
  - if not found return key unchanged
  - at this point koi entry target field should be the name of a diacritic eg OXIA
  - if found and it is type 2:
  - check the previous char (945) has an entry in combinations table
  - if not,return current key
  - retrieve the entry (the koi entry for 945).
  - get the diacritics array for the previous char (in this case [])
  - append to it the name of the current diacritic (OXIA)
  - this is the target diacritic array
  - for each entry in the combined array (of the koi for 945)
  -    get the combinations entry
  -    if the diacritics array of combinations entry matches the target diacritic array
  -       return the matching combined array entry (8049)

  // sample (nonsensical)
        "1587": {
            "base": "ARABIC LETTER SEEN",
            "combined": [
                1917,
                1918
            ],
            "diacritics": ["WAVY HAMZA ABOVE"]
            parent : 1575
        },

 */
/*
KeyInput * koi_new() {
  KeyInput * k = g_new(KeyInput,1);
  k->source = g_string_new("");
  k->target = g_string_new("");
  k->type = -1;
  return k;
}
void koi_free(KeyInput * k) {
  g_string_free(k->source,TRUE);
  g_string_free(k->target,TRUE);
  g_free(k);
}
void koi_set_source(KeyInput * k, gchar * value) {
  g_string_append(k->source,value);
}
void koi_set_target(KeyInput * k, gchar * value) {
  g_string_append(k->target,value);
}
void koi_print(KeyInput * k) {
  g_print("Source : %s\n",k->source->str);
  g_print("Target : %s\n",k->target->str);
  g_print("Type   : %d\n",k->type);
}
//====================================================
KeyEntry * ke_new() {
  KeyEntry * k = g_new(KeyEntry,1);
  k->base = g_string_new("");
  k->parent = -1;
  k->combined = g_array_new(FALSE,FALSE,sizeof (gint));
  k->diacritics = g_ptr_array_new();
  g_ptr_array_set_free_func(k->diacritics,(GDestroyNotify)km_free_gstring);
  return k;
}
void ke_set_base(KeyEntry * k, gchar * value) {
  g_string_append(k->base,value);
}
void ke_print(KeyEntry * k) {
  g_print("{\n");
  g_print("Base  :%s\n",k->base->str);
  g_print("Combined:[\n");
  for (gint i=0;i < k->combined->len;i++) {
    g_print("\t%d\n",g_array_index(k->combined,gint,i));
  }
  g_print("]\n");
  g_print("Diacritics:[\n");
  for (gint i=0;i < k->diacritics->len;i++) {
    GString * ptr = (GString *)g_ptr_array_index(k->diacritics,i);
    g_print("\t%s\n",ptr->str);
  }
  g_print("]\n");

  g_print("Parent: %d\n",k->parent);
  g_print("}\n");
}
void ke_add_combined(KeyEntry * k,gint v) {
  g_array_append_val(k->combined,v);
}
void ke_add_diacritic(KeyEntry * k,const gchar * data) {
  g_ptr_array_add(k->diacritics,(gpointer)g_string_new(data));
}
void ke_set_parent(KeyEntry * k,gint v) {
  k->parent = v;
}
void ke_free(KeyEntry * k) {
  g_string_free(k->base,TRUE);
  g_ptr_array_free(k->diacritics,TRUE);
  g_array_free(k->combined,TRUE);
  g_free(k);
}
//====================================================
gchar * km_free_gstring(GString * s) {
  //  g_print("freeing gstring [%s]\n",s->str);
  return g_string_free(s,TRUE);
}
void km_unicode_key_destroy(gpointer * data) {
  g_free((gchar *)data);
}
void km_free_unicode_value(gpointer * s) {
  //  g_printf("in free hash value %d\n",GPOINTER_TO_INT(s));
  //  g_free(s);
}
void km_free_combination_value(gpointer * data) {
  KeyEntry * p = (KeyEntry *)data;
  //  g_print("freeing keyentry");
  ke_free(p);
}
void km_free_koi_value(gpointer * data) {
  //KeyEntry * p = (KeyEntry *)data;
  //  g_print("freeing keyentry");
  // ke_free(p);
}
KeyMap * km_new() {
  KeyMap * m = g_new(KeyMap, 1);

  m->codes = g_ptr_array_new();
  g_ptr_array_set_free_func(m->codes,(GDestroyNotify)km_free_gstring);

  m->language = g_string_new("");
  m->name = g_string_new("");
  m->parseDate  = g_string_new("");
  m->script = g_string_new("");
  m->version = g_string_new("");

  m->unicode = g_hash_table_new_full(g_str_hash,g_str_equal,
                                     (GDestroyNotify)km_unicode_key_destroy,
                                     (GDestroyNotify)km_free_unicode_value);

  m->combinations = g_hash_table_new_full(g_str_hash,g_str_equal,
                                          (GDestroyNotify)km_unicode_key_destroy,
                                          (GDestroyNotify)km_free_combination_value);

  m->koi = g_hash_table_new_full(g_str_hash,g_str_equal,
                                          (GDestroyNotify)km_unicode_key_destroy,
                                          (GDestroyNotify)koi_free);

  return m;
}
//
//   Search the combinations has for the given value and
//   return the entry
//
//
KeyEntry * km_get_combination_entry(KeyMap * map,gint v) {
  GString * key;
  key = g_string_new("");
  g_string_printf(key,"%d",v);
  //  g_debug("searching combinations for %s",key->str);
  KeyEntry * ke = NULL;
  if (g_hash_table_contains(map->combinations,key->str)) {
      gpointer * p = g_hash_table_lookup(map->combinations,key->str);
      if (p) {
        ke = (KeyEntry *)p;
      }
  }
  g_string_free(key,TRUE);
  return ke;
}
void km_print_combinations(gpointer key,gpointer value,gpointer user_data) {
  g_print("code \t%s\n",(gchar *)key);
  ke_print((KeyEntry * )value);
}
void km_print_koi(gpointer key,gpointer value,gpointer user_data) {
  g_print("code \t%s\n",(gchar *)key);
  koi_print((KeyInput * )value);
}
void km_print(KeyMap * map) {
  g_print("Name     :%s\n",map->name->str);
  g_print("Langauge :%s\n",map->language->str);
  g_print("Version  :%s\n",map->version->str);
  g_print("Script   :%s\n",map->script->str);
  g_print("Combinations:[\n");
  g_hash_table_foreach(map->combinations,(GHFunc)km_print_combinations,NULL);
  g_print("]\n");
  g_print("Koi:{\n");
  g_hash_table_foreach(map->koi,(GHFunc)km_print_koi,NULL);
  g_print("}\n");
}
void km_free(KeyMap * map) {
  g_string_free(map->language,TRUE);
  g_string_free(map->name,TRUE);
  g_string_free(map->parseDate,TRUE);
  g_string_free(map->script,TRUE);
  g_string_free(map->version,TRUE);

  g_ptr_array_free(map->codes,TRUE);

  g_hash_table_destroy(map->unicode);
  g_hash_table_destroy(map->combinations);
  g_hash_table_destroy(map->koi);
  g_free(map);
}
void km_add_code(KeyMap * map,gchar * code) {
  g_ptr_array_add(map->codes,(gpointer)g_string_new(code));
}
void km_add_unicode(KeyMap * map,gchar * key,gint value) {
  g_hash_table_insert(map->unicode,g_strdup(key),GINT_TO_POINTER(value));
}
gboolean km_find_unicode_value(gpointer key,gpointer value,gpointer user_data) {
  g_print("comparing []..\n");
  //  return g_string_equal(key,user_data);
  return FALSE;
}
gint km_get_unicode_value(KeyMap * map,gchar *key) {
  //  gpointer ptr = g_hash_table_find(map->unicode,(GHRFunc)km_find_unicode_value,key);
  if (g_hash_table_contains(map->unicode,(gconstpointer)key)) {
    return GPOINTER_TO_INT(g_hash_table_lookup(map->unicode,key));
  }
  else {
    return -1;
  }
}
void km_add_keyentry(KeyMap * map,gchar * key,KeyEntry * ke) {
  g_hash_table_insert(map->combinations,g_strdup(key),ke);
}

void km_add_koi(KeyMap * map,gchar * key,KeyInput * koi) {
  g_hash_table_insert(map->koi,g_strdup(key),koi);
}

*/
KeyMap::KeyMap() {

}
KeyMap::~KeyMap() {

}
//----------------------------------
KeyMap * km_new() {
  KeyMap * m = new KeyMap();
  return m;
}
void km_free(KeyMap * map) {
  delete map;
}
void km_add_keyentry(KeyMap * map,int key,KeyEntry * ke) {
  map->combinations.insert(key,ke);
}
void km_add_koi(KeyMap * map,int key,KeyInput * koi) {
  map->koi.insert(key,koi);
}
void km_add_unicode(KeyMap * map,QString key,int value) {
  map->unicode.insert(key,value);
}
KeyEntry * km_get_combination_entry(KeyMap * map,int v) {
  if (map->combinations.contains(v)) {
    return map->combinations.value(v);
  }
  else {
  return NULL;
  }
}
//--------------------------------------------------
KeyEntry::KeyEntry() {
}
KeyEntry * ke_new() {
  KeyEntry * k =  new KeyEntry();
  k->parent = -1;
  return k;
}
void ke_free(KeyEntry * ke) {
  delete ke;
}
void ke_add_combined(KeyEntry * k,int v) {
  k->combined.append(v);
}
void ke_add_diacritic(KeyEntry * k,const QString  data) {
  k->diacritics.append(data);
}
KeyInput * koi_new() {
  KeyInput * k = new KeyInput();
  return k;
}
