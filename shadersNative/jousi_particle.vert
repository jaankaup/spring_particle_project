#version 430

layout(location = 0) in float particle_index;

out float geom_in;

uniform mat4 MVP;

void main() {
  geom_in = particle_index;
}
