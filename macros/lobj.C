Int_t lobj(const char * cobj = "TClonesArray")
{
   gROOT->ProcessLine("gObjectTable->Print() ; > lobjlist.dat");
   TString cmd(".!grep ");
   cmd += cobj;
   cmd += " lobjlist.dat";
   gROOT->ProcessLine(cmd);
}
