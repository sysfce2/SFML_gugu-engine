#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/System/Types.h"

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {

namespace EResourceType
{
    enum Type
    {
        Unknown = 0,

        Texture,
        Font,
        Sound,
        Music,
        SoundCue,
        ImageSet,
        AnimSet,
        ParticleEffect,
        Datasheet,

        Custom,
    };
}

enum class EResourceEvent : uint8
{
    ResourceRemoved,
    DependencyRemoved,
};

}   // namespace gugu
