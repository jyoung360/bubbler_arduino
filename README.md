# Bubble Detector

Particle Photon based fermentation monitoring.

### Have you ever wanted to know much alcohol is in your homebrew?

Great!  Go buy a [Hydrometer](https://en.wikipedia.org/wiki/Hydrometer) for a couple bucks and take some simple measurements to get a nice accurate answer.


### Have you ever wanted to know much alcohol is in your homebrew in an overly complicated and less reliable way?

Well, you've come to the right place!  This project is not really about a practical solution to an unsolved problem, but a test bed on what can be done with different sensors and IoT devices.

### Goals
- Detect CO<sub>2</sub> produced during fermentation.
- Accurately measure the volume of CO<sub>2</sub> emitted.
- Stream results to a server for more detailed processing and analysis.

### Version 1.0

I currently use the following style of airlock.

![Airlock](https://cdn.shortpixel.ai/client/q_glossy,ret_img,w_467/https://thesmartshoponline.com/wp-content/uploads/2018/07/31HemU2Bte0L.jpg)

While I have found numerous posts online where people have been able to detect bubbles in the U-Trap with optical sensors, I had little luck with this.

Instead, my first attempt went with what I thought would be a reliable (if short lived) design.  Two electrodes would form a broken circuit slightly above the static water level.  As a bubble passed through the airlock, the water level would rise temporarily completing the circuit.

####Pros
- Bubbles could be detected
- False positives were very low
- Simple code and design

####Cons
- Low accuracy ( approx 40% of bubbles were missed )
- High corrosion of electrodes
- Leaky and potential for dropping metallic corrosion into brew

### Version 2.0

![Sound Sensor](https://osoyoo.com/wp-content/uploads/2017/07/soundsensor-pinoutput-1.jpg)

I switched to a microphone based sensor.  While this initially seemed a bad idea due to ambient noise, I found that these sensors are kinda shitty at picking up any kind of noise that isn't extremely close, sharp, and loud.  So it is kind of perfect for detecting a bubble if you put it right in the neck of the airlock.

Initially, I tried to use the digital output signal, but the potentiometer wasn't able to reliably signal bubbles, so I used the analog output instead.

Wiring this up and watching the analog values showed that I now had the ability to reliably detect bubbles, but it also highlighted a new issue.  I had hoped to just set a threshold value for ambient noise and count any signals above that level, but a bubble registered as a series of ascending and then descending values.  A naive count would usually register a bubble as 10-15 times.  I was able to work around this by only allowing a bubble to be counted every .5 secs.  This worked, mostly, but was obviously an ugly and error prone hack.

Finally, in version 2.0, I was doing all the processing on the Particle board itself.  So it would count up bubbles, keep an average for the past minute, and display it on a basic OLED screen.  Kinda neat, but it lost everything if it lost power, and it provided no long term storage of data which would allow me to track fermatation rates over time or more complex analysis of the data.

### Version 3.0

This version actually simplified the arduino code greatly.  Instead of tracking everything on the board and using it to try and filter out bubbles, I just piped all the analog values up to a server running [MQTT](http://mqtt.org/).  Then I could process the raw data with any code I wanted, store it all if I wanted, and send it off to other APIs or webpages.

I set up a python subscriber of the MQTT channel and used [NumPy](https://numpy.org/) to do some much better data analysis that could actually detect what a bubble analog signature looked like and didn't just pick the first value above a base threshold.  This actually gave MUCH better results.

Check out the python code [here](https://github.com/jyoung360/bubbler_python) or the [Bubbler Website](http://34.217.20.120:8080/)