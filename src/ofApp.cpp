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
    }else{
        ofLogVerbose("Could not load xml. Reverting to default values.");
    }
    
    
    
    incomingPortOsc = xmlSettings.getValue("incomingPortOsc", 54321);
    outGoingPortOsc    = xmlSettings.getValue("outGoingPortOsc", 12344);
    //midiOutChannel = xmlSettings.getValue("midiOutChannel", 1);
    //outgoingIpOSC = xmlSettings.getValue("outgoingIpOSC", "127.0.0.1");
    string sMidiInPort = xmlSettings.getValue("midiInPort", "");
    string sMidiOutPort = xmlSettings.getValue("midiOutPort", "");
    string sOscNetwork = xmlSettings.getValue("oscNetwork", "");
    
    midiOut.openVirtualPort("ofxOscMidi Out");
    
    
// instantiate and position the gui //
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    
    // add a dropdown menu //
    vector<string> optsMidi_In = {midiIn.getInPortList()/*"option - 1", "option - 2", "option - 3", "option - 4"*/};
    vector<string> optsMidi_Out = {midiOut.getOutPortList()};
    
    vector<string> optsNic;
    siteLocalInterfaces = ofxNet::NetworkUtils::listNetworkInterfaces(ofxNet::NetworkUtils::SITE_LOCAL);

    optsNic.push_back("127.0.0.1");
    for (const auto& interface: siteLocalInterfaces)
    {
        //optsNic.push_back( interface.address().toString() );
        optsNic.push_back( interface.broadcastAddress().toString() );
    }

    
    cmbMidiIn = gui->addDropdown(LBL_MIDI_PORT_IN, optsMidi_In);
    cmbMidiOut = gui->addDropdown(LBL_MIDI_PORT_OUT, optsMidi_Out);
    cmbNetwork = gui->addDropdown("Select Network", optsNic);
    gui->addBreak();
    gui->addLabel("OSC IP:" + ofToString(outgoingIpOSC) + " Port(out):" + ofToString(outGoingPortOsc) + " Port(in):" + ofToString(incomingPortOsc) );
    
    if(!sMidiInPort.empty()){
        for(int i=0; i<optsMidi_In.size(); i++){
            if(optsMidi_In[i] == sMidiInPort){
                cmbMidiIn->select(i);
                setMidiPort_In(sMidiInPort);
                break;
            }
        }
    }
    
    if(!sMidiOutPort.empty()){
        for(int i=0; i<optsMidi_Out.size(); i++){
            if(optsMidi_Out[i] == sMidiOutPort){
                cmbMidiOut->select(i);
                setMidiPort_Out(sMidiOutPort);
                break;
            }
        }
    }
    
    if(!sOscNetwork.empty()){
        for(int i=0; i<optsNic.size(); i++){
            if(optsNic[i] == sOscNetwork){
                cmbNetwork->select(i);
                outgoingIpOSC=sOscNetwork;
                oscSender.setup(outgoingIpOSC, outGoingPortOsc);
                break;
            }
        }
    }
    
    // once the gui has been assembled, register callbacks to listen for component specific events
    //gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    //gui->setTheme(new ofxDatGuiThemeSmoke());
    gui->setTheme(new myCustomTheme() );
    gui->setWidth(1024);
    
    
    //oscSender.setup(outgoingIpOSC, outGoingPortOsc);
    oscReceiver.setup(incomingPortOsc);
    message="No messages yet";
}


void ofApp::saveSettings(){
    //ofLogNotice(ofToString( cmbMidiOut->getSelected()->getName() ));
    if(!cmbMidiIn->getSelected()->getName().empty())
    xmlSettings.setValue("midiInPort", cmbMidiIn->getSelected()->getName() );
    
    if(!cmbMidiOut->getSelected()->getName().empty())
    xmlSettings.setValue("midiOutPort", cmbMidiOut->getSelected()->getName() );
    
    if(!cmbNetwork->getSelected()->getName().empty())
        xmlSettings.setValue("oscNetwork", cmbNetwork->getSelected()->getName() );
        
    xmlSettings.saveFile();
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getName() << " Selected" << endl;
    string sTarget = ofToString(e.target->getName());
    if(sTarget.compare("Select Network")==0){
        //cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        outgoingIpOSC=e.target->getLabel();
        oscSender.setup(outgoingIpOSC, outGoingPortOsc);
        
        /*
        int iPort= xmlSettings.getValue("testValue", 666);
        ofLogNotice("XML" + ofToString(iPort));
        xmlSettings.setValue("testValue", 2);
        //put some some settings into a file
        if(xmlSettings.saveFile("test.xml")){
            ofLogNotice("JEPP");
        }else{
            ofLogNotice("NOpe");
        }
        */
    }else if(sTarget.compare(LBL_MIDI_PORT_IN)==0){
        cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        setMidiPort_In( e.target->getLabel() );
    }else if(sTarget.compare(LBL_MIDI_PORT_OUT)==0){
        cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        setMidiPort_Out( e.target->getLabel() );
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
        parseMsg( m );
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
            m.setAddress("/noteOn/" + ofToString(message.pitch));
            //m.addIntArg(midiMessage.channel);
            //m.addIntArg(message.pitch);
            m.addIntArg(message.velocity);
            oscSender.sendMessage(m);
            break;
        case MIDI_NOTE_OFF:
            /*
            midiOut.sendNoteOff(message.channel, message.pitch, message.velocity);
            //ofLogNotice("Note Off:" + ofToString(message.pitch) + " " + ofToString(message.velocity) + " Channel:" + ofToString(message.channel));
            m.setAddress("/noteOff/"+ ofToString(message.pitch));
            //m.addIntArg(midiMessage.channel);
            //m.addIntArg(message.pitch);
            m.addIntArg(message.velocity);
            oscSender.sendMessage(m);
            */
            midiOut.sendNoteOn(message.channel, message.pitch, 0);
            //ofLogNotice("Note Off:" + ofToString(message.pitch) + " " + ofToString(message.velocity) + " Channel:" + ofToString(message.channel));
            m.setAddress("/noteOn/"+ ofToString(message.pitch));
            //m.addIntArg(midiMessage.channel);
            //m.addIntArg(message.pitch);
            m.addIntArg(0);
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

void ofApp::parseMsg(ofxOscMessage m){
    
    
    string sAddress = m.getAddress();
    ofLogNotice( ofToString(sAddress) );
    //ofLogNotice( m.getTypeString() );
    //ofLogNotice( ofToString(m.getNumArgs()) );
    //ofLogNotice( "Arg 0:" + ofToString( m.getArgAsInt(0) ));
    ofxMidiMessage midiMsg;
    
    //----Leere Nachrichten sind uninteressant
    if( sAddress.empty() ){
        return;
    }
    
    if(sAddress.back()=='/'){
        sAddress=sAddress.substr(0,sAddress.length()-1/*sAddress.find_last_of("/")*/);
        
    }
    
    if(ofToLower(sAddress).rfind("/noteoff/",0)==0     ){
        //ofLogNotice("Treffer");
    }
    
    else if(ofToLower(sAddress).rfind("/noteon/",0)==0     ){
        int iPos;
        int iNote;
        
        try{
            iPos = sAddress.find_last_of("/");
            iNote = stoi(sAddress.substr(iPos+1));
        }catch(const ExceptionInfo e){
            return;
        }
        
        midiMsg.channel = 3;
        midiMsg.pitch = iNote;
        midiMsg.velocity = m.getArgAsInt(0);
        
        midiOut.sendNoteOn(midiMsg.channel, midiMsg.pitch, midiMsg.velocity);
        
        
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


void ofApp::setMidiPort_In(string pPortName){
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

void ofApp::setMidiPort_Out(string pPortName){
    midiOut.closePort();
    //midiOut.removeListener(this);
    
    midiOut.openPort( pPortName ); // open a virtual port
    //midiIn.ignoreTypes(false, // sysex  <-- don't ignore timecode messages!
      //                 false, // timing <-- don't ignore clock messages!
        //               true); // sensing
    
    // add ofApp as a listener
    //midiIn.addListener(this);
    cout << "setMidiPoprt: " << pPortName << " Selected" << endl;
}
    
void ofApp::exit(){
    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
    
    midiOut.closePort();
    saveSettings();
}

