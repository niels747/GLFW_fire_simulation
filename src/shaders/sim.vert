#version 330 core
layout(location = 0) in vec2 vertPosition;
layout(location = 1) in vec2 vertTexCoord;

out vec2 texCoord;

uniform vec2 texelSize;

out vec2 texCoordXmY0; // left
out vec2 texCoordX0Ym; // down
out vec2 texCoordXpY0; // right
out vec2 texCoordX0Yp; // up

void main() {
  texCoord = vertTexCoord;

  texCoordXmY0 = texCoord + vec2(-texelSize.x, 0.0); // left
  texCoordX0Ym = texCoord + vec2(0.0, -texelSize.y); // down
  texCoordXpY0 = texCoord + vec2(texelSize.x, 0.0);  // right
  texCoordX0Yp = texCoord + vec2(0.0, texelSize.y);  // up

  gl_Position = vec4(vertPosition.x, vertPosition.y, 0.0, 1.0);
}