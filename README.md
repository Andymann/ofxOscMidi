# ofxOscMidi
Midi/OSC helper based on openFrameworks.  
  
  
Important: Windows binaries ("exe-files") are not updated any more. If you want the latest version on Windows you have to compile it yourself.  
  
Midi In -> OSC Out.  
OSC In -> Midi Out.  
Midi In -> Midi Thru.  
  
You can decide if OSC messages (in and out) are treated as normalized or as 7bit (0..127).  
Settings are stored in 'settings.xml' upon termination.  
  
OSC messages are formatted like this:  
  
&nbsp;&nbsp;&nbsp;noteOn/[Channel]/[Pitch].  
&nbsp;&nbsp;&nbsp;controlChange/[Channel]/[value].  
  
<img src="image.png" alt="drawing" width="500"/>  
  
## Dependencies:  
ofxDatGui https://github.com/braitsch/ofxDatGui  
ofxMidi https://github.com/danomatika/ofxMidi  
ofxNetworkUtils https://github.com/bakercp/ofxNetworkUtils  
ofxPoco.  
ofxOsc.  
ofxXmlSettings.  

## Build instructions  
These instructions are tested on MacOS 'Big Sur' and above.  
They are basically copy-pasted from https://github.com/Andymann/ofxMidiClockAbletonLink with the important bits adjusted.  
They will download and prepare openframeworks v0.12.0 on your machine, download all dependencies and build and run ofxOscMidi. (You only have to do this once. When all instructions finished successfully the ofxOscMidi.app can be found in the /bin  subdirectory.)  
  
<nobr>export OFURL=https://github.com/openframeworks/openFrameworks/releases/download/0.12.0/of_v0.12.0_osx_release.zip</nobr>  
curl -k -L -o ofx.zip \$OFURL  
unzip ofx.zip  
rm ofx.zip  
mv of_v* openFrameworks/  
cd openFrameworks/  
export OF_ROOT=$(pwd)  
cd addons/  
git clone https://github.com/braitsch/ofxDatGui  
gi clone https://github.com/danomatika/ofxMidi  
git clone https://github.com/bakercp/ofxNetworkUtils  
cd ../apps/myApps/  
git clone https://github.com/Andymann/ofxOscMidi.git  
cd ofxOscMidi/  
echo y | rm -r ./bin/ofxMidiClockAbletonLink.app  
make clean && make && make RunRelease &  
open ./bin  