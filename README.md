# SDFont : Signed Distance Font Generator and Runtime Utility


Demo Video (Click to play)
<a href="https://youtu.be/p1f0NFHqdbI">
    <img src="docs/readme/VideoCapture.png">
</a>


# Overview


Signed Distance Field Font is a technology proposed by 
<a href="https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf">
Chris Green of Valve at SIGGRAPH 2007
</a>
It is based on the anti-aliasing processing in the GPUs.
Each glyph is drawn as a textured quadrilateral.
Thanks to the very clever use of alpha channel in conjunction with the 
anti-aliasing sampling in the GPUs,
the resultant shape of the fonts is clean with little artifacts
for most of the sizes.

Following are some samples rendered with a signed distance field font.

<a href="docs/readme/Type0.png">
    <img src="docs/readme/Thumb0.png" height="90">
</a>
<a href="docs/readme/Type1.png">
    <img src="docs/readme/Thumb1.png" height="90">
</a>
<a href="docs/readme/Type2.png">
    <img src="docs/readme/Thumb2.png" height="90">
</a>
<a href="docs/readme/Type3.png">
    <img src="docs/readme/Thumb3.png" height="90">
</a>
<a href="docs/readme/Type4.png">
    <img src="docs/readme/Thumb4.png" height="90">
</a>
<a href="docs/readme/Type5.png">
    <img src="docs/readme/Thumb5.png" height="90">
</a>
<a href="docs/readme/Type6.png">
    <img src="docs/readme/Thumb6.png" height="90">
</a>

From left to right:
- Type 0: Raw output  by a pass-thru fragment shader.
- Type 1: Softened edge by smooth-step function.
- Type 2: Hard-edge by threshold on alpha.
- Type 3: Glare/Helo effect
- Type 4: Outline by hard thresholds
- Type 5: Hollow fonts with soft edges
- Type 6: Correspondint quadrilaterals to which the texture is mapped.
