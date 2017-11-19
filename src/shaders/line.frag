R"zzz(
#version 330 core
uniform float selected_bone;
in float bone_id;
out vec4 fragment_color;
void main() {
    if (selected_bone == int(bone_id)) {
        fragment_color = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        fragment_color = vec4(1.0, 1.0, 0.0, 1.0);
    }
    
    if (bone_id == -2) {
        fragment_color = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (bone_id == -3) {
        fragment_color = vec4(0.0, 0.5, 1.0, 1.0);
    } else if (bone_id == -5) {
        fragment_color = vec4(0.0, 1.0, 1.0, 1.0);
    }
}
)zzz"
