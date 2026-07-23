@echo off

call glslangValidator -V -o texturedQuad.vert.spirv texturedQuad.vert.glsl
call glslangValidator -V -o texturedQuad.frag.spirv texturedQuad.frag.glsl

call glslangValidator -V -o circle.vert.spirv circle.vert.glsl
call glslangValidator -V -o circle.frag.spirv circle.frag.glsl
pause