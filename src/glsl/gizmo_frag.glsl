// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#version 330 core

in vec3 f_color;

out vec4 out_color;

uniform float alpha;

void main()
{
  out_color = vec4(f_color, alpha);
}
