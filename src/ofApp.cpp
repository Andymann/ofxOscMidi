#include "ofApp.h"

/*
    All components instantiated within a gui
    https://github.com/braitsch/ofxDatGui @braitsch
*/

void ofApp::setup()
{
    thisHost = ofxNet::NetworkUtils::getThisHost();
    nodeName = ofxNet::NetworkUtils::getNodeName();
    publicIp = ofxNet::NetworkUtils::getPublicIPAddress();
    
    
    
    ofSetBackgroundColor(0, 0, 0);
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofSetEscapeQuitsApp(false);
    
    font.load(OF_TTF_MONO, 23);
        
    if (xmlSettings.loadFile("settings.xml")) {
            ofLogVerbose()<<"XML loaded"<<endl;
        }
    
    incomingPortOsc = xmlSettings.getValue("incomingPortOsc", 12345);
    outGoingPortOsc    = xmlSettings.getValue("outGoingPortOsc", 12344);
    //midiOutChannel = xmlSettings.getValue("midiOutChannel", 1);
    //outgoingIpOSC = xmlSettings.getValue("outgoingIpOSC", "127.0.0.1");
    
    midiOut.openVirtualPort("ofxOscMidi Out");
    
    
// instantiate and position the gui //
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    
    // add a dropdown menu //
    vector<string> optsMidi = {midiIn.getInPortList()/*"option - 1", "option - 2", "option - 3", "option - 4"*/};
    
    vector<string> optsNic;
    siteLocalInterfaces = ofxNet::NetworkUtils::listNetworkInterfaces(ofxNet::NetworkUtils::SITE_LOCAL);

    optsNic.push_back("127.0.0.1");
    for (const auto& interface: siteLocalInterfaces)
    {
        //optsNic.push_back( interface.address().toString() );
        optsNic.push_back( interface.broadcastAddress().toString() );
    }
    
    gui->addDropdown("Select MidiPort", optsMidi);
    gui->addDropdown("Select Network", optsNic);
    gui->addBreak();
    gui->addLabel("OSC IP:" + ofToString(outgoingIpOSC) + " Port(out):" + ofToString(outGoingPortOsc) + " Port(in):" + ofToString(incomingPortOsc) );
    
    //gui->addLabel("Midi out Channel:" + ofToString(midiOutChannel));
      
    
// once the gui has been assembled, register callbacks to listen for component specific events //
    //gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    //gui->setTheme(new ofxDatGuiThemeSmoke());
    gui->setTheme(new myCustomTheme() );
    gui->setWidth(1024);
    
    
    //oscSender.setup(outgoingIpOSC, outGoingPortOsc);
    oscReceiver.setup(incomingPortOsc);
    message="No messages yet";
    

}


void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getName() << " Selected" << endl;
    string sTarget = ofToString(e.target->getName());
    if(sTarget.compare("Select Network")==0){
        //ofLogNotice("Network");
        cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        outgoingIpOSC=e.target->getLabel();
        //oscSender.clear();
        oscSender.setup(outgoingIpOSC, outGoingPortOsc);
    }else if(sTarget.compare("Select MidiPort")==0){
        //ofLogNotice("Midi");
        cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        setMidiPort( e.target->getLabel() );
    }

}

void ofApp::draw(){
    
    int iHeight = ofGetWindowHeight();
    int iWidth = ofGetWindowWidth();
        
    ofSetColor(100, 100, 100);
    //font.drawString(txtMsg, 10, ofGetHeight()-10);
    
}



void ofApp::update(){
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        if(m.getAddress() == "/noteOn"){
            if (m.getNumArgs()>1) {
                //midiOut.sendNoteOn(1,m.getArgAsInt32(0),m.getArgAsInt32(1));
                message ="Sending note on: Note ID " + ofToString(m.getArgAsInt32(0)) + " With Velocity "+ ofToString(m.getArgAsInt32(1));
                ofLogVerbose()<<message<<endl;
            }
            
            if (m.getNumArgs()==1) {
                //midiOut.sendNoteOn(1,m.getArgAsInt32(0));
                message ="Sending note on: Note ID " + ofToString(m.getArgAsInt32(0)) + " With Velocity "+ ofToString(64);
                ofLogVerbose()<<message<<endl;
            }
        }
        
        else if(m.getAddress() == "/noteOff"){
            //midiOut.sendNoteOff(1,m.getArgAsInt32(0));
            message ="Sending note off Note ID " + ofToString(m.getArgAsInt32(0));
            ofLogVerbose()<<message<<endl;
        
        }
        else{
            //ofLogVerbose()<<message<<endl;
            int i = m.getNumArgs();
            string sTmp = "";
            for (int j=0;j<i;j++){
                sTmp += m.getArgAsString(j) + " ";
            }
            ofLogNotice(m.getAddress() + " " + sTmp);
        }
    }
}

void ofApp::keyPressed(int key){

}


void ofApp::newMidiMessage(ofxMidiMessage& message) {
    ofxOscMessage m;
    switch(message.status) {
        case MIDI_TIME_CLOCK:
            break;
        case MIDI_START: case MIDI_CONTINUE:
            break;
        case MIDI_STOP:
            break;
        case MIDI_NOTE_ON:
            midiOut.sendNoteOn(message.channel, message.pitch, message.velocity);
            //ofLogNotice("Note On:" + ofToString(message.pitch) + " " + ofToString(message.velocity) + " Channel:" + ofToString(message.channel));
            m.setAddress("/noteOn");
            //m.addIntArg(midiMessage.channel);
            m.addIntArg(message.pitch);
            m.addIntArg(message.velocity);
            oscSender.sendMessage(m);
            break;
        case MIDI_NOTE_OFF:
            midiOut.sendNoteOff(message.channel, message.pitch, message.velocity);
            //ofLogNotice("Note Off:" + ofToString(message.pitch) + " " + ofToString(message.velocity) + " Channel:" + ofToString(message.channel));
            m.setAddress("/noteOff");
            //m.addIntArg(midiMessage.channel);
            m.addIntArg(message.pitch);
            m.addIntArg(message.velocity);
            oscSender.sendMessage(m);
            break;
        case MIDI_CONTROL_CHANGE:
            midiOut.sendControlChange(message.channel, message.control, message.value);
            //ofLogNotice("Midi CC:" + ofToString(message.control) + " " + ofToString(message.value) + " Channel:" + ofToString(message.channel));
            m.setAddress("/ControlChange/" + ofToString( message.control )+ "/x");
            //m.addIntArg(message.channel);
            //m.addIntArg(message.control);
            m.addIntArg(message.value);
            oscSender.sendMessage(m);
            break;
        default:
            break;
    }
    
    
}

void ofApp::setMidiPort(string pPortName){
    midiIn.closePort();
    midiIn.removeListener(this);
    
    midiIn.openPort( pPortName ); // open a virtual port
    midiIn.ignoreTypes(false, // sysex  <-- don't ignore timecode messages!
                       false, // timing <-- don't ignore clock messages!
                       true); // sensing
    
    // add ofApp as a listener
    midiIn.addListener(this);
    cout << "setMidiPoprt: " << pPortName << " Selected" << endl;
}
    
void ofApp::exit(){
    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
}

