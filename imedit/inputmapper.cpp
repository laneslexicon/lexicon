#include "inputmapper.h"
#include "keymap.h"
#include <QDebug>
#include <QJsonArray>
/*
im_char * im_char_new() {
  im_char * c = g_new(im_char,1);
  c->c = NULL;
  c->consume = FALSE;
  c->processed = FALSE;

  return c;
}
void im_char_free(im_char * c) {
  if (c->c != NULL) {
    g_free(c->c);
  }
  g_free(c);
}
void im_char_set(InputMapper * m,im_char * ic,gchar * c, gint v,gboolean consume) {
  if (ic->c != NULL) {
    g_free(ic->c);
  }
  ic->c = c;
  ic->uc = v;
  ic->consume = consume;
  ic->processed = TRUE;
}
KeyEntry * get_key_entry(JsonNode * node) {
  JsonObject * obj = json_node_get_object(node);
  GList * list = json_object_get_members(obj);
  GList * i = g_list_first(list);
  KeyEntry * k = ke_new();
  while( i != NULL ) {
    JsonNode * jsnode = json_object_get_member(obj,i->data);
    gchar * name = (gchar *) i->data;
    //    g_printf("%s\n",name);
    //    get_key_entry(jsnode);
    if (g_strcmp0(name,"base") == 0) {
      ke_set_base(k,(gchar *)json_node_get_string(jsnode));
    }
    else if (g_strcmp0(name,"combined") == 0) {
      JsonArray * a = json_node_get_array(jsnode);
      for(guint j=0; j < json_array_get_length(a);j++) {
        JsonNode * nv = json_array_get_element(a,j);
        ke_add_combined(k,json_node_get_int(nv));
      }
    }
    else if (g_strcmp0(name,"diacritics") == 0) {
      JsonArray * a = json_node_get_array(jsnode);
      for(guint j=0; j < json_array_get_length(a);j++) {
        JsonNode * nv = json_array_get_element(a,j);
        ke_add_diacritic(k,json_node_get_string(nv));
      }
    }
    else if (g_strcmp0(name,"parent") == 0) {
      ke_set_parent(k,json_node_get_int(jsnode));
    }
    i = g_list_next(i);
  }
  g_list_free(list);
  return k;
}
KeyInput * get_key_item(JsonNode * node) {
  JsonObject * obj = json_node_get_object(node);
  GList * list = json_object_get_members(obj);
  GList * i = g_list_first(list);
  KeyInput * k = koi_new();
  while( i != NULL ) {
    JsonNode * jsnode = json_object_get_member(obj,i->data);
    gchar * name = (gchar *) i->data;
    //    g_printf("%s\n",name);
    //    get_key_entry(jsnode);
    if (g_strcmp0(name,"s") == 0) {
      koi_set_source(k,(gchar *)json_node_get_string(jsnode));
    }
    else if (g_strcmp0(name,"t") == 0) {
      koi_set_target(k,(gchar *)json_node_get_string(jsnode));
    }
    else if (g_strcmp0(name,"type") == 0) {
      k->type  = json_node_get_int(jsnode);
    }
    i = g_list_next(i);
  }
  g_list_free(list);
  return k;
}
void load_combinations(KeyMap * map,JsonNode * node) {
  //  g_print("loading combinations\n");
  JsonObject * obj = json_node_get_object(node);
  GList * list = json_object_get_members(obj);
  //  g_printf("combinations length %d\n",g_list_length(list));
  GList * i = g_list_first(list);
  while( i != NULL ) {
    JsonNode * jsnode = json_object_get_member(obj,i->data);
    gchar * name = (gchar *) i->data;
    //    g_printf("%s\n",name);
    KeyEntry * ke = get_key_entry(jsnode);
    km_add_keyentry(map,name,ke);
    i = g_list_next(i);
  }
  g_list_free(list);
}
void load_koi(KeyMap * map,JsonNode * node) {
  //  g_print("loading keys of interest\n");
  JsonObject * obj = json_node_get_object(node);
  GList * list = json_object_get_members(obj);
  //  g_printf("combinations length %d\n",g_list_length(list));
  GList * i = g_list_first(list);
  while( i != NULL ) {
    JsonNode * jsnode = json_object_get_member(obj,i->data);
    gchar * name = (gchar *) i->data;
    //    g_printf("%s\n",name);
    KeyInput * ke = get_key_item(jsnode);
    km_add_koi(map,name,ke);
    i = g_list_next(i);
  }
  g_list_free(list);
}
void load_unicode(KeyMap * map,JsonNode * node) {
  //  g_print("loading keys of interest\n");
  JsonObject * obj = json_node_get_object(node);
  GList * list = json_object_get_members(obj);
  //  g_printf("combinations length %d\n",g_list_length(list));
  GList * i = g_list_first(list);
  while( i != NULL ) {
    JsonNode * jsnode = json_object_get_member(obj,i->data);
    gchar * name = (gchar *) i->data;
    km_add_unicode(map,name,json_node_get_int(jsnode));
    //    g_printf("%s\n",name);
    i = g_list_next(i);
  }
  g_list_free(list);
}
void load_properties(KeyMap * map,JsonNode * node) {
  JsonObject * obj = json_node_get_object(node);
  GList * list = json_object_get_members(obj);
  GList * i = g_list_first(list);
  //  g_printf("object length %d\n",g_list_length(list));
  while( i != NULL ) {
    JsonNode * jsnode = json_object_get_member(obj,i->data);

    gchar * name = (gchar *) i->data;
    //    g_printf("%s\n",name);
    if (g_strcmp0(name,"combinations") == 0) {
      load_combinations(map,jsnode);
    }
    if (g_strcmp0(name,"koi") == 0) {
      load_koi(map,jsnode);
    }
    if (g_strcmp0(name,"unicode") == 0) {
      load_unicode(map,jsnode);
    }
    else if (g_strcmp0(name,"name") == 0) {
      g_string_append(map->name,(gchar *)json_node_get_string(jsnode));
    }
    else if (g_strcmp0(name,"script") == 0) {
      g_string_append(map->script,(gchar *)json_node_get_string(jsnode));
    }
    else if (g_strcmp0(name,"language") == 0) {
      g_string_append(map->language,(gchar *)json_node_get_string(jsnode));
    }
    else if (g_strcmp0(name,"version") == 0) {
      g_string_append(map->version,(gchar *)json_node_get_string(jsnode));
    }

    i = g_list_next (i);
  }
  //    json_object_unref(obj);
  g_list_free(list);
}
gboolean im_load_map_from_json(InputMapper * im,gchar * filename) {
  JsonParser * parser = json_parser_new();
  JsonReader * reader = json_reader_new(NULL);
  GError *error = NULL;


  if (json_parser_load_from_file(parser,filename,&error)) {
    g_print("ok\n");
  }
  else {
    g_print("not ok %s\n",error->message);
    g_error_free(error);
    return FALSE;
  }

  KeyMap * m = km_new();
  JsonNode * root = json_parser_get_root(parser);
  load_properties(m,root);
  g_ptr_array_add(im->maps,(gpointer)m);
  //  km_print(m);
  g_object_unref(reader);
  g_object_unref(parser);
  return TRUE;
}
KeyMap * im_get_map(InputMapper * im,gchar * mapname) {
  g_debug("search for map %s",mapname);
  for(gint i=0; i < im->maps->len;i++) {
    KeyMap * map = g_ptr_array_index(im->maps,i);
    if (g_strcmp0(map->name->str,mapname) == 0) {
      return map;
    }
  }
  return NULL;
}
gboolean im_has_map(InputMapper * im,gchar * mapname) {
  for(gint i=0; i < im->maps->len;i++) {
    KeyMap * map = g_ptr_array_index(im->maps,i);
    if (g_strcmp0(map->name->str,mapname) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}
im_char * imcontext_convert(InputMapper * im,gchar * mapname,gunichar curr_char,gunichar prev_char)
{
  GError * error = NULL;

  im_char * ret = im_char_new();
  im_char_set(im,ret,g_strdup_printf("%c",curr_char),curr_char,FALSE);
  ret->iv = curr_char;
  ret->pv = prev_char;
  ret->processed = FALSE;
  KeyMap * map = im_get_map(im,mapname);
  if (map == NULL) {
    // return the current char
    return ret;
  }
  GString * key = g_string_new("");
  g_string_printf(key,"%d",curr_char);
  KeyInput * ki;
  g_debug("searching koi for %s",key->str);
  if (g_hash_table_contains(map->koi,key->str)) {
    //    g_debug("Found key of interest");
    ki = (KeyInput *)g_hash_table_lookup(map->koi,key->str);
  }
  else {
    // return the current char
    g_string_free(key,TRUE);
    return ret;
  }
  if (ki->type == 1) {
    //g_debug("Non-combining key");
    //g_debug("Target %s",ki->target->str);
    if (g_hash_table_contains(map->unicode,ki->target->str)) {
      //g_debug("found target in unicode");
      gpointer * p = g_hash_table_lookup(map->unicode,ki->target->str);
      gunichar u = GPOINTER_TO_INT(p);
      //      ret->uc = u;
      //      ret->c = g_ucs4_to_utf8(&u,1,NULL,NULL,&error);
      im_char_set(im,ret,g_ucs4_to_utf8(&u,1,NULL,NULL,&error),u,FALSE);
      if (error) {
        g_debug("Error %s",error->message);
        g_error_free(error);
      }
      //      g_debug("%c ==> %s [%d]",curr_char,ret->c,ret->uc);
    }
    else {
    }
    g_string_free(key,TRUE);
    return ret;
  }
  g_string_free(key,TRUE);
  if (ki->type == 2) {
    //    g_debug("combining key [%d] %s",(int)prev_char,ki->target->str);
    KeyEntry * ke = km_get_combination_entry(map,(gint)prev_char);
    if (ke != NULL) {
      gint v = im_search(map,ki,ke,prev_char);
      if (v != 0) {
        im_char_set(im,ret,g_ucs4_to_utf8(&v,1,NULL,NULL,&error),v,TRUE);
        if (error) {
          g_debug("Error %s",error->message);
          g_error_free(error);
        }
      }
    }
  }
  return ret;

}
//
// in the original javascript, the curr_char variables were created by using
// String.charCodeAt(n), so the type was not explicit
//
im_char * im_convert(InputMapper * im,gchar * mapname,gchar curr_char, gunichar prev_char) {
  GError * error = NULL;

  im_char * ret = im_char_new();
  im_char_set(im,ret,g_strdup_printf("%c",curr_char),curr_char,FALSE);
  ret->iv = curr_char;
  ret->pv = prev_char;
  ret->processed = FALSE;
  KeyMap * map = im_get_map(im,mapname);
  if (map == NULL) {
    // return the current char
    return ret;
  }
  GString * key = g_string_new("");
  g_string_printf(key,"%d",curr_char);
  KeyInput * ki;
  g_debug("searching koi for %s",key->str);
  if (g_hash_table_contains(map->koi,key->str)) {
    //    g_debug("Found key of interest");
    ki = (KeyInput *)g_hash_table_lookup(map->koi,key->str);
  }
  else {
    // return the current char
    g_string_free(key,TRUE);
    return ret;
  }
  if (ki->type == 1) {
    //g_debug("Non-combining key");
    //g_debug("Target %s",ki->target->str);
    if (g_hash_table_contains(map->unicode,ki->target->str)) {
      //g_debug("found target in unicode");
      gpointer * p = g_hash_table_lookup(map->unicode,ki->target->str);
      gunichar u = GPOINTER_TO_INT(p);
      //      ret->uc = u;
      //      ret->c = g_ucs4_to_utf8(&u,1,NULL,NULL,&error);
      im_char_set(im,ret,g_ucs4_to_utf8(&u,1,NULL,NULL,&error),u,FALSE);
      if (error) {
        g_debug("Error %s",error->message);
        g_error_free(error);
      }
      //      g_debug("%c ==> %s [%d]",curr_char,ret->c,ret->uc);
    }
    else {
    }
    g_string_free(key,TRUE);
    return ret;
  }
  g_string_free(key,TRUE);
  if (ki->type == 2) {
    //    g_debug("combining key [%d] %s",(int)prev_char,ki->target->str);
    KeyEntry * ke = km_get_combination_entry(map,(gint)prev_char);
    if (ke != NULL) {
      gint v = im_search(map,ki,ke,prev_char);
      if (v != 0) {
        im_char_set(im,ret,g_ucs4_to_utf8(&v,1,NULL,NULL,&error),v,TRUE);
        if (error) {
          g_debug("Error %s",error->message);
          g_error_free(error);
        }
      }
    }
  }
  return ret;
}
// ki has the combinations entry for the previous character
gboolean im_match_diacritics(KeyEntry * k,GPtrArray * d) {
  if (d->len != k->diacritics->len) {
    return FALSE;
  }

  gboolean found;
  GString * p1;
  GString * p2;
  gint match_count = 0;
  for (gint i=0; i < d->len;i++) {
    found = FALSE;
    for (gint j=0; j < k->diacritics->len;j++) {
      p1 = g_ptr_array_index(d,i);
      p2 = g_ptr_array_index(k->diacritics,j);
      //      g_debug("matching [%s] [%s]",p1->str,p2->str);
      if (g_strcmp0(p1->str,p2->str) == 0) {
        //        g_debug(" - match");
        match_count++;
      }
    }
    if (found) {
      return TRUE;
    }
    //    if (! found ) {
    //      return FALSE;
    //    }
  }
  //  g_debug("Match count %d, length %d",match_count,d->len);
  return (match_count == d->len);
}
gunichar im_search(KeyMap * map,KeyInput * ki,KeyEntry * ke,gunichar pchar) {
  // build an array of diacritics
  gunichar ret;
  ret = 0;
  GPtrArray * dc = g_ptr_array_new ();
  for(gint i=0; i < ke->diacritics->len;i++) {
    GString * item = g_ptr_array_index(ke->diacritics,i);
    g_ptr_array_add(dc,item);
  }
  g_ptr_array_add(dc,ki->target);
  //g_ptr_array_foreach(dc,(GFunc)g_print,NULL);
  //g_debug("err, does this still work %s",ki->target->str);

  GArray * c;
  c = g_array_new(FALSE, FALSE, sizeof (gint));
  for(gint i=0; i < ke->combined->len;i++) {
    g_array_append_val(c,g_array_index(ke->combined,gint,i));
  }
  if (ke->parent != -1) {
    KeyEntry * parent = km_get_combination_entry(map,ke->parent);
    if (parent != NULL) {
      for(gint i=0; i < parent->combined->len;i++) {
        g_array_append_val(c,g_array_index(parent->combined,gint,i));
      }
    }
  }
  // we now have an array of candidate characters (in GArray c)
  // and an array of diacritics
  // for each one we need to check if its diacritics array mathces ours
  //
  gint kv;
  for(gint i=0;i < c->len;i++) {
    kv = g_array_index(c,gint,i);
    KeyEntry * k = km_get_combination_entry(map,kv);
    if (k) {
      if (im_match_diacritics(k,dc)) {
        g_debug("Found %d",kv);
        ret = kv;
      }
    }
  }
  g_array_free (c,TRUE);
  g_ptr_array_free(dc,TRUE);
  return ret;
}
GString * im_convert_string(InputMapper * im,gchar * mapping,gchar * str) {
  GString * retval;
  retval = NULL;
  KeyMap * map = im_get_map(im,mapping);
  if (map == NULL) {
    return retval;
  }
  GArray * ustr = g_array_new(FALSE,FALSE,sizeof (gunichar));
  GString * source = g_string_new(str);
  gunichar pc;
  pc = -1;
  for (gint i=0;i < source->len;i++) {
    gchar c = source->str[i];
    im_char * cc = im_convert(im,mapping,c,pc);
    if (cc->consume) {
      // pop of the last character
      g_array_remove_index(ustr,ustr->len - 1);
    }
    g_array_append_val(ustr,cc->uc);
    pc = cc->uc;
    im_char_free(cc);
  }
  g_string_free(source,TRUE);

  retval = g_string_new("");
  for(gint i=0;i < ustr->len;i++) {
    pc = g_array_index(ustr,gunichar,i);
    g_string_append_unichar(retval,pc);
  }
  g_array_free(ustr,TRUE);
  return retval;
}
*/
InputMapper::InputMapper() {
  pv = 0;
}
void im_free(InputMapper * im) {
  // g_ptr_array_free(im->maps,TRUE);
  for (int i=0;i < im->maps.size();i++) {
    delete im->maps[i];
  }
  delete im;
}

InputMapper * im_new() {
  InputMapper * im = new InputMapper();
  //  im->maps = g_ptr_array_new();
  //  im->pv = 0;
  //  g_ptr_array_set_free_func(im->maps,(GDestroyNotify)km_free);

  return im;
}
bool im_load_map_from_json(InputMapper * im,const char * filename,const char  * mapname) {
   QJsonValue v;
  bool ok = true;
 QFile file;


//  setAttribute(Qt::WA_InputMethodEnabled,true);
  file.setFileName(filename);
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  QString js = file.readAll();
  file.close();
  QJsonDocument jsonDoc = QJsonDocument::fromJson(js.toUtf8());
  QJsonObject obj;
  if (jsonDoc.isObject()) {
    obj = jsonDoc.object();
  }
  KeyMap * m = km_new();
  load_properties(m,obj);
  if (mapname != 0) {
    m->name = QString(mapname);
  }
  if (im->m_debug) {
    qDebug() << Q_FUNC_INFO << "loaded map" << mapname << m->name;
  }
  im->maps.append(m);
  return ok;
}
void load_properties(KeyMap * map,const QJsonObject node) {

  QStringList list = node.keys();
  for (int i=0;i < list.size();i++) {
    QString p = list[i];
    if (p == "combinations") {
      load_combinations(map,node.value(p).toObject());
    }
    if (p == "koi") {
      //      qDebug() << "loading koi";
      load_koi(map,node.value(p).toObject());
    }
    if (p == "unicode")  {
      //      qDebug() << "loading unicode";
      load_unicode(map,node.value(p).toObject());
    }
    else if (p == "name") {
      map->name = node.value(p).toString();
    }
    else if (p == "script") {
      map->script = node.value(p).toString();
    }
    else if (p == "language") {
      map->language = node.value(p).toString();
    }
    else if (p == "version") {
      map->version = node.value(p).toString();
    }
  }
}
void load_combinations(KeyMap * map,QJsonObject node) {
  //  qDebug() << "loading combinations";
  QStringList list = node.keys();
  for (int i=0;i < list.size();i++) {
    QString p = list[i];
    QJsonObject jsnode = node.value(p).toObject();
    //    qDebug() << "Adding" << p.toInt();
    KeyEntry * ke = get_key_entry(jsnode);
    km_add_keyentry(map,p.toInt(),ke);
  }
}
KeyEntry * get_key_entry(QJsonObject node) {
  QStringList list = node.keys();
  KeyEntry * k = ke_new();
  for (int i=0;i < list.size();i++) {
    //  JsonNode * jsnode = json_object_get_member(obj,i->data);
    //    gchar * name = (gchar *) i->data;

    //    g_printf("%s\n",name);
    //    get_key_entry(jsnode);
    QString p = list[i];
    if (p == "base") {
      k->base = node.value(p).toString();
    }
    else if (p == "combined") {
      QJsonArray  a = node.value(p).toArray();
      for(int j=0; j < a.size();j++) {
        QJsonValue nv = a.at(j);
        ke_add_combined(k,(int)nv.toDouble());
      }
    }
    else if (p == "diacritics") {
      QJsonArray  a = node.value(p).toArray();
      for(int j=0; j < a.size();j++) {
        QJsonValue nv = a.at(j);
        ke_add_diacritic(k,nv.toString());
      }
    }
    else if (p == "parent") {
      k->parent = (int)node.value(p).toDouble();
    }
  }
  //  qDebug() << "ke" << k->base << k->combined.size() << k->diacritics.size() << k->parent;
  return k;
}
void load_koi(KeyMap * map,QJsonObject node) {
  QStringList list = node.keys();
  for(int i=0;i < list.size();i++) {
    QString p = list[i];
    QJsonObject jsnode = node.value(p).toObject();
    KeyInput * ke = get_key_item(jsnode);
    //    qDebug() << "added koi" << p.toInt() << ke->source << ke->target << ke->type;
    km_add_koi(map,p.toInt(),ke);
  }
}
KeyInput * get_key_item(QJsonObject node) {
  KeyInput * k = koi_new();
  QStringList list = node.keys();
  for(int i=0;i < list.size();i++) {
    QString p = list[i];
    if (p == "s") {
      k->source = node.value(p).toString();
    }
    else if (p == "t") {
      k->target = node.value(p).toString();
    }
    else if (p == "type") {
      k->type = (int)node.value(p).toDouble();
    }
  }
  return k;
}
void load_unicode(KeyMap * map,QJsonObject node) {
  QStringList list = node.keys();
  for(int i=0;i < list.size();i++) {
    QString p = list[i];
    int  v  = (int)node.value(p).toDouble();
    //      qDebug() << "add unicode" << p << v;
      km_add_unicode(map,p,v);
  }
}
KeyMap * im_get_map(InputMapper * im,const QString & mapname) {
  for(int i=0; i < im->maps.size();i++) {
    if (im->maps[i]->name.toCaseFolded() == mapname.toCaseFolded())  {
      return im->maps[i];
    }
  }
  return NULL;
}
void im_char_set(InputMapper * m,im_char * ic,QString c, int v,bool consume) {
  ic->c = c;
  ic->uc = v;
  ic->consume = consume;
  ic->processed = true;
}
im_char * im_convert(InputMapper * im,const QString & mapname,int curr_char, int prev_char) {
  //  qDebug() << "im_convert" << mapname << curr_char << prev_char;
  im_char * ret = new im_char();
  im_char_set(im,ret,QString(curr_char),curr_char,false);
  ret->iv = curr_char;
  ret->pv = prev_char;
  ret->processed = false;
  /// this is case insensitive
  KeyMap * map = im_get_map(im,mapname);
  if (map == NULL) {
    qDebug() << "map is NULL";
    return ret;
  }
  KeyInput * ki;
  //  qDebug() << "searching koi for " << curr_char;
  if (map->koi.contains(curr_char)) {
    //    g_debug("Found key of interest");
    ki = map->koi.value(curr_char);
  }
  else {
    // return the current char
    return ret;
  }
  if (ki->type == 1) {
    //g_debug("Non-combining key");
    //g_debug("Target %s",ki->target->str);
    if (map->unicode.contains(ki->target)) {
      //g_debug("found target in unicode");
      int u = map->unicode.value(ki->target);
      // ret->uc = u;
      //      ret->c = g_ucs4_to_utf8(&u,1,NULL,NULL,&error);
      im_char_set(im,ret,QString(u),u,false);
      //      qDebug() << QString(curr_char) << " ==>" << QString(u);
    }
    else {
    }
    return ret;
  }
  if (ki->type == 2) {
    //    g_debug("combining key [%d] %s",(int)prev_char,ki->target->str);
    KeyEntry * ke = km_get_combination_entry(map,prev_char);
    if (ke != NULL) {
      int v = im_search(map,ki,ke,prev_char);
      if (v != 0) {
        im_char_set(im,ret,QString(v),v,true);
      }
    }
  }
  return ret;
}
int im_search(KeyMap * map,KeyInput * ki,KeyEntry * ke,int pchar) {
  int ret = 0;

  // build an array of diacritics
  QStringList  dc;
  dc << ke->diacritics << ki->target;
  QList<int> c;
  //  GArray * c;
  c << ke->combined;
  if (ke->parent != -1) {
    KeyEntry * parent = km_get_combination_entry(map,ke->parent);
    if (parent != NULL) {
      c << parent->combined;
    }
  }
  // we now have an array of candidate characters (in QList<in> c)
  // and an array of diacritics
  // for each one we need to check if its diacritics array mathces ours
  //
  int kv;
  for(int i=0;i < c.size();i++) {
    kv = c[i];
    KeyEntry * k = km_get_combination_entry(map,kv);
    if (k) {
      if (im_match_diacritics(k,dc)) {
        //     qDebug() << "Found " << kv;
        ret = kv;
      }
    }
  }
  return ret;
}
//
bool im_match_diacritics(KeyEntry * k,QStringList & d) {
  if (d.size() != k->diacritics.size()) {
    return false;
  }
  for (int i=0; i < d.size();i++) {
    if (! k->diacritics.contains(d[i])) {
      return false;
    }
  }
  return true;
}
QString im_convert_string(InputMapper * im,const QString & mapping,const QString & source, bool * ok) {
  QString retval;
  //  qDebug() <<  Q_FUNC_INFO << mapping << source;
  QString mname(mapping);
  KeyMap * map = im_get_map(im,mname);
  if (map == NULL) {
 //   qDebug() << "map not found";
    return retval;
  }
  QList<int>  ustr;
  int pc;
  pc = -1;
  for (int i=0;i < source.length();i++) {
    QChar c = source.at(i);
    im_char * cc = im_convert(im,mapping,c.unicode(),pc);
    if (ok != 0) {
      if ((! cc->processed ) &&
          (c != ' '))
        { // indicates possible incorrect char
        qDebug() << "convert error" << source << "at" << c;
        *ok = false;
      }
      else {
        *ok = true;
      }
    }
    if (cc->consume) {
      ustr.removeLast();
    }
    ustr.append(cc->uc);
    pc = cc->uc;
    delete cc;
  }
  for(int i=0;i < ustr.size();i++) {
    retval.append(ustr[i]);
  }
  return retval;
}
bool InputMapper::hasMap(const QString & name) const {
  for(int i=0;i < maps.size();i++)  {
    if (name.toCaseFolded() == maps[i]->name.toCaseFolded()) {
      return true;
    }
  }
  return false;
}
void InputMapper::getMapNames(QStringList & m) {
  for(int i=0;i < maps.size();i++)  {
    m.append(maps[i]->name);
  }
}
QStringList InputMapper::getMaps() {
  QStringList mapnames;
  for(int i=0;i < maps.size();i++)  {
    mapnames << maps[i]->name;
  }
  return mapnames;
}
void InputMapper::getScripts(QStringList & s) {
  for(int i=0;i < maps.size();i++)  {
    s.append(maps[i]->script);
  }
}
QString InputMapper::getScript(const QString & mapname) {
  for(int i=0;i < maps.size();i++)  {
    if (maps[i]->name == mapname) {
      return maps[i]->script;
    }
  }
  return QString();
}
