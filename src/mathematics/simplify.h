#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include <stdlib.h>

#include "derivative.h"

ssize_t
SimplifyNeutralMultipliers(derivative_t derivative,
                           ssize_t      current_node);

ssize_t
SimplifyConst(derivative_t derivative,
              ssize_t      current_node);

#endif // SIMPLIFY_H