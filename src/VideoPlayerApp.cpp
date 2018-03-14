#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"

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

	// gstreamer
	void loadMovieFile(const fs::path &path);

	gl::TextureRef			mFrameTexture;
	qtime::MovieGlRef		mMovie;
	// Warping
	WarpList		mWarps;
	fs::path		mSettings;
};


VideoPlayerApp::VideoPlayerApp()
{
	disableFrameRate();

	fs::path moviePath = getOpenFilePath();
	console() << "moviePath: " << moviePath << std::endl;

	if (!moviePath.empty())
		loadMovieFile(moviePath);

	// initialize warps
	mSettings = getAssetPath("") / "warps.xml";
	if (fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarps = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarps.push_back(WarpBilinear::create());
		mWarps.push_back(WarpPerspective::create());
		mWarps.push_back(WarpPerspectiveBilinear::create());
	}
	// adjust the content size of the warps
	Warp::setSize(mWarps, mMovie->getSize());
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
			fs::path moviePath = getOpenFilePath();
			if (!moviePath.empty())
				loadMovieFile(moviePath);
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
		//mMovie->setLoop();
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

	if (mFrameTexture) {
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
	settings->setWindowSize(640, 480);
	settings->setMultiTouchEnabled(false);
	settings->setConsoleWindowEnabled();
}

CINDER_APP(VideoPlayerApp, RendererGl, prepareSettings)
