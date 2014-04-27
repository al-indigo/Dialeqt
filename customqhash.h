#ifndef __CUSTOMQHASH_H
#define __CUSTOMQHASH_H

#include "dictglobalattributes.h"

uint qHash(const DictGlobalAttributes &attrs) {
               return qHash(attrs.getDictname());
             }

#endif // __CUSTOMQHASH_H
