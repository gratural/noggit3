// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#version 330 core

in vec3 position;
in vec3 color;

out vec3 f_color;

uniform mat4 model_view_projection;
uniform vec3 offset;
uniform float scale;

void main()
{
  gl_Position = model_view_projection * vec4(position * scale + offset, 1.);

  f_color = color;
}
