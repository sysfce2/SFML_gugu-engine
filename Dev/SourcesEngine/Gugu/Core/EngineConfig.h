#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include <SFML/Graphics/Color.hpp>

#include <string>

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {

namespace EGameWindow
{
    enum Type
    {
        None,
        Sfml,
    };
}

struct EngineConfig
{
    // Application
    std::string applicationName;
    std::string applicationIcon;

    // Resources
    bool useAssetsFullPaths;
    std::string pathAssets;
    std::string pathScreenshots;
    std::string defaultFont;
    std::string debugFont;
    bool defaultTextureSmooth;

    // Graphics
    EGameWindow::Type gameWindow;
    int windowWidth;
    int windowHeight;
    bool enableVerticalSync;
    int framerateLimit;
    sf::Color backgroundColor;
    bool maximizeWindow;

    // Audio
    int maxSoundTracks;     // Total tracks should not exceed 256
    int maxMusicTracks;     // Total tracks should not exceed 256

    // Debug
    bool showStats;
    bool showFPS;


    EngineConfig()
    {
        applicationName = "Game Project";
        applicationIcon = "";

        useAssetsFullPaths = false;
        pathAssets = "";
        pathScreenshots = "Screenshots";
        defaultFont = "";
        debugFont = "";
        defaultTextureSmooth = false;

        gameWindow = EGameWindow::Sfml;
        windowWidth = 800;
        windowHeight = 600;
        enableVerticalSync = false;
        framerateLimit = 60;
        backgroundColor = sf::Color(128, 128, 128, 255);
        maximizeWindow = false;

        maxSoundTracks = 240;   // Total tracks should not exceed 256
        maxMusicTracks = 16;    // Total tracks should not exceed 256

        showStats = false;
        showFPS = false;
    }
};

}   // namespace gugu
