#include "ofApp.h"

/*
    All components instantiated within a gui
    https://github.com/braitsch/ofxDatGui @braitsch
*/


//----Auswahl: NoteOff sendet NoteOn mit Velocity=0 oder NoteOff oder beides?
//----OSC In /ControlChange

void ofApp::setup()
{
    /*
    thisHost = ofxNet::NetworkUtils::getThisHost();
    nodeName = ofxNet::NetworkUtils::getNodeName();
    publicIp = ofxNet::NetworkUtils::getPublicIPAddress();
    */
    ofSetBackgroundColor(0, 0, 0);
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofSetEscapeQuitsApp(false);
    ofSetWindowTitle(TITLE);
    
    font.load(OF_TTF_MONO, 11);
        
    if (xmlSettings.loadFile("settings.xml")) {
        //ofLogVerbose()<<"XML loaded"<<endl;
        addLog("XML loaded");
    }else{
        //ofLogVerbose("Could not load xml. Reverting to default values.");
        addLog("Could not load xml. Reverting to default values.");
    }
    
    incomingPortOsc = xmlSettings.getValue("incomingPortOsc", 54321);
    outGoingPortOsc    = xmlSettings.getValue("outGoingPortOsc", 12344);
    
    sMidiInPort = xmlSettings.getValue("midiInPort", "");
    sMidiOutPort = xmlSettings.getValue("midiOutPort", "");
    sMidiThruPort = xmlSettings.getValue("midiThruPort", "");
    sOscNetwork = xmlSettings.getValue("oscNetwork", "");
  
// instantiate and position the gui //
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    
    // add a dropdown menu //
    vector<string> optsMidi_In = {midiIn.getInPortList()};
    vector<string> optsMidi_Out = {midiOut.getOutPortList()};
    vector<string> optsMidi_Thru = {midiThru.getOutPortList()};
    
    vector<string> optsNic;
    siteLocalInterfaces = ofxNet::NetworkUtils::listNetworkInterfaces(ofxNet::NetworkUtils::SITE_LOCAL);

    optsNic.push_back("127.0.0.1");
    for (const auto& interface: siteLocalInterfaces)
    {
        optsNic.push_back( interface.broadcastAddress().toString() );
    }

    
    cmbMidiIn = gui->addDropdown(LBL_MIDI_PORT_IN, optsMidi_In);
    cmbMidiOut = gui->addDropdown(LBL_MIDI_PORT_OUT, optsMidi_Out);
    cmbMidiThru = gui->addDropdown(LBL_MIDI_PORT_THRU, optsMidi_Thru);
    cmbNetwork = gui->addDropdown("Select Network", optsNic);
    gui->addBreak();
    btnClear = gui->addButton(LBL_BTN_CLEAR);
    
    
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
    
    if(!sMidiThruPort.empty()){
        for(int i=0; i<optsMidi_Thru.size(); i++){
            if(optsMidi_Thru[i] == sMidiThruPort){
                cmbMidiThru->select(i);
                setMidiPort_Thru(sMidiThruPort);
                break;
            }
        }
    }
    
    if(!sOscNetwork.empty()){
        for(int i=0; i<optsNic.size(); i++){
            if(optsNic[i] == sOscNetwork){
                cmbNetwork->select(i);
                //sOscNetwork=sOscNetwork;
                oscSender.setup(sOscNetwork, outGoingPortOsc);
                break;
            }
        }
    }
    
    // once the gui has been assembled, register callbacks to listen for component specific events
    //gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    btnClear->onButtonEvent(this, &ofApp::onButtonEvent);
    //gui->setTheme(new ofxDatGuiThemeSmoke());
    gui->setTheme(new myCustomTheme() );
    gui->setWidth(1024);
    
    
    //oscSender.setup(outgoingIpOSC, outGoingPortOsc);
    oscReceiver.setup(incomingPortOsc);
    
    addLog("OSC:" + ofToString(sOscNetwork) + " Port(out):" + ofToString(outGoingPortOsc) + " Port(in):" + ofToString(incomingPortOsc));
   
}


void ofApp::addLog(string p){
    if(logText.size()>15){
        logText.erase( logText.begin() );
    }
    logText.push_back( p );
}

void ofApp::showLog(){
    string sTmp;
    for(int i=0; i<logText.size(); i++){
        sTmp += logText[i] + "\n";
    }
    font.drawString(sTmp, 10,cmbNetwork->getY() + 100);
}


void ofApp::saveSettings(){
    //ofLogNotice(ofToString( cmbMidiOut->getSelected()->getName() ));
    if(!cmbMidiIn->getSelected()->getName().empty())
    xmlSettings.setValue("midiInPort", sMidiInPort );
    
    if(!cmbMidiOut->getSelected()->getName().empty())
    xmlSettings.setValue("midiOutPort", sMidiOutPort );
    
    if(!cmbMidiThru->getSelected()->getName().empty())
    xmlSettings.setValue("midiThruPort", sMidiThruPort );
    
    if(!cmbNetwork->getSelected()->getName().empty())
        xmlSettings.setValue("oscNetwork", sOscNetwork );
        
    xmlSettings.saveFile();
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    logText.clear();
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getName() << " Selected" << endl;
    string sTarget = ofToString(e.target->getName());
    if(sTarget.compare("Select Network")==0){
        //cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        sOscNetwork=e.target->getLabel();
        oscSender.setup(sOscNetwork, outGoingPortOsc);
       
    }else if(sTarget.compare(LBL_MIDI_PORT_IN)==0){
        cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        setMidiPort_In( e.target->getLabel() );
        sMidiInPort = e.target->getLabel();
        
    }else if(sTarget.compare(LBL_MIDI_PORT_OUT)==0){
        cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        setMidiPort_Out( e.target->getLabel() );
        sMidiOutPort = e.target->getLabel();
        
    }else if(sTarget.compare(LBL_MIDI_PORT_THRU)==0){
        cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
        setMidiPort_Thru( e.target->getLabel() );
        sMidiThruPort = e.target->getLabel();
    }

}

void ofApp::draw(){
    
    int iHeight = ofGetWindowHeight();
    int iWidth = ofGetWindowWidth();
        
    ofSetColor(100, 100, 100);
    //font.drawString(txtMsg, 10, ofGetHeight()-10);
    showLog();

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
    
    string sMidiInMsg;
    string sMidiThruMsg;
    string sOscMsg;
    
    ofxOscMessage m;
    switch(message.status) {
        case MIDI_TIME_CLOCK:
            break;
        case MIDI_START: case MIDI_CONTINUE:
            break;
        case MIDI_STOP:
            break;
        case MIDI_NOTE_ON:
            
            midiThru.sendNoteOn(message.channel, message.pitch, message.velocity);
            
            m.setAddress("/NoteOn/" + ofToString(message.channel) + "/" + ofToString(message.pitch));
            m.addIntArg(message.velocity);
            oscSender.sendMessage(m);
            
            sMidiInMsg = "Midi In: Note On:" + ofToString(message.pitch) + " " + ofToString(message.velocity) + " Channel:" + ofToString(message.channel) + " " + midiIn.getInPortName(midiIn.getPort());
            
            sMidiThruMsg = "Midi Out: Note On:" + ofToString(message.pitch) + " " + ofToString(message.velocity) + " Channel:" + ofToString(message.channel) + " " + midiOut.getOutPortName(midiThru.getPort());
            
            sOscMsg = "OSC Out: /noteOn/" + ofToString(message.channel) + "/" + ofToString(message.pitch) + " " + ofToString( message.velocity );
            break;
        case MIDI_NOTE_OFF:

            //----instead of sending a dedicated NOTE OFF event we are sending
            //----a NOTE ON with velocity=0
            midiThru.sendNoteOn(message.channel, message.pitch, 0);
            //midiThru.sendNoteOff(message.channel, message.pitch, message.velocity);
            
            
            
            m.setAddress("/noteOn/" + ofToString(message.channel) + "/" + ofToString(message.pitch));
            m.addIntArg(0);
            oscSender.sendMessage(m);
            
            sMidiInMsg = "Midi In: Note Off:" + ofToString(message.pitch) + " " + ofToString(message.velocity) + " Channel:" + ofToString(message.channel) + " " + midiIn.getInPortName(midiIn.getPort());
            
            sMidiThruMsg = "Midi Out: Note On:" + ofToString(message.pitch) + " " + ofToString("0") + " Channel:" + ofToString(message.channel) + " " + midiOut.getOutPortName(midiThru.getPort());
            
            sOscMsg = "OSC Out: /NoteOn/" + ofToString(message.channel) + "/" + ofToString(message.pitch) + " " + ofToString( 0 /*message.velocity*/ );
            
            break;
        case MIDI_CONTROL_CHANGE:
            
            midiThru.sendControlChange(message.channel, message.control, message.value);
            
            m.setAddress("/ControlChange/" + ofToString(message.channel) + "/"  + ofToString( message.control )+ "/x");
            //m.addIntArg(message.channel);
            //m.addIntArg(message.control);
            m.addIntArg(message.value);
            oscSender.sendMessage(m);
            
            sMidiInMsg = "Midi In: Control Change:" + ofToString(message.control) + " " + ofToString(message.value) + " Channel:" + ofToString(message.channel) + " " + midiIn.getInPortName(midiIn.getPort());
            
            sMidiThruMsg = "Midi Out: Control Change:" + ofToString(message.control) + " " + ofToString(message.value) + " Channel:" + ofToString(message.channel) + " " + midiOut.getOutPortName(midiThru.getPort());
            
            sOscMsg="OSC Out: /ControlChange/" + ofToString(message.channel) + "/"  + ofToString(message.control) + "/x " + ofToString( message.value );
            break;
        default:
            break;
    }
    
    addLog (sMidiInMsg);
    addLog (sMidiThruMsg);
    addLog (sOscMsg);
}

void ofApp::parseMsg(ofxOscMessage m){
    
    ofxMidiMessage midiMsg;
    string sAddress = m.getAddress();
    
    //----Leere Nachrichten sind uninteressant
    if( sAddress.empty() ){
        return;
    }

    if(m.getNumArgs()<1){
        return;
    }
    
    addLog("Osc In:" + m.getAddress() + " " + ofToString(m.getArgAsInt(0)) );
    
    if(sAddress.back()=='/'){
        sAddress=sAddress.substr(0,sAddress.length()-1);
    }
    
    if(ofToLower(sAddress).rfind("/noteoff/",0)==0     ){
        //ofLogNotice("Treffer");
    }
    
    else if(ofToLower(sAddress).rfind("/noteon/",0)==0     ){
        int iPos;
        int iNote;
        int iChannel;
        try{
            //----von hinten
            iPos = sAddress.find_last_of("/");
            iNote = stoi(sAddress.substr(iPos+1));
        }catch(const ExceptionInfo e){
            return;
        }
        
        try{
            //----von vorne
            iPos = ofToLower(sAddress).find("noteon/");
            sAddress = sAddress.substr(iPos+7);
            iPos = sAddress.find_last_of("/");
            iChannel = stoi(sAddress.substr(iPos-1));
            
        }catch(const ExceptionInfo e){
            return;
        }
        
        midiMsg.channel = iChannel;
        midiMsg.pitch = iNote;
        midiMsg.velocity = m.getArgAsInt(0);
        
        midiOut.sendNoteOn(midiMsg.channel, midiMsg.pitch, midiMsg.velocity);
        midiThru.sendNoteOn(midiMsg.channel, midiMsg.pitch, midiMsg.velocity);
        
        
        addLog("Midi Out: Note On:" + ofToString(midiMsg.pitch) + " " + ofToString("0") + " Channel:" + ofToString(midiMsg.channel) + " " + midiOut.getOutPortName(midiOut.getPort()));
        
        addLog("Midi Out: Note On:" + ofToString(midiMsg.pitch) + " " + ofToString("0") + " Channel:" + ofToString(midiMsg.channel) + " " + midiOut.getOutPortName(midiThru.getPort()));
    }
    else{
        //ofLogVerbose()<<message<<endl;
        int i = m.getNumArgs();
        string sTmp = "";
        for (int j=0;j<i;j++){
            sTmp += m.getArgAsString(j) + " ";
        }
        addLog(m.getAddress() + " " + sTmp);
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
    //cout << "setMidiPoprt: " << pPortName << " Selected" << endl;
    addLog("Midi In:" + pPortName);
}

void ofApp::setMidiPort_Out(string pPortName){
    midiOut.closePort();
    midiOut.openPort( pPortName ); // open a virtual port
    addLog("Midi Out:" + pPortName);
}

void ofApp::setMidiPort_Thru(string pPortName){
    midiThru.closePort();
    midiThru.openPort( pPortName ); // open a virtual port
    addLog("Midi Thru:" + pPortName);
}
    
void ofApp::exit(){
    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
    
    midiOut.closePort();
    saveSettings();
}

