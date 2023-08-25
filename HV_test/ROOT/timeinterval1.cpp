#include <iostream>
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>

using namespace std;

//This code intends to give the time between one hit and another hit just after the one.

void timeinterval1()
{
    TCanvas* c = new TCanvas("c","c",800,600);
    TFile* file = new TFile("./8119_230815-1711_2296_0.root","READ");
    TTree* tree = (TTree*)file->Get("OutTree");

    UInt_t  ID;
    UInt_t channel_nb;
    UInt_t coarsetime;
    UInt_t finetime;

    tree->SetBranchAddress("Evt_ID",&ID);
    tree->SetBranchAddress("channel_nb",&channel_nb);
    tree->SetBranchAddress("Coarse_time",&coarsetime);
    tree->SetBranchAddress("Fine_time",&finetime);

    TH1F* h_time = new TH1F("h_time","h_time", 1000, 0, 200000);
    UInt_t nEv = tree->GetEntries();
    int time;

    std::vector<UInt_t> channelIntervals[128];
    std::vector<UInt_t> evt_ID[128];

    int event_counter[128];
    int interval;
    int event = 0;
    int period_times = 0;


    for (Long64_t i = 0; i < nEv; i++) 
    {
        tree->GetEntry(i);
        time = coarsetime *25 + (1024. - finetime) /1024. * 25.;
        channelIntervals[channel_nb].push_back(time);
        //remember to give the ID a channel_nb, otherwise it will be meaningless
        evt_ID[channel_nb].push_back(ID);
        //cout<<time<<"\t"<<ID<<endl;
    }

    for(int t =0; t<128;t++)
    {
        std::vector<UInt_t>& times = channelIntervals[t];
        std::vector<UInt_t>& evt = evt_ID[t];
        event_counter[t] = times.size();
        //cout<<event_counter[t]<<endl;
        //Because the type of data is UInt, so can not add or subtract directly
            for (size_t k = 1; k < times.size(); k++)
            {
                if(times[k] > times[k - 1])
                {
                    interval = times[k] - times[k - 1];
                    //cout<<interval<<endl;
                }
                else if(times[k] < times[k - 1])
                {
                    //cout<<interval<<endl;
                    interval = 1677721600 + (times[k] - times[k - 1]);
                    period_times++;
                }
                //cout<<interval<<"\t"<<times[k]<<"\t"<<evt_ID[k]<<endl;
                if(evt[k] != evt[k-1])
                {
                h_time->Fill(interval);
                }
                
                //std::cout << "Interval " << i << ": " << interval << std::endl;
            }
    
    }
        h_time->SetXTitle("time_interval(ns)");
        h_time->Draw();

        for(int q = 0; q<128; q++)
            {
                event = event + event_counter[q];
            }
    cout<<"The total event are: "<<event<<endl;
    cout<<"Times of period are: "<<period_times<<endl;
    cout<<"Event rate is: "<<event/(period_times* 1.67772160)<<"Hz"<<endl;
}

int main()
{
    timeinterval1(); 
    
    return 0;
}