FFMean Shift
===================

Real Time Tracking is a native implementation based in Mean Shift framework using LIBAV and SDL libraries (don't use [OpenCV Library](http://opencv.org/)).

----------

What's Mean Shift?
----------
Mean shift is a non-parametric feature-space analysis technique for locating the maxima of a density function, a so-called mode-seeking algorithm. Application domains include (computer vision and image processing area): 

  - Clustering;
  - Tracking;
  - Smoothing;

#### Overview
Mean shift is a procedure for locating the max point of a density function given discrete data sampled from that function. It is useful for detecting the modes of this density. This is an iterative method, and we start with an initial estimate. For more information, see the article on [Wikipedia](http://en.wikipedia.org/wiki/Mean_shift).

#### Strengths
1. Mean shift is an application-independent tool suitable for real data analysis.
2. Does not assume any predefined shape on data clusters.
3. It is capable of handling arbitrary feature spaces.
4. The procedure relies on choice of a single parameter: bandwidth.
5. The bandwidth/window size 'h' has a physical meaning, unlike k-means.

#### Weaknesses
1. The selection of a window size is not trivial.
2. Inappropriate window size can cause modes to be merged, or generate additional “shallow” modes.
3. Often requires using adaptive window size.

----------

LIBAV
----------
<b>Libav</b> is a friendly and community-driven effort to provide its users with a set of portable, functional and high-performance libraries for dealing with multimedia formats of all sorts. It originates from the [FFmpeg](http://ffmpeg.org/) codebase, but goes its own way these days, providing its users with reliable releases and a clear vision how to go forward.

----------

SDL
----------
[Simple DirectMedia Layer](https://www.libsdl.org/) is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D. It is used by video playback software, emulators, and popular games including Valve's award winning catalog and many Humble Bundle games.

----------

Algorithm Implementation
-------------

The implementation is based on the algorithm described on this [paper](http://comaniciu.net/Papers/MsTracking.pdf) (page 4).

-------------

Requirements
-------------
- [LIBAV (less than or equal 0.8.16)](https://libav.org/releases/libav-0.8.17.tar.xz);
- [SDL (less than or equal 1.2)](https://www.libsdl.org/download-1.2.php);
- 1 webcam (In this case, I have used the [playstation eye](http://www.amazon.com/PlayStation-Eye-3/dp/B000VTQ3LU));

-------------

Using FFMean Shift
-------------
In shell console, enter in directory:
> $ cd  ffmean-shift/src

To compile the application:
> $ make

To run application:
> $ ./ffmean-shift /dev/video0

-------------

Results
-------------
You can view the results in these videos

[![ffmean shift results](http://img.youtube.com/vi/6zny-7wZXB8/0.jpg)](http://www.youtube.com/watch?v=6zny-7wZXB8)

[![ffmean shift results](http://img.youtube.com/vi/MVzGjAZE-04/0.jpg)](http://www.youtube.com/watch?v=MVzGjAZE-04)

-------------

License
-------------

This software is licensed under [MIT](https://opensource.org/licenses/MIT).