#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <png.h>

#include "sdfont_metrics_parser.hpp"
#include "sdfont_texture_loader.hpp"
#include "sdfont_runtime_helper.hpp"
#include "sdfont_vanilla_shader_manager.hpp"

using namespace std;

/** @file sdfont_demo.cpp
 *
 *  @brief visual demo for signed distant fonts.
 *
 *  @dependencies
 *    OpenGL 3.3 or later
 *    GLFW3
 *    GLEW
 *    GLM
 */

static size_t splitLine(
    const string&   txt,
    vector<string>& strs,
    char            ch
) {

    auto   pos        = txt.find( ch );
    size_t initialPos = 0;

    strs.clear();

    while( pos != std::string::npos && initialPos < txt.size() ) {

        if ( pos > initialPos ) {

            strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        }

        initialPos = pos + 1;

        if ( initialPos < txt.size() ) {

            pos = txt.find( ch, initialPos );
        }

    }

    if ( initialPos < txt.size() ) {

        strs.push_back( txt.substr( initialPos, txt.size() - initialPos) );
    }
    return strs.size();
}


class GlfwManager  {

  public:

    GlfwManager( int requestedWindowWidth, int requestedWindowHeight );

    virtual ~GlfwManager();

    void configGLFW();

    int windowWidth()         const { return mWindowWidth; }

    int windowHeight()        const { return mWindowHeight; }

    int windowWidthInPixel()  const { return mWindowWidthInPixel; }

    int windowHeightInPixel() const { return mWindowHeightInPixel; }

    void update();


    /** @param: keyCode (in): GLFW_KEY_* defined in glfw3.h
     *
     *  @return true if the key is pressed now.
     *
     *  @reference /usr/local/include/GLFW/glfw3.h
     */
    bool isPressed( int keyCode ) const {

        if ( glfwGetKey( mWindow, GLFW_KEY_SPACE ) == GLFW_PRESS ) {

            return true;
        }
        else {
            return false;
        }
    }

    virtual bool shouldExit();

    virtual void callbackWindowSize(
        GLFWwindow* w,
        int         width,
        int         height
    );

    virtual  void callbackMouseButton(
        GLFWwindow* window,
        int         button,
        int         action,
        int         mods
    );

    virtual void callbackMousePos(
        GLFWwindow* window,
        double      xpos,
        double      ypos
    );

    virtual void callbackMouseWheel(
        GLFWwindow* window,
        double      xoffset,
        double      yoffset
    );

    virtual void callbackKey(
        GLFWwindow* window,
        int         key,
        int         scancode,
        int         action,
        int         mods
    );

    virtual void callbackChar(
        GLFWwindow* window,
        int         codepoint
    );


  private:

    GLFWwindow*   mWindow;
    int           mWindowWidth;
    int           mWindowHeight;
    int           mWindowWidthInPixel;
    int           mWindowHeightInPixel;

    static GlfwManager* mThisSingleton;

    static void dispatchWindowSize  ( GLFWwindow* w, int x, int y );
    static void dispatchMouseButton ( GLFWwindow* w, int b, int a, int m );
    static void dispatchMousePos    ( GLFWwindow* w, double x, double y );
    static void dispatchMouseWheel  ( GLFWwindow* w, double x, double y );
    static void dispatchKey         ( GLFWwindow* w,int k,int s,int a,int m );
    static void dispatchChar        ( GLFWwindow* w, int c );
};


GlfwManager* GlfwManager::mThisSingleton = nullptr;


GlfwManager::GlfwManager(

    int requestedWindowWidth,
    int requestedWindowHeight

)
{
    // Check for singleton.
    if ( mThisSingleton != nullptr )  {

        cerr << "Duplicated GlfwManager.\n";
    }

    // Initialise GLFW
    if ( !glfwInit() ) {

        cerr << "glfwInit() failed.\n";
    }

    glfwWindowHint( GLFW_SAMPLES,               4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE );

    mWindow = glfwCreateWindow(

                  requestedWindowWidth,
                  requestedWindowHeight,
                  "SDFont Demo",
                  NULL,
                  NULL

              );

    if ( mWindow == NULL ) {

        cerr << "glfwCreateWindow failed\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent( mWindow );
    
    glfwGetFramebufferSize(

        mWindow,
        &mWindowWidthInPixel,
        &mWindowHeightInPixel

    );

    glfwGetWindowSize( mWindow, &mWindowWidth, &mWindowHeight );

    glewExperimental = true;

    if ( glewInit() != GLEW_OK ) {

        cerr << "Failed to initialize GLEW\n";;
        terminate();

    }

    glViewport(
        0,
        0,
        (GLsizei)mWindowWidthInPixel,
        (GLsizei)mWindowHeightInPixel
    );

    GlfwManager::mThisSingleton = this;
}


void GlfwManager::configGLFW()
{

    glfwSetWindowSizeCallback
                         ( mWindow, (GLFWwindowsizefun)  dispatchWindowSize );

    glfwSetMouseButtonCallback 
                         ( mWindow, (GLFWmousebuttonfun) dispatchMouseButton );

    glfwSetCursorPosCallback   
                         ( mWindow, (GLFWcursorposfun)   dispatchMousePos );
                                        
    glfwSetScrollCallback( mWindow, (GLFWscrollfun)      dispatchMouseWheel );
                                        
    glfwSetKeyCallback   ( mWindow, (GLFWkeyfun)         dispatchKey );
                                        
    glfwSetCharCallback  ( mWindow, (GLFWcharfun)        dispatchChar );

    glfwSetInputMode     ( mWindow, GLFW_STICKY_KEYS, GL_TRUE );

    glfwSetInputMode     ( mWindow, GLFW_CURSOR,      GLFW_CURSOR_NORMAL );

    glfwPollEvents();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glDepthFunc(GL_LESS); 

}


void GlfwManager::update()
{

    glfwSwapBuffers( mWindow );
    glfwPollEvents();

}


GlfwManager::~GlfwManager()
{
    glfwTerminate();
}


bool GlfwManager::shouldExit()
{
    return    glfwGetKey( mWindow, GLFW_KEY_ESCAPE ) != GLFW_PRESS
           && glfwWindowShouldClose( mWindow )       == 0          ;
}


void GlfwManager::dispatchWindowSize( GLFWwindow* w, int x, int y )
{
    if ( mThisSingleton != nullptr ) {

        glfwGetFramebufferSize(
            mThisSingleton->mWindow,
            &(mThisSingleton->mWindowWidthInPixel),
            &(mThisSingleton->mWindowHeightInPixel)
        );
    
        glfwGetWindowSize(
            mThisSingleton->mWindow,
            &(mThisSingleton->mWindowWidth),
            &(mThisSingleton->mWindowHeight)
        );

        glViewport(
            0,
            0,
            (GLsizei)(mThisSingleton->mWindowWidthInPixel), 
            (GLsizei)(mThisSingleton->mWindowHeightInPixel)
        );

        mThisSingleton->callbackWindowSize( w, x, y );
    }
}


void GlfwManager::dispatchMouseButton( GLFWwindow* w, int b, int a, int m )
{
    if ( mThisSingleton != nullptr ) {

        mThisSingleton->callbackMouseButton( w, b, a, m );
    }
}


void GlfwManager::dispatchMousePos( GLFWwindow* w, double x, double y )
{
    if ( mThisSingleton != nullptr ) {

        mThisSingleton->callbackMousePos( w, x, y );
    }
}


void GlfwManager::dispatchMouseWheel( GLFWwindow* w, double x, double y )
{
    if ( mThisSingleton != nullptr ) {

        mThisSingleton->callbackMouseWheel( w, x, y );
    }
}


void GlfwManager::dispatchKey( GLFWwindow* w,int k,int s,int a,int m )
{
    if ( mThisSingleton != nullptr ) {

        mThisSingleton->callbackKey( w, k, s, a, m );
    }
}


void GlfwManager::dispatchChar( GLFWwindow* w, int c )
{
    if ( mThisSingleton != nullptr ) {

        mThisSingleton->callbackChar( w, c );
    }
}


void GlfwManager::callbackWindowSize(
    GLFWwindow* window, 
    int         width,
    int         height
) {
   ;
}

void GlfwManager::callbackMouseButton(
    GLFWwindow* window,
    int         button,
    int         action,
    int         mods
) {
    ;
}


void GlfwManager::callbackMousePos(
    GLFWwindow* window,
    double      xpos,
    double      ypos
) {
    ;
}


void GlfwManager::callbackMouseWheel(
    GLFWwindow* window,
    double      xoffset,
    double      yoffset
) {
    ;
}


void GlfwManager::callbackKey(
    GLFWwindow* window,
    int         key,
    int         scancode,
    int         action,
    int         mods
) {
    ;
}


void GlfwManager::callbackChar(
    GLFWwindow* window,
    int         codepoint
) {
    ;
}


class DeltaTime {

  public:

    DeltaTime(): mMicroSecondsCur(0.0)
    {
        struct timeval  mTimeVal;

        gettimeofday( &mTimeVal, NULL );

        mMicroSecondsCur = mTimeVal.tv_sec*1000000.0 + 
                           mTimeVal.tv_usec ;
    }

    ~DeltaTime(){;}

    double delta()
    {
        struct timeval  mTimeVal;
        double          microSecondsPrev;

        gettimeofday( &mTimeVal, NULL );

        microSecondsPrev = mMicroSecondsCur;

        mMicroSecondsCur  = mTimeVal.tv_sec * 1000000.0 + mTimeVal.tv_usec ;

        double delta = ( mMicroSecondsCur - microSecondsPrev ) / 1000000.0 ;
        
        return delta;
    }


  private:

    double mMicroSecondsCur;

};


class Word {

  public:
    Word( SDFont::RuntimeHelper& helper, string str, float fontSize, float spreadRatio ):
        mHelper       (helper),
        mString       (str),
        mLeftX        (0.0),
        mBaselineY    (0.0),
        mFontSize     (fontSize),
        mDistribution (1.0),
        mSpreadRatio  (spreadRatio),
        mZ            (0.0)
    {
        mHelper.getMetrics( mString,
                            mFontSize,
                            mWidth,
                            mTextureXs,
                            mFirstBearingX,
                            mBearingY,
                            mBelowBaselineY,
                            mAdvanceY,
                            mGlyphs          );

    }

    long len() const { return mGlyphs.size(); }

    float width() const {return mWidth; }

    float firstBearingX() const {return mFirstBearingX; }

    float bearingY() const {return mBearingY; }

    float belowBaselineY() const {return mBelowBaselineY; }

    float advanceY() const {return mAdvanceY; }

    void setPos(float leftX, float baselineY) {
        mLeftX     = leftX;
        mBaselineY = baselineY;
    }

    void setDistribution(float s) { mDistribution = s; }

    void generateElements(float* elements, GLuint* indices, long startIndex) {

        mHelper.generateOpenGLDrawElements (
            mGlyphs,
            mTextureXs,
            mLeftX - firstBearingX(),
            mBaselineY,
            mFontSize,
            mSpreadRatio,
            mDistribution,
            mZ,
            &( elements[   SDFont::RuntimeHelper::NUM_FLOATS_PER_GLYPH
                         * startIndex                                   ] ) ,
            SDFont::RuntimeHelper::NUM_POINTS_PER_GLYPH * startIndex,
            & ( indices[   SDFont::RuntimeHelper::NUM_INDICES_PER_GLYPH
                         * startIndex                                   ] )
        );
    }

    ~Word() {;}

    SDFont::RuntimeHelper&   mHelper;
    string                   mString;
    float                    mWidth;
    float                    mFirstBearingX;
    float                    mBearingY;
    float                    mBelowBaselineY;
    float                    mAdvanceY;
    vector< SDFont::Glyph* > mGlyphs;
    vector< float >          mTextureXs;

    float                    mLeftX;
    float                    mBaselineY;
    float                    mFontSize;
    float                    mDistribution;
    float                    mSpreadRatio;
    float                    mZ;
};


class Line {

  public:

    Line(SDFont::RuntimeHelper& helper, string str, float fontSize, float spreadRatio ) :
        mHelper ( helper ),
        mWidth  ( 1.0    )
    {

        vector<string> strs;

        splitLine( str, strs, ' ' );
        for (auto& s : strs) {
            mWords.emplace_back(helper, s, fontSize, spreadRatio );
        }
    }

    virtual ~Line(){;}

    vector<Word>& words() { return mWords; }

    long len() const { 
        long sum = 0;
        for ( auto& w : mWords ) {
            sum += w.len();
        }
        return sum;
    }

    float bearingY() const {

        float maxBearingY = 0.0;
        for ( auto& w : mWords ) {
            maxBearingY = std::max(maxBearingY, w.bearingY());
        }
        return maxBearingY;
    }

    float belowBaselineY() const {

        float maxBelowBaselineY = 0.0;
        for ( auto& w : mWords ) {
            maxBelowBaselineY = std::max(maxBelowBaselineY,w.belowBaselineY());
        }
        return maxBelowBaselineY;
    }

    float advanceY() const {

        float maxAdvanceY = 0.0;
        for ( auto& w : mWords ) {
            maxAdvanceY = std::max( maxAdvanceY, w.advanceY() );
        }
        return maxAdvanceY;
    }

    float totalWidth() const {

        float sum = 0.0;
        for ( auto& w : mWords ) {
            sum += w.width();
        }
        return sum;
    }

    void setWidth( float w ) { mWidth = w; }

    float width() const { return mWidth; }

    void setPos( float leftX, float baselineY ) {
        mLeftX     = leftX;
        mBaselineY = baselineY;
    }

    void assignPosForWords() {

        auto minWidth = totalWidth();
        auto margin   = mWidth - minWidth;
        auto space    = margin / (float)(mWords.size()- 1);
        auto leftX    = mLeftX;
        for ( auto& w : mWords ) {
            w.setPos(leftX, mBaselineY);
            leftX += ( w.width() + space );
        }

    }

    void generateElements(float* elements, GLuint* indices, long startIndex) {
        
        for ( auto& w : mWords ) {

            w.generateElements ( elements, indices, startIndex );
            startIndex += w.len();
        }
    }

  private:

    SDFont::RuntimeHelper& mHelper;    
    float                  mLeftX;
    float                  mBaselineY;
    float                  mWidth;
    vector<Word>           mWords;
};


class Paragraph {

  public:

    Paragraph(SDFont::RuntimeHelper& helper, string str, float fontSize, float spreadRatio) :
        mHelper ( helper ),
        mWidth  ( 1.0    )
    {

        vector<string> strs;

        splitLine( str, strs, '\n' );
        for (auto& s : strs) {
            mLines.emplace_back(helper, s, fontSize, spreadRatio);
        }
    }

    virtual ~Paragraph(){;}

    vector<Line>& lines() { return mLines; }

    long len() const { 
        long sum = 0;
        for ( auto& line : mLines ) {
            sum += line.len();
        }
        return sum;
    }

    float maxWidth() const {

        float width = 0.0;

        for ( auto& line : mLines ) {

            width = std::max ( width, line.totalWidth() );
        }
        return width;
    }


    void setWidth( float w ) { 

        mWidth = w;
        for ( auto& line : mLines ) {

            line.setWidth( w );
        }
    }

    float width() const { return mWidth; }

    void setPos( float leftX, float bottomBaselineY ) {
        mLeftX           = leftX;
        mBottomBaselineY = bottomBaselineY;
    }


    float totalHeight() {   

        float sum = 0.0;

        sum += mLines[0].bearingY();

        for ( auto i = 0; i < ( mLines.size() - 1 ) ; i++ ) {

            sum += mLines[ i ].advanceY();
        }
        sum += mLines[ mLines.size() - 1 ].belowBaselineY();

        mMinHeight = sum;
        return sum;

    }

    void setHeight(float h) { mHeight = h; }

    float height() { return mHeight; }

    void assignPosForWords() {

        float scale        = mHeight / mMinHeight;
        float curBaselineY = mBottomBaselineY;

        for ( auto i = mLines.size() - 1 ; i > 0 ; i-- ) {

            auto& line = mLines[i];

            line.setPos( mLeftX, curBaselineY ) ;
            line.assignPosForWords() ;

            auto& prevLine = mLines[ i - 1 ];

            curBaselineY += ( prevLine.advanceY() * scale );
            
        }

        mLines[0].setPos( mLeftX, curBaselineY ) ;
        mLines[0].assignPosForWords() ;
    }

    void generateElements(float* elements, GLuint* indices, long startIndex) {
        
        for ( auto& line : mLines ) {

            line.generateElements ( elements, indices, startIndex );
            startIndex += line.len();
        }
    }

  private:

    SDFont::RuntimeHelper& mHelper;    
    float                  mLeftX;
    float                  mBottomBaselineY;
    float                  mWidth;
    float                  mHeight;
    float                  mMinHeight;
    vector<Line>           mLines;

};



class SequenceElement {

  public:
    SequenceElement(
        SDFont::RuntimeHelper&        helper,
        SDFont::VanillaShaderManager& shader 
    ) :
        mCurTime   ( 0.0     ),
        mHelper    ( helper  ),    
        mShader    ( shader  ),
        mGLattr    ( nullptr ),
        mGLindices ( nullptr ) {;}

    virtual ~SequenceElement() {
        if ( mGLattr != nullptr ) {
            free(mGLattr);
        }
        if ( mGLindices != nullptr ) {
            free(mGLindices);
        }
    }

    virtual float hintConstructionTime(){ return -1;}
    virtual float hintDesctuctionTime() { return -1;}

    virtual void step(float t, float dt) { mCurTime = t; }

  protected:


    void allocAttrIndices() {        

        mGLattr    = (float*)malloc (

                               sizeof(float)
                             * SDFont::RuntimeHelper::NUM_FLOATS_PER_GLYPH
                             * mNumElements
                     );
        mGLindices = (GLuint*) malloc (

                               sizeof(GLuint)
                             * SDFont::RuntimeHelper::NUM_INDICES_PER_GLYPH
                             * mNumElements
                     );

    }

    void draw() {
        mShader.draw(
            mGLattr ,
            SDFont::RuntimeHelper::NUM_FLOATS_PER_GLYPH  * mNumElements ,
            mGLindices ,
            SDFont::RuntimeHelper::NUM_INDICES_PER_GLYPH * mNumElements ,
            mEffect ,
            mLightingEffect,
            mLowThreshold ,
            mHighThreshold ,
            mSmoothing ,
            mBaseColor ,
            mBorderColor ,
            mP, mM, mV, mLightWCS
        );
    }

    float                         mCurTime;
    SDFont::RuntimeHelper&        mHelper;
    SDFont::VanillaShaderManager& mShader;

    int                  mEffect ;
    bool                 mLightingEffect;
    float                mLowThreshold ;
    float                mHighThreshold ;
    float                mSmoothing ;
    glm::vec3            mBaseColor ;
    glm::vec3            mBorderColor ;
    glm::mat4            mM ;
    glm::mat4            mV ;
    glm::mat4            mP ;
    glm::vec3            mLightWCS ;

    long                 mNumElements ;
    float*               mGLattr ;
    GLuint*              mGLindices ;
};


class SeqPrologue :public SequenceElement {

  public:

    SeqPrologue(
        SDFont::RuntimeHelper&        helper,
        SDFont::VanillaShaderManager& shader,
        float                         fontSize,
        float                         spreadRatio
    ) : SequenceElement( helper, shader )
    {

        Line line1 ( mHelper, mStr1, fontSize, spreadRatio );
        Line line2 ( mHelper, mStr2, fontSize, spreadRatio );

        mNumElements = line1.len() + line2.len();

        line1.setWidth( line1.totalWidth() * 1.2 );
        line2.setWidth( line2.totalWidth() * 1.1 );

        allocAttrIndices();

        line1.setPos( -0.5 * line1.width(), +0.5 * line1.advanceY());
        line1.assignPosForWords();
        line1.generateElements( mGLattr, mGLindices, 0 );

        line2.setPos( -0.5 * line1.width(), -0.5 * line2.advanceY());
        line2.assignPosForWords();
        line2.generateElements( mGLattr, mGLindices, line1.len() );

        mEffect         = 1;
        mLightingEffect = false;
        mLowThreshold   = 0.45;
        mHighThreshold  = 0.55;
        mSmoothing      = 2.0/16.0;
        mBaseColor      = glm::vec3( 0.0, 0.5, 1.0 );
        mBorderColor    = glm::vec3( 0.0, 0.5, 1.0 );

        mM = glm::translate(
                             glm::mat4(1.0),
                             glm::vec3( 0.0, 0.0, -4.0 )
                           );

        mV = glm::lookAt(
                          glm::vec3(  0.0f,  0.0f,  1.0f ), // Cam pos
                          glm::vec3(  0.0f,  0.0f,  0.0f ), // and looks here
                          glm::vec3(  0.0f,  1.0f,  0.0f )  // Head is up
                      );

        mP = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);

        mLightWCS       = glm::vec3( 0.0, 0.0, 2.0 );

    }

    virtual ~SeqPrologue() {;}

    virtual void step ( float t, float dt ) {

        if (0.0 <= t && t < 1.0) {
            mBaseColor = glm::vec3( 0.0, 0.5 * t, 1.0 * t );
            draw();
        }
        else if (1.0 <= t && t < 3.0) {
            mBaseColor = glm::vec3( 0.0, 0.5, 1.0 );
            draw();
        }
        else if (3.0 <= t && t < 4.0) {
            mBaseColor = glm::vec3( 0.0, 0.5 * (4.0 - t), 1.0 * (4.0 - t) );
            draw();
        }

        mCurTime = t;

    }

  private:

    static const string  mStr1 ;
    static const string  mStr2 ;

};

const string SeqPrologue::mStr1 = "A long time ago, in a galaxy far" ;
const string SeqPrologue::mStr2 = "far away...."  ;



class SeqTitle :public SequenceElement {

  public:

    SeqTitle(
        SDFont::RuntimeHelper&        helper,
        SDFont::VanillaShaderManager& shader,
        float                         fontSize,
        float                         spreadRatio
    ) : SequenceElement( helper, shader )
    {

        Line line1 ( mHelper, mStr1, fontSize, spreadRatio );
        Line line2 ( mHelper, mStr2, fontSize, spreadRatio );

        mNumElements = line1.len() + line2.len();

        line1.setWidth( line1.totalWidth() * 1.0 );
        line2.setWidth( line2.totalWidth() * 1.0 );

        allocAttrIndices();

        line1.setPos( -0.5 * line1.width(), +0.5 * line1.advanceY());
        line1.assignPosForWords();
        line1.generateElements( mGLattr, mGLindices, 0 );

        line2.setPos( -0.5 * line2.width(), -0.5 * line2.advanceY());
        line2.assignPosForWords();
        line2.generateElements( mGLattr, mGLindices, line1.len() );

        mEffect         = 5;
        mLightingEffect = false;
        mLowThreshold   = 0.45;
        mHighThreshold  = 0.55;
        mSmoothing      = 4.0/16.0;
        mBaseColor      = glm::vec3( 1.0, 1.0, 0.0 );
        mBorderColor    = glm::vec3( 1.0, 1.0, 0.0 );

        mM = glm::translate(
                             glm::mat4(1.0),
                             glm::vec3( 0.0, 0.0, 0.0 )
                           );

        mV = glm::lookAt(
                          glm::vec3(  0.0f,  0.0f,  1.0f ), // Cam pos
                          glm::vec3(  0.0f,  0.0f,  0.0f ), // and looks here
                          glm::vec3(  0.0f,  1.0f,  0.0f )  // Head is up
                      );

        mP = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);

        mLightWCS       = glm::vec3( 0.0, 0.0, 2.0 );
    }

    virtual ~SeqTitle() {;}

    virtual void step ( float t, float dt ) {

        if (5.0 <= t && t < 10.0) {

            mM = glm::translate(
                     glm::mat4(1.0),
                     glm::vec3( 0.0, 0.0, -1.0 * (2.0* t - 10.0))
                 );

            mBaseColor = glm::vec3( 1.0, 1.0, 0.0 );
            draw();

        }
        else if  ( 10.0 <= t && t < 12.0 ) {

            mM = glm::translate(
                     glm::mat4(1.0),
                     glm::vec3( 0.0, 0.0, -1.0 * (2.0* t - 10.0))
                 );

            mBaseColor = glm::vec3( 0.5 * (12.0 - t), 0.5 * (12.0 - t), 0.0 );

            draw();
        }

        mCurTime = t;
    }

  private:

    static const string  mStr1 ;
    static const string  mStr2 ;
    glm::mat4            mMrot;
};

const string SeqTitle::mStr1 = "STAR" ;
const string SeqTitle::mStr2 = "WARS" ;



class SeqMainRoll :public SequenceElement {

  public:

    SeqMainRoll(
        SDFont::RuntimeHelper&        helper,
        SDFont::VanillaShaderManager& shader ,
        float                         fontSize,
        float                         spreadRatio
    ) : SequenceElement( helper, shader )
    {

        Line      line1 ( mHelper, mEpisode   , fontSize, spreadRatio );
        Line      line2 ( mHelper, mTitle     , fontSize, spreadRatio );
        Paragraph para1 ( mHelper, mParagraph1, fontSize, spreadRatio );
        Paragraph para2 ( mHelper, mParagraph2, fontSize, spreadRatio );
        Paragraph para3 ( mHelper, mParagraph3, fontSize, spreadRatio );

        mNumElements =   line1.len() + line2.len()
                       + para1.len() + para2.len() + para3.len();

        line1.setWidth( line1.totalWidth() * 1.2 );
        line2.setWidth( line2.totalWidth() * 1.1 );

        allocAttrIndices();
        
        line1.setPos( -0.5 * line1.width(), +0.5 * line1.advanceY());
        line1.assignPosForWords();
        line1.generateElements( mGLattr, mGLindices, 0 );

        line2.setPos( -0.5 * line2.width(), -1.2 * line2.advanceY());
        line2.assignPosForWords();
        line2.generateElements( mGLattr, mGLindices, line1.len() );

        para1.setWidth(2.0);
        para2.setWidth(2.0);
        para3.setWidth(2.0);

        para1.setHeight( para1.totalHeight() * 1.2 );
        para2.setHeight( para2.totalHeight() * 1.2 );
        para3.setHeight( para3.totalHeight() * 1.2 );

        para1.setPos( -0.5 * para1.width(), -1.2 );
        para2.setPos( -0.5 * para2.width(), -2.7 );
        para3.setPos( -0.5 * para3.width(), -4.0 );

        para1.assignPosForWords();
        para2.assignPosForWords();
        para3.assignPosForWords();

        para1.generateElements( mGLattr, mGLindices, 
            line1.len() + line2.len() );

        para2.generateElements( mGLattr, mGLindices, 
            line1.len() + line2.len() + para1.len() );

        para3.generateElements( mGLattr, mGLindices, 
            line1.len() + line2.len() + para1.len() + para2.len() );

        mEffect         = 1;
        mLightingEffect = true;
        mLowThreshold   = 0.45;
        mHighThreshold  = 0.55;
        mSmoothing      = 2.0/16.0;
        mBaseColor      = glm::vec3( 1.0, 1.0, 0.0 );
        mBorderColor    = glm::vec3( 1.0, 1.0, 0.0 );

        float angle = -0.2 * M_PI;
        glm::quat Qrot(cos(angle), sin(angle), 0.0, 0.0 );
        mMrot = glm::toMat4(Qrot);

        mM = glm::translate(
                             mMrot,
                             glm::vec3( 0.0, 0.5, -1.3 )
                           );

        mV = glm::lookAt(
                          glm::vec3(  0.0f,  0.0f,  1.0f ), // Cam pos
                          glm::vec3(  0.0f,  0.0f,  0.0f ), // and looks here
                          glm::vec3(  0.0f,  1.0f,  0.0f )  // Head is up
                      );          

        mP = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);

        mLightWCS       = glm::vec3( 0.0, 0.0, 0.0 );

    }

    virtual ~SeqMainRoll() {;}

    virtual void step ( float t, float dt ) {

        if  ( 9.0 <= t && t < 30.0 ) {

            mM = glm::translate( mMrot, glm::vec3( 0.0, 
                                                   0.3 * (t - 9.0) + 0.5, 
                                                   -1.3                    ));
            mBaseColor      = glm::vec3( 1.0, 1.0, 0.0 );
            draw();

        }
        else if ( 30.0 <= t && t < 40.0) {

            mM = glm::translate( mMrot, glm::vec3( 0.0, 
                                                   0.3 * (t - 9.0) + 0.5,
                                                   -1.3                    ));
            mBaseColor      = glm::vec3( 0.1 * (40.0 - t), 
                                         0.1 * (40.0 - t),
                                         0.0              );
            draw();
        }
        mCurTime = t;

    }

  private:

    static const string  mEpisode ;
    static const string  mTitle ;
    static const string  mParagraph1 ;
    static const string  mParagraph2 ;
    static const string  mParagraph3 ;

    glm::mat4            mMrot;

};

const string SeqMainRoll::mEpisode    = "Episode IV" ;
const string SeqMainRoll::mTitle      = "A NEW HOPE" ;
const string SeqMainRoll::mParagraph1 = "It is a period of civil war.\n"
                                        "Rebel spaceships, striking\n"
                                        "from a hidden base, have won\n"
                                        "their first victory against\n"
                                        "the evil Galactic Empire." ;

const string SeqMainRoll::mParagraph2 = "During the battle, Rebel\n"
                                        "spies managed to steal secret\n"
                                        "plans to the Empire's\n"
                                        "ultimate weapon, the DEATH\n"
                                        "STAR, an armored space\n"
                                        "station with enough power\n"
                                        "to destroy an entire planet." ;

const string SeqMainRoll::mParagraph3 = "Pursued by the Empire's\n"
                                        "sinister agents, Princess\n"
                                        "Leia races home aboard her\n"
                                        "starship, custodian of the\n"
                                        "stolen plans that can save her\n"
                                        "people and restore\n"
                                        "freedom to the galaxy...." ;

class WM : public GlfwManager  {

  public:
    WM( int width, int height ) :
        GlfwManager ( width, height),
        mSeqStarted ( false )
        {;}
  
    void callbackKey(
        GLFWwindow* window,
        int         key,
        int         scancode,
        int         action,
        int         mods
    ) {
        mSeqStarted = true;
    }

    bool mSeqStarted;

};

int main( int argc, char* argv[] )
{

    if ( argc != 2 ) {

        cerr << "Usage: sdfont_demo <font_file_name_without_extention>\n";
        exit(1);
    }

    WM glfw ( 1024, 768 );

    string baseFilePathWOExt( argv[1] );
    string extTXT(".txt");
    string extPNG(".png");

    SDFont::RuntimeHelper helper ( baseFilePathWOExt + extTXT );
    SDFont::TextureLoader loader ( baseFilePathWOExt + extPNG );

    SDFont::VanillaShaderManager shader ( loader.GLtexture(), 0 );

    glfw.configGLFW();

    SeqPrologue seqElem01( helper, shader, 0.2, 0.1 );
    SeqTitle    seqElem02( helper, shader, 1.0, 0.4 );
    SeqMainRoll seqElem03( helper, shader, 0.15, 0.1 );

    DeltaTime dt;
    double    absT = 0.0;
    do {
        auto deltaT = dt.delta();

        glfw.update();

        shader.load();

        glClearColor ( 0.0, 0.0, 0.0, 0.0 );

        glClear      ( GL_COLOR_BUFFER_BIT );

        seqElem01.step( absT, deltaT );
        seqElem02.step( absT, deltaT );
        seqElem03.step( absT, deltaT );

        shader.unload();

        if ( glfw.mSeqStarted ) {
            absT += deltaT;
        }

    } while ( glfw.shouldExit() );

    return 0;
}

