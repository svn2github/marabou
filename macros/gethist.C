TH1 * gethist(const char * m = "hQ1") {

  Int_t nobs, retval;
  TH1 * hist = 0; 
  TMessage * message;
  TString  mess("M_client gethist ");
  mess += m;
// Open connection to server
 TSocket *sock = new TSocket("localhost", 9091);
//  TSocket *sock = new TSocket("ramoth.bl", 9091);

// Wait till we get the start message
  nobs= sock->Recv(*&message);

  cout <<"MessageToM_analyze, send: " << mess << endl;
  sock->Send(mess);          // send message
  Int_t  retval=0;

//  cout << "Send" << endl;

  while((nobs = sock->Recv(*&message))) // receive next message
  {
     cout << "nobs " << nobs << endl;
     if ( message->What() == kMESS_STRING ) {
        char *str0 = new char[nobs];
            
        cout << str0 << endl;
        delete str0;
        break;
     } else if ( message->What() == kMESS_OBJECT ) {

      	hist = (TH1*) message->ReadObject(message->GetClass());
         break; 
     }  else {
       cout << "Unknown message type" << endl;
     }
  }
// Close the socket
  sock->Close();
  delete sock;
//  cout << "Int_t:" << retval << endl;
  gDirectory = gROOT;
  return hist;
}
