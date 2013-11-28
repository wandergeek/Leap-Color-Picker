#include "testApp.h"

/* Note on OS X, you must have this in the Run Script Build Phase of your project. 
where the first path ../../../addons/ofxLeapMotion/ is the path to the ofxLeapMotion addon. 

cp -f ../../../addons/ofxLeapMotion/libs/lib/osx/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/libLeap.dylib"; install_name_tool -change ./libLeap.dylib @executable_path/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";

   If you don't have this you'll see an error in the console: dyld: Library not loaded: @loader_path/libLeap.dylib
*/

//--------------------------------------------------------------
void testApp::setup(){

//    ofSetFrameRate(60);
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_SILENT);

	leap.open(); 

	l1.setPosition(200, 300, 50);   
	l2.setPosition(-200, -200, 50);

	cam.setOrientation(ofPoint(-20, 0, 0));

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    isConnected = tcpClient.setup("192.168.2.3", 9000);
//    isConnected = tcpClient.setup("127.0.0.1", 9000);
    tcpClient.setMessageDelimiter("\n");
    c = c.black;
    
    lastSentTime = 0;
    tcpClient.setVerbose(true);

    connectTime = 0;
	deltaTime = 0;
    numFingers = 0;
    verdana30.loadFont("verdana.ttf", 30, true, true);
	verdana30.setLineHeight(34.0f);
	verdana30.setLetterSpacing(1.035);
    
}


//--------------------------------------------------------------
void testApp::update(){
 

	fingersFound.clear();

//
    //Option 1: Use the simple ofxLeapMotionSimpleHand - this gives you quick access to fingers and palms. 
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
    
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
    
        for(int i = 0; i < simpleHands.size(); i++){
        
            for(int j = 0; j < simpleHands[i].fingers.size(); j++){
                int id = simpleHands[i].fingers[j].id;
            
                ofPolyline & polyline = fingerTrails[id]; 
                ofPoint pt = simpleHands[i].fingers[j].pos;
                
                //if the distance between the last point and the current point is too big - lets clear the line 
                //this stops us connecting to an old drawing
                if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){
                    polyline.clear(); 
                }
                
                //add our point to our trail
                polyline.addVertex(pt); 
                
                //store fingers seen this frame for drawing
                fingersFound.push_back(id);
            }
        }
    }
    if(simpleHands.size() > 0) {
        int hue = ofMap(simpleHands[0].handPos.x, -ofGetWidth()/2, ofGetWidth()/2, 0, 255);
        int brightness = ofMap(simpleHands[0].handPos.y, -ofGetHeight()/2, ofGetHeight()/2, 0, 255);
        int sat = ofMap(simpleHands[0].handPos.z, -200, 200, 0, 255);
        c.setHsb(hue, 255-sat, brightness);

        int r = c.r;
        int g = c.g;
        int b = c.b;
        stringstream s;
        s << r << "," << g << "," << b;
        strColor = s.str();
        
        if(simpleHands[0].fingers.size()>=4) {
            tcpClient.send(strColor);
        }
        numFingers = simpleHands[0].fingers.size();
    
    } else {
        numFingers = 0;
    }
    
    
    


	//IMPORTANT! - tell ofxLeapMotion that the frame is no longer new. 
	leap.markFrameAsOld();	
}

//--------------------------------------------------------------
void testApp::draw(){
    


    ofFill();
    ofSetColor(c);
    ofRect(0,0,ofGetWidth(), ofGetHeight());
    ofSetColor(255,255,255);
    ofDrawBitmapString("Num Fingers: " + ofToString(numFingers),20,20);
    if(numFingers < 5) {
        ofSetColor(255,255,255);
        verdana30.drawString("Grabbed color: " + strColor, ofGetWidth()/2, ofGetHeight()/2);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    
    if(key == 'a') {
        tcpClient.send("255,255,255");
    }

    if(key == 'b') {
        tcpClient.send("0,0,0");
    }
    
    if(key == ' ') {
        if(tcpClient.isConnected()) {
            cout << "tcpclient connected, sending: " << strColor << endl;
            tcpClient.send(strColor);
        }

    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    cout << "sending " << strColor << endl;
    tcpClient.send(strColor);
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void testApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
