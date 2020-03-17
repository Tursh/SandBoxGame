/*
 * CloudShader.cpp
 *
 * Created by tursh on 3/17/20.
*/

#include "World/Cloud/CloudShader.h"

const char *VERTEX_SHADER_PATH = "res/graphics/shaders/CloudVertexShader.glsl";
const char *FRAGMENT_SHADER_PATH = "res/graphics/shaders/CloudFragmentShader.glsl";

CloudShader::CloudShader()
    :BasicShader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH, true)
{}
