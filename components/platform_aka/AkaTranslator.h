#pragma once

#include "ITranslator.h"
#include "aka_runtime/aka_runtime.h"

class AkaTranslator : public ITranslator {
  public:
    const char* translate( const char* key ) override {
        return akaRuntime.translate( key );
    }
};
