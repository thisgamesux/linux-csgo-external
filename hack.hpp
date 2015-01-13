#pragma once

#include <stddef.h>
#include "remote.hpp"
#include "log.hpp"

#define TEAM_SPECTATOR          1

#define	LIFE_ALIVE				0 // alive
#define	LIFE_DYING				1 // playing death animation or still falling off of a ledge waiting to hit ground
#define	LIFE_DEAD				2 // dead. lying still.
#define LIFE_RESPAWNABLE		3
#define LIFE_DISCARDBODY		4

#define MAX_TRAIL_LENGTH	    30
#define MAX_PLAYER_NAME_LENGTH  128

namespace hack {
    template<class T> class CUtlVector {
    public:
        T*              DataPtr;                    //0000 (054612C0)
        unsigned int    Max;                        //0004 (054612C4)
        unsigned int    unk02;                      //0008 (054612C8)
        unsigned int    Count;                      //000C (054612CC)
        unsigned int    DataPtrBack;                //0010 (054612D0)
    };

    struct GlowObjectDefinition_t {
        bool ShouldDraw( int nSlot ) const {
            return m_pEntity && ( m_nSplitScreenSlot == -1 || m_nSplitScreenSlot == nSlot ) && ( m_bRenderWhenOccluded || m_bRenderWhenUnoccluded );
        }

        bool IsUnused() const {
            return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE;
        }

        long writeStart() {
            return (long(&(this)->m_flGlowRed) - long(this));
        }

        long writeEnd() {
            return (long(&(this)->unk5) - long(this));
        }

        void*           m_pEntity;                      //0000
        float           m_flGlowRed;                    //0004
        float           m_flGlowGreen;                  //0008
        float           m_flGlowBlue;                   //000C
        float           m_flGlowAlpha;                  //0010
        unsigned char   unk0;                           //0014
        unsigned char   unk1[3];                        //0015 (align padding)
        float           m_flUnknown;                    //0018
        float           m_flBlurAmount;                 //001C
        unsigned int    unk2;                           //0020
        bool            m_bRenderWhenOccluded : 8;      //0024
        bool            m_bRenderWhenUnoccluded : 8;    //0025
        bool            m_bFullBloomRender : 8;         //0026
        unsigned char   unk5;                           //0027 (align padding?)
        int             m_nFullBloomStencilTestValue;   //0028
        int             m_nSplitScreenSlot;             //002C
        int             m_nNextFreeSlot;                //0030

        static const int END_OF_FREE_LIST = -1;
        static const int ENTRY_IN_USE = -2;
    }; // sizeof() == 0x34

    class CGlowObjectManager
    {
    public:
        CUtlVector<GlowObjectDefinition_t> m_GlowObjectDefinitions; //0000
        int m_nFirstFreeSlot; //0014 (054612D4)
        unsigned int unk1; //0018 (054612D8)
        unsigned int unk2; //001C (054612DC)
        unsigned int unk3; //0020 (054612E0)
        unsigned int unk4; //0024 (054612E4)
        unsigned int unk5; //0028 (054612E8)
    };

    struct Entity
    {
        unsigned char   unk0[0xE4];                     //0000
        int             m_iTeamNum;                     //00E4
        int             unk1;                           //00E8
        int             unk2;                           //00EC
        int             m_iHealth;                      //00F0
        unsigned char   unk3[0x15B];                    //00F4
        int             m_lifeState;                    //024F
    };

    struct Color {
        unsigned char _color[4];
    };

    struct Vector {
        float x, y, z;
    };

    struct QAngle {
        float x, y, z;
    };

    struct Vector2D {
        float x, y;
    };

    extern void Glow(remote::Handle* csgo, remote::MapModuleMemoryRegion* client, unsigned long glowAddress);
};