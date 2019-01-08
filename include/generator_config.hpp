#ifndef __GENERATOR_CONFIG_HPP__
#define __GENERATOR_CONFIG_HPP__

#include <string>
#include <iostream>

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
        mSpreadInPixels         ( DefaultSpreadInPixels )
        {;}

    virtual ~GeneratorConfig(){;}

    void setLocale         ( string s ) { mLocale         = s ; }
    void setFontPath       ( string s ) { mFontPath       = s ; }
    void setOutputFileName ( string s ) { mOutputFileName = s ; }
    void setMaxCodePoint   ( long   v ) { mMaxCodePoint   = v ; }
    void setTextureSize    ( long   v ) { mTextureSize    = v ; }

    void factorScale       ( float factor ) {

        mScale = (long) ( (float)mScale * factor );
    }

    string locale()            const { return mLocale ;                       }
    string fontPath()          const { return mFontPath ;                     }
    string outputFileName()    const { return mOutputFileName ;               }
    long   maxCodePoint()      const { return mMaxCodePoint ;                 }
    long   textureSize()       const { return mTextureSize ;                  }
    long   scale()             const { return mScale ;                        }
    float  fscale()            const { return (float)mScale ;                 }
    long   defaultSpread()     const { return DefaultSpreadInPixels ;         }
    long   defaultScale()      const { return DefaultScale ;                  }
    long   defaultResolution() const { return DefaultBaseResolutionInPixels ; }
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
#endif /*__GENERATOR_CONFIG_HPP__*/

