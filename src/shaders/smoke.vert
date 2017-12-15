R"zzz(
#version 330 core
in vec4 vertex_position;
in vec4 point_velocity;
in float color_type;
out vec4 velocity;
out float weight;
out float color_id;
void main() {
    weight = point_velocity[2];
    velocity = normalize(point_velocity);
    color_id = color_type;
    velocity[2] = 0;
    gl_Position = vertex_position;
}
)zzz"
