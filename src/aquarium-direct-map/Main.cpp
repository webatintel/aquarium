//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Main.cpp: Create context and window for OpenGL graphics API.
// Data preparation, load resources and wrap them into scenes.
// Implements logic of rendering background, fishes, seaweeds and
// other models. Calculate fish count for each type of fish.
// Update uniforms for each frame. Show fps for each frame.

#ifdef _WIN32
#include <direct.h>
#include "Windows.h"
#elif __APPLE__
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Globals.h"
#include "Matrix.h"
#include "Model.h"
#include "Program.h"

#include "GLFW/glfw3.h"
#include "common/AQUARIUM_ASSERT.h"
#include "include/CmdArgsHelper.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#define min(a,b) ((a)<(b)?(a):(b))

void render();

// Define glfw window and the size of window
GLFWwindow *window;
int clientWidth;
int clientHeight;

// Get current path of the binary
std::string mPath;

// The number of fish is passed from cmd args directly
int g_numFish;

// Variables calculate time
float then = 0.0f;
float mClock = 0.0f;
float eyeClock = 0.0f;

const G_ui_per g_ui[] = {
    { "globals", "speed", 1.0f, 4.0f },{ "globals", "targetHeight", 0.0f, 150.0f },
    { "globals", "targetRadius", 88.0f, 200.0f },{ "globals", "eyeHeight", 19.0f, 150.0f },
    { "globals", "eyeSpeed", 0.06f, 1.0f },{ "globals", "fieldOfView", 85.0f, 179.0f, 1.0f },
    { "globals", "ambientRed", 0.22f, 1.0f },{ "globals", "ambientGreen", 0.25f, 1.0f },
    { "globals", "ambientBlue", 0.39f, 1.0f },{ "globals", "fogPower", 14.5f, 50.0f },
    { "globals", "fogMult", 1.66f, 10.0f },{ "globals", "fogOffset", 0.53f, 3.0f },
    { "globals", "fogRed", 0.54f, 1.0f },{ "globals", "fogGreen", 0.86f, 1.0f },
    { "globals", "fogBlue", 1.0f, 1.0f },{ "fish", "fishHeightRange", 1.0f, 3.0f },
    { "fish", "fishHeight", 25.0f, 50.0f },{ "fish", "fishSpeed", 0.124f, 2.0f },
    { "fish", "fishOffset", 0.52f, 2.0f },{ "fish", "fishXClock", 1.0f, 2.0f },
    { "fish", "fishYClock", 0.556f, 2.0f },{ "fish", "fishZClock", 1.0f, 2.0f },
    { "fish", "fishTailSpeed", 1.0f, 30.0f },{ "innerConst", "refractionFudge", 3.0f, 50.0f },
    { "innerConst", "eta", 1.0f, 1.20f },{ "innerConst", "tankColorFudge", 0.8f, 2.0f } };

G_sceneInfo g_sceneInfo[] = { { "SmallFishA",{ "fishVertexShader", "fishReflectionFragmentShader" }, true },
{ "MediumFishA",{ "fishVertexShader", "fishNormalMapFragmentShader" }, true },
{ "MediumFishB",{ "fishVertexShader", "fishReflectionFragmentShader" }, true },
{ "BigFishA",{ "fishVertexShader", "fishNormalMapFragmentShader" }, true },
{ "BigFishB",{ "fishVertexShader", "fishNormalMapFragmentShader" }, true },
{ "Arch",{ "", "" }, true },
{ "Coral",{ "", "" }, true },
{ "CoralStoneA",{ "", "" }, true },
{ "CoralStoneB",{ "", "" }, true },
{ "EnvironmentBox",{ "diffuseVertexShader", "diffuseFragmentShader" }, false, "outside" },
{ "FloorBase_Baked",{ "", "" }, true },
{ "FloorCenter",{ "", "" }, true },
{ "GlobeBase",{ "diffuseVertexShader", "diffuseFragmentShader" } },
{ "GlobeInner",{ "innerRefractionMapVertexShader", "innerRefractionMapFragmentShader" }, true, "inner" },
{ "RockA",{ "", "" }, true },
{ "RockB",{ "", "" }, true },
{ "RockC",{ "", "" }, true },
{ "RuinColumn",{ "", "" }, true },
{ "Skybox",{ "diffuseVertexShader", "diffuseFragmentShader" }, false, "outside" },
{ "Stone",{ "", "" }, true },
{ "Stones",{ "", "" }, true },
{ "SunknShip",{ "", "" }, true },
{ "SunknSub",{ "", "" }, true },
{ "SupportBeams",{ "", "" }, false, "outside" },
{ "SeaweedA",{ "seaweedVertexShader", "seaweedFragmentShader" }, false, "seaweed", true },
{ "SeaweedB",{ "seaweedVertexShader", "seaweedFragmentShader" }, false, "seaweed", true },
{ "TreasureChest",{ "", "" }, true } };

std::unordered_map<std::string, G_sceneInfo> g_sceneInfoByName;

Fish g_fishTable[] = { { "SmallFishA", 1.0f, 1.5f, 30.0f, 25.0f, 10.0f, 0.0f, 16.0f,{ 10.0f, 1.0f, 2.0f } },
{ "MediumFishA", 1.0f, 2.0f, 10.0f, 20.0f, 1.0f, 0.0f, 16.0f,{ 10.0f, -2.0f, 2.0f } },
{ "MediumFishB", 0.5f, 4.0f, 10.0f, 20.0f, 3.0f, -8.0f, 5.0f,{ 10.0f, -2.0f, 2.0f } },
{ "BigFishA", 0.5f, 0.5f, 50.0f, 3.0f, 1.5f, 0.0f, 16.0f,{ 10.0f, -1.0f, 0.5f }, true, 0.04f,{ 0.0f, 0.1f, 9.0f },{ 0.3f, 0.3f, 1000.0f } },
{ "BigFishB", 0.5f, 0.5f, 45.0f, 3.0f, 1.0f, 0.0f, 16.0f,{ 10.0f, -0.7f, 0.3f }, true, 0.04f,{ 0.0f, -0.3f, 9.0f },{ 0.3f, 0.3f, 1000.0f } } };

void setGenericConstMatrix(GenericConst *genericConst) {
    genericConst->viewProjection = &viewProjection;
    genericConst->viewInverse    = &viewInverse;
    genericConst->lightWorldPos  = &lightWorldPos;
    genericConst->lightColor     = &lightColor;
    genericConst->specular       = &specular;
    genericConst->ambient        = &ambient;
    genericConst->fogColor       = &fogColor;
}

void setGenericPer(GenericPer *genericPer)
{
    genericPer->world                 = &world;
    genericPer->worldViewProjection  = &worldViewProjection;
    genericPer->worldInverse          = &worldInverse;
    genericPer->worldInverseTranspose = &worldInverseTraspose;
}

void initializeUniforms() {
    sandConst.shininess      = sand_shininess;
    sandConst.specularFactor = sand_specularFactor;

    genericConst.shininess      = generic_shininess;
    genericConst.specularFactor = generic_specularFactor;

    outsideConst.shininess      = outside_shininess;
    outsideConst.specularFactor = outside_shininess;

    seaweedConst.shininess      = seaweed_shininess;
    seaweedConst.specularFactor = seaweed_specularFactor;

    innerConst.shininess                   = inner_shininess;
    innerConst.specularFactor              = inner_specularFactor;
    innerConst.eta                         = g_viewSettings.eta;
    innerConst.refractionFudge             = g_viewSettings.refractionFudge;
    innerConst.tankColorFudge              = g_viewSettings.tankColorFudge;

    fishConst.genericConst.shininess      = fish_shininess;
    fishConst.genericConst.specularFactor = fish_specularFactor;

    setGenericConstMatrix(&sandConst);
    setGenericConstMatrix(&genericConst);
    setGenericConstMatrix(&seaweedConst);
    setGenericConstMatrix(&innerConst);
    setGenericConstMatrix(&outsideConst);
    setGenericConstMatrix(&fishConst.genericConst);
    setGenericPer(&sandPer);
    setGenericPer(&genericPer);
    setGenericPer(&seaweedPer);
    setGenericPer(&innerPer);
    setGenericPer(&outsidePer);
    setGenericPer(&laserPer);

    skyConst.viewProjectionInverse = &viewProjectionInverse;

    fishPer.worldPosition.resize(3);
    fishPer.nextPosition.resize(3);
}

void initializeGlobalInfo()
{
    for (auto &value : g_ui)
    {
        g[value.obj][value.name] = value.value;
    }
}

// Load json file from assets. Initialize g_sceneGroups and classify groups.
void LoadPlacement()
{
    std::ostringstream oss;
    oss << mPath << resourceFolder << slash << "PropPlacement.js";
    std::string proppath = oss.str();
    std::ifstream PlacementStream(proppath, std::ios::in);
    rapidjson::IStreamWrapper isPlacement(PlacementStream);
    rapidjson::Document document;
    document.ParseStream(isPlacement);

    ASSERT(document.IsObject());

    ASSERT(document.HasMember("objects"));
    const rapidjson::Value &objects = document["objects"];
    ASSERT(objects.IsArray());

    for (auto &info : g_sceneInfo)
    {
        g_sceneInfoByName[info.name] = info;
    }

    for (rapidjson::SizeType i = 0; i < objects.Size(); ++i)
    {
        const rapidjson::Value &name        = objects[i]["name"];
        const rapidjson::Value &worldMatrix = objects[i]["worldMatrix"];
        ASSERT(worldMatrix.IsArray() && worldMatrix.Size() == 16);

        std::string groupName = g_sceneInfoByName[name.GetString()].group;
        if (groupName == "")
        {
            groupName = "base";
        }

        std::multimap<std::string, std::vector<float>> &group = g_sceneGroups[groupName];

        std::vector<float> matrix;
        for (rapidjson::SizeType j = 0; j < worldMatrix.Size(); ++j)
        {
            matrix.push_back(worldMatrix[j].GetFloat());
        }
        group.insert(make_pair(name.GetString(), matrix));
    }
}

Scene *loadScene(const std::string &name, const std::string opt_programIds[2])
{
    Scene *scene = new Scene(opt_programIds);
    scene->load(mPath, name);
    return scene;
}

// Initialize g_scenes.
void LoadScenes()
{
    for (auto &info : g_sceneInfo)
    {
        g_scenes[info.name] = loadScene(info.name, info.program);
    }
}

void onDestroy()
{
    for (auto &program : g_programMap)
    {
        if (program.second != nullptr)
        {
            delete program.second;
            program.second = nullptr;
        }
    }

    for (auto &texture : g_textureMap)
    {
        if (texture.second != nullptr)
        {
            delete texture.second;
            texture.second = nullptr;
        }
    }

    for (auto &scene : g_scenes)
    {
        delete scene.second;
    }
}

void getCurrentPath()
{
    // Get path of current build.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    TCHAR temp[200];
    GetModuleFileName(NULL, temp, MAX_PATH);
    std::wstring ws(temp);
    mPath       = std::string(ws.begin(), ws.end());
    size_t nPos = mPath.find_last_of(slash);
    mPath       = mPath.substr(0, nPos) + slash + ".." + slash + ".." + slash;
#elif __APPLE__
    char temp[200];
    uint32_t size = sizeof(temp);
    _NSGetExecutablePath(temp, &size);
    mPath    = std::string(temp);
    int nPos = mPath.find_last_of(slash);
    mPath    = mPath.substr(0, nPos) + slash + ".." + slash + ".." + slash;
#else
    char temp[200];
    readlink("/proc/self/exe", temp, sizeof(temp));
    mPath    = std::string(temp);
    int nPos = mPath.find_last_of(slash);
    mPath    = mPath.substr(0, nPos) + slash + ".." + slash + ".." + slash;
#endif
}

bool initialize(int argc, char **argv)
{
    getCurrentPath();

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initializeGlobalInfo();
    initializeUniforms();
    LoadPlacement();

    LoadScenes();

    // "--num-fish" {numfish}: imply rendering fish count.
    char* pNext;
    for (int i = 1; i < argc; ++i)
    {
        std::string cmd(argv[i]);
        if (cmd == "--h" || cmd == "-h")
        {
            std::cout << cmdArgsStrAquarium << std::endl;
            return false;
        }
        else if (cmd == "--num-fish")
        {
            g_numFish = strtol(argv[i++ + 1], &pNext, 10);
        }
    }

    // Calculate fish count for each float of fish
    std::string floats[3] = {"Big", "Medium", "Small"};
    int totalFish = g_numFish;
        int numLeft = totalFish;
        for (auto &type : floats)
        {
            for (auto &fishInfo : g_fishTable)
            {
                std::string &fishName = fishInfo.name;
                if (fishName.find(type))
                {
                    continue;
                }
                int numfloat = numLeft;
                if (type == "Big")
                {
                    int temp = totalFish < numFishSmall ? 1 : 2;
                    numfloat = min(numLeft, temp);
                }
                else if (type == "Medium")
                {
                    if (totalFish < numFishMedium)
                    {
                        numfloat = min(numLeft, totalFish / 10);
                    }
                    else if (totalFish < numFishBig)
                    {
                        numfloat = min(numLeft, numFishLeftSmall);
                    }
                    else
                    {
                        numfloat = min(numLeft, numFishLeftBig);
                    }
                }
                numLeft = numLeft - numfloat;
                fishInfo.num = numfloat;
            }
    }

    return true;
}

int main(int argc, char **argv) {

    // initialize GLFW
    if (!glfwInit())
    {
        std::cout << stderr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#elif _WIN32 || __linux__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    GLFWmonitor *pMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
    clientWidth = mode->width;
    clientHeight = mode->height;

    window = glfwCreateWindow(clientWidth, clientHeight, "Aquarium", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        std::cout << "Something went wrong!" << std::endl;
        exit(-1);
    }

    if (!initialize(argc, argv))
    {
        return -1;
    }

    const char *renderer = (const char *)glGetString(GL_RENDERER);
    std::cout << renderer << std::endl;

    // Get the resolution of screen. The resolution on mac is about 4 times larger than size of
    // window.
    glfwGetFramebufferSize(window, &clientWidth, &clientHeight);
    glViewport(0, 0, clientWidth, clientHeight);

    glfwShowWindow(window);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    onDestroy();

    return 0;
}

void DrawGroup(const std::multimap<std::string, std::vector<float>> &group,
               const GenericConst &constUniforms,
               GenericPer *perUniforms)
{
    Model *currentModel = nullptr;
    int ii              = 0;
    for (auto &object : group)
    {
        if (g_scenes.find(object.first) == g_scenes.end())
        {
            continue;
        }
        auto &scene = g_scenes[object.first];
        auto &info  = g_sceneInfoByName[object.first];

        if (info.blend)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }

        auto &models = scene->getModels();
        for (auto &model : models)
        {
            if (model != currentModel)
            {
                currentModel = model;
                model->prepareForDraw(constUniforms);
            }

            world = std::vector<float>(object.second.begin(), object.second.end());
            matrix::mulMatrixMatrix4(worldViewProjection, world, viewProjection);
            matrix::inverse4(worldInverse, world);
            matrix::transpose4(worldInverseTraspose, worldInverse);
            perUniforms->time = mClock + (ii++);
            model->draw(*perUniforms);
        }
    }
}

void render() {
    // Update our time
#ifdef _WIN32
    float now = GetTickCount64() / 1000.0f;
#else
    float now = clock() / 1000000.0f;
#endif
    float elapsedTime = 0.0f;
    if (then == 0.0f)
    {
        elapsedTime = 0.0f;
    }
    else
    {
        elapsedTime = now - then;
    }
    then = now;

    g_fpsTimer.update(elapsedTime, 0, 1);
    std::string text =
        "Aquarium FPS: " + std::to_string(static_cast<unsigned int>(g_fpsTimer.getAverageFPS()));
    glfwSetWindowTitle(window, text.c_str());

    mClock += elapsedTime * g_speed;
    eyeClock += elapsedTime * g_viewSettings.eyeSpeed;

    eyePosition[0] = sin(eyeClock) * g_viewSettings.eyeRadius;
    eyePosition[1] = g_viewSettings.eyeHeight;
    eyePosition[2] = cos(eyeClock) * g_viewSettings.eyeRadius;
    target[0]      = static_cast<float>(sin(eyeClock + M_PI)) * g_viewSettings.targetRadius;
    target[1]      = g_viewSettings.targetHeight;
    target[2]      = static_cast<float>(cos(eyeClock + M_PI)) * g_viewSettings.targetRadius;

    ambient[0] = g_viewSettings.ambientRed;
    ambient[1] = g_viewSettings.ambientGreen;
    ambient[2] = g_viewSettings.ambientBlue;

    glColorMask(true, true, true, true);
    glClearColor(0, 0.8f, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    float nearPlane = 1;
    float farPlane  = 25000.0f;
    float aspect    = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
    float top       = tan(matrix::degToRad(g_viewSettings.fieldOfView * g_fovFudge) * 0.5f) * nearPlane;
    float bottom    = -top;
    float left      = aspect * bottom;
    float right     = aspect * top;
    float width     = abs(right - left);
    float height    = abs(top - bottom);
    float xOff      = width * g_net_offset[0] * g_net_offsetMult;
    float yOff      = height * g_net_offset[1] * g_net_offsetMult;

    matrix::frustum(projection, left + xOff, right + xOff, bottom + yOff, top + yOff, nearPlane,
                    farPlane);
    matrix::cameraLookAt(viewInverse, eyePosition, target, up);
    matrix::inverse4(view, viewInverse);
    matrix::mulMatrixMatrix4(viewProjection, view, projection);
    matrix::inverse4(viewProjectionInverse, viewProjection);

    skyView     = view;
    skyView[12] = 0.0;
    skyView[13] = 0.0;
    skyView[14] = 0.0;
    matrix::mulMatrixMatrix4(skyViewProjection, skyView, projection);
    matrix::inverse4(skyViewProjectionInverse, skyViewProjection);

    matrix::getAxis(v3t0, viewInverse, 0);
    matrix::getAxis(v3t1, viewInverse, 1);
    matrix::mulScalarVector(20.0f, v3t0);
    matrix::mulScalarVector(30.0f, v3t1);
    matrix::addVector(lightWorldPos, eyePosition, v3t0);
    matrix::addVector(lightWorldPos, lightWorldPos, v3t1);

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_CULL_FACE);
    
    matrix::resetPseudoRandom();

    glDepthMask(true);

    if (g_fog)
    {
        genericConst.fogPower             = g_viewSettings.fogPower;
        genericConst.fogMult              = g_viewSettings.fogMult;
        genericConst.fogOffset            = g_viewSettings.fogOffset;
        fishConst.genericConst.fogPower   = g_viewSettings.fogPower;
        fishConst.genericConst.fogMult    = g_viewSettings.fogMult;
        fishConst.genericConst.fogOffset  = g_viewSettings.fogOffset;
        innerConst.fogPower               = g_viewSettings.fogPower;
        innerConst.fogMult                = g_viewSettings.fogMult;
        innerConst.fogOffset              = g_viewSettings.fogOffset;
        seaweedConst.fogPower             = g_viewSettings.fogPower;
        seaweedConst.fogMult              = g_viewSettings.fogMult;
        seaweedConst.fogOffset            = g_viewSettings.fogOffset;
        fogColor[0]                     = g_viewSettings.fogRed;
        fogColor[1]                     = g_viewSettings.fogGreen;
        fogColor[2]                     = g_viewSettings.fogBlue;
    }

    // Draw Scene
    if (g_sceneGroups.find("base") != g_sceneGroups.end())
    {
        DrawGroup(g_sceneGroups["base"], genericConst, &genericPer);
    }

    // Draw Fishes
    glEnable(GL_BLEND);
    for (auto &fishInfo : g_fishTable)
    {
        std::string &fishName = fishInfo.name;
        int numFish           = fishInfo.num;

        Scene *scene = g_scenes[fishName];
        if (scene->loaded)
        {
            Model *fish             = scene->getModels()[0];
            auto &f                 = g["fish"];
            fishConst.constUniforms = fishInfo.constUniforms;
            fish->prepareForDraw(fishConst);
            float fishBaseClock                  = mClock * f["fishSpeed"];
            float fishRadius                     = fishInfo.radius;
            float fishRadiusRange                = fishInfo.radiusRange;
            float fishSpeed                      = fishInfo.speed;
            float fishSpeedRange                 = fishInfo.speedRange;
            float fishTailSpeed                  = fishInfo.tailSpeed * f["fishTailSpeed"];
            float fishOffset                     = f["fishOffset"];
            // float fishClockSpeed                 = f["fishSpeed"];
            float fishHeight                     = f["fishHeight"] + fishInfo.heightOffset;
            float fishHeightRange                = f["fishHeightRange"] * fishInfo.heightRange;
            float fishXClock                     = f["fishXClock"];
            float fishYClock                     = f["fishYClock"];
            float fishZClock                     = f["fishZClock"];
            std::vector<float> &fishPosition     = fishPer.worldPosition;
            std::vector<float> &fishNextPosition = fishPer.nextPosition;
            for (int ii = 0; ii < numFish; ++ii)
            {
                float fishClock = fishBaseClock + ii * fishOffset;
                float speed =
                    fishSpeed + static_cast<float>(matrix::pseudoRandom()) * fishSpeedRange;
                float scale = 1.0f + static_cast<float>(matrix::pseudoRandom()) * 1;
                float xRadius =
                    fishRadius + static_cast<float>(matrix::pseudoRandom()) * fishRadiusRange;
                float yRadius = 2.0f + static_cast<float>(matrix::pseudoRandom()) * fishHeightRange;
                float zRadius =
                    fishRadius + static_cast<float>(matrix::pseudoRandom()) * fishRadiusRange;
                float fishSpeedClock = fishClock * speed;
                float xClock         = fishSpeedClock * fishXClock;
                float yClock         = fishSpeedClock * fishYClock;
                float zClock         = fishSpeedClock * fishZClock;

                fishPosition[0]     = sin(xClock) * xRadius;
                fishPosition[1]     = sin(yClock) * yRadius + fishHeight;
                fishPosition[2]     = cos(zClock) * zRadius;
                fishNextPosition[0] = sin(xClock - 0.04f) * xRadius;
                fishNextPosition[1] = sin(yClock - 0.01f) * yRadius + fishHeight;
                fishNextPosition[2] = cos(zClock - 0.04f) * zRadius;
                fishPer.scale       = scale;

                fishPer.time = fmod((mClock + ii * g_tailOffsetMult) * fishTailSpeed * speed,
                                    static_cast<float>(M_PI) * 2);
                fish->draw(fishPer);
            }
        }
    }

    // Draw tank
    if (g_sceneGroups.find("inner") != g_sceneGroups.end())
    {
        DrawGroup(g_sceneGroups["inner"], innerConst, &innerPer);
    }

    // Draw seaweed
    if (g_sceneGroups.find("seaweed") != g_sceneGroups.end())
    {
        DrawGroup(g_sceneGroups["seaweed"], seaweedConst, &seaweedPer);
    }

    // Draw outside
    if (g_sceneGroups.find("outside") != g_sceneGroups.end())
    {
        DrawGroup(g_sceneGroups["outside"], outsideConst, &outsidePer);
    }
}
