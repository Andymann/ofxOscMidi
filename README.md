# ofxOscMidi
Midi/OSC helper based on openFrameworks.  
  
Midi In -> OSC Out.  
OSC In -> Midi Out.  
Midi In -> Midi Thru.  
  
You can decide if OSC messages (in and out) are treated as normalized or as 7bit (0..127).  
Settings are stored in 'settings.xml' upon termination.  
  
OSC messages are formatted like this:  
  
&nbsp;&nbsp;&nbsp;noteOn/[Channel]/[Pitch].  
&nbsp;&nbsp;&nbsp;controlChange/[Channel]/[value].  
  
<img src="image.png" alt="drawing" width="500"/>  
  
Dependencies:  
ofxDatGui https://github.com/braitsch/ofxDatGui  
ofxMidi https://github.com/danomatika/ofxMidi  
ofxNetworkUtils https://github.com/bakercp/ofxNetworkUtils  
ofxPoco.  
ofxOsc.  
ofxXmlSettings.  
