////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Common.h"

#include "Demo.h"

#include "Gugu/Engine.h"

using namespace demoproject;
using namespace gugu;

////////////////////////////////////////////////////////////////
// File Implementation

int main(int argc, char* argv[])
{
    //Init engine
    EngineConfig config;
    config.applicationName = "GuguEngine Demo Grid";
    config.pathAssets = "Assets";
    config.defaultFont = "Spaceranger.ttf";
    config.debugFont = "Roboto-Regular.ttf";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    
    GetEngine()->Init(config);

    //--------

    GetEngine()->SetApplication(new Demo);

    //--------

    GetEngine()->StartLooping();
    GetEngine()->Release();

    return 0;
}