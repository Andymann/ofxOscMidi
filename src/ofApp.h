#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxMidi.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxNetworkUtils.h"

class ofApp : public ofBaseApp, public ofxMidiListener{

    public:
    
        const string LBL_CMD_MIDI_IN = "Midi In to OSC: ";
        const string LBL_CMD_MIDI_OUT = "OSC to Midi Out: ";
        const string LBL_CMD_MIDI_THRU = "Midi THRU: ";
        const string LBL_MIDI_PORT_IN = "Select MidiPort (In)";
        const string LBL_MIDI_PORT_OUT = "Select MidiPort (Out)";
        const string LBL_MIDI_PORT_THRU = "Select MidiPort (Thru)";
        const string LBL_NETWORK = "Select Network";
        const string LBL_BTN_CLEAR = "Clear Log";
        const string LBL_BTN_NORMALIZE = "Normalize OSC";

    
        Poco::Net::HostEntry thisHost;
        std::string nodeName;
        Poco::Net::IPAddress publicIp;
        Poco::Net::NetworkInterface::List siteLocalInterfaces;
        
        void setup();
        void draw();
        void update();
        void exit();

        ofTrueTypeFont font;
        string TITLE = "ofxOscMidi v0.4";
        string txtMsg = "www.Andyland.info";
    
    string sMidiInPort;
    string sMidiOutPort;
    string sMidiThruPort;
    string sOscNetwork;
        
    ofColor colBG;
    ofColor colToggleActive = ofColor(39, 135, 214);
    
        ofxDatGui* gui;
        
    
        void keyPressed(int key);
        //void onSliderEvent(ofxDatGuiSliderEvent e);
        void onDropdownEvent(ofxDatGuiDropdownEvent e);
        void onButtonEvent(ofxDatGuiButtonEvent e);
        void onToggleEvent(ofxDatGuiToggleEvent e);
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
        ofxDatGuiToggle* btnNormalize;
        
        ofxMidiMessage midiMessage;
        ofxOscSender oscSender;
        ofxXmlSettings xmlSettings;
        void saveSettings();
        void addLog(string p);
        void showLog();
        
        int incomingPortOsc, outGoingPortOsc/*, midiOutChannel*/;
        bool bNormalizeOsc;
    
};

class myCustomTheme : public ofxDatGuiTheme{
public:
    myCustomTheme(){
        font.size = 12;
        //font.file = "path/to/font.ttf";
        init();
    }
};
