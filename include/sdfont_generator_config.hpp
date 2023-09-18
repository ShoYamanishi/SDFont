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
        mLocale                 ( DefaultLocale ),
        mFontPath               ( DefaultFontPath ),
        mOutputFileName         ( DefaultOutputFileName ),
        mMaxCodePoint           ( DefaultMaxCodePoint ),
        mTextureSize            ( DefaultTextureSize ),
        mScale                  ( DefaultScale ),
        mResolution             ( DefaultBaseResolutionInPixels ),
        mSpreadInPixels         ( DefaultSpreadInPixels )
        {;}

    virtual ~GeneratorConfig(){;}

    void setLocale         ( string s ) { mLocale         = s ; }
    void setFontPath       ( string s ) { mFontPath       = s ; }
    void setOutputFileName ( string s ) { mOutputFileName = s ; }
    void setMaxCodePoint   ( long   v ) { mMaxCodePoint   = v ; }
    void setTextureSize    ( long   v ) { mTextureSize    = v ; }
    void setResolution     ( long   v ) { mResolution     = v ; }
    void setSpreadInPixels ( long   v ) { mSpreadInPixels = v ; }
    void setScale          ( float  v ) { mScale          = ceil( v ); }

    string locale()            const { return mLocale ;                       }
    string fontPath()          const { return mFontPath ;                     }
    string outputFileName()    const { return mOutputFileName ;               }
    long   maxCodePoint()      const { return mMaxCodePoint ;                 }
    long   textureSize()       const { return mTextureSize ;                  }
    long   scale()             const { return mScale ;                        }
    float  fscale()            const { return (float)mScale ;                 }
    long   defaultSpread()     const { return DefaultSpreadInPixels ;         }
    long   defaultResolution() const { return DefaultBaseResolutionInPixels ; }
    long   defaultScale()      const { return mScale ;                        }
    long   resolution()        const { return mResolution ;                   }
    long   spreadInPixels()    const { return mSpreadInPixels ;               }
    long   signedDistExtent()  const { return   (float)mSpreadInPixels
                                              / (float)fscale();              }

    void   emitVerbose () const;
    void   outputMetricsHeader ( ostream& os ) const;

  private:

    string mLocale ;
    string mFontPath ;
    string mOutputFileName ;
    long   mMaxCodePoint ;
    long   mTextureSize ;
    long   mScale ;
    long   mResolution;
    long   mSpreadInPixels ;


    static const string DefaultLocale ;
    static const string DefaultFontPath ;
    static const string DefaultOutputFileName ;

    static const long   DefaultMaxCodePoint ;
    static const long   DefaultTextureSize ;
    static const long   DefaultScale ;
    static const long   DefaultSpreadInPixels ;
    static const long   DefaultBaseResolutionInPixels ;

};


} // namespace SDFont
#endif /*__SDFONT_GENERATOR_CONFIG_HPP__*/

