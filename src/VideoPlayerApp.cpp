#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"
#include "cinder/qtime/QuickTimeGl.h"
#include "Warp.h"

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;

class VideoPlayerApp : public App {

public:
	VideoPlayerApp();
	void mouseMove(MouseEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;
	void fileDrop(FileDropEvent event) override;
	void resize() override;
	void update() override;
	void draw() override;
	void cleanup() override;
	void setUIVisibility(bool visible);
private:
	// layout
	bool					mAutoLayout;
	int						mDisplayCount, mMainWindowWidth, mMainWindowHeight, mRenderWidth, mRenderHeight, mRenderX, mRenderY;
	bool					mCursorVisible;
	int						getWindowsResolution();
	bool					restore();
	// gstreamer
	void loadMovieFile(const fs::path &path);
	gl::TextureRef			mFrameTexture;
	qtime::MovieGlRef		mMovie;
	// Warping
	WarpList				mWarps;
	fs::path				mSettings;
};


VideoPlayerApp::VideoPlayerApp()
{
	disableFrameRate();
	fs::path moviePath = getOpenFilePath();
	console() << "moviePath: " << moviePath << std::endl;

	if (!moviePath.empty())
		loadMovieFile(moviePath);

	mCursorVisible = false;
	setUIVisibility(mCursorVisible);
	mAutoLayout = true;
	if (restore()) {
		getWindow()->setPos(mRenderX, mRenderY);
		setWindowSize(mRenderWidth, mRenderHeight);
	}
	else {
		getWindowsResolution();
	}
	// initialize warps
	mSettings = getAssetPath("") / "warps.xml";
	if (fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarps = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarps.push_back(WarpPerspectiveBilinear::create());
	}
	// initialize the content size of the warps
	Warp::setSize(mWarps, ivec2(1280, 720));
}
bool VideoPlayerApp::restore()
{
	// check to see if Settings.xml file exists
	fs::path params = getAssetPath("") / "VDSettings.xml";
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");
			if (settings.hasChild("AutoLayout")) {
				XmlTree AutoLayout = settings.getChild("AutoLayout");
				mAutoLayout = AutoLayout.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("CursorVisible")) {
				XmlTree CursorVisible = settings.getChild("CursorVisible");
				mCursorVisible = CursorVisible.getAttributeValue<bool>("value");
			}
			// if AutoLayout is false we have to read the custom screen layout
			if (mAutoLayout)
			{
				// init, overriden by GetWindowsResolution
				mMainWindowWidth = 1280;
				mMainWindowHeight = 720;
				mRenderWidth = 1280;
				mRenderHeight = 720;
				mRenderX = 0;
				mRenderY = 0;
			}
			else
			{
				if (settings.hasChild("RenderWidth")) {
					XmlTree RenderWidth = settings.getChild("RenderWidth");
					mRenderWidth = RenderWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderHeight")) {
					XmlTree RenderHeight = settings.getChild("RenderHeight");
					mRenderHeight = RenderHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderX")) {
					XmlTree RenderX = settings.getChild("RenderX");
					mRenderX = RenderX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderY")) {
					XmlTree RenderY = settings.getChild("RenderY");
					mRenderY = RenderY.getAttributeValue<int>("value");
				}
			}
			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

int VideoPlayerApp::getWindowsResolution()
{
	mDisplayCount = 0;
	int w = Display::getMainDisplay()->getWidth();
	int h = Display::getMainDisplay()->getHeight();

	// Display sizes
	if (mAutoLayout)
	{
		mMainWindowWidth = w;
		mMainWindowHeight = h;
		mRenderX = mMainWindowWidth;
		// for MODE_MIX and triplehead(or doublehead), we only want 1/3 of the screen centered	
		for (auto display : Display::getDisplays())
		{
			mDisplayCount++;
			mRenderWidth = display->getWidth();
			mRenderHeight = display->getHeight();
		}
		mRenderY = 0;
	}
	else
	{
		for (auto display : Display::getDisplays())
		{
			mDisplayCount++;
		}
	}
	return w;
}
void VideoPlayerApp::resize()
{
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarps);
}
void VideoPlayerApp::setUIVisibility(bool visible)
{
	if (visible)
	{
		showCursor();
	}
	else
	{
		hideCursor();
	}
}
void VideoPlayerApp::fileDrop(FileDropEvent event)
{
	loadMovieFile(event.getFile(0));

}
void VideoPlayerApp::update()
{
	if (mMovie)
		mFrameTexture = mMovie->getTexture();

	static bool sPrintedDone = false;
	if (!sPrintedDone && mMovie->isDone()) {
		console() << "Done Playing" << std::endl;
		sPrintedDone = true;
	}
}
void VideoPlayerApp::cleanup()
{
	// save warp settings
	Warp::writeSettings(mWarps, writeFile(mSettings));
	quit();
}
void VideoPlayerApp::mouseMove(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarps, event)) {
		// let your application perform its mouseMove handling here
	}
}
void VideoPlayerApp::mouseDown(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarps, event)) {
		// let your application perform its mouseDown handling here
	}
}
void VideoPlayerApp::mouseDrag(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarps, event)) {
		// let your application perform its mouseDrag handling here
	}
}
void VideoPlayerApp::mouseUp(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarps, event)) {
		// let your application perform its mouseUp handling here
	}
}

void VideoPlayerApp::keyDown(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarps, event)) {
		fs::path moviePath;
		// warp editor did not handle the key, so handle it here
		switch (event.getCode()) {
		case KeyEvent::KEY_ESCAPE:
			// quit the application
			quit();
			break;
		case KeyEvent::KEY_f:
			// toggle full screen
			setFullScreen(!isFullScreen());
			break;
		case KeyEvent::KEY_v:
			// toggle vertical sync
			gl::enableVerticalSync(!gl::isVerticalSyncEnabled());
			break;
		case KeyEvent::KEY_w:
			// toggle warp edit mode
			Warp::enableEditMode(!Warp::isEditModeEnabled());
			break;
		case KeyEvent::KEY_o:
			moviePath = getOpenFilePath();
			if (!moviePath.empty())
				loadMovieFile(moviePath);
			break;
		case KeyEvent::KEY_h:
			// mouse cursor and ui visibility
			mCursorVisible = !mCursorVisible;
			setUIVisibility(mCursorVisible);
			break;
		}
	}
}
void VideoPlayerApp::keyUp(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarps, event)) {
		// let your application perform its keyUp handling here
	}
}
void VideoPlayerApp::loadMovieFile(const fs::path &moviePath)
{
	try {
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGl::create(moviePath);
		
		mMovie->setLoop();
		mMovie->play();

		console() << "Playing: " << mMovie->isPlaying() << std::endl;
	}
	catch (ci::Exception &exc) {
		console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
		mMovie.reset();
	}

	mFrameTexture.reset();
}
void VideoPlayerApp::draw()
{
	gl::clear(Color::black());
	gl::color(Color::white());
	getWindow()->setTitle(toString(floor(getAverageFps())) + " fps");
	if (mFrameTexture) {
		
		Warp::setSize(mWarps, mFrameTexture->getSize());
		for (auto &warp : mWarps) {
			warp->begin();
			Rectf centeredRect = Rectf(mFrameTexture->getBounds()).getCenteredFit(getWindowBounds(), true);
			gl::draw(mFrameTexture, centeredRect);
			warp->end();
		}
	}
}

void prepareSettings(App::Settings *settings)
{
	settings->setWindowSize(1280, 720);
	settings->setMultiTouchEnabled(false);
	settings->setBorderless();
#ifdef _DEBUG
	//settings->setConsoleWindowEnabled();
#endif
}

CINDER_APP(VideoPlayerApp, RendererGl, prepareSettings)
