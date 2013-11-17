/// @file Colour.fs
/// @brief a basic unshaded solid colour shader
/// @brief the colour to shade draw with
uniform vec4 Colour;
out vec4 outColour;

void main ()
{
  outColour = Colour;
}

