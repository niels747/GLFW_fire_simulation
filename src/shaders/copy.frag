#version 330 core
in vec2 texCoord;
layout(location = 0) out vec4 newVel;
precision highp float;

uniform sampler2D velTex;

void main() { newVel = texture(velTex, texCoord); }