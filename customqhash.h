#ifndef CUSTOMQHASH_H
#define CUSTOMQHASH_H

#include "dictglobalattributes.h"

uint qHash(const DictGlobalAttributes &attrs) {
               return qHash(attrs.getDictname());
             }

#endif // CUSTOMQHASH_H
