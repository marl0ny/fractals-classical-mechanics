/* Color the double pendulum based on its angular separations */
#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
    
#if __VERSION__ <= 120
varying vec2 UV;
#define fragColor gl_FragColor
#else
in vec2 UV;
out vec4 fragColor;
#endif

uniform sampler2D coordFragTex;

const float PI = 3.141592653589793;

vec3 argumentToColor(float argVal) {
    float maxCol = 1.0;
    float minCol = 50.0/255.0;
    float colRange = maxCol - minCol;
    if (argVal <= PI/3.0 && argVal >= 0.0) {
        return vec3(maxCol,
                    minCol + colRange*argVal/(PI/3.0), minCol);
    } else if (argVal > PI/3.0 && argVal <= 2.0*PI/3.0){
        return vec3(maxCol - colRange*(argVal - PI/3.0)/(PI/3.0),
                    maxCol, minCol);
    } else if (argVal > 2.0*PI/3.0 && argVal <= PI){
        return vec3(minCol, maxCol,
                    minCol + colRange*(argVal - 2.0*PI/3.0)/(PI/3.0));
    } else if (argVal > PI && argVal <= 4.0*PI/3.0){
        return vec3(minCol,
                    maxCol - (colRange*(argVal - PI)/(PI/3.0)), 
                    maxCol);
    } else if (argVal > 4.0*PI/3.0 && argVal <= 5.0*PI/3.0) {
        return vec3(minCol + (colRange*(argVal - 4.0*PI/3.0)/(PI/3.0)),
                    minCol, maxCol);
    } else if (argVal > 5.0*PI/3.0 && argVal < 2.0*PI){
        return vec3(maxCol, minCol,
                    maxCol - colRange*(argVal - 5.0*PI/3.0)/(PI/3.0));
    } else {
        return vec3(minCol, maxCol, maxCol);
    }
}

float modAngle0To2Pi(float phi) {
    return (phi < 0.0)? (2.0*PI - mod(-phi, 2.0*PI)): mod(phi, 2.0*PI);
}


void main() {
    vec4 coord = texture2D(coordFragTex, UV);
    float phi1 = coord[2], phi2 = coord[3];
    float phi12 = modAngle0To2Pi(phi1 + phi2);
    vec3 color = argumentToColor(phi12);
    fragColor = vec4(color, 0.5);
}