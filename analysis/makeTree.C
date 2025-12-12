#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <TString.h>
#include <string>

#define EVENT 1
#define HEADERNUM 7
#define HEADBLOCK 2
#define PAPILLONCH 24
#define BLOCKLENGTH 4
#define DEFAULTSAMP 640
#define SAMPLENUM 640*EVENT
#define PEDESTALSAMPLES 64
#define QSTRIPCUT 1200

//Qstrip per each ch
TH1F* h1_Qstrip;
//The criterion of Qstrip selection
TF1* f1_criterion;

//==== Proto-declaration of functions
TString EndianConvertor(TString data);
std::string EndianConvertor(std::string data);
int EndianConvertor(int data);
void ShowConvertedHeader(TString header[7][2]);
float GetPedestal(int ch);
float GetStripPos(int ch, float xstrip);
void SetHisto();


void makeTree(TString dataname = "./sin1000kHz400mV_20251211_00.txt") {

  //==== File encording and opening
  TString fname = dataname;
  std::cout << fname << std::endl;
  //system("./encoder.sh \'dataname\'"); //encording: .bin to .txt


  std::ifstream fin(fname);
  if (fin.fail()) {
    std::cout << "File: " << fname << " could not open..." << std::endl;
    exit(1);
  }
  else std::cout << "File: " << fname << " loaded..." << std::endl;

  //==== Extracting of header info
  int TrigID = -1;
  TString header[HEADERNUM][HEADBLOCK];   // header information
  int waveform[PAPILLONCH][SAMPLENUM];    // 1st index: CH, 2nd index: sampling number per ch
  int sample[SAMPLENUM];                  // 1st index: sampling number per ch
  int adcwave[PAPILLONCH][SAMPLENUM];     // 1st index: CH, 2nd index: sampling number per ch
  //float Qstrip[PAPILLONCH];               // Predicted accumulated signal quantities
  int Qstrip[PAPILLONCH];               // Predicted accumulated signal quantities
  for (int ch=0; ch<PAPILLONCH; ch++) {
      Qstrip[ch] = 0.;
      for (int isample=0; isample<SAMPLENUM; isample++) {
          waveform[ch][isample] = 0;
          if(ch==0) sample[isample] = isample;
      }
  }
  TFile *fout = TFile::Open("test.root", "recreate");
      fout->cd();
  TTree *tree = new TTree("papillon", "papillon");
      tree->Branch("TrigID", &TrigID, "TrigID/I");
      tree->Branch("waveform", waveform, Form("waveform[%d][%d]/I", PAPILLONCH, SAMPLENUM));
      tree->Branch("adcwave", adcwave, Form("adcwave[%d][%d]/I", PAPILLONCH, SAMPLENUM));
      tree->Branch("sample", sample, Form("sample[%d]/I", SAMPLENUM));
      tree->Branch("Qstrip", Qstrip, Form("Qstrip[%d]/I", PAPILLONCH));

  bool linefull = true; //read from the head of the line or not?
  int blocknum = 0;     //Count data block
  int sampling = 0;     //Sampling number
  float PAPILLONbeampos = 0;
  float PAPILLONbeamwid = 0;
  bool firstheader = true;

  while ( 1 ) { //main loop
    //Get header info
    TString hoge;
    //if (blocknum==0 || blocknum%(14+48*DEFAULTSAMP)==0) {
    //if (blocknum==0 || blocknum%15374==0) { //14 (header) + NCH*NSAMPLE
    if (blocknum==0 || blocknum%(HEADERNUM*HEADBLOCK+PAPILLONCH*DEFAULTSAMP)==0) {
        if(TrigID >= 0) tree->Fill();
        if (linefull==true) {
            fin >> hoge >> header[0][0] >> header[0][1]
                        >> header[1][0] >> header[1][1]
                        >> header[2][0] >> header[2][1]
                        >> header[3][0] >> header[3][1];
            fin >> hoge >> header[4][0] >> header[4][1]
                        >> header[5][0] >> header[5][1]
                        >> header[6][0] >> header[6][1];
            if (firstheader) {
                PAPILLONbeampos = atof(header[4][1]);
                PAPILLONbeamwid = atof(header[5][1]);
                firstheader = false;
            }
            blocknum += 14;
            linefull = false;
        } else { 
            for (int i=0; i<HEADERNUM; i++) {
                for (int j=0; j<HEADBLOCK; j++) {
                    if ( blocknum%8 !=0 ) fin >> header[i][j];
                    else fin >> hoge >> header[i][j];  //pick up from the next line
                    blocknum++;
                }
            }
        }
        TrigID++;
        sampling = 0;
        if(fin.eof()){
            cout << "end of file!" << endl;
            break;
        }


        //  Show header info
        cout << Form("TrigID: %d, blocknum %d, line %d", TrigID, blocknum, blocknum/8) << endl;
        ShowConvertedHeader(header);
    }


    //==== Extracting of ADC data
    //  Show header info
    //if (sampling%(DEFAULTSAMP)==0) ShowConvertedHeader(header);
    //  Fill row wave form
    for (int ch=0; ch<PAPILLONCH; ch++) {
        if ( blocknum%8 !=0) {
            fin >> std::hex >> waveform[ch][sampling];
            blocknum++;
        } else {
            fin >> hoge >> waveform[ch][sampling]; //pick up from the next line
            blocknum++;
        }
        adcwave[ch][sampling] = (unsigned int)EndianConvertor(waveform[ch][sampling]);
    }
    sampling++;
    //if ( sampling == SAMPLENUM ) break;
    if (fin.eof()){
        tree->Fill();
        break; //end of file
    }
  } // while() end


  fout->Write();
  //return 0;
}




//==== Functions =====

//Endian converting for TString
// 1st index data: any TString what you want to convert
// return        : converted TString
TString EndianConvertor(TString data) {
  //copy string for endian converting
  TString tmpdata = data;

  data[2] =    data[0];  // 0th -> 2nd
  data[3] =    data[1];  // 1th -> 3rd
  data[0] = tmpdata[2];  // 2nd -> 0th
  data[1] = tmpdata[3];  // 3rd -> 1st
  //std::cout << data[0] << data[1] << data[2] << data[3] << std::endl;

  return data;
}


//Endian converting for std::string
// 1st index data: any std::string what you want to convert
// return        : converted std::string
std::string EndianConvertor(std::string data) {
  //copy string for endian converting
  std::string tmpdata = data;

  data[2] =    data[0];  // 0th -> 2nd
  data[3] =    data[1];  // 1th -> 3rd
  data[0] = tmpdata[2];  // 2nd -> 0th
  data[1] = tmpdata[3];  // 3rd -> 1st
  //std::cout << data[0] << data[1] << data[2] << data[3] << std::endl;

  return data;
}

//Endian converting for int
// 1st index data: any int what you want to convert
// return        : converted int
int EndianConvertor(int data) {
  //std::cout.fill('0');
  //std::cout << "int value: 0x" << std::hex << std::setw(4) << data << std::endl;
  
  int and_mask = 0xff;
  int little_endian = 0x00;
  int modifiedwave  = 0x00;
  while (data != 0x00) {
    little_endian += (data & and_mask);
    //printf("little_endian: 0x%x, waveform: 0x%x\n", little_endian, data);
    modifiedwave = little_endian;
    data = data >> 8;
    little_endian = little_endian << 8;
  }
  //std::cout << "int value(endian modified): 0x" << std::hex << modifiedwave
                                      //<< " = 0d" << std::dec << modifiedwave << std::endl;
  
  //to avoid miss converting for little value (e.g. 0x0008 -> 0x0800 not 0x0008)
  if (modifiedwave < 0x00ff) {
    modifiedwave *= 16*16;
  }

  return modifiedwave;
}


//Show all header information (endian converted)
// 1st index header: header data that is taken from .txt file
void ShowConvertedHeader(TString header[7][2]) {
  for (int i=0; i<HEADERNUM; i++) {
    for (int j=0; j<HEADBLOCK; j++) {
      header[i][j] = EndianConvertor(header[i][j]);
    }
  }

  std::cout << "" << std::endl;
  std::cout << "==== Header info ====" << std::endl;
  std::cout << "Magic           : " << header[0][0] << header[0][1] << std::endl;
  std::cout << "Board ID        : " << header[1][0] << header[1][1] << std::endl;
  std::cout << "Length          : " << header[2][0] << header[2][1] << std::endl;
  std::cout << "Trigger Count   : " << header[3][0] << header[3][1] << std::endl;
  std::cout << "Beam position   : " << header[4][0] << header[4][1] << std::endl;
  std::cout << "Beam width      : " << header[5][0] << header[5][1] << std::endl;
  std::cout << "Interlock status: " << header[6][0] << header[6][1] << std::endl;
  std::cout << "" << std::endl;
}


float GetPedestal(int ch) {

  float extpedestal = 0.;

  switch(ch) {
    case 12:
      extpedestal = 138;
      break;
    default:
      extpedestal = 0.;
      break;
  }
  return extpedestal;
}

float GetStripPos(int ch, float xstrip) {
  switch (ch) {
    case 0:
      xstrip = -51.329;
      break;
    case 1:
      xstrip = -46.829;
      break;
    case 2:
      xstrip = -42.329;
      break;
    case 3:
      xstrip = -37.829;
      break;
    case 4:
      xstrip = -33.329;
      break;
    case 5:
      xstrip = -28.829;
      break;
    case 6:
      xstrip = -24.329;
      break;
    case 7:
      xstrip = -19.829;
      break;
    case 8:
      xstrip = -15.329;
      break;
    case 9:
      xstrip = -10.829;
      break;
    case 10:
      xstrip =  -6.329;
      break;
    case 11:
      xstrip =  -1.829;
      break;
    case 12:
      xstrip =   2.671;
      break;
    case 13:
      xstrip =   7.171;
      break;
    case 14:
      xstrip =  11.671;
      break;
    case 15:
      xstrip =  16.171;
      break;
    case 16:
      xstrip =  20.671;
      break;
    case 17:
      xstrip =  25.171;
      break;
    case 18:
      xstrip =  29.671;
      break;
    case 19:
      xstrip =  34.171;
      break;
    case 20:
      xstrip =  38.671;
      break;
    case 21:
      xstrip =  43.171;
      break;
    case 22:
      xstrip =  47.671;
      break;
    case 23:
      xstrip =  52.171;
      break;
    default:
      xstrip = 0.;
      break;
  }
  return xstrip;
}


void SetHisto() {
  h1_Qstrip = new TH1F("h1_Qstrip", "Qstrip per ch; SSEM19 Ch; Signal Quantities Q_{ch}", 24, 0, 24);
  h1_Qstrip -> SetStats(0);
  h1_Qstrip -> SetTitleOffset(1.3, "Y");
  h1_Qstrip -> SetLineColor(kCyan-8);
  h1_Qstrip -> SetLineWidth(3);

  h1_Qstrip -> GetXaxis()->SetBinLabel(1, "1");
  h1_Qstrip -> GetXaxis()->SetBinLabel(2, "2");
  h1_Qstrip -> GetXaxis()->SetBinLabel(3, "3");
  h1_Qstrip -> GetXaxis()->SetBinLabel(4, "4");
  h1_Qstrip -> GetXaxis()->SetBinLabel(5, "5");
  h1_Qstrip -> GetXaxis()->SetBinLabel(6, "6");
  h1_Qstrip -> GetXaxis()->SetBinLabel(7, "7");
  h1_Qstrip -> GetXaxis()->SetBinLabel(8, "8");
  h1_Qstrip -> GetXaxis()->SetBinLabel(9, "9");
  h1_Qstrip -> GetXaxis()->SetBinLabel(10, "10");
  h1_Qstrip -> GetXaxis()->SetBinLabel(11, "11");
  h1_Qstrip -> GetXaxis()->SetBinLabel(12, "12");
  h1_Qstrip -> GetXaxis()->SetBinLabel(13, "13");
  h1_Qstrip -> GetXaxis()->SetBinLabel(14, "14");
  h1_Qstrip -> GetXaxis()->SetBinLabel(15, "15");
  h1_Qstrip -> GetXaxis()->SetBinLabel(16, "16");
  h1_Qstrip -> GetXaxis()->SetBinLabel(17, "17");
  h1_Qstrip -> GetXaxis()->SetBinLabel(18, "18");
  h1_Qstrip -> GetXaxis()->SetBinLabel(19, "19");
  h1_Qstrip -> GetXaxis()->SetBinLabel(20, "20");
  h1_Qstrip -> GetXaxis()->SetBinLabel(21, "21");
  h1_Qstrip -> GetXaxis()->SetBinLabel(22, "22");
  h1_Qstrip -> GetXaxis()->SetBinLabel(23, "23");
  h1_Qstrip -> GetXaxis()->SetBinLabel(24, "24");

  f1_criterion = new TF1("f1_criterion", TString::Format("%d", QSTRIPCUT), 0, 24);
  f1_criterion -> SetLineColor(kOrange+5);
  f1_criterion -> SetLineWidth(3);  
}
