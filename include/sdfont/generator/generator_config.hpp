#ifndef __SDFONT_GENERATOR_CONFIG_HPP__
#define __SDFONT_GENERATOR_CONFIG_HPP__

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <utility>

namespace SDFont {

using namespace std;

class GeneratorConfig {

  public:

    static const string FileNameExtraGlyphLineFeed;
    static const string FileNameExtraGlyphBlank;

    GeneratorConfig():
        mFontPath                   { DefaultFontPath },
        mExtraGlyphPath             { DefaultExtraGlyphPath },
        mOutputFileName             { DefaultOutputFileName },
        mOutputTextureSize          { DefaultOutputTextureSize },
        mGlyphScalingFromSamplingToPackedSignedDist
                                    { 1.0f },
        mGlyphBitmapSizeForSampling { DefaultGlyphBitmapSizeForSampling },
        mRatioSpreadToGlyph         { DefaultRatioSpreadToGlyph },
        mProcessHiddenGlyphs        { DefaultProcessHiddenGlyphs },
        mNumThreads                 { DefaultNumThreads },
        mEncoding                   { DefaultEncoding },
        mEnableDeadReckoning        { DefaultEnableDeadReckoning },
        mReverseYDirectionForGlyphs { DefaultReverseYDirectionForGlyphs },
        mFaceHasGlyphNames          { DefaultFaceHasGlyphNames }
        {;}

    virtual ~GeneratorConfig(){;}

    void setFontPath           ( string s ) { mFontPath       = s ; }
    void setExtraGlyphPath     ( string s ) { mExtraGlyphPath = s ; }
    void setOutputFileName     ( string s ) { mOutputFileName = s ; }
    void setOutputTextureSize  ( long   v ) { mOutputTextureSize    = v ; }
    void setGlyphBitmapSizeForSampling
                               ( long   v ) { mGlyphBitmapSizeForSampling = v ; }
    void setRatioSpreadToGlyph ( float v  ) { mRatioSpreadToGlyph = v ; }
    void setProcessHiddenGlyphs( const bool b )
                                            { mProcessHiddenGlyphs = b ; }
    void addCharCodeRange      ( const uint32_t s, const uint32_t f )
                                            { mCharCodeRanges.push_back( std::pair( s, f ) ); }
    void setNumThreads         ( long v   ) { mNumThreads = v ; }
    void setGlyphScalingFromSamplingToPackedSignedDist
                               ( float v  ) { mGlyphScalingFromSamplingToPackedSignedDist = v; }
    void setEncoding           ( string s ) { mEncoding = s; }
    void setDeadReckoning      ( bool b )   { mEnableDeadReckoning = b; }
    void setReverseYDirectionForGlyphs
                               ( bool b )   { mReverseYDirectionForGlyphs = b; }

    string fontPath()          const { return mFontPath ;                         }
    string extraGlyphPath()    const { return mExtraGlyphPath ;                   }
    string outputFileName()    const { return mOutputFileName ;                   }
    long   outputTextureSize() const { return mOutputTextureSize ;                }
    long   defaultRatioSpreadToGlyph()
                               const { return DefaultRatioSpreadToGlyph ;         }
    long   defaultGlyphBitmapSizeForSampling()
                               const { return DefaultGlyphBitmapSizeForSampling ; }
    long   glyphBitmapSizeForSampling()
                               const { return mGlyphBitmapSizeForSampling ;       }
    float  ratioSpreadToGlyph()const { return mRatioSpreadToGlyph ;               }
    bool   processHiddenGlyphs()
                               const { return mProcessHiddenGlyphs;               }
    long   numThreads()        const { return mNumThreads;                        }
    float  glyphScalingFromSamplingToPackedSignedDist()
                               const { return mGlyphScalingFromSamplingToPackedSignedDist ; }
    long   signedDistExtent()  const { return   (long)( mGlyphBitmapSizeForSampling
                                              * mGlyphScalingFromSamplingToPackedSignedDist
                                              * mRatioSpreadToGlyph );                      }
    const string& encoding()   const { return mEncoding;                          }

    bool   isDeadReckoningSet()
                               const { return mEnableDeadReckoning; }
    bool   isReverseYDirectionForGlyphsSet()
                               const { return mReverseYDirectionForGlyphs; }

    void   emitVerbose () const;
    void   outputMetricsHeader ( ostream& os ) const;

    bool   isInACharCodeRange( const long charcode ) const
    {
        if ( mCharCodeRanges.empty() ) {
            return true;
        }
        for ( const auto& pair: mCharCodeRanges ) {
            if ( pair.first <= charcode && charcode < pair.second ) {
                return true;
            }
        }
        return false;
    }

    void    setFaceHasGlyphNames() { mFaceHasGlyphNames = true; }
    bool    faceHasGlyphNames() const { return mFaceHasGlyphNames; }

  private:

    string mFontPath ;
    string mExtraGlyphPath ;
    string mOutputFileName ;
    long   mOutputTextureSize ;
    long   mGlyphBitmapSizeForSampling;
    float  mGlyphScalingFromSamplingToPackedSignedDist ;
    float  mRatioSpreadToGlyph;
    bool   mProcessHiddenGlyphs;
    long   mNumThreads;
    vector< pair< long, long > >
           mCharCodeRanges;
    string mEncoding;
    bool   mEnableDeadReckoning;
    bool   mReverseYDirectionForGlyphs;
    bool   mFaceHasGlyphNames;

    static const string DefaultFontPath ;
    static const string DefaultExtraGlyphPath ;
    static const string DefaultOutputFileName ;
    static const long   DefaultOutputTextureSize ;
    static const long   DefaultGlyphBitmapSizeForSampling ;
    static const float  DefaultRatioSpreadToGlyph ;
    static const bool   DefaultProcessHiddenGlyphs ;
    static const long   DefaultNumThreads;
    static const string DefaultEncoding;
    static const bool   DefaultEnableDeadReckoning;
    static const bool   DefaultReverseYDirectionForGlyphs;
    static const bool   DefaultFaceHasGlyphNames;

    void trim( string& line ) const;
    bool isCommentLine( const std::string& line ) const;
    size_t splitLine( const string& txt, vector< string >& strs, const char ch ) const;
};

} // namespace SDFont
#endif /*__SDFONT_GENERATOR_CONFIG_HPP__*/

