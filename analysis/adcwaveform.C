#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <TString.h>
#include <string>

#define EVENT 10
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


void adcwaveform(TString dataname) {

  //==== File encording and opening
  TString fname = dataname + ".txt";
  std::cout << fname << std::endl;
  //system("./encoder.sh \'dataname\'"); //encording: .bin to .txt


  std::ifstream fin(fname);
  if (fin.fail()) {
    std::cout << "File: " << fname << " could not open..." << std::endl;
    return 1;
  }
  else std::cout << "File: " << fname << " loaded..." << std::endl;
  

  //==== Extracting of header info
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
    }
  }

  bool linefull = true; //read from the head of the line or not?
  int blocknum = 0;     //Count data block
  int sampling = 0;     //Sampling number
  float PAPILLONbeampos = 0;
  float PAPILLONbeamwid = 0;
  bool firstheader = true;
  while ( 1 ) {

    //Get header info
    TString hoge;
    //if (blocknum==0 || blocknum%(14+48*DEFAULTSAMP)==0) {
    if (blocknum==0 || blocknum%15374==0) {
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
      }
      else { 

        for (int i=0; i<HEADERNUM; i++) {
          for (int j=0; j<HEADBLOCK; j++) {
            if ( blocknum%8 !=0 ) fin >> header[i][j];
            else fin >> hoge >> header[i][j];  //pick up from the next line
            blocknum++;
          }
        }

      }
    }


    //==== Extracting of ADC data
    //  Show header info
    if (sampling%(DEFAULTSAMP)==0) ShowConvertedHeader(header);
    //  Fill row wave form
    for (int ch=0; ch<PAPILLONCH; ch++) {

      if ( blocknum%8 !=0) {
        fin >> std::hex >> waveform[ch][sampling];
        blocknum++;
      }
      else {
        fin >> hoge >> waveform[ch][sampling]; //pick up from the next line
        blocknum++;
      }
    }
    sampling++;

    if ( sampling == SAMPLENUM ) break;
    if (fin.eof()) break; //end of file

  } // while() end



  //==== Endian converting of ADC row data
  for (int ch = 0; ch<PAPILLONCH; ch++) {
    for (int isample=0; isample<DEFAULTSAMP; isample++) {
      adcwave[ch][isample] = (unsigned int)EndianConvertor(waveform[ch][isample]);
    }
  }



  //==== Pedestal calculation
  float Pedestal[PAPILLONCH];
  for (int i=0; i<PAPILLONCH; i++) Pedestal[i] = 0.;

  for (int ch = 0; ch<PAPILLONCH; ch++) {
    for (int isample=0; isample<PEDESTALSAMPLES; isample++) {
      Pedestal[ch] += adcwave[ch][isample];
    }
    Pedestal[ch] /= PEDESTALSAMPLES;
  }



  //==== Wave form drawing
  int PedestalLine[PAPILLONCH][SAMPLENUM];
  int ZeroLine[SAMPLENUM];
  for (int isample=0; isample<DEFAULTSAMP; isample++) {
    sample[isample] = (unsigned int)isample+1;
    ZeroLine[isample] = 0;
    for (int ch=0; ch<PAPILLONCH; ch++) PedestalLine[ch][isample] = Pedestal[ch];
  }

  TGraph* g_adcwave[PAPILLONCH];
  TGraph* g_pedestal[PAPILLONCH];
  for (int ch=0; ch<PAPILLONCH; ch++) {
    g_adcwave[ch] = new TGraph(DEFAULTSAMP, sample, adcwave[ch]);
    g_pedestal[ch] = new TGraph(DEFAULTSAMP, sample, PedestalLine[ch]);

    g_pedestal[ch] -> SetLineColor(kPink-3);
    g_pedestal[ch] -> SetLineWidth(2);
  }

  int x[2] = {PEDESTALSAMPLES, PEDESTALSAMPLES};
  int y[2] = {-4000, 4000};
  TGraph* g_pedestalsamples = new TGraph(2, x, y);
  g_pedestalsamples -> SetLineStyle(7);
  g_pedestalsamples -> SetLineColor(kAzure+7);
  g_pedestalsamples -> SetLineWidth(3);

  TCanvas* c_raw = new TCanvas("c_raw", "ADC Raw Wave Form", 800, 800);
  c_raw -> Divide(4,6);
  for (int ch=0; ch<PAPILLONCH; ch++) {
    c_raw -> cd(ch+1);
    g_adcwave[ch] -> GetYaxis()->SetLabelSize(0.07);
    g_adcwave[ch] -> Draw("APL");
    g_pedestal[ch] -> Draw("SAME");
    g_pedestalsamples -> Draw("SAME");

    //Ch legend
    std::string ChNum = std::to_string(ch+1);
    TText* ChLegend;
    ChLegend = new TText(0.9, 0.9, (TString)ChNum);
    ChLegend -> SetNDC(1);
    ChLegend -> SetTextSize(0.12);
    ChLegend -> Draw();
  }

#if 0
  TCanvas* c2 = new TCanvas("c2", "Channel Focus, 800, 400);
  int drawch1 = 19;
  g_adcwave[drawch1+1] -> Draw("APL");
  g_pedestal[drawch1+1] -> Draw("SAME");
  g_pedestalsamples -> Draw("SAME");

  //Ch legend
  std::string ChNum = std::to_string(drawch1+1);
  TText* ChLegend;
  ChLegend = new TText(0.9, 0.9, (TString)ChNum);
  ChLegend -> SetNDC(1);
  ChLegend -> SetTextSize(0.12);
  ChLegend -> Draw();
#endif



  //==== Accumulation of signal quantities
  int QstripHistory[PAPILLONCH][SAMPLENUM];
  int QstripCriterion[SAMPLENUM];
  for (int ch=0; ch<PAPILLONCH; ch++) {
    for (int isample=0; isample<DEFAULTSAMP; isample++) {
      QstripHistory[ch][isample] = 0;
    }
  }
  for (int isample=0; isample<DEFAULTSAMP; isample++) QstripCriterion[isample] = QSTRIPCUT;

  for (int ch = 0; ch<PAPILLONCH; ch++) {

    //Rounding off of pedestal calculation results
    int pedcheck = Pedestal[ch]*10;
    if (pedcheck%10 > 4) Pedestal[ch] = ceil(Pedestal[ch]);
    else Pedestal[ch] = (int)Pedestal[ch];

    //Get the pedestal-subtracted wave form
    //Sum the pedestal-subtracted wave height
    //for (int isample=0; isample<DEFAULTSAMP; isample++) {
    for (int isample=PEDESTALSAMPLES; isample<DEFAULTSAMP; isample++) { 
      adcwave[ch][isample] = adcwave[ch][isample] - Pedestal[ch];
      Qstrip[ch] += adcwave[ch][isample];
      QstripHistory[ch][isample] = Qstrip[ch];
    }
  }


  //Pedestal-subtracted wave form
  TGraph* g_noped_adcwave[PAPILLONCH];
  //TGraph* g_pedestal[PAPILLONCH];
  for (int ch=0; ch<PAPILLONCH; ch++) {
    g_noped_adcwave[ch] = new TGraph(DEFAULTSAMP, sample, adcwave[ch]);
  }
  TGraph* g_zeroline = new TGraph(DEFAULTSAMP, sample, ZeroLine);
  g_zeroline -> SetLineStyle(7);
  g_zeroline -> SetLineColor(kGray+2);
  g_zeroline -> SetLineWidth(2);


  //Qstrip accumulation history
  TGraph* g_Qstrip[PAPILLONCH];
  
  for (int ch=0; ch<PAPILLONCH; ch++) {
    g_Qstrip[ch] = new TGraph(DEFAULTSAMP, sample, QstripHistory[ch]);
  }
  TGraph* g_QstripCriterion = new TGraph(DEFAULTSAMP, sample, QstripCriterion);
  g_QstripCriterion -> SetLineWidth(2);
  g_QstripCriterion -> SetLineColor(kOrange+5);


  //==== Get strip position and other quantities
  SetHisto();
  float SumQstrip_x_xstrip  = 0.;
  float SumQstrip_x_xstrip2 = 0.;
  float SumQstrip           = 0.;
  float xstrip = 0.;

  for (int ch=0; ch<PAPILLONCH; ch++) {
    xstrip = GetStripPos(ch, xstrip);
    std::cout << "[Ch." << ch+1 << "] xstrip: " << xstrip << " mm, Pedestal: " << Pedestal[ch] << " Qstrip: " << Qstrip[ch];
    h1_Qstrip -> fArray[ch+1] = Qstrip[ch];

    // Selection of accumulated signal quantities
    if (Qstrip[ch]>QSTRIPCUT) {
      std::cout << " <-- use this strip" << std::endl;
      SumQstrip_x_xstrip  += xstrip*Qstrip[ch];
      SumQstrip_x_xstrip2 += xstrip*xstrip*Qstrip[ch];
      SumQstrip           += Qstrip[ch];
    }
    else std::cout << " " << std::endl;
  }


  //==== Beam position and width calculation
  float beampos = SumQstrip_x_xstrip/SumQstrip;
  float beamwid = std::sqrt( std::fabs(SumQstrip_x_xstrip2/SumQstrip - beampos*beampos) );
  std::cout << "SumQstrip           : "  << SumQstrip           << std::endl;
  //std::cout << "SumQstrip_x_xstrip2 : "  << SumQstrip_x_xstrip2 << std::endl;
  //std::cout << "beampos*beampos     : "  << beampos*beampos     << std::endl;
  //std::cout << "SumQstrip_x_xstrip2/SumQstrip - beampos*beampos: "  << SumQstrip_x_xstrip2/SumQstrip - beampos*beampos << std::endl;
  std::cout << "Predicted Beam pos: " << beampos << " mm" << std::endl;
  std::cout << "Predicted Beam wid: " << beamwid << " mm" << std::endl;

#if 1
  TCanvas* c0 = new TCanvas("c0", "Accumulation History", 800, 800);
  c0 -> Divide(4,6);
  for (int ch=0; ch<PAPILLONCH; ch++) {
    c0 -> cd(ch+1);
    g_Qstrip[ch] -> GetYaxis()->SetLabelSize(0.07);
    g_Qstrip[ch] -> Draw("APL");
    g_pedestalsamples -> Draw("SAME");
    g_zeroline        -> Draw("SAME");
    g_QstripCriterion -> Draw("SAME");

    //Ch legend
    std::string ChNum = std::to_string(ch+1);
    TText* ChLegend;
    ChLegend = new TText(0.9, 0.9, (TString)ChNum);
    ChLegend -> SetNDC(1);
    ChLegend -> SetTextSize(0.12);
    ChLegend -> Draw();
  }
#endif

#if 1
  TCanvas* c1 = new TCanvas("c1", "Pedestal-subtracted Wave Form", 800, 800);
  c1 -> Divide(4,6);
  for (int ch=0; ch<PAPILLONCH; ch++) {
    c1 -> cd(ch+1);
    g_noped_adcwave[ch] -> GetYaxis()->SetLabelSize(0.07);
    g_noped_adcwave[ch] -> Draw("APL");
    g_pedestal[ch]      -> Draw("SAME");
    g_pedestalsamples   -> Draw("SAME");
    g_zeroline          -> Draw("SAME");

    //Ch legend
    std::string ChNum = std::to_string(ch+1);
    TText* ChLegend;
    ChLegend = new TText(0.9, 0.9, (TString)ChNum);
    ChLegend -> SetNDC(1);
    ChLegend -> SetTextSize(0.12);
    ChLegend -> Draw();
  }
#endif

#if 1
  TCanvas* c3 = new TCanvas("c3", "Qstrip per ch", 500, 500);
  c3 -> cd();
  c3 -> SetGrid();
  h1_Qstrip -> Draw();
  f1_criterion -> Draw("SAME");
#endif

  fin.close();

  return 0;
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
