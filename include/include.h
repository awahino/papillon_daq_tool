#include <TStopwatch.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TMath.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TText.h>

#define N_CH 24
#define N_SAMP 4000
#define N_MAX_LEN (8+N_SAMP*N_CH/2)  // need to check

#define HIST_YMIN 0
#define HIST_YMAX 4000

#define HIST_TMIN 0
#define HIST_TMAX 4000


class display {
  public:
  	display();  //Constructor
  	~display(); //Destructor

  	void ClearData();
  	void ShowPlot();
  	void DoIt();
  private:
    unsigned int* m_data;
    int m_len;
    int m_ch_to_show;     //Ch.
    TCanvas* m_c1;
    TCanvas* m_c2;
    TH1D** m_h_adc;
    TH1D** m_h_noise;
    TText* m_tt;
};


display::display()
: m_data(0), m_len(0), m_ch_to_show(0), m_c1(0), m_c2(0), m_h_adc(0), m_h_noise(0), m_tt(0) {

    //
    m_c2 = new TCanvas("c2","c2",1200,800);
    m_c1 = new TCanvas("c1","c1",1000,500);
    assert(m_c1);
    assert(m_c2);
	
    m_h_adc   = new TH1D* [N_CH];
    m_h_noise = new TH1D* [N_CH];
    for(int i=0; i<N_CH; i++) {
	  m_h_adc[i] = new TH1D(Form("h_adc_ch%d",i), Form("ADC ch%d",i), N_SAMP, 0, N_SAMP);
	  assert(m_h_adc[i]);

	  m_h_noise[i] = new TH1D(Form("h_noise_ch%d",i), Form("Ped ch%d",i), 100, -100, 100);
	  assert(m_h_noise[i]);

	  m_h_adc[i]->SetStats(kFALSE);
    }

    //
    m_tt = new TText();
    m_tt->SetTextSize(0.06);
    m_tt->SetTextColor(kBlack);
    m_tt->SetNDC(1);

    //
    m_data = new unsigned int[N_MAX_LEN];
    assert(m_data);
    m_len  = N_MAX_LEN;
    ClearData();
}


void display::ClearData() {
  for(int i=0; i<m_len; i++) m_data[i] = 0;
}


void display::ShowPlot() {
    // parse
    if( m_data[0]==0x5555ffff ) {
      //32 bit network byte order to host byte order
	  int board_id = ntohl(m_data[1]);
	  int datalen  = ntohl(m_data[2]);
	  int trgcn    = ntohl(m_data[3]);
	  assert(datalen==(4*(m_len-4)));
	  int bpos     = ntohl(m_data[4]); // TBC
	  int bwid     = ntohl(m_data[5]); // TBC
	  int ilk      = ntohl(m_data[6]); // TBC
	  int ilk2     = ntohl(m_data[7]); // TBC
	
	  int nsamp = datalen/N_CH/2;
	  assert(nsamp==N_SAMP);
	
	  std::printf("Trigger = %d, Board id = 0x%08x, len = 0x%08x (%d), bpos = 0x%08x, bwid = 0x%08x, ilk = 0x%08x\n",
		    trgcn, board_id, datalen, datalen, bpos, bwid, ilk );
	
      //ADC data starts from line.9?
	  unsigned short* p = (unsigned short*)(&(m_data[8]));
	  int idx=0;
	  for(int j=0; j<nsamp; j++){
	    for(int ch=0; ch<N_CH; ch++) {
          //16 bit network byte order to host byte order
		  unsigned short adc = ntohs(p[idx]);
		  m_h_adc[ch]->fArray[j+1] = adc;
		  idx++;
      }
	}

	// 
	double mean[N_CH] = {0};
	double rms[N_CH]  = {0};
	for(int ch=0; ch<N_CH; ch++) {
	    mean[ch] = TMath::Mean( 500, &(m_h_adc[ch]->fArray[1]) );
	    rms[ch]  = TMath::RMS( 500, &(m_h_adc[ch]->fArray[1]) );

        //Fill pedestal plots
	    m_h_noise[ch]->Reset();
	    for(int bin=1; bin<=500; bin++) {
		  m_h_noise[ch]->Fill( m_h_adc[ch]->fArray[bin] - mean[ch] );
	    }
	}

	
	//
	//m_c1->Clear();
	m_c1->Divide(2,1);
	m_c1->cd(1);
	m_h_adc[m_ch_to_show]->SetTitle(Form("ADC ch%d, Event# %d",m_ch_to_show,trgcn));
	m_h_adc[m_ch_to_show]->GetXaxis()->SetRangeUser( HIST_TMIN, HIST_TMAX );
	m_h_adc[m_ch_to_show]->GetYaxis()->SetRangeUser( HIST_YMIN, HIST_YMAX );
	m_h_adc[m_ch_to_show]->Draw();

	m_c1->cd(2);
	m_h_noise[m_ch_to_show]->SetTitle(Form("Ped ch%d, Event# %d",m_ch_to_show,trgcn));
	m_h_noise[m_ch_to_show]->Draw();
	m_tt->DrawText(0.2,0.6, Form("rms=%.2f ADC (%.2f mV)", rms[m_ch_to_show], rms[m_ch_to_show]*2000./4095.));
	
	//
	//m_c2->Clear();
	m_c2->Divide(8,8);
	for(int j=0; j<64; j++) {
	    //m_c2->cd(j-32+1);
	    m_c2->cd(j+1);
	    m_h_adc[j]->GetXaxis()->SetRangeUser( HIST_TMIN, HIST_TMAX );
	    m_h_adc[j]->GetYaxis()->SetRangeUser( HIST_YMIN, HIST_YMAX );
	    m_h_adc[j]->Draw();
	}
	
	
	//
	//m_c1->Update();
	//m_c2->Update();
    }
}



void display::DoIt(int ch_to_show) {
    assert((0<=ch_to_show)&&(ch_to_show<N_CH));
    m_ch_to_show = ch_to_show;
    
    //
    std::string line;
    int nct=0;
    while(getline(std::cin,line)) {
	  //std::cout<<line<<std::endl;
	  //continue;

	  unsigned int lnum=0;
	  unsigned int x[4]={0};

	  // assuming the format as "od -H" output
	  std::sscanf(line.c_str(),"%d %x %x %x %x",&lnum,&(x[0]),&(x[1]),&(x[2]),&(x[3]));

	  //std::cout << lnum << " " << x[0] << " " << x[1] << " " << x[2] << " " << x[3] << std::endl;

	  for(int i=0; i<4; i++) {
        //
        if(nct==N_MAX_LEN) {
		  ShowPlot();
		
		  // clear
		  ClearData();
		  nct = 0;
        }
          
        // store
        m_data[nct++] = x[i];
	  }
    }

    if(nct==N_MAX_LEN) ShowPlot();
    
}