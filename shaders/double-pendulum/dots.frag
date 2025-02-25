/* Expressions for the time derivatives of each of the coordinates */
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

uniform sampler2D coordinateTex;
uniform float mass1;
uniform float mass2;
uniform float length1;
uniform float length2;
uniform float gravity;

/* Time derivative of the angular positions phi1 and phi2. */
float dotPhi(int i, vec4 coord) {
    float pi1 = coord[0], pi2 = coord[1];
    float phi1 = coord[2], phi2 = coord[3];
    float m11 = (mass1 + mass2)*length1;
    float m12 = mass2*length1*length2*cos(phi1 - phi2);
    float m21 = mass2*length1*length2*cos(phi1 - phi2);
    float m22 = mass2*length2;
    if (i == 1)
        return -m22*pi1/(m12*m21 - m22*m11) + m12*pi2/(m12*m21 - m22*m11);
    else if (i == 2)
        return m21*pi1/(m12*m21 - m22*m11) - m11*pi2/(m12*m21 - m22*m11);
}

float dotPhi1(vec4 coord) {
    return dotPhi(1, coord);
}

float dotPhi2(vec4 coord) {
    return dotPhi(2, coord);
}

/* Time derivative of the conjugate momenta pi1 and pi2.
 These are computed from the Hamiltonian using Sympy.
 */
float dotPi(int i, vec4 coord) {
    float pi1 = coord[0], pi2 = coord[1];
    float phi1 = coord[2], phi2 = coord[3];
    if (i == 1)
        return -gravity*length1*(mass1+mass2)*sin(phi1)+4.0*length1*length2*mass2*(0.5*mass1+0.5*mass2)*pow((length1*pi2*cos(phi1-phi2)-pi1),2.0)*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)+4.0*length1*length2*mass2*(length1*pi2*cos(phi1-phi2)-pi1)*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)*pow(cos(phi1-phi2),2.0)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)+2.0*length1*length2*pow((length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2)),2.0)*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)-2.0*length1*pi2*(0.5*mass1+0.5*mass2)*(length1*pi2*cos(phi1-phi2)-pi1)*sin(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)-3.0*length1*pi2*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)-3.0*length2*mass2*pi1*(length1*pi2*cos(phi1-phi2)-pi1)*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)-1.0*length2*pi1*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)+2.0*pi1*pi2*sin(phi1-phi2)/(length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2)-(length1*pi2*cos(phi1-phi2)-pi1)*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0);
    else if (i == 2)
        return -gravity*length2*mass2*sin(phi2)-4.0*length1*length2*mass2*(0.5*mass1+0.5*mass2)*pow((length1*pi2*cos(phi1-phi2)-pi1),2.0)*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)-4.0*length1*length2*mass2*(length1*pi2*cos(phi1-phi2)-pi1)*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)*pow(cos(phi1-phi2),2.0)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)-2.0*length1*length2*pow((length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2)),2.0)*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)+2.0*length1*pi2*(0.5*mass1+0.5*mass2)*(length1*pi2*cos(phi1-phi2)-pi1)*sin(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)+3.0*length1*pi2*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)+3.0*length2*mass2*pi1*(length1*pi2*cos(phi1-phi2)-pi1)*sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)+1.0*length2*pi1*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)-2.0*pi1*pi2*sin(phi1-phi2)/(length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2)+(length1*pi2*cos(phi1-phi2)-pi1)*(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))*sin(phi1-phi2)/pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0);
}

float dotPi1(vec4 coord) {
    return dotPi(1, coord);
}

float dotPi2(vec4 coord) {
    return dotPi(2, coord);
}

void main() {
    vec4 coord = texture2D(coordinateTex, UV);
    fragColor = vec4(dotPi1(coord), dotPi2(coord), 
                     dotPhi1(coord), dotPhi2(coord));
}
