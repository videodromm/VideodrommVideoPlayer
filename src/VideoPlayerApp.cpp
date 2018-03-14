#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"

#include "cinder/qtime/QuickTimeGl.h"

using namespace ci;
using namespace ci::app;
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
	void update() override;
	void draw() override;
	void cleanup() override;
	void setUIVisibility(bool visible);
private:

	// gstreamer
	void loadMovieFile(const fs::path &path);

	gl::TextureRef			mFrameTexture;
	qtime::MovieGlRef		mMovie;
};


VideoPlayerApp::VideoPlayerApp()
{
	
	fs::path moviePath = getOpenFilePath();
	console() << "moviePath: " << moviePath << std::endl;

	if (!moviePath.empty())
		loadMovieFile(moviePath);
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
	
		quit();

}
void VideoPlayerApp::mouseMove(MouseEvent event)
{
	
}
void VideoPlayerApp::mouseDown(MouseEvent event)
{
	
}
void VideoPlayerApp::mouseDrag(MouseEvent event)
{
		
}
void VideoPlayerApp::mouseUp(MouseEvent event)
{
	
}

void VideoPlayerApp::keyDown(KeyEvent event)
{
	
		if (event.getChar() == 'o') {
			fs::path moviePath = getOpenFilePath();
			if (!moviePath.empty())
				loadMovieFile(moviePath);
		}
	
}
void VideoPlayerApp::keyUp(KeyEvent event)
{
	
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
	
	if (mFrameTexture) {
		Rectf centeredRect = Rectf(mFrameTexture->getBounds()).getCenteredFit(getWindowBounds(), true);

		gl::draw(mFrameTexture, centeredRect);
	}
}

void prepareSettings(App::Settings *settings)
{
	settings->setWindowSize(640, 480);
	settings->setMultiTouchEnabled(false);
	settings->setConsoleWindowEnabled();
}

CINDER_APP(VideoPlayerApp, RendererGl, prepareSettings)
