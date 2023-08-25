#include <iostream>
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include "TLegend.h"

using namespace std;

TH1F* fun_gain(const char* A, const char* B)
{
    TCanvas* c = new TCanvas("c","c",800,600);
    TFile* file1 = new TFile(A,"READ");
    TFile* file2 = new TFile(B,"READ");
    TTree* tree_pedestal = (TTree*)file1->Get("OutTree");
    TTree* tree_physical = (TTree*)file2->Get("OutTree");

    UInt_t pedestal_ID;
    UInt_t pedestal_channel_nb;
    UInt_t pedestal_charge;

    UInt_t physical_ID;
    UInt_t physical_channel_nb;
    UInt_t physical_charge;

    tree_pedestal->SetBranchAddress("Evt_ID",&pedestal_ID);
    tree_pedestal->SetBranchAddress("channel_nb",&pedestal_channel_nb);
    tree_pedestal->SetBranchAddress("Charge",&pedestal_charge);

    tree_physical->SetBranchAddress("Evt_ID",&physical_ID);
    tree_physical->SetBranchAddress("channel_nb",&physical_channel_nb);
    tree_physical->SetBranchAddress("Charge",&physical_charge);

    int nBins = 800;
    double xMin = 0.0;
    double xMax = 800.0;
    int nHistograms = 128;

    std::vector<TH1F*> histograms_ped;
    std::vector<TH1F*> histograms_phy;

    TF1 *fitFunc1 = new TF1("fitFunc1", "gaus");
    TF1 *fitFunc2 = new TF1("fitFunc2", "gaus");

    double mean_ped = 0.0;
    double mean_phy = 0.0;
    double rms_ped = 0.0;
    double rms_phy = 0.0;

    TH1F* h_gain = new TH1F("gain", "gain", 100, 30, 100);

    for (int i = 0; i < nHistograms; i++) 
    {
        TString histName = Form("h_ped_%d", i);
        TH1F* hist = new TH1F(histName, "", nBins, xMin, xMax);
        histograms_ped.push_back(hist);

        TString histName1 = Form("h_phy_%d", i);
        TH1F* hist1 = new TH1F(histName1, "", nBins, xMin, xMax);
        histograms_phy.push_back(hist1);
    }
        cout<<"Histograms have created!"<<endl;


        for(Long64_t j = 0; j < tree_pedestal->GetEntries(); j++)
        {
            tree_pedestal->GetEntry(j);
            for(int k = 0; k < 128; k++)
            {
                if(pedestal_channel_nb == k)
                {
                histograms_ped[k]->Fill(pedestal_charge);
                }
            }
        }
        cout<<"Pedestal charge gets!"<<endl;
        for(Long64_t a = 0; a < tree_physical->GetEntries(); a++)
        {
            tree_physical->GetEntry(a);
            for(int x = 0; x < 128; x++)
            {
                if(pedestal_channel_nb == x)
                {
                histograms_phy[x]->Fill(physical_charge);
                }
            }
        }
        //histograms_phy[0]->Draw();
        cout<<"Physical charge get!"<<endl;

    for(int d = 0; d< 128; d++)
    {
            histograms_ped[d]->Fit(fitFunc1, "R", "", 45, 100);
            mean_ped = fitFunc1->GetParameter(1);
            rms_ped = fitFunc1->GetParameter(2);
            cout<<"mean_ped: "<<mean_ped<<endl;

            histograms_phy[d]->Fit(fitFunc2, "R", "", 80, 210);
            mean_phy = fitFunc2->GetParameter(1);
            rms_phy = fitFunc2->GetParameter(2);
            cout<<"mean_phy: "<<mean_phy<<endl;

            double gain = (mean_phy*1.0 - mean_ped*1.0) * 1.0;
            h_gain->Fill(gain);
    }
    return h_gain;
}

int gain()
{
    //I have make fun_gain a function, if you want to draw more gain, please use this!
    const char* A = "8119_230815-1616_2291_0.root";
    const char* B = "8119_230815-1632_2292_0.root";
    const char* C = "8119_230815-1658_2294_0.root";
    const char* D = "8119_230815-1711_2296_0.root";

    TH1F* HV_normal = fun_gain(A, B);
    TH1F* HV_p50 = fun_gain(A, C);
    TH1F* HV_s50 = fun_gain(A, D);

    HV_normal->Draw();
    HV_normal->SetStats(false);
    HV_p50->SetLineColor(kRed);
    HV_p50->Draw("same");
    HV_s50->SetLineColor(kGreen);
    HV_s50->Draw("same");

    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9); 
    
    legend->AddEntry(HV_normal, "HV_normal", "l");
    legend->AddEntry(HV_p50, "HV_p50", "l");
    legend->AddEntry(HV_s50, "HV_s50", "l");
    
    legend->Draw("same");
    return 0;
}