#version 330 core
in vec2 texCoord;
out vec4 FragColor;

void main() {

  float x = texCoord.x * 20.0;
  int xi = int(floor(x));
  float y = texCoord.y * 20.0;
  int yi = int(floor(y));

  int ii = yi * 20 + xi;

  vec2 center = vec2(0.5, 0.5);

  vec2 dist = texCoord - center;

  float radius = 0.30;

  float angle = acos(dot(dist, vec2(0., 1.)));

  radius += sin(angle * 138.2) * 0.005;

  if (length(dist) < radius + 0.01 && length(dist) > radius - 0.01)
    ii = 0;

  FragColor = vec4(ii / 400.0, mod(ii, 20.0) / 20.0, mod(ii, 10.0) / 10.0, 1.0);
}