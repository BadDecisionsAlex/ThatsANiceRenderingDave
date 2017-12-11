R"zzz(
#version 330 core
out vec4 fragment_color;
in float alpha;
in float red;
in float green;
in float blue;
void main() {
    fragment_color = vec4(red, green, 0.7, alpha);
}
)zzz"
