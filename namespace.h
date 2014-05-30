#ifndef __NAMESPACE_H__
#define __NAMESPACE_H__
namespace Lane {
enum Tabs {
Create_Tab = 0x00000001,
  Switch_Tab = 0x00000002,
};
 enum Search {
   Ignore_Diacritics = 0x00000010,
   Whole_Word_Match =  0x00000020
 };
}
#endif
