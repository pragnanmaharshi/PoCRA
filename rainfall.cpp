#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<cmath>
using namespace std;


// every quatity is in mm 
float MoistureHoldingCapacity; // in other words C
float groundWaterFraction;    // in other words  gamma 
 
class Data{
    public:
        // given variables 
        string date;
        float rain;
        // calculated variables 
        float runoff;  // this is run off due to rain 
        float excess;
        float RUNOFF ; // sum of runoff + excess
        float cropUptake;
        float soilMoisture;
        float percGroundwater; // this is if the water  holding capacity of soil is exceeded 
        // temporary parameters
        float alpha;
        float infiltration;


    // constructor :
    Data(){
        date="";
        rain=0;
        runoff=0;
        excess=0;
        RUNOFF=0;
        soilMoisture=0;
        percGroundwater=0;
        cropUptake=0;
    }
    // methods :
    void setValues(string d, float r){
        date=d;
        rain=r;
    }
    void  CalcVariables(){
        if(rain>=0 && rain <25){alpha=0.2;}
        if(rain>=25 && rain <50){alpha=0.3;}
        if(rain >=50 && rain<75){alpha=0.4;}
        if(rain>=75 && rain <100){alpha=0.5;}
        if(rain>100){alpha=0.7;}
        runoff = alpha * rain;  
        excess=(1-alpha) * rain - MoistureHoldingCapacity;
        if(excess<0){excess=0;}
        RUNOFF=excess + runoff;
        infiltration =rain-RUNOFF;
        
    }

    void display(){
        cout<< "========================================================================"<<endl;
        cout<< "date   " << date  << endl;
        cout<< "rain   "<< rain<<endl;
        cout<< "runoff   "<<runoff<<endl;
        cout<< "excess   "<<excess<<endl;
        cout<<"Runoff   " <<RUNOFF<<endl;
        cout<< "crop uptake   "<<cropUptake<<endl;
        cout<< "soil moisture   "<<soilMoisture <<endl;
        cout<<"ground water   "<<percGroundwater <<endl;
    }
};



void ExceptionHandler(){
    cout<<" Sorry, soil name entered is not in the expected format " <<endl;
    exit;
}


// this function should calculate soil moisture at n th day , ground water and up take of plant 
void  UptakeCalc(vector<Data> & dataVector){
    int n= size(dataVector);
    // which means that soil moisture the day before is assumed to be zero 
    if(n==1){
        if(dataVector[n-1].infiltration>=4){
        dataVector[n-1].cropUptake=4;
        dataVector[n-1].infiltration-=4;
        dataVector[n-1].soilMoisture=dataVector[n-1].infiltration /(1+ groundWaterFraction);
        dataVector[n-1].percGroundwater=dataVector[n-1].soilMoisture * groundWaterFraction;
        }
        else{
            dataVector[n-1].cropUptake=dataVector[n-1].infiltration;
            dataVector[n-1].infiltration=0;
            dataVector[n-1].percGroundwater=dataVector[n-1].soilMoisture=0;
        }
        return;

    }
    else{
        if(dataVector[n-1].infiltration + dataVector[n-2].soilMoisture >=4){
            dataVector[n-1].cropUptake=4;
            
            dataVector[n-1].soilMoisture=(dataVector[n-2].soilMoisture +dataVector[n-1].infiltration -4)/(1+ groundWaterFraction);
            dataVector[n-1].percGroundwater= dataVector[n-1].soilMoisture * groundWaterFraction;
        }
        else{
            dataVector[n-1].cropUptake=dataVector[n-1].infiltration+dataVector[n-2].soilMoisture;
            dataVector[n-1].soilMoisture=dataVector[n-1].percGroundwater=0;

        }
        return;
    }
    return ;
}




int  main(int  argc, char* argv[]){  
    //===========================================================================================================
    // =============================== NORMAL MODE ==============================================================
    //============================================================================================================
    if(argc==1){ 

        string soilName;
        cout<< " there are two kinds of soil inputs  a) deep or Deep   b) shallow or Shallow " <<endl; 
        cout<< "enter the soil kind:    " ;
        cin>> soilName;

        if(soilName== "deep" || soilName=="Deep"){
            MoistureHoldingCapacity=100;
            groundWaterFraction=0.2;
        }
        else if (soilName=="shallow" || soilName=="Shallow"){
            MoistureHoldingCapacity=42;
            groundWaterFraction=0.4; 
        }
        else {
            ExceptionHandler();
        }

        vector< Data > dateAndRain;

        // Reading the given csv file and copying the data into a vector 

        ifstream csvFile;
        csvFile.open("daily_rainfall.csv"); 
        string line="";
        getline(csvFile,line);
        line=""; // clearing the header line 
    
        while(getline(csvFile,line)){

            string Idate; // Idate represent input data 
            float Irain;
            string tempString;
            stringstream inputString(line);

            getline(inputString,Idate,',');
           // for getting a float value first take the value to tempString and the convert it into float value 
           getline(inputString,tempString,',');
           Irain=atof(tempString.c_str());
        
            Data newData;
            newData.setValues(Idate,Irain);
            newData.CalcVariables();
        
           dateAndRain.push_back(newData);
           UptakeCalc(dateAndRain);  // calling the function 
           
           tempString="";
           line="";
        }
   
        ofstream my_file;
        string name;
        name="output_"+ soilName + ".csv";
        my_file.open(name,ios_base::app);
        // header line 
        my_file<< "    DAY " << ", " << "     Rainfall " <<", " << " Runoff + Excess " << " , " << "Crop Uptake " <<",   "<< "Soil Moisture " << ", " << "Percolation to groundwater " <<endl; 
        for(int i=0; i< size(dateAndRain); i++){
            my_file<< dateAndRain[i].date << ",       " << dateAndRain[i].rain <<",              " << dateAndRain[i].RUNOFF << " ,                " << dateAndRain[i].cropUptake<<",          "<< dateAndRain[i].soilMoisture << ",                       " << dateAndRain[i].percGroundwater <<endl;  
        }
        my_file.close();
        // for(auto i:dateAndRain){
        //     i.display();
        // }
    }
    

    // ====================================================================================================
    //=====================================================================================================
    // ========================== SPECIAL MODE ============================================================



    else if( argc>=2){
        cout<< " ============================================================================="<<endl;
        cout<< "Special mode activated "<<endl;
        cout<< "type 1 for displaying contents of vector in terminal " <<endl;
        cout<< "type 2 for running the code multiple times  " <<endl;
        cout<<"type 3 to exit special mode" <<endl;
        int input;
        cin>> input;
        int nDeep=1;
        int nShallow=1;
        vector< Data > dateAndRain;
         string soilName;
                cout<< " there are two kinds of soil inputs  a) deep or Deep   b) shallow or Shallow " <<endl; 
                cout<< "enter the soil kind:    " ;
                cin >> soilName;
                
                if(soilName== "deep" || soilName=="Deep"){
                    MoistureHoldingCapacity=100;
                    groundWaterFraction=0.2;
                }
                else if (soilName=="shallow" || soilName=="Shallow"){
                    MoistureHoldingCapacity=42;
                    groundWaterFraction=0.4; 
                }
                else {
                    ExceptionHandler();
                } 
                dateAndRain.clear(); // to clear all the data from previous iteration 
                ifstream csvFile;
                csvFile.open("daily_rainfall.csv"); 
                string line="";
                getline(csvFile,line);
                line=""; // clearing the header line 
                while(getline(csvFile,line)){
                    string Idate; // Idate represent input data 
                    float Irain;
                    string tempString;
                    stringstream inputString(line);
                    getline(inputString,Idate,',');
                    // for getting a float value first take the value to tempString and the convert it into float value 
                    getline(inputString,tempString,',');
                    Irain=atof(tempString.c_str());
                    Data newData;
                    newData.setValues(Idate,Irain);
                    newData.CalcVariables();
                    dateAndRain.push_back(newData);
                    UptakeCalc(dateAndRain);  // calling the function 
                    tempString="";
                    line="";
                }
        
        while (input!=3){
            if(input==1){
                for(auto  i : dateAndRain){
                    i.display();
                }
            }
            else if(input==2){
               
                ofstream my_file;

                string name;
                if(soilName=="deep" || soilName=="Deep"){
                    name="output_"+ soilName+to_string(nDeep) + ".csv";
                    nDeep+=1;

                }
                else{
                    name="output_"+ soilName+to_string(nShallow) + ".csv";
                    nShallow+=1;
                }
                

                my_file.open(name,ios_base::app);
                // header line 
                my_file<< "    DAY " << ", " << "     Rainfall " <<", " << " Runoff + Excess " << " , " << "Crop Uptake " <<",   "<< "Soil Moisture " << ", " << "Percolation to groundwater " <<endl; 
                for(int i=0; i< size(dateAndRain); i++){
                    my_file<< dateAndRain[i].date << ",       " << dateAndRain[i].rain <<",              " << dateAndRain[i].RUNOFF << " ,                " << dateAndRain[i].cropUptake<<",          "<< dateAndRain[i].soilMoisture << ",                       " << dateAndRain[i].percGroundwater <<endl;  
                }
                my_file.close();
                // increasing the number of times by 1
                
            }
            else{
                cout<<" incorrect input "<<endl;
            }
            
            cout<<"========================================================================"<<endl;
            cout<< "enter new operation" <<endl;
            cout<< "type 1 for displaying contents of vector in terminal " <<endl;
            cout<< "type 2 for running the code multiple times  " <<endl;
            cout<<"type 3 to exit special mode" <<endl;
            cin>> input;
            
        }   
    }
   
    
}
























