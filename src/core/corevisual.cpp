#include <stdio.h>

#include "core.h"
#include "corevisual.h"

CoreVisual::CoreVisual()
{
  key = 0;
}

void CoreVisual::registerVisual()
{
  if (!key) {
    key =  Core::registerVisual(this);
    //printf("visual: %p\n", this);
  }
}
