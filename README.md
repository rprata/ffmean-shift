FFMean Shift
===================

Real Time Tracking based in Mean Shift framework using LIBAV and SDL libraries (Without using [OpenCV Library](http://opencv.org/)).

----------

What's Mean Shift?
----------
Mean shift is a non-parametric feature-space analysis technique for locating the maxima of a density function, a so-called mode-seeking algorithm. Application domains include (computer vision and image processing area): 

  - Clustering;
  - Tracking;
  - Smoothing;

#### Overview
Mean shift is a procedure for locating the max point of a density function given discrete data sampled from that function. It is useful for detecting the modes of this density. This is an iterative method, and we start with an initial estimate  ![equation](https://github.com/rprata/ffmean-shift/blob/master/imgs/k_xi_x.png). Let a kernel function   $K(x_i - x)$  be given. This function determines the weight of nearby points for re-estimation of the mean. Typically a <b>Gaussian kernel</b> on the distance to the current estimate is used, $ K(x_i - x) = e^{-c||x_i - x||^2}$ . The weighted mean of the density in the window determined by  K:

$$m(x) = \frac{ \sum_{x_i \in N(x)} K(x_i - x) x_i } {\sum_{x_i \in N(x)} K(x_i - x)}$$
 
where $N(x)$  is the neighborhood of  $x$ , a set of points for which  $K(x) \neq 0$.
The mean-shift algorithm now sets  $x \leftarrow m(x)$ , and repeats the estimation until  m(x)  converges.

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

The implementation is based on the algorithm described in this [paper](http://comaniciu.net/Papers/MsTracking.pdf) (page 4).
-------------

Requirements
-------------
- [LIBAV (less than or equal 0.8.16)](https://libav.org/releases/libav-0.8.17.tar.xz);
- [SDL (less than or equal 1.2)](https://www.libsdl.org/download-1.2.php);
- 1 webcam (in my case, I used the [playstation eye](http://www.amazon.com/PlayStation-Eye-3/dp/B000VTQ3LU));

-------------

Using FFMean Shift
-------------
In shell console:
> $ cd  ffmean-shift/src

> $ make
> $ ./ffmean-shift /dev/video0

-------------

Results
-------------
You can view the results in this video

[![ffmean shift results](http://img.youtube.com/vi/6wH6gkhirhw/0.jpg)](http://www.youtube.com/watch?v=6wH6gkhirhw)

-------------

License
-------------

This software is licensed under [gpl2](https://www.gnu.org/licenses/gpl-2.0.html).