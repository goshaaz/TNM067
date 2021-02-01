#include "utils/structs.glsl"

uniform sampler2D vfColor;
uniform ImageParameters vfParameters;
in vec3 texCoord_;

float passThrough(vec2 coord){
    return texture(vfColor,coord).x;
}

float magnitude( vec2 coord ){
    float Vx = texture(vfColor,coord).x;
	float Vy = texture(vfColor,coord).y;
	float mag = sqrt(Vx * Vx + Vy * Vy);
    return mag;
}

float divergence(vec2 coord){
	vec2 pixelSize = vfParameters.reciprocalDimensions;
	vec2 dxtermleft = texture2D(vfColor, vec2(coord.x + pixelSize.x, coord.y)).xy;
	vec2 dxtermright = texture2D(vfColor, vec2(coord.x - pixelSize.x, coord.y)).xy;
	vec2 dytermleft = texture2D(vfColor, vec2(coord.x, coord.y + pixelSize.y)).xy;
	vec2 dytermright = texture2D(vfColor, vec2(coord.x, coord.y - pixelSize.y)).xy;
	vec2 dV_x = (dxtermleft - dxtermright) / (2 * pixelSize.x);
	vec2 dV_y = (dytermleft - dytermright) / (2 * pixelSize.y);

	float div = dV_x.x + dV_y.y;
	return div;
}

float rotation(vec2 coord){
	vec2 pixelSize = vfParameters.reciprocalDimensions;
	vec2 dxtermleft = texture2D(vfColor, vec2(coord.x + pixelSize.x, coord.y)).xy;
	vec2 dxtermright = texture2D(vfColor, vec2(coord.x - pixelSize.x, coord.y)).xy;
	vec2 dytermleft = texture2D(vfColor, vec2(coord.x, coord.y + pixelSize.y)).xy;
	vec2 dytermright = texture2D(vfColor, vec2(coord.x, coord.y - pixelSize.y)).xy;

	vec2 dV_x = (dxtermleft - dxtermright) / (2 * pixelSize.x);
	vec2 dV_y = (dytermleft - dytermright) / (2 * pixelSize.y);

	float rot = dV_x.y - dV_y.x;
    return rot;
}

void main(void) {
    float v = OUTPUT(texCoord_.xy);
    FragData0 = vec4(v);
}
