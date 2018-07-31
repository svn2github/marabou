Int_t stop_ma(Int_t snr = 9091, const char * m = "terminate") {
//
// send a message to M_analyze using socket/port snr at localhost
// default port: 9091, message: "terminate"
// other possible commands: "pause", "resume", "zero *" (clear all histos)
// 
// commands like "gethist hh" return a histogram and need more
// code to handle the returned histogram
//
  Int_t nobs = 0;
  static TMessage *mess0;
  TString mess("M_client ");
  mess += m;
// Open connection to server
  TSocket *sock = new TSocket("localhost", snr);

// Wait till we get the start message
  nobs= sock->Recv(*&mess0);
  char *str00=new char[nobs+1];
  mess0->ReadString(str00, nobs);
  cout << "At start received: " << str00 << endl;

  cout <<"MessageToM_analyze, send: " << mess << endl;
  
  sock->Send(mess);          // send message

//  cout << "Send" << endl;

  while((nobs = sock->Recv(*&mess0))) // receive next message
  {
//    cout << "nobs " << nobs << endl;
    char *str0=new char[nobs];
    mess0->ReadString(str0, nobs);
    cout << "In loop received: " << str0 << endl;
    delete mess0;
    if(!strncmp(str0, "ACK", 3))  // on ACK just exit
    {
      cout << str0 << endl;
      delete [] str0;
      break;
   }
//    printf("ret string: %s\n", str0);
    if(!strncmp(str0,"ERROR",5))  // on error, print and exit
    {
      cout << str0 << endl;
      delete [] str0;
      break;
    }
//    else retval=atoi(str0);
//    cout << "Int_t:" << retval << endl;
    delete [] str0; 
    break;   
  }
// Close the socket
  sock->Close();
  delete sock;
  cout << "Received bytes:" << nobs << endl;
  return nobs;
}
