#ifndef __SDFONT_GENERATOR_CONFIG_HPP__
#define __SDFONT_GENERATOR_CONFIG_HPP__

#include <string>
#include <iostream>
#include <cmath>

namespace SDFont {

using namespace std;

class GeneratorConfig {

  public:

    GeneratorConfig():
        mLocale                     { DefaultLocale },
        mFontPath                   { DefaultFontPath },
        mOutputFileName             { DefaultOutputFileName },
        mMaxCodePoint               { DefaultMaxCodePoint },
        mOutputTextureSize          { DefaultOutputTextureSize },
        mGlyphScalingFromSamplingToPackedSignedDist
                                    { 1.0f },
        mGlyphBitmapSizeForSampling { DefaultGlyphBitmapSizeForSampling },
        mRatioSpreadToGlyph         { DefaultRatioSpreadToGlyph }
        {;}

    virtual ~GeneratorConfig(){;}

    void setLocale             ( string s ) { mLocale         = s ; }
    void setFontPath           ( string s ) { mFontPath       = s ; }
    void setOutputFileName     ( string s ) { mOutputFileName = s ; }
    void setMaxCodePoint       ( long   v ) { mMaxCodePoint   = v ; }
    void setOutputTextureSize  ( long   v ) { mOutputTextureSize    = v ; }
    void setGlyphBitmapSizeForSampling
                               ( long   v ) { mGlyphBitmapSizeForSampling = v ; }
    void setRatioSpreadToGlyph ( float v  ) { mRatioSpreadToGlyph = v ; }
    void setGlyphScalingFromSamplingToPackedSignedDist
                               ( float v  ) { mGlyphScalingFromSamplingToPackedSignedDist = v; }

    string locale()            const { return mLocale ;                           }
    string fontPath()          const { return mFontPath ;                         }
    string outputFileName()    const { return mOutputFileName ;                   }
    long   maxCodePoint()      const { return mMaxCodePoint ;                     }
    long   outputTextureSize() const { return mOutputTextureSize ;                }
    long   defaultRatioSpreadToGlyph()
                               const { return DefaultRatioSpreadToGlyph ;         }
    long   defaultGlyphBitmapSizeForSampling()
                               const { return DefaultGlyphBitmapSizeForSampling ; }
    long   glyphBitmapSizeForSampling()
                               const { return mGlyphBitmapSizeForSampling ;       }
    float  ratioSpreadToGlyph()const { return mRatioSpreadToGlyph ;               }
    float  glyphScalingFromSamplingToPackedSignedDist()
                               const { return mGlyphScalingFromSamplingToPackedSignedDist ; }
    long   signedDistExtent()  const { return   (long)( mGlyphBitmapSizeForSampling
                                              * mGlyphScalingFromSamplingToPackedSignedDist
                                              * mRatioSpreadToGlyph );                      }

    void   emitVerbose () const;
    void   outputMetricsHeader ( ostream& os ) const;

  private:

    string mLocale ;
    string mFontPath ;
    string mOutputFileName ;
    long   mMaxCodePoint ;
    long   mOutputTextureSize ;
    long   mGlyphBitmapSizeForSampling;
    float  mGlyphScalingFromSamplingToPackedSignedDist ;
    float  mRatioSpreadToGlyph;

    static const string DefaultLocale ;
    static const string DefaultFontPath ;
    static const string DefaultOutputFileName ;
    static const long   DefaultMaxCodePoint ;
    static const long   DefaultOutputTextureSize ;
    static const long   DefaultGlyphBitmapSizeForSampling ;
    static const float  DefaultRatioSpreadToGlyph ;
};


} // namespace SDFont
#endif /*__SDFONT_GENERATOR_CONFIG_HPP__*/

