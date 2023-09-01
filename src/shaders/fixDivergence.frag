#version 420 core
in vec2 texCoord;
layout(location = 0) out vec4 newVel;
precision highp float;

layout(binding = 0) uniform sampler2D velTex;

in vec2 texCoordXpY0; // right
in vec2 texCoordX0Yp; // up

void main() {
  vec4 thisVel = texture(velTex, texCoord);

  float thisDiv = thisVel[2];

  float rightDiv = texture(velTex, texCoordXpY0)[2];
  float upDiv = texture(velTex, texCoordX0Yp)[2];

  float divergenceFix_x = thisDiv - rightDiv;
  float divergenceFix_y = thisDiv - upDiv;

  // fix divergence:

  thisVel.x += divergenceFix_x;
  thisVel.y += divergenceFix_y;

  thisVel.xy *= 0.9998; // drag

  newVel = thisVel;
}