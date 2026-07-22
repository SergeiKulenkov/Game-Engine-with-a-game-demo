@echo off

call glslangValidator -V -o rectangle.vert.spirv texturedQuad.vert.glsl
call glslangValidator -V -o rectangle.frag.spirv texturedQuad.frag.glsl

call glslangValidator -V -o circle.vert.spirv circle.vert.glsl
call glslangValidator -V -o circle.frag.spirv circle.frag.glsl
pause