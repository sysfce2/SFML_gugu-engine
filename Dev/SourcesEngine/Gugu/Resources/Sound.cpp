////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/Resources/Sound.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/System/Memory.h"
#include "Gugu/System/String.h"
#include "Gugu/Debug/Logger.h"

#include <SFML/Audio/SoundBuffer.hpp>

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

Sound::Sound()
: m_sfSoundBuffer(nullptr)
{
}

Sound::~Sound()
{
    Unload();
}

sf::SoundBuffer* Sound::GetSFSoundBuffer() const
{
    return m_sfSoundBuffer;
}

EResourceType::Type Sound::GetResourceType() const
{
    return EResourceType::Sound;
}

void Sound::Unload()
{
    SafeDelete(m_sfSoundBuffer);
}

bool Sound::LoadFromFile()
{
    Unload();

    m_sfSoundBuffer = new sf::SoundBuffer;
    if (!m_sfSoundBuffer->loadFromFile(GetFileInfo().GetFileSystemPath()))
    {
        GetLogEngine()->Print(ELog::Warning, ELogEngine::Resources, StringFormat("Sound not found : {0}", GetFileInfo().GetFilePath_utf8()));
        return false;
    }

    return true;
}

}   // namespace gugu
