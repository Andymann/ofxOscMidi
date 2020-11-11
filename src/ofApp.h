#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxMidi.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxNetworkUtils.h"

class ofApp : public ofBaseApp, public ofxMidiListener{

    public:
    
        const string LBL_MIDI_PORT_IN = "Select MidiPort (In)";
        const string LBL_MIDI_PORT_OUT = "Select MidiPort (Out)";
    const string LBL_MIDI_PORT_THRU = "Select MidiPort (Thru)";
        const string LBL_BTN_CLEAR = "Clear Log";

    
        Poco::Net::HostEntry thisHost;
        std::string nodeName;
        Poco::Net::IPAddress publicIp;
        Poco::Net::NetworkInterface::List siteLocalInterfaces;
        
        void setup();
        void draw();
        void update();
        void exit();

        ofTrueTypeFont font;
        string TITLE = "ofxOscMidi v0.2";
        string txtMsg = "www.Andyland.info";
    
    string sMidiInPort;
    string sMidiOutPort;
    string sMidiThruPort;
    string sOscNetwork;
        
        ofxDatGui* gui;
    
        //ofxMidiClock clock; //< clock message parser
        //bool clockRunning = false; //< is the clock sync running?
        //unsigned int beats = 0; //< song pos in beats
        //unsigned int beatsOfficial = 0;
        //double seconds = 0; //< song pos in seconds, computed from beats
        //string txtBPM = "BPM:";
        //string txtAbletonLinkPeers ="Link Peers:";
        //double bpm = 120; //< song tempo in bpm, computed from clock length
        //Boolean bReact = true;
        //Boolean bNewDownbeat = false;
        //Boolean bWaitingForDownbeat = true;
        //Boolean bResetNext = false;
        //int iRetriggerDelay = 50;
        //string txtMidiClockState  ="MIDI Clock:";
        //string txtMidiClockBeats = "MIDI Beats:";
    
        // MIDI TIMECODE
        //ofxMidiTimecode timecode; //< timecode message parser
        //bool timecodeRunning = false; //< is the timecode sync running?
        //long timecodeTimestamp = 0; //< when last quarter frame message was received
        //ofxMidiTimecodeFrame frame; //< timecode frame data, ie. H M S frame rate
    
    
        void keyPressed(int key);
        //void onSliderEvent(ofxDatGuiSliderEvent e);
        void onDropdownEvent(ofxDatGuiDropdownEvent e);
        void onButtonEvent(ofxDatGuiButtonEvent e);
        void newMidiMessage(ofxMidiMessage& eventArgs);
        void setMidiPort_In(string pPortName);
        void setMidiPort_Out(string pPortName);
    void setMidiPort_Thru(string pPortName);
        void parseMsg(ofxOscMessage pMessage);
    
        ofxOscReceiver oscReceiver;
        ofxMidiOut midiOut;
        ofxMidiOut midiThru;
        ofxMidiIn midiIn;
        
        vector<string>logText;
        stringstream text;
    
        ofxDatGuiDropdown* cmbMidiOut;
        ofxDatGuiDropdown* cmbMidiIn;
    ofxDatGuiDropdown* cmbMidiThru;
        ofxDatGuiDropdown* cmbNetwork;
        ofxDatGuiButton* btnClear;
        
        ofxMidiMessage midiMessage;
        ofxOscSender oscSender;
        ofxXmlSettings xmlSettings;
        void saveSettings();
        void addLog(string p);
        void showLog();
        
        int incomingPortOsc, outGoingPortOsc/*, midiOutChannel*/;
        //string outgoingIpOSC;
    
};

class myCustomTheme : public ofxDatGuiTheme{
public:
    myCustomTheme(){
        font.size = 12;
        //font.file = "path/to/font.ttf";
        init();
    }
};
