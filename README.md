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

# Supported Environment

* Linux

* MacOS (tested with Mac mini M1 2020 & Ventura 13.5.2)

# Reuirements

## General

* [Freetype](https://freetype.org) : used to access the true type fonts, to draw glyphs, and to obtain the font metrics.
* [LibGD](https://libgd.github.io) : used to handle raster graphics for the signed distance font glyphs.
* [LibPNG](http://www.libpng.org/pub/png/libpng.html) : used to read & write PNG files.

* [OpenGL](https://www.opengl.org) : used to render the fonts on the screen.
* [GLEW](https://glew.sourceforge.net) : a quasi requirement to use OpenGL.
* [GLFW3](https://www.glfw.org) : window manager for OpenGL.

Following is the sample installation process tested on Ubuntu 22.04.3 LTS.
```
sudo add-apt-repository universe
sudo add-apt-repository multiverse
sudo apt update
# The following three lines are apparently needed to make 'sudo apt-get build-dep' work.
# Ref: https://askubuntu.com/questions/496549/error-you-must-put-some-source-uris-in-your-sources-list
cat /etc/apt/sources.list | grep deb-src
sudo sed -i~orig -e 's/# deb-src/deb-src/' /etc/apt/sources.list
sudo apt-get update

sudo apt install build-essential
sudo apt install cmake
sudo apt-get build-dep libglfw3
sudo apt-get build-dep libglew-dev
sudo apt install libglew-dev
sudo apt install libglm-dev
sudo apt install libglfw3
sudo apt install libglfw3-dev
sudo apt install libfreetype-dev
sudo apt install libpng-dev
```

## Installing LibGD
The installation by the package managers such as apt and homebrew is missing or does not seem to work.
The best way to install LibGD seems to be to build and install it manually.
```
git clone https://github.com/libgd/libgd.git
cd libgd
git brnach -l
* master
git log -n 1 --oneline
0d75136b (HEAD -> master, origin/master, origin/HEAD) add static qualifier to multiple functions
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
VERBOSE=1 make 2>&1 | tee make.log
VERBOSE=1 sudo make install 2>&1 | tee make_isntall.log
```

Please copy **FindGD.cmake** to the top directory of **SDFont**, where **CMakeLists.txt** resides.

```
# Find the location for FindGD.cmake
cd ..
find . -name "FindGD.cmake"
./cmake/modules/FindGD.cmake
cp ./cmake/modules/FindGD.cmake /path/to/your/SDFont
```

## Installing GLFW on MacOS.
On MacOS the requirements except for LibGD can be installed by [Homebrew](https://brew.sh).
However, the GLFW installed by Homebrew does not seem to work for me and I had to manually build it.
(The **glfwCreateWindow()** returns **nullptr** when it is linked with Homebrew's glfw library.)

```
$ git clone https://github.com/glfw/glfw.git
$ cd glfw
$ mkdir build
$ cd build
$ cmake .. 2>&1 | tee cmake_console_out.txt
$ VERBOSE=1 make 2>&1 | tee make.log
$ VERBOSE=1 sudo make install 2>&1 | tee make_install.log
```

# Build & Try
SDFont comes with the two libraries and executables.

* **libsdfont_lib** : library to generate signed distance fonts from the truetype fonts.

* **libsdfont_runtime** : small library to parse the font metrics and load the signed distance font glyph bitmap into a OpenGL texture.

* **sdfont_generator** : a command-line tool to generate signed distance fonts. It interacts with libsdfont_lib.

* **sdfont_demo** : a demo program that shows the opening crawl of Star Wars.

They are build with the standard CMake process.

```
$ cd <path/to>/SDFont
# make sure you have copied FindGD.cmake.
$ ls FindGD.cmake
FindGD.cmake
$ mkdir build
$ cd build
$ cmake ..
$ VERBOSE=1 make
```

A sample signed distance font can be generated by the following command.
Please specify a correct path to a truetype font to the option *-font_path* below.
The fonts are usually found in `/usr/share/fonts, /usr/local/fonts` etc on Linux, and `/System/Library/Fonts/` on MacOS.

```
./sdfont_generator -locale en_US -verbose -font_path <path/to/truetype/fonts/such/as>/Helvetica.ttc -max_code_point 512 -texture_size 1024  -resolution 1024 -spread_in_pixels 128 signed_dist_font
```

This will generate two files: `signed_dist_font.png` and `signed_dist_font.txt`. The former contains the glyph shapes in signed distance. It will be loaded at runtime to an OpenGL texture. The latter contains the metrics information useful for type setting.

To run the Star Wars demo, run the following command.

```
./sdfont_demo signed_dist_font
```

# Usage of the Command-Line Tool
```
Usage: sdfont_generator -locale [Locale] -font_path [FontPath] -max_code_point [num] -texture_size [num] -resolution [num] -spread_in_pixels [num] [output file name w/o ext]
```
* -verbose : Switch to turn on the verbose output.

* -locale : Locale used for the string. FreeType requires this. The default value is "C".

* -font_path : Path to the true type font including the extention.

* -max_code_point [num] : The upper limit to process the glyphs. Some fonts contain many glyphs, which are not used in most of the cases. If all the glyphs you use reside in the index range of 0-num, then you can specify num to reduce the sizes of the PNG file. For example, many fonts contains thousands of glyphs, but the used ones are usually within [32-255]. In this case you can specify 255 to this option to process the glyphs only in the range of [0-255]. The default value is 255.

* -texture_size [num] : The height and width of the PNG files in pixels. The default value is 512.

* -resolution [num] : The font size in pixels. The generator draws each glyph to a bitmap of this size to sample the signed distance. It affects the visual quality of the resultant signed distance font. The default value is 1024.

* -spread_in_pixels [num] : The extra margin around each glyph to sample the signed distance values. An appropriate range is 1/10 to 1/5 of the resolution. The default is 128.


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
Please note that the glyphs are drawn upside down in the figure due to the
difference in the vertical coordinate axes between PNG (downward) and
OpenGL Texture (upward).

<a href="docs/readme/sample_texture.png">
<img src="docs/readme/sample_texture.png">
</a>

6. Generate the metrics of each glyph. It inclues the position of the glyph
in the UV texture coordinates as well as the font metrics such as bearing and
kernings.


## Font Rendering at Runtime

The glyph is drawn as a transformed textured quadrilateral on the screen.
The user gives the coordinates of the quadrilateral together with their
attributes such as the corresponding texture coordinates, normals and colors.
The rest is handled by a graphic subcomponent such as OpenGL.
The geometric transformation (either 3D or Otrhogonal 2D) and 
rasterization are done by the hardward utilizing GPU.
The rasterization is considered up-sampling from the texture.
Each pixel in the screen obtains a signed-distance value in the alpha channel
pretty accurately, thanks to the interpolation mechanism in the rasterizer.

The fragment shader then use the recovered signed distance value to draw the 
pixel there on the screen.
For example, for Type 2 agove, a pixel is set if the recorvered signed distance
in the normalized alpha is above the threshold 0.5 (128). This gives an clear
edge for the glyphs.
User can supply their own shaders for other effects shown above from Type 0 to
6.


# SDFont Implementation
Here's an overview of SDFont, which consists of three parts: 
    sdfont_generator, libsdfont, and the shader pair.

<a href="docs/readme/overview.png">
<img src="docs/readme/overview.png">
</a>

The output PNG file looks like the one shown in Font Generation above.
It wll be loaded as a texture map at runtime.

The output TXT file consists of three parts: Margin, Glyphs, and Kernings.
The Margin has one value that represents the rectangular extent around each 
glyph in which the signed distance fades out.
In Glyphs section, each line represents a glyph metrics.
The line consists of the following fields.

- Code Point
- Width
- Height
- Horizontal Bearing X
- Horizontal Bearing Y
- Horizontal Advance
- Vertical Bearing X
- Vertical Bearing Y
- Vertical Advance

The fields above are taken from the input TrueType font but scaled to the
normalized texture coordinates.

- Texture Coord X : Left side of the glyph bit map in the texture coordinates.
- Texture Coord Y : Bottom side of the glyph bit map in the texture coordinates.
- Texture Width : Width of the bitmap in the texture coordinates.
Same as 'Width' above.
- Texture Height : Height of the bitmap in the texture coordinates.
Same as 'Height' above.

## Sample Shaders
SDFont provides a pair of vertex & fragment shader programs.
The vertex shader 
[shaders/VanillaSignedDistFontVertex.glsl](shaders/VanillaSignedDistFontVertex.glsl)
is an orthodox 3D perspetcive projectio shader.

The defined uniforms are:
- P (mat4) : Projection matrix.
- M (mat4) : Model matrix.
- V (mat4) : View magrix.
- lightWCS (vec3) : Light source in the world coordinate system.

The VBOs for this vertex shader, which is the output from 
SDFont::RuntimeHelper::generateOpenGLDrawElements() and input to 
SDFont::mShader.draw() has the following format.

- vertexLCS(vec3) : Point in the model coordinate system.
- normalLCS(vec3) : Normal in the model coordinate system. Not used at moment.
- texCoordIn(vec2): UV texture coordinates.

The fragment shader 
[shaders/VanillaSignedDistFontFragment.glsl](shaders/VanillaSignedDistFontFragment.glsl)
is the main part and it takes care of the glyph rendering.
It has the following uniforms.

- fontTexture (sampler2D) : The ID (name) of the texture created from the PNG.
- effect (int) : type of face to be rendered. See tye types above.
- useLight (bool) : set to true if light source specified by lightWCS is used.
- lowThreshold (float) : The threshold for the alpha channel.
Used by type 2,3, and 4. It is used to find the boundary beteen glyph and
non-glyph pixels.
- highThreshold (float) : Another threshold for the alpha channel.
Used by type 4. It is used to find the boundary for the inner curve for the
follow face.

- smoothing (float) : Smomothing parameter for the smooth step function.
- baseColor (vec3) :  Main color for the glyph.
- borderColor (vec3) : Secondary color for the glyph.

# Sample
A sample code for the opening crawl of Star Wars is found in 
[src_bin/sdfont_demo.cpp](src_bin/sdfont_demo.cpp).


# Limitations

- Can’t beat the Shannon’s sampling theorem.
When a small texture (e.g. 128x128) is used, 
magnified glyphs will show some noticeable artifacts on the screen.

- The number of different fonts shown simultaneously at realtime is limited
to the number of active loaded Textures (usually 16 for OpenGL).
One way to get around this is to pack multiple fonts into a single texture.
For example a texture of size 4K x 4K can accommodate 64 512x512 SD fonts, 
or 256 256x256 SD fonts. SDFont currently supports only one font per texture.
Multiple fonts per texture may be supported in the future.
This technique is not suitable for some East Asian writing systems, which have
thousands of glyphs.

# TODO

- Add API documentation for the libraries.
- Packing multiple fonts into a single texture.

# License


Copyright (c) 2019 Shoichiro Yamanishi

Wailea is released under MIT license. See `LICENSE` for details.

# Contact

For any inquiries, please contact:
Shoichiro Yamanishi

yamanishi72@gmail.com



# Reference


* [Green07]
Chris Green. 2007. Improved alpha-tested magnification for vector textures and special effects. In ACM SIGGRAPH 2007 courses (SIGGRAPH '07). ACM, New York, NY, USA, 9-18. DOI: https://doi.org/10.1145/1281500.1281665
