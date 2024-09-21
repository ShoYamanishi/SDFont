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
        mMaxCodePoint               { DefaultMaxCodePoint },
        mOutputTextureSize          { DefaultOutputTextureSize },
        mGlyphScalingFromSamplingToPackedSignedDist
                                    { 1.0f },
        mGlyphBitmapSizeForSampling { DefaultGlyphBitmapSizeForSampling },
        mRatioSpreadToGlyph         { DefaultRatioSpreadToGlyph },
        mNumThreads                 { DefaultNumThreads },
        mCodepointRangeFilePath     { DefaultCodepointRangeFilePath },
        mEncoding                   { DefaultEncoding },
        mEnableDeadReckoning        { DefaultEnableDeadReckoning },
        mReverseYDirectionForGlyphs { DefaultReverseYDirectionForGlyphs },
        mFaceHasGlyphNames          { DefaultFaceHasGlyphNames }
        {;}

    virtual ~GeneratorConfig(){;}

    void setFontPath           ( string s ) { mFontPath       = s ; }
    void setExtraGlyphPath     ( string s ) { mExtraGlyphPath = s ; }
    void setOutputFileName     ( string s ) { mOutputFileName = s ; }
    void setMaxCodePoint       ( long   v ) { mMaxCodePoint   = v ; }
    void setOutputTextureSize  ( long   v ) { mOutputTextureSize    = v ; }
    void setGlyphBitmapSizeForSampling
                               ( long   v ) { mGlyphBitmapSizeForSampling = v ; }
    void setRatioSpreadToGlyph ( float v  ) { mRatioSpreadToGlyph = v ; }
    void setNumThreads         ( long v   ) { mNumThreads = v ; }
    void setGlyphScalingFromSamplingToPackedSignedDist
                               ( float v  ) { mGlyphScalingFromSamplingToPackedSignedDist = v; }
    void setCodepointRangeFilePath( string s );
    void setEncoding           ( string s ) { mEncoding = s; }
    void setDeadReckoning      ( bool b )   { mEnableDeadReckoning = b; }
    void setReverseYDirectionForGlyphs
                               ( bool b )   { mReverseYDirectionForGlyphs = b; }
    string fontPath()          const { return mFontPath ;                         }
    string extraGlyphPath()    const { return mExtraGlyphPath ;                   }
    string outputFileName()    const { return mOutputFileName ;                   }
    long maxCodePoint()      const {
        if ( mCodepointRangePairs.empty() ) {
            return mMaxCodePoint ;
        }
        else {
            long v = 0;
            for ( const auto& p : mCodepointRangePairs ) {
                v = std::max( v, p.second );
            }
            return v;
        }
    }
    long   outputTextureSize() const { return mOutputTextureSize ;                }
    long   defaultRatioSpreadToGlyph()
                               const { return DefaultRatioSpreadToGlyph ;         }
    long   defaultGlyphBitmapSizeForSampling()
                               const { return DefaultGlyphBitmapSizeForSampling ; }
    long   glyphBitmapSizeForSampling()
                               const { return mGlyphBitmapSizeForSampling ;       }
    float  ratioSpreadToGlyph()const { return mRatioSpreadToGlyph ;               }
    long   numThreads() const { return mNumThreads; }
    float  glyphScalingFromSamplingToPackedSignedDist()
                               const { return mGlyphScalingFromSamplingToPackedSignedDist ; }
    long   signedDistExtent()  const { return   (long)( mGlyphBitmapSizeForSampling
                                              * mGlyphScalingFromSamplingToPackedSignedDist
                                              * mRatioSpreadToGlyph );                      }
    const string& encoding()   const { return mEncoding;                          }

    bool   isDeadReckoningSet() const { return mEnableDeadReckoning; }
    bool   isReverseYDirectionForGlyphsSet() const { return mReverseYDirectionForGlyphs; }
    void   emitVerbose () const;
    void   outputMetricsHeader ( ostream& os ) const;

    bool   isInACodepointRange( const long charcode ) const
    {
        if ( mCodepointRangePairs.empty() ) {
            return true;
        }
        for ( const auto& pair: mCodepointRangePairs ) {
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
    long   mMaxCodePoint ;
    long   mOutputTextureSize ;
    long   mGlyphBitmapSizeForSampling;
    float  mGlyphScalingFromSamplingToPackedSignedDist ;
    float  mRatioSpreadToGlyph;
    long   mNumThreads;
    string mCodepointRangeFilePath;
    vector< pair< long, long > >
           mCodepointRangePairs;
    string mEncoding;
    bool   mEnableDeadReckoning;
    bool   mReverseYDirectionForGlyphs;
    bool   mFaceHasGlyphNames;

    static const string DefaultFontPath ;
    static const string DefaultExtraGlyphPath ;
    static const string DefaultOutputFileName ;
    static const long   DefaultMaxCodePoint ;
    static const long   DefaultOutputTextureSize ;
    static const long   DefaultGlyphBitmapSizeForSampling ;
    static const float  DefaultRatioSpreadToGlyph ;
    static const long   DefaultNumThreads;
    static const string DefaultCodepointRangeFilePath;
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

