#version 420 core
in vec2 texCoord;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D velTex;
layout(binding = 1) uniform sampler2D gasTex;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

#define PRESS 2
#define TEMP 3

#define METHANE 0
#define OXYGEN 1
#define CO2 2

void main() {
  vec4 vel = texture(velTex, texCoord);
  vec4 gas = texture(gasTex, texCoord);

  // FragColor = vec4(1. + vel[3], 1. - abs(vel[3]), 1. -vel[3], 1.0); // blue
  // white red temperature scale

  FragColor = vec4(0.5 + vel[3], 0.5 - abs(vel[3]), 0.5 - vel[3],
                   1.0); // blue gray red temperature scale

  // FragColor = vec4(hsv2rgb(vec3(0.3 - vel[3]* 0.3, 1.0, 1.0)), 1.0); // hue
  // temperature scale

  FragColor.rg += vec2(gas[METHANE]);
  FragColor.rgb -= gas[CO2] * 1.0;

  // if(mod(vel[2], 0.02) < 0.002) // pressure lines
  // 	FragColor.rgb = vec3(0);
}