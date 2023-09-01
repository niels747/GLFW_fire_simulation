#version 330 core
in vec2 texCoord;
layout(location = 0) out vec4 newVel;
precision highp float;
// precision highp sampler2D;

uniform sampler2D velTex;

in vec2 texCoordXmY0; // left
in vec2 texCoordX0Ym; // down

void main() {
  vec4 thisVel = texture(velTex, texCoord);

  float leftVel = texture(velTex, texCoordXmY0).x;
  float downVel = texture(velTex, texCoordX0Ym).y;

  float divergence = leftVel - thisVel.x + downVel - thisVel.y;

  thisVel[2] += divergence * 0.499; // pressure

  // thisVel[2] = divergence * 0.245;

  newVel = thisVel;
}