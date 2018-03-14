# videodromm-video-player
Based on gstreamer

Setup:
git clone Cinder --recursive

In the parent folder of Cinder create a folder named Videodromm and git clone https://github.com/videodromm/VideodrommVideoPlayer

Follow instructions on https://github.com/cinder/Cinder/pull/1947:
- Apply patch to Cinder 
- setup a complete installation of gstreamer-1.0-devel-x86_64-1.12.4.msi and gstreamer-1.0-x86_64-1.12.4.msi 
- PATH add `C:\gstreamer\1.0\x86_64\bin` and `C:\gstreamer\1.0\x86_64\lib`
- In Cinder/proj/cmake run `cmake .. -G "Visual Studio 15 2017 Win64" -DCINDER_MSW_USE_GSTREAMER=1` and compile Cinder

In proj/cmake folder run 
`cmake  -G "Visual Studio 15 2017 Win64"`


