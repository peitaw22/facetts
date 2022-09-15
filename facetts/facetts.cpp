#include<iostream>
#include<algorithm>
#include<cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include<map>
#include<time.h>
#include<random>
#include<unordered_map>
#include<bitset>
#include<random>
#include<set>
#include<chrono>
#include "RTree.h"



using namespace std;

//parameter
const int dimensions =5;



typedef int ValueType;
typedef RTree<ValueType, double, dimensions, double> MyTree;

//group
struct Group{
    /* data */
    double max_norm;
    vector<int> id_vec;
    MyTree tree;
};





vector<string> split(string& input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    return result;
}


double input_threshold(){
    ifstream ifs("./parameter/threshold.txt");//dataset path
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }
    string line;
    double threshold;
    while (getline(ifs, line)) {
        vector<string> strvec = split(line,' ');
        threshold = stod(strvec[0]);
    }
    return threshold;
}

int input_sampling_number(){
    ifstream ifs("./parameter/sampling_number.txt");//dataset path
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }
    string line;
    int sampling_num;
    while (getline(ifs, line)) {
        vector<string> strvec = split(line,' ');
        sampling_num = stoi(strvec[0]);
    }
    return sampling_num;
}


void input(vector<vector<double> > &data_p, vector<vector<double> > &data_q){
    ifstream ifs("./dataset/netflix_mf-50.txt");//dataset path
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }

    string line;
    int i=0;
    int j=0;
    while (getline(ifs, line)) {
        vector<string> strvec = split(line,' ');
        if(strvec[1]=="F"){
            continue;
        }
        string tmp_str = strvec[0];
        if(tmp_str[0] == 'p'){
            strvec.erase(strvec.begin());
            strvec.erase(strvec.begin());
            vector<double> tmp;
            for(j=0;j<strvec.size();j++){
                double d= stod(strvec.at(j));
                tmp.push_back(d);
            }
            data_p.push_back(tmp);
        }

        if(tmp_str[0] == 'q'){
            strvec.erase(strvec.begin());
            strvec.erase(strvec.begin());
            vector<double> tmp;
            for(j=0;j<strvec.size();j++){
                double d= stod(strvec.at(j));
                tmp.push_back(d);
            }
            data_q.push_back(tmp);
        }        
    }

}


double normfun(vector<double> &vec){//compute norm
    double norm_tmp=0;
    for(int i=0;i<vec.size();i++){
        norm_tmp=norm_tmp + vec[i]*vec[i];
    }
    double norm =sqrt(norm_tmp);
    return norm;
}

double ip_fun(vector<double> &a, vector<double> &b){//compute inner product
    double ip =0;
    for(int i=0; i< a.size(); i++){
        ip = ip + a[i]*b[i];
    }
    return ip;
}


void to_gruoup(vector<Group> &vec_g, vector<vector<double> > &data_p_tran, int &number){//divide into groups. transform data
    vector<pair<double, int > > pairs;
    int n= data_p_tran.size();
    for(int i=0; i< n ;i++){
        double tmp = normfun(data_p_tran[i]);
        pair<double, int > tmp_pair(tmp, i);
        pairs.push_back(tmp_pair);
    }
    sort(pairs.rbegin(), pairs.rend());
    double MAX_norm = pairs[0].first;
    for(int i=0; i<n; i = i + number){
        Group tmp_g;
        tmp_g.max_norm=pairs[i].first;
        for(int j=i; j<i+number && j<n ;j++){
            int id = pairs[j].second;
            tmp_g.id_vec.push_back(id);
            data_p_tran[id].push_back( sqrt( MAX_norm * MAX_norm  - pairs[j].first * pairs[j].first ));
        }
        vec_g.push_back(tmp_g);
    }   
}



void make_random(vector<vector<double > > &random, int a){
    mt19937 mt(4);
    normal_distribution<> dist(0.0, 1.0);
    for(int i=0; i < dimensions; i++){
        vector<double> tmp;
        for(int j=0; j< a; j++){
            tmp.push_back(dist(mt));
        }
        random.push_back(tmp);
    }
}

void project (vector<double> &vec_new , vector<double> &vec_old ,vector<vector<double> > &random ){
    for(int i=0; i < random.size(); i++){
        vec_new.push_back( ip_fun(vec_old, random[i]) );
    }
}



void Project (vector<vector<double> > &data_p_tran, vector<vector<double> > &data_p_proj, vector<vector<double> > &random){//project data
    for(int i=0; i < data_p_tran.size(); i++){
        vector<double> tmp;
        project(tmp, data_p_tran[i], random);
        data_p_proj.push_back(tmp);
    }
}


void G_tree_Insert(vector<Group> &vec_g, vector<vector<double> > &data_p_proj){

    for(int i=0; i< vec_g.size(); i++){
        if(i == 0){
            for(int j=0; j< vec_g[i].id_vec.size(); j++){
                double tmp[dimensions];
                int id = vec_g[i].id_vec[j];
                for(int k=0; k < dimensions; k++){
                    tmp[k] = data_p_proj[id][k];
                }
                vec_g[i].tree.Insert(tmp,tmp, id);
            }

        }else{
            MyTree tree_tmp = vec_g[i-1].tree;
            vec_g[i].tree = tree_tmp;
            for(int j=0; j< vec_g[i].id_vec.size(); j++){
                double tmp[dimensions];
                int id = vec_g[i].id_vec[j];
                for(int k=0; k < dimensions; k++){
                    tmp[k] = data_p_proj[id][k];
                }
                vec_g[i].tree.Insert(tmp,tmp, id);
            }
        }
    
    }
}



int Cauchy(vector<double> &g_norm, double &q_norm, double &threshold){//cauchy-schwartz inequality
    int i=0;
    for( i=0; i < g_norm.size(); i++){
        if(q_norm * g_norm[i] < threshold){
            break;
        }
    }
    return i;
}


void retreval(Group &group, vector<double> &q_proj, double &range, vector< MyTree::Node* >& leaf_node){
    double min[dimensions];
    for(int i=0; i< dimensions; i++){
        
        min[i] = q_proj[i];
    }
    group.tree.Search_cir_leaf(range, min, leaf_node);
    
}


double sampling( vector<double> &query, vector<vector<double> > &data_p, vector< MyTree::Node* >& leaf_node, double &threshold, int &sampling_num){
    int num = leaf_node.size();
    if( num ==0){
        cout<<" not sampling" <<endl;
        
        return 0;
    }

    mt19937 mt(1);
    vector<int> probabilities;
    int total =0;

    for(int i=0; i < num ; i++){
        int tmp = leaf_node[i]->m_count;
        probabilities.push_back(tmp);
        total = total + tmp;
    }

    discrete_distribution<std::size_t> dist(probabilities.begin(),probabilities.end() );
    int hit = 0;
    for(int i=0; i < sampling_num; i++){
        int tmp = dist(mt);
        uniform_int_distribution<>rand(0, probabilities[tmp]-1);
        int id = leaf_node[tmp]->m_branch[rand(mt)].m_data;
        if( ip_fun(query, data_p[id]) >= threshold){
            hit++;
        }
    }
    double estimation = (double) total * (double) hit / sampling_num;
    return estimation;
}

//input exact
void exa_in(vector<vector<int> > &exa_vec){
    ifstream ifs("./exact/netflix_thr4.csv");
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }
    string line;
    int i=0;
    int j=0;
    while (getline(ifs, line)) {
        vector<string> strvec = split(line,',');
        vector<int> tmp;

       for(i=0; i<strvec.size(); i++){
           int d=  stoi(strvec.at(i));
           tmp.push_back(d);
       }
       exa_vec.push_back(tmp);
   
    }    
}


int main(){
    double threshold = input_threshold();
    int sampling_num = input_sampling_number();
    cout << "threshold " << threshold << endl;
    cout << "sampling number " << sampling_num << endl;


    vector<vector<double> > data_p;
    vector<vector<double> > data_q;

    input(data_p, data_q);//input data
    cout <<"number of users is "<< data_p.size()<<endl;


    int number = data_p.size() / log2(data_p.size()); //number of data in a group
    //int number = data_p.size(); //number of data in a group


    vector<vector<double> > data_p_tran = data_p;

    vector<Group> vec_g;
    to_gruoup(vec_g, data_p_tran, number);//divide into groups. transform data

    vector<vector<double > > random ;
    make_random(random, data_p_tran[0].size() );//make random vectors

    vector<vector<double> > data_p_proj;
    Project(data_p_tran, data_p_proj, random);//project data

    G_tree_Insert(vec_g, data_p_proj);//insert projected data into a tree



    vector<double> g_norm;//max norm in each group
    for(int i=0; i < vec_g.size();i++){
        g_norm.push_back(vec_g[i].max_norm);
    }
    
    
    vector<vector<int> > exact_vec;
    exa_in(exact_vec);//input exact 
    


    vector<vector<double> > record;
    for(int i = 0; i < exact_vec.size(); i++){


        int q_id = exact_vec[i][0];
        vector<double> query = data_q[ q_id ];
           
        
        
        int exact = exact_vec[i][1];
        if(exact ==0){
            continue;
        }

        double MAX_norm = g_norm[0];
        vector< MyTree::Node* > leaf_point;




        chrono::system_clock::time_point  start, end, start1, end1;
        start = chrono::system_clock::now();//start measure
        double q_norm = normfun(query);//search range
        query.push_back(0);

        double tmp = MAX_norm * MAX_norm + q_norm * q_norm -( 2 * threshold );
        if(tmp <0){
            continue;
        }
        double thre_new = sqrt( tmp );//threashold about distance
        double range = sqrt(dimensions) * thre_new ;
        vector<double> q_proj;
        project(q_proj, query, random);

        const int group_num = Cauchy(g_norm, q_norm, threshold);//number of groups considered
        if(group_num == 0 ){
            continue;
        }

    
        retreval(vec_g[group_num - 1], q_proj, range, leaf_point);//retreval leaf nodes
        query.pop_back();
        double estimation = sampling( query, data_p, leaf_point, threshold, sampling_num);
        end= chrono::system_clock::now();//finish measure



        auto time = end - start;
        double msec = std::chrono::duration_cast<chrono::microseconds>(time).count();
        msec =(double)msec/1000;

        vector<double> record_tmp;
        record_tmp.push_back(q_id);
        record_tmp.push_back(exact);
        record_tmp.push_back(estimation);
        record_tmp.push_back( (double)abs( exact - estimation)/exact);
        record_tmp.push_back(msec);

        
        record.push_back(record_tmp);
    }

    string output_path = "netflix_threshold" + to_string(threshold) + "samplings" + to_string(sampling_num) + ".csv";
    ofstream file(output_path);//output 
    for(int i=0; i< record.size();i++){
        for(int j=0; j<record[i].size();j++){
            file<< record[i][j];
            file<<',';
        }
        file <<'\n';
    }
    file.close();
    cout<<"finish"<<endl;

    return 0;
}

