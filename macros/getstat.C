TMrbStatistics * getstat() {

  Int_t nobs, retval;
  TMrbStatistics * stat = 0; 
  TMessage * mess0;
  TString  mess("M_client getstat ");
//  mess += m;
// Open connection to server
//  TSocket *sock = new TSocket("localhost", 9091);
//  TSocket *sock = new TSocket("ramoth.bl.physik.uni-muenchen.de", 9091);
  TSocket *sock = new TSocket("bizet", 9091);

// Wait till we get the start message
  nobs= sock->Recv(*&mess0);

  cout <<"MessageToM_analyze, send: " << mess << endl;
  sock->Send(mess);          // send message
  Int_t  retval=0;

//  cout << "Send" << endl;

  while((nobs = sock->Recv(*&mess0))) // receive next message
  {
     cout << "Bytes received: " << nobs << endl;
     if(nobs <= 0) {
        cout << "no Bytes received: " << endl;        
        break;
     }
     if ( mess0->What() == kMESS_STRING ) {
        char *str0 = new char[nobs];
        mess0->ReadString(str0, nobs);
        cout << str0 << endl;
        delete str0;
        break;
     } else if ( mess0->What() == kMESS_OBJECT ) {

      	stat = (TMrbStatistics *) mess0->ReadObject(mess0->GetClass());
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
  return stat;
}
