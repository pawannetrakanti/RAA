// Raghav Kunnawalkam Elayavalli
// Dec 9th 2014
// Rutgers
// comments: raghav.k.e at CERN dot CH

// Macro which makes plots to check the Jet ID cut contributions for each triggered dataset. 


#include <iostream>
#include <stdio.h>
#include <fstream>
#include <fstream>
#include <TH1F.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TStyle.h>
#include <TStopwatch.h>
#include <TRandom3.h>
#include <TChain.h>
#include <TProfile.h>
#include <TStopwatch.h>
#include <TCut.h>
#include <cstdlib>
#include <cmath>
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"
#include "TLine.h"


#include "Headers/plot.h"
 
static const int nbins_pt = 39;
static const double boundaries_pt[nbins_pt+1] = {
  3, 4, 5, 7, 9, 12, 
  15, 18, 21, 24, 28,
  32, 37, 43, 49, 56,
  64, 74, 84, 97, 114,
  133, 153, 174, 196,
  220, 245, 272, 300, 
  330, 362, 395, 430,
  468, 507, 548, 592,
  638, 686, 1000 
};


static const int nbins_eta = 15;
static const double boundaries_eta[nbins_eta][2] = {
  {-1.0,+1.0}, {-2.0,+2.0}, {-3.0,+3.0},
  {-3.0,-2.5}, {-2.5,-2.0}, {-2.0,-1.5}, 
  {-1.5,-1.0}, {-1.0,-0.5}, {-0.5,0}, {0,+0.5}, 
  {+0.5,+1.0}, {+1.0,+1.5}, {+1.5,+2.0}, 
  {+2.0,+2.5}, {+2.5,+3.0}
};

static const double delta_eta[nbins_eta] = {
  2.0, 4.0, 6.0, 
  0.5, 0.5, 0.5, 
  0.5, 0.5, 0.5, 0.5, 
  0.5, 0.5, 0.5, 
  0.5, 0.5
};

static const char etaWidth [nbins_eta][256] = {
  "n10_eta_p10","n20_eta_p20","n30_eta_p30",
  "n30_eta_n25","n25_eta_n20","n20_eta_n15",
  "n15_eta_n10","n10_eta_n05","n05_eta_0","0_eta_p05",
  "p05_eta_p10","p10_eta_p15","p15_eta_p20",
  "p20_eta_p25","p25_eta_p30"
};

/*
static const int nbins_eta = 2;
static const double boundaries_eta[nbins_eta][2] = {
  {-1.0,+1.0},
  {-2.0,+2.0}
};

static const double delta_eta[nbins_eta] = {
  2.0,4.0
};

static const char etaWidth[nbins_eta][256] = {
  "n10_eta_p10","n20_eta_p20"
};
*/

//static const int no_radius = 2;//testing purposes 
//static const int list_radius[no_radius] = {3,4};

//these are the only radii we are interested for the RAA analysis: 2,3,4,5
static const int no_radius = 7; 
static const int list_radius[no_radius] = {1,2,3,4,5,6,7};
/*
// divide by bin width
void divideBinWidth(TH1 *h)
{
  h->Sumw2();
  for (int i=0;i<=h->GetNbinsX();i++){
    Float_t val = h->GetBinContent(i);
    Float_t valErr = h->GetBinError(i);
    if(val!=0){
      val/=h->GetBinWidth(i);
      valErr/=h->GetBinWidth(i);
      h->SetBinContent(i,val);
      h->SetBinError(i,valErr);
    }  
  }
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
}
*/

static const int TrigValue = 3;
static const int CutValue = 2;
static const char TrigName [TrigValue][256] = {"HLT55","HLT65","HLT80"};
static const char isJetID [CutValue][256] = {"without","with"};


using namespace std;

void RAA_plot_JetID(int radius = 3, char *algo = "Pu", char *jet_type = "PF"){

  TStopwatch timer;
  timer.Start();

  TDatime date;

  gStyle->SetOptStat(0);

  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();
  
  const int nbins_cent = 6;
  double boundaries_cent[nbins_cent+1] = {0,2,4,12,20,28,36};
  double ncoll[nbins_cent+1] = {1660,1310,745,251,62.8,10.8,362.24};

  TFile *fin = TFile::Open("/Users/keraghav/WORK/RAA/Output/RAA_JetID_output.root");
  
  TH1F *hData[3][2][nbins_cent+1];
  TH1F *hData_Ratio[TrigValue][nbins_cent+1];

  for(int i = 0;i<nbins_cent+1;i++){

    for(int a = 0;a<TrigValue;a++){

      for(int b = 0;b<CutValue;b++){

	hData[a][b][i] = (TH1F*)fin->Get(Form("hData_%s_%s_JetID_cent%d",TrigName[a],isJetID[b],i));
	hData[a][b][i] = (TH1F*)hData[a][b][i]->Rebin(nbins_pt,Form("hData_%s_%s_JetID_cent%d",TrigName[a],isJetID[b],i),boundaries_pt);
	divideBinWidth(hData[a][b][i]);
      }

      hData_Ratio[a][i] = (TH1F*)fin->Get(Form("hRatio_ID_over_noIDCut_%s_cent%d",TrigName[a],i));
      hData_Ratio[a][i] = (TH1F*)hData_Ratio[a][i]->Rebin(nbins_pt,Form("hRatio_ID_over_noIDCut_%s_cent%d",TrigName[a],i),boundaries_pt);
      divideBinWidth(hData_Ratio[a][i]);
    }

  }

  // start making the plots: 

  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // TCut Jet55 = "jet55 && l1sj36 && !jet65 && !jet80";
  // TCut Jet65 = "jet65 && l1sj36 && !jet80";
  // TCut Jet80 = "jet80 && l1sj52";
  // TCut elRjc = "eMax/jtpt<0.3";
  // TCut neRjc = "neMax/(chMax+neMax+phMax)<0.9";
  // TCut phRjc = "phMax/(chMax+neMax+phMax)<0.9";
  // TCut chRjc = "chMax/(chMax+neMax+phMax)<0.9";
  // TCut chID  = "chMax/jtpt>0.05";
  // TCut muRjc = "muMax/(chMax+neMax+phMax)<0.9";

  TCanvas *cData_Ratio = new TCanvas("cData_Ratio","",1000,800);
  makeMultiPanelCanvas(cData_Ratio,,3,2,0.0,0.0,0.2,0.15,0.07);

  for(int i = 0;i<nbins_cent;i++){

    cData_Ratio->cd(nbins_cent-i);
    
    makeHistTitle(hData_Ratio[0][i],"","Jet p_{T} (GeV/c)","Jet ID over no Jet ID");
    hData_Ratio[0][i]->SetMarkerStyle(24);
    hData_Ratio[0][i]->SetMarkerColor(kBlack);
    hData_Ratio[0][i]->SetAxisRange(30,500,"X");
    hData_Ratio[0][i]->SetAxisRange(0,1.4,"Y");
    hData_Ratio[0][i]->Draw();

    hData_Ratio[1][i]->SetMarkerStyle(25);
    hData_Ratio[1][i]->SetMarkerColor(kRed);
    hData_Ratio[1][i]->Draw("same");

    hData_Ratio[2][i]->SetMarkerStyle(25);
    hData_Ratio[2][i]->SetMarkerColor(kRed);
    hData_Ratio[2][i]->Draw("same");
        
    drawText(Form("%2.0f-%2.0f%%",2.5*boundaries_cent[i],2.5*boundaries_cent[i+1]),0.75,0.7,20);
  }

  cData_Ratio->cd(1);
  TLegend *LRatio = myLegend(0.15,0.2,0.8,0.45);
  LRatio->AddEntry(hData_Ratio[0][i],"HLT55 && L1SJ36 && !HLT65 && !HLT80","pl");
  LRatio->AddEntry(hData_Ratio[1][i],"HLT65 && L1SJ26 && !HLT80","pl");
  LRatio->AddEntry(hData_Ratio[2][i],"HLT80 && L1SJ52","pl");
  LRatio->SetTextSize(0.04);
  LRatio->Draw();

  putCMSPrel();
  
  cData_Ratio->cd(2);
  drawText("|#eta|<2, |vz|<15",0.2,0.2,16);
  drawText("pCES, HBHE",0.2,0.3,16);
  drawText("Supernova Rejection Cut",0.2,0.4,16);

  cData_Ratio->cd(3);
  drawText("eMax/jtpt<0.3",0.2,0.2,16);
  drawText("neMax/(chMax+neMax+phMax)<0.9",0.2,0.3,16);
  drawText("phMax/(chMax+neMax+phMax)<0.9",0.2,0.4,16);

  cData_Ratio->cd(4);
  drawText("chMax/jtpt>0.05",0.2,0.3,16);
  drawText("muMax/(chMax+neMax+phMax)<0.9",0.2,0.4,16);

  cData_Ratio->SaveAs("PbPb_akPu3PF_Ratio_JetID_cut.pdf","RECREATE");


  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  TCanvas *cData_Spectra[TrigValue];

  for(int a = 0;a<TrigValue;a++){

    cData_Spectra[a] = new TCanvas(Form("cData_Spectra_%d",a),"",1000,800);
    makeMultiPanelCanvas(cData_Spectra[a],,3,2,0.0,0.0,0.2,0.15,0.07);

    for(int i = 0;i<nbins_cent;i++){

      cData_Spectra[a]->cd(nbins_cent-i);
      cData_Spectra[a]->cd(nbins_cent-i)->SetLogy();
    
      makeHistTitle(hData_Spectra[a][0][i],"","Jet p_{T} (GeV/c)","N / #Delta p_{T}");
      hData_Spectra[a][0][i]->SetMarkerStyle(24);
      hData_Spectra[a][0][i]->SetMarkerColor(kBlack);
      hData_Spectra[a][0][i]->SetAxisRange(30,500,"X");
      // hData_Spectra[a][0][i]->SetAxisRange(0,1.4,"Y");
      hData_Spectra[a][0][i]->Draw();
      
      hData_Spectra[a][1][i]->SetMarkerStyle(25);
      hData_Spectra[a][1][i]->SetMarkerColor(kRed);
      hData_Spectra[a][1][i]->Draw("same");
      
      drawText(Form("%2.0f-%2.0f%%",2.5*boundaries_cent[i],2.5*boundaries_cent[i+1]),0.75,0.7,20);
    
    }

    cData_Spectra[a]->cd(1);
    TLegend *LSpectra = myLegend(0.15,0.2,0.8,0.45);
    LSpectra->AddEntry(hData_Spectra[a][0][i],"Without Jet ID Cut","pl");
    LSpectra->AddEntry(hData_Spectra[a][1][i],"With Jet ID Cuts","pl");
    LSpectra->SetTextSize(0.04);
    LSpectra->Draw();

    putCMSPrel();
  
    cData_Spectra[a]->cd(2);
    drawText("|#eta|<2, |vz|<15",0.2,0.2,16);
    drawText("pCES, HBHE",0.2,0.3,16);
    drawText("Supernova Rejection Cut",0.2,0.4,16);

    cData_Spectra[a]->cd(3);
    drawText("eMax/jtpt<0.3",0.2,0.2,16);
    drawText("neMax/(chMax+neMax+phMax)<0.9",0.2,0.3,16);
    drawText("phMax/(chMax+neMax+phMax)<0.9",0.2,0.4,16);

    cData_Spectra[a]->cd(4);
    drawText("chMax/jtpt>0.05",0.2,0.3,16);
    drawText("muMax/(chMax+neMax+phMax)<0.9",0.2,0.4,16);

    cData_Spectra[a]->cd(5);
    if(a==0)drawText("HLT55 && L1SJ36 && !HLT65 && !HLT80",0.2,0.2,16);
    if(a==1)drawText("HLT65 && L1SJ36 && !HLT80",0.2,0.2,16);
    if(a==2)drawText("HLT80 && L1SJ52",0.2,0.2,16);

    cData_Spectra[a]->SaveAs(Form("PbPb_akPu3PF_Jet_%s_Spectra_JetID_cut.pdf",TrigName[a]),"RECREATE");
  }


  //
  timer.Stop();
  cout<<" Total time taken CPU = "<<timer.CpuTime()<<endl;
  cout<<" Total time taken Real = "<<timer.RealTime()<<endl;

}