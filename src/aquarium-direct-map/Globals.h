//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Globals.h: Define global variables, constant variables, global texture map, program map and
// scene map.

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cmath>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "Scene.h"

#include "common/FPSTimer.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
const std::string slash = "\\";
#define M_PI 3.141592653589793
#else
const std::string slash = "/";
#endif

class Scene;
class Program;
class Texture;

static FPSTimer g_fpsTimer;           // object to measure frames per second;
static std::unordered_map<std::string, Scene *> g_scenes;  // each of the models
static std::unordered_map<std::string, std::multimap<std::string, std::vector<float>>>
    g_sceneGroups;  // the placement of the models
static std::unordered_map<std::string, Program *>
    g_programMap;  // store all compiled program in this map
static std::unordered_map<std::string, Texture *>
    g_textureMap;  // store all loaded textures in this map
constexpr bool g_fog = true;

constexpr float g_tailOffsetMult = 1.0f;
constexpr float g_speed = 1.0f;

constexpr int numFishSmall = 100;
constexpr int numFishMedium = 1000;
constexpr int numFishBig = 10000;
constexpr int numFishLeftSmall = 80;
constexpr int numFishLeftBig = 160;
constexpr float sand_shininess = 5.0f;
constexpr float sand_specularFactor = 0.3f;
constexpr float generic_shininess = 50.0f;
constexpr float generic_specularFactor = 1.0f;
constexpr float outside_shininess = 50.0f;
constexpr float outside_specularFactor = 0.0f;
constexpr float seaweed_shininess = 50.0f;
constexpr float seaweed_specularFactor = 1.0f;
constexpr float inner_shininess = 50.0f;
constexpr float inner_specularFactor = 1.0f;
constexpr float fish_shininess = 5.0f;
constexpr float fish_specularFactor = 0.3f;

struct G_ui_per
{
    const char* obj;
    const char* name;
    float value;
    float max;
    float min;
};

static std::unordered_map<std::string, std::unordered_map<std::string, float>>
    g;  // set min value as initialize value

constexpr float g_fovFudge = 1.0f;
constexpr float g_net_offset[3] = { 0.0f, 0.0f, 0.0f };
constexpr float g_net_offsetMult = 1.21f;

struct G_viewSettings {
    float targetHeight = 63.3f;
    float targetRadius = 91.6f;
    float eyeHeight = 7.5f;
    float eyeRadius = 13.2f;
    float eyeSpeed = 0.0258f;
    float fieldOfView = 82.699f;
    float ambientRed = 0.218f;
    float ambientGreen = 0.502f;
    float ambientBlue = 0.706f;
    float fogPower = 16.5f;
    float fogMult = 1.5f; //2.02,
    float fogOffset = 0.738f;
    float fogRed = 0.338f;
    float fogGreen = 0.81f;
    float fogBlue = 1.0f;
    float refractionFudge = 3.0f;
    float eta = 1.0f;
    float tankColorFudge = 0.796f;
}constexpr g_viewSettings;

static std::vector<float> projection(16);
static std::vector<float> view(16);
static std::vector<float> world(16);
static std::vector<float> worldInverse(16);
static std::vector<float> worldInverseTraspose(16);
static std::vector<float> viewProjection(16);
static std::vector<float> worldViewProjection(16);
static std::vector<float> viewInverse(16);
static std::vector<float> viewProjectionInverse(16);
static std::vector<float> skyView(16);
static std::vector<float> skyViewProjection(16);
static std::vector<float> skyViewProjectionInverse(16);
static std::vector<float> eyePosition(3);
static std::vector<float> target(3);
static std::vector<float> up = {0, 1, 0};
static std::vector<float> lightWorldPos(3);
static std::vector<float> v3t0(3);
static std::vector<float> v3t1(3);
static std::vector<float> m4t0(16);
static std::vector<float> m4t1(16);
static std::vector<float> m4t2(16);
static std::vector<float> m4t3(16);
static std::vector<float> colorMult  = {1, 1, 1, 1};
static std::vector<float> lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
static std::vector<float> specular   = {1.0f, 1.0f, 1.0f, 1.0f};
static std::vector<float> ambient(4);
static std::vector<float> fogColor = {1.0f, 1.0f, 1.0f, 1.0f};

//Generic uniforms
struct GenericConst
{
    std::vector<float> *viewProjection;
    std::vector<float> *viewInverse;
    std::vector<float> *lightWorldPos;
    std::vector<float> *lightColor;
    std::vector<float> *specular;
    std::vector<float> *ambient;
    std::vector<float> *fogColor;
    float shininess;
    float specularFactor;
    float fogPower;
    float fogMult;
    float fogOffset;

    float eta;
    float tankColorFudge;
    float refractionFudge;
};
static GenericConst sandConst, genericConst, seaweedConst, outsideConst, innerConst;

struct GenericPer
{
    std::vector<float> *world;
    std::vector<float> *worldViewProjection;
    std::vector<float> *worldInverse;
    std::vector<float> *worldInverseTranspose;
    float time;
};

static GenericPer sandPer, genericPer, seaweedPer, outsidePer, innerPer, laserPer, skyPer;

struct SkyConst
{
    std::vector<float> *viewProjectionInverse;
};

static SkyConst skyConst;

struct FishPer
{
    std::vector<float> worldPosition;
    std::vector<float> nextPosition;
    float scale;
    float time;
};

static FishPer fishPer;

struct G_sceneInfo
{
    std::string name;
    std::string program[2];
    bool fog;
    std::string group;
    bool blend;
};

struct ConstUniforms
{
    float fishLength;
    float fishWaveLength;
    float fishBendAmount;
};

struct Fish {
    std::string name;
    float speed;
    float speedRange;
    float radius;
    float radiusRange;
    float tailSpeed;
    float heightOffset;
    float heightRange;

    ConstUniforms constUniforms;

    bool lasers;
    float laserRot;
    float laserOff[3];
    float laserScale[3];
    int num;
};

struct FishConst
{
    GenericConst genericConst;
    ConstUniforms constUniforms;
};

static FishConst fishConst;

const std::string repoFolder     = "aquarium-native";
const std::string sourceFolder   = "src";
const std::string shaderFolder   = "shaders";
const std::string shaderVersion  = "opengl/450";
const std::string resourceFolder = "assets";

#endif  // GLOBALS_H
