#ifndef __G_PUBLIC_MP_H__
#define __G_PUBLIC_MP_H__

#include <stdint.h>

#ifndef __cplusplus

typedef struct
{
  uint16_t number;
  uint16_t infoIndex;
}EntHandle;

#else

class EntHandle
{
    public:
        bool __cdecl isDefined();
        struct gentity_s* __cdecl ent( );
        void __cdecl setEnt(struct gentity_s *ent);
        static void __cdecl Shutdown();
        static void __cdecl Init();

        uint16_t number;
        uint16_t infoIndex;

};

#endif

#include "entity.h"
#include "player.h"

unsigned int __cdecl GScr_AllocString(const char *s);

#endif