#ifndef __NAMESPACE_H__
#define __NAMESPACE_H__
namespace Lane {
enum Tabs {
  Create_Tab          = 0x00000001,
  Switch_Tab          = 0x00000002,
};
 enum SearchType {
  Root                = 0x00000004,
  Word                = 0x00000008,
  Entry               = 0x00000010
 };
 enum Search {
   Ignore_Diacritics =  0x00000020,
   Whole_Word        =  0x00000040,
   Buckwalter        =  0x00000080,
   Arabic            =  0x00000100,
   Normal_Search     =  0x00000200,
   Regex_Search      =  0x00000400,
   Include_Heads     =  0x00000800
 };
 enum Keymaps {
   Keymaps_Enabled   =  0x00001000
 };
 enum Other {
   Local_Search        =  0x00002000,
   Force_LTR           =  0x00004000,
   Sticky_select       =  0x00008000
 };
}
#endif
