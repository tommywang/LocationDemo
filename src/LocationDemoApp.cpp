#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/app/LocationEvent.h"
#include "cinder/app/HeadingEvent.h"
#include "cinder/Utilities.h"
#include "cinder/Font.h"

#include "cinder/CinderMath.h"
#include "cinder/System.h"
#include <vector>
#include <map>
#include <list>
using std::list;
using namespace ci;
using namespace ci::app;
using namespace std;

static const bool PREMULT = false;

class LocationDemoApp : public AppCocoaTouch {
public:
	virtual void	setup();
	virtual void	update();
	virtual void	draw();
    
    virtual void    didUpdateToLocation(LocationEvent oldLocation, LocationEvent newLocation);
    virtual void    compassUpdated(HeadingEvent newHeading);
    gl::Texture	    mLocationText;
    gl::Texture     mHeadingText;
    gl::Texture     mSettingsText;
    
    bool            isRetina;
    float           fontSize;
};

void LocationDemoApp::setup()
{
    
    if (getWindowSize().x==640) {
        isRetina=true;
        fontSize=25;
    }
    else{
        isRetina=false;
        fontSize=12.5;
    }
    
    enableLocationSevices();
    startUpdatingHeading();
    startUpdatingLocation();
    
    //will display heading calibration
    bool decisionHeadingCalibration=true;
    shouldDisplayHeadingCalibration(decisionHeadingCalibration);
    
    //set the desired accuracy level to best
    Accuracy accuracyLevelDesired=AccuracyBestForNavigation;
    setAccuracyLevelDesired(accuracyLevelDesired);
    
    //set the heading filter to 2 degrees
    setHeadingFilter(2);
    
    //set the distance filter to 5 meters
    setDistanceFilter(5);
    
    /*Display*/
    string accuracy;
    switch (accuracyLevelDesired) {
        case AccuracyBest:
            accuracy="AccuracyBest";
            break;
        case AccuracyBestForNavigation:
            accuracy="AccuracyBestForNavigation";
            break;
        case AccuracyNearestTenMeters:
            accuracy="AccuracyNearestTenMeters";
            break;
        case AccuracyHundredMeters:
            accuracy="AccuracyHundredMeters";
            break;
        case AccuracyKilometer:
            accuracy="AccuracyKilometer";
            break;
        case AccuracyThreeKilometers:
            accuracy="AccuracyThreeKilometers";
            break;
        default:
            break;
    }
    
    string decisionCalibration;
    if (decisionHeadingCalibration) {
        decisionCalibration="Display Heading Calibration: YES";
    }
    else{
        decisionCalibration="Display Heading Calibration: NO";
    }
    
    std::string normalFont( "Arial" );
    TextLayout Tlayout;
    Tlayout.setBorder(0,10);
    Tlayout.clear( ColorA( 0.1f, 0.1f, 0.1f, 0.3f ) );
    Tlayout.setFont( Font( normalFont, fontSize) );
    Tlayout.setColor( Color( 1, 1, 1 ) );
    Tlayout.addLine("old speed=");
    Tlayout.addLine(" ");
    Tlayout.addLine("new speed=");
    Tlayout.addLine(" ");
    Tlayout.addLine("altitude is=");
    Tlayout.addLine(" ");
    Tlayout.addLine("old location=");
    Tlayout.addLine(" ");
    Tlayout.addLine("new location=");
    Tlayout.addLine(" ");
	Surface8u rendered = Tlayout.render( true, PREMULT );
	mLocationText = gl::Texture( rendered );
    
    TextLayout TlayoutHeading;
    TlayoutHeading.setBorder(0,10);
    TlayoutHeading.clear( ColorA( 0.1f, 0.1f, 0.1f, 0.3f ) );
    TlayoutHeading.setFont( Font( normalFont, fontSize) );
    TlayoutHeading.setColor( Color( 1, 1, 1 ) );
    TlayoutHeading.addLine("Heading Accuracy: ");
    TlayoutHeading.addLine(" ");
    TlayoutHeading.addLine("Magnetic Heading: ");
    TlayoutHeading.addLine(" ");
    TlayoutHeading.addLine("True Heading: ");
    TlayoutHeading.addLine(" ");
    TlayoutHeading.addLine("Magnetism Vector: ");
    Surface8u renderedHeading = TlayoutHeading.render( true, PREMULT );
	mHeadingText = gl::Texture( renderedHeading );
    
    TextLayout TlayoutSetting;
    TlayoutSetting.setBorder(0,10);
    TlayoutSetting.clear( ColorA( 0.1f, 0.1f, 0.1f, 0.3f ) );
    TlayoutSetting.setFont( Font( normalFont, fontSize) );
    TlayoutSetting.setColor( Color( 1, 1, 1 ) );
    
    if (locationServicesEnabled()) {
        TlayoutSetting.addLine("Location Service Available: YES");
    }
    else{
        TlayoutSetting.addLine("Location Service Available: NO");
    }
    TlayoutSetting.addLine(" ");
    if (headingAvailable()) {
        TlayoutSetting.addLine("Heading Service Available: YES");
    }
    else{
        TlayoutSetting.addLine("Heading Service Available: NO");
    }
    TlayoutSetting.addLine(" ");
    
    TlayoutSetting.addLine(decisionCalibration);
    TlayoutSetting.addLine(" ");
    TlayoutSetting.addLine("Accuracy desired: "+accuracy);
    TlayoutSetting.addLine(" ");
    
    stringstream oss;
    oss << getHeadingFilter();
    TlayoutSetting.addLine("Heading Filter: "+oss.str());
    TlayoutSetting.addLine(" ");
    
    stringstream ossBis;
    ossBis << getDistanceFilter();
    TlayoutSetting.addLine("Distance Filter: "+ossBis.str());
    TlayoutSetting.addLine(" ");
    
    Surface8u renderedSetting = TlayoutSetting.render( true, PREMULT );
    mSettingsText = gl::Texture( renderedSetting );
}

void LocationDemoApp::didUpdateToLocation(LocationEvent oldLocation, LocationEvent newLocation)
{
    std::string normalFont( "Arial" );
    TextLayout Tlayout;
    Tlayout.setBorder(0,10);
    Tlayout.clear( ColorA( 0.1f, 0.1f, 0.1f, 0.3f ) );
    Tlayout.setFont( Font( normalFont, fontSize) );
    Tlayout.setColor( Color( 1, 1, 1 ) );
    stringstream oss;
    oss << oldLocation.getSpeed();
    Tlayout.addLine("old speed="+oss.str());
    Tlayout.addLine(" ");
    
    stringstream ossBis;
    ossBis << newLocation.getSpeed();
    Tlayout.addLine("new speed="+ossBis.str());
    Tlayout.addLine(" ");
    
    stringstream ossBiss;
    ossBiss << newLocation.getAltitude();
    Tlayout.addLine("altitude is="+ossBiss.str());
    Tlayout.addLine(" ");
    
    stringstream ossBisss;
    ossBisss << oldLocation.getLatitude();
    ossBisss << ", ";
    ossBisss << oldLocation.getLongitude();
    Tlayout.addLine("old location="+ossBisss.str());
    Tlayout.addLine(" ");
    
    stringstream ossBissss;
    ossBissss << newLocation.getLatitude();
    ossBissss << ", ";
    ossBissss << newLocation.getLongitude();
    Tlayout.addLine("new location="+ossBissss.str());
    Tlayout.addLine(" ");
    
    Surface8u rendered = Tlayout.render( true, PREMULT );
    mLocationText = gl::Texture( rendered );
    
}

void LocationDemoApp::compassUpdated(HeadingEvent newHeading)
{
    std::string normalFont( "Arial" );
    TextLayout Tlayout;
    Tlayout.setBorder(0,10);
    Tlayout.clear( ColorA( 0.1f, 0.1f, 0.1f, 0.3f ) );
    Tlayout.setFont( Font( normalFont, fontSize) );
    Tlayout.setColor( Color( 1, 1, 1 ) );
    stringstream oss;
    oss << newHeading.getHeadingAccuracy();
    Tlayout.addLine("Heading Accuracy: "+oss.str());
    Tlayout.addLine(" ");
    
    stringstream ossBis;
    ossBis << newHeading.getMagneticHeading();
    Tlayout.addLine("Magnetic Heading: "+ossBis.str());
    Tlayout.addLine(" ");
    
    stringstream ossBiss;
    ossBiss << newHeading.getTrueHeading();
    Tlayout.addLine("True Heading: "+ossBiss.str());
    Tlayout.addLine(" ");
    
    stringstream ossBisss;
    Vec3f rawGeoMagnetismVector=newHeading.getRawGeoMagnetismVector();
    ossBisss << "(";
    ossBisss << rawGeoMagnetismVector.x;
    ossBisss << ", ";
    ossBisss << rawGeoMagnetismVector.y;
    ossBisss << ", ";
    ossBisss << rawGeoMagnetismVector.z;
    ossBisss << ")";
    Tlayout.addLine("Magnetism Vector: "+ossBisss.str());
    Tlayout.addLine(" ");
    
    Surface8u rendered = Tlayout.render( true, PREMULT );
    mHeadingText = gl::Texture( rendered );
    
}

void LocationDemoApp::update()
{
    
}

void LocationDemoApp::draw()
{
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	gl::setMatricesWindow( getWindowSize() );
    
	gl::enableAlphaBlending( PREMULT );
    
	gl::color( Color::white() );
    if (isRetina) {
        gl::draw( mSettingsText, Vec2f( 10, 50 ));
        gl::draw( mLocationText, Vec2f( 10, 400 ));
        gl::draw( mHeadingText, Vec2f( 10, 700 ));
    }
    else{
        gl::draw( mSettingsText, Vec2f( 5, 25));
        gl::draw( mLocationText, Vec2f( 5, 200 ));
        gl::draw( mHeadingText, Vec2f( 5, 350 ));
    }
    
}

CINDER_APP_COCOA_TOUCH( LocationDemoApp, RendererGl )
