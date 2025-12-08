/*
The MIT License (MIT)
Copyright © 2025 Matt Wells

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "shaderPixel_Main.hlsli"


float4 PS_Basic_Tex_8_scan_lines(in PS_INPUT IN) : SV_TARGET {
    //colour_opt.w = original height. P2 TGV's height is half of display height, no gaps between scan lines.
    //this shader is used when scaling to height x2.
    float half_y = IN.TexCoords.y * colour_opt.w;
    float delta = round(half_y) - half_y;

    if (delta < 0.1) {
        float palOffset = clamp(Texture_Main.Sample(Sampler_Main, IN.TexCoords).x, 0, 1.0);
        return Texture_Pal.Sample(Sampler_Main, palOffset);
    }
    return float4(0, 0, 0, 0); 
}
