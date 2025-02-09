// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#version 330 core

in vec3 position;
in vec3 color;

out vec3 f_color;

uniform mat4 view_projection;
uniform mat4 model;

void main()
{
  gl_Position = view_projection * model * vec4(position, 1.);

  f_color = color;
}
