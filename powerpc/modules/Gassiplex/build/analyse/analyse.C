#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include <TH2I.h>
#include <TFile.h>

using namespace std;

#define BUFFER_SIZE (8 * 5 * 65)

static void swapInt(uint32_t *buffer, size_t size) {
  size_t i;
  uint32_t val = 0; 
  uint8_t* old = reinterpret_cast<uint8_t*>(&val);
  uint8_t* buf = NULL;
  
  for (i = 0; i < size; i++) {
    val = buffer[i];
    buf = reinterpret_cast<uint8_t*>(&buffer[i]);
    buf[0] = old[3];
    buf[1] = old[2];
    buf[2] = old[1];
    buf[3] = old[0];
  }
}

static TH2I* statHistPort0 = NULL;
static TH2I* statHistPort1 = NULL;
static TH2I* statHistAll = NULL;

int main(int argc, char **argv)
{
  FILE *infile = NULL;
  uint32_t* data = NULL;
  uint32_t* dataEnd = NULL;
  uint32_t* p = NULL;
  uint32_t evtCtr = 0;
  uint32_t header[4];
  uint32_t len = 0;
  bool status = true;/*should always be true!*/
  bool dump = false;/*enable/disable printout of raw data*/
  char* filename = NULL;
  char* rootfilename = NULL;

  if(argc!=2){
    perror("ERROR, usage is: ./analyse datafile_without_.dat\n");
    return -1;
  }

  filename = new char[200];
  rootfilename = new char[200];
  strcpy(filename,argv[1]);
  strcpy(rootfilename,argv[1]);
  strcat(filename,".dat");
  fprintf(stdout,"Reading data from %s\n",filename);

  strcat(rootfilename,".root");
  fprintf(stdout,"Saving results in %s\n",rootfilename);


  if ((data = new uint32_t[BUFFER_SIZE * sizeof(uint32_t)]) == NULL) abort();
  
  infile = fopen(filename, "r");
  if (infile == NULL) abort();

  // Create HIst
  statHistPort0 = new TH2I("statHistPort0", "Stat-HistPort0", 400, 0, 400, 1024, 0, 1024);
  statHistPort0->GetXaxis()->SetTitle("Channel# + (ModuleId * 100)");
  statHistPort0->GetYaxis()->SetTitle("ADC-Value");
  statHistPort1 = new TH2I("statHistPort1", "Stat-HistPort1", 400, 0, 400, 1024, 0, 1024);
  statHistPort1->GetXaxis()->SetTitle("Channel# + (ModuleId * 100)");
  statHistPort1->GetYaxis()->SetTitle("ADC-Value");
  statHistAll = new TH2I("statHistAll", "Stat-HistAll", 800, 0, 800, 1024, 0, 1024);
  statHistAll->GetXaxis()->SetTitle("Channel# + ModuleId *100 + Port# *400");
  statHistAll->GetYaxis()->SetTitle("ADC-Value");
  evtCtr = 0;
  while (status) {
    if (dump) {
      fprintf(stdout, "# ------- EvtCtr: %04d ----------\n", evtCtr);
    }
    if (fread(&header,  sizeof(uint32_t),  4, infile) != 4) {
      // End of File?
      break;
    }
    swapInt(header, 4);
    len = header[0] / 4;
    if (len < 4) {
      break; // no header??
    }
    
    len -= 4;
    if (fread(data,  sizeof(uint32_t),  len, infile) != len) {
      // sollte auch nicht sein
      break;
    }
    
    swapInt(data, len);

    dataEnd = data + len;
    p = data;
    while (p < dataEnd) {
      uint8_t channel;
      uint8_t module;
      uint8_t port;
      uint16_t adcVal;
      uint8_t race;
      uint8_t sector;
      uint8_t dprRest;
      adcVal  = (*p     ) & 0x03ff;
      channel = (*p >> 10) & 0x003f;
      module  = (*p >> 16) & 0x0007;
      port    = (*p >> 19) & 0x0007;
      race    = (*p >> 22) & 0x0001;
      sector  = (*p >> 23) & 0x0007;
      dprRest = (*p >> 26) & 0x00ff;
     
      if (dump) {
        fprintf(stdout,
                "0x%08x  "
                "dpr: 0x%02x  "
                "s: 0x%02x  "
                "r: 0x%02x  "
                "p: 0x%02x  "
                "m: 0x%02x  "
                "c: 0x%02x  "
                "alsdc: 0x%03x"
                "\n",
                *p, dprRest, sector, race, port, module, channel, adcVal);
      }
      
      // Fill histos
      if (port == 0) statHistPort0->Fill(channel + module * 100, adcVal);
      if (port == 1) statHistPort1->Fill(channel + module * 100, adcVal);
      statHistAll->Fill(channel + module * 100 + port * 400, adcVal);
  
      p++;
    }
    evtCtr++;
  }
  
  // Write Hist to Root File
  TFile* rootFile = new TFile(rootfilename, "RECREATE");

  if (rootFile->IsZombie()) abort();

  rootFile->cd();
  statHistPort0->Write();
  statHistPort1->Write();
  statHistAll->Write();
 
  rootFile->Close();
  
  delete(statHistPort0);
  delete(statHistPort1);
  delete(statHistAll);
  delete(data);
  
  exit(EXIT_SUCCESS);
}
