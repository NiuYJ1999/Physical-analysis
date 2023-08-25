#include<iostream>
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TClass.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>
using namespace std;

//This code intends to give the different channel's charge
//For use, the code gives the first group for 16 channels in default
//If you want to give other channel, please modify the k(int k = ?,  k<?)

void charge() {
    TCanvas* c = new TCanvas("c", "c", 800, 800);
    TFile* file = new TFile("./8119_230815-1711_2296_0.root","READ");
    TTree* tree = (TTree*)file->Get("OutTree");

    UInt_t  ID;
    UInt_t channel_nb;
    UInt_t charge;

    tree->SetBranchAddress("Evt_ID",&ID);
    tree->SetBranchAddress("channel_nb",&channel_nb);
    tree->SetBranchAddress("Charge",&charge);

    //c->Divide(4,4);
    c->Divide(1,2);

    int nBins = 800;
    double xMin = 0.0;
    double xMax = 800.0;
    int nHistograms = 128;

    std::vector<TH1F*> histograms;

    TF1 *fitFunc = new TF1("fitFunc", "gaus");
    TH1F* h_mean = new TH1F("mean of charge", "mean of charge", 100, 80, 180);
    TH1F* h_sigma = new TH1F("sigma of charge", "sigma of charge", 100, 0, 50);
    double mean = 0.0;
    double rms = 0.0;

 //this code include a method of giving a vector of histograms!!!!!       
    for (int i = 0; i < nHistograms; i++) {
        TString histName = Form("h_c_%d", i);

        TH1F* hist = new TH1F(histName, "", nBins, xMin, xMax);

        histograms.push_back(hist);
    }

    for(Long64_t j = 0; j < tree->GetEntries(); j++)
    {
        tree->GetEntry(j);
        for(int k = 0; k < 128; k++)
        {
            if(channel_nb == k)
            {
            histograms[k]->Fill(charge);
            }
        }
    }
    for(int a = 0; a< 128; a++)
    {
            c->cd(a+1);
            //give the histograms title
            histograms[a]->SetTitle(Form("channel %d", a+1));
            histograms[a]->Fit(fitFunc, "R", "", 50, 210);
            mean = fitFunc->GetParameter(1);
            rms = fitFunc->GetParameter(2);
            h_mean->Fill(mean);
            h_sigma->Fill(rms);
            //histograms[a]->Draw();
    }
    c->cd(1);
    h_mean->Draw();
    c->cd(2);
    h_sigma->Draw();
}
