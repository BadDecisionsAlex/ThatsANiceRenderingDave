R"zzz(
#version 330 core
out vec4 fragment_color;
in float alpha;
void main() {
    fragment_color = vec4(alpha - 0.1, 0.0, alpha, 1.0);
}
)zzz"
