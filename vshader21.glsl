attribute vec2 vPosition;
attribute vec4 vColor;
varying vec4 color;

void
main()
{
    gl_Position = vec4( vPosition, 0.0, 1.0 );
    color = vColor;
}
