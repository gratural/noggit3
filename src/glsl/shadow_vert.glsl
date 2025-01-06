#version 410 core

in vec3 position;
in vec3 normal;

out vec3 f_position;
out vec4 f_light_position;
out vec3 f_normal;

uniform mat4 view_proj;
uniform mat4 light_view_proj;

void main()
{
  gl_Position = view_proj * vec4(position, 1.f);

  f_position = position;
  f_light_position = light_view_proj * vec4(position, 1.);
  f_normal = normal;
}