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
(Click to enlarge.)

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


# Benefits


- Continuously scalable font size, maintaing the clean edges without major
artefacts.
- Handle unlimited number of sizes at the same time without memory and
processing overhead.
- Ability to dynamically change the size and shape of each glyph at runtime
without memory and processing overhead.

These benefits come form the fact that each glyph, which is usually in 
a Flyweight object in a program, is represented by a textured quadrilateral
region in a set of coordinates into the output screen space, and another set 
of coordinates into the texture map.


# How It Works

## Font Generation


1. For each glyph, generate glyph bitmap in big enough size, (e.g. 4096 * 4096)

2. Generate signed distance for each point in the bitmap.

Signed distance is the distance to the closest point of the opposite bit.

For example, if the current point is set and it finds the closest unset point
 at 4 points away, then the signed distance is 4.

On the other hand, if the current point is unset and it finds the closest set
point at 2 points away, then the signed distance is -2.

3. Down-sample the points in the signed distance field (e.g. 10x10 for low res
and 40x40 for high res).

4. Clamp the signed distance value of each sampled pixel into a limited range
[-spread, spread], and normalize it into [0, 255].
This means the pixel on the boundary of the glypy gets the value of 128, and
an unset point far from the glyph gets 0.

5. Pack the downsampled glyphs into a single texture map with the normalied
signed distance into the alpha channel.
Following figure illustrates a generated texture.

<a href="docs/readme/sample_texture.png">
<img src="docs/readme/sample_texture.png">
</a>

6. Generate the metrics of each glyph. It inclues the position of the glyph
in the UV texture coordinates as well as the font metrics such as bearing and
kernings.


## Font Rendering at Runtime


