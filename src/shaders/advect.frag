#version 420 core
precision highp float;

uniform vec2 texelSize;

uniform vec3 mousePos;

in vec2 texCoord;
layout(location = 0) out vec4 newVel;
layout(location = 1) out vec4 newGas;

layout(binding = 0) uniform sampler2D velTex;
layout(binding = 1) uniform sampler2D gasTex;

#define PRESS 2
#define TEMP 3

#define METHANE 0
#define OXYGEN 1
#define CO2 2

void main() {
  vec2 vel = texture(velTex, texCoord).xy;
  newVel = texture(velTex, texCoord - vel * texelSize);
  newGas = texture(gasTex, texCoord - vel * texelSize);

  newVel.y += newVel[TEMP] * 0.0004; // boyancy

  if (length(texCoord - mousePos.xy) < 0.03) // mouse position
  {

    if (mousePos.z > 0.5) {

      if (mousePos.z < 1.5) { // left mouse
        newGas[METHANE] = 1.0;
      } else { // right mouse

        newGas[CO2] = 1.0;
      }
    }
  }

  if (length(texCoord - vec2(0.15, 0.25)) < 0.009) {
    newVel.x += 0.010; // moving left
    newVel.y -= 0.005; // moving up
    newGas[METHANE] = 1.0;
  }

  if (length(texCoord - vec2(0.15, 0.20)) < 0.009) {
    newVel.x += 0.020; // moving right
                       // newVel.y += 0.000; // moving up
    newGas[METHANE] = 1.0;
  }

  if (length(texCoord - vec2(0.15, 0.15)) < 0.009) {
    newVel.x += 0.020; // moving left
    newVel.y += 0.005; // moving up
    newGas[METHANE] = 1.0;
  }

  if (length(texCoord - vec2(0.4, 0.1)) < 0.002) // ignition source
  {
    newVel[TEMP] = 1.0; // heat
  }

  if (length(texCoord - vec2(0.85, 0.25)) < 0.009) {
    newVel.x -= 0.010; // moving left
    newVel.y -= 0.005; // moving up
    newGas[METHANE] = 1.0;
  }

  if (length(texCoord - vec2(0.85, 0.20)) < 0.009) {
    newVel.x -= 0.020; // moving right
                       // newVel.y += 0.000; // moving up
    newGas[METHANE] = 1.0;
  }

  if (length(texCoord - vec2(0.85, 0.15)) < 0.009) {
    newVel.x -= 0.020; // moving left
    newVel.y += 0.005; // moving up
    newGas[METHANE] = 1.0;
  }

  if (length(texCoord - vec2(0.6, 0.1)) < 0.002) // ignition source
  {
    newVel[TEMP] = 1.0; // heat
  }
}