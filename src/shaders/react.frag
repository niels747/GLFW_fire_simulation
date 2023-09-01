#version 420 core
in vec2 texCoord;
layout(location = 0) out vec4 newVel;
layout(location = 1) out vec4 newGas;
precision highp float;

layout(binding = 0) uniform sampler2D velTex;
layout(binding = 1) uniform sampler2D gasTex;

#define PRESS 2
#define TEMP 3

#define METHANE 0
#define OXYGEN 1
#define CO2 2

void main() {
  newVel = texture(velTex, texCoord);
  newGas = texture(gasTex, texCoord);

  float prevTemperature = newVel[TEMP];

  newGas[OXYGEN] = 1.0 - newGas[METHANE] - newGas[CO2];

  float mixingRatio = newGas[OXYGEN] / newGas[METHANE];

  if (abs(mixingRatio - 9.0) < 4.0 && newVel[TEMP] > 0.01) { // burnning

    float burnRate = newGas[METHANE] * newVel[TEMP];

    newVel[TEMP] += burnRate * 3.0; // heat
                                    // newVel[PRESS] += burnRate * 1.;
    newGas[METHANE] -= burnRate * 1.0;
    newGas[CO2] += burnRate * 1.0;
  }

  newVel[TEMP] *= 0.998; // temperature dissapation

  newVel[PRESS] += (newVel[TEMP] - prevTemperature) *
                   0.4; // pressure change is proportional to temperature change

  newGas[CO2] *= 0.9998; // co2 dissapation

  newGas[OXYGEN] = 1.0 - newGas[METHANE] - newGas[CO2];
}