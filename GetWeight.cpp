#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;
const int fNofcell = 100;

int radius = 100; // 查询半径为100mm的权重，注意，必须为整数
int cellID = 55; // 查询中心点为 cellID 时，其余各方格的权重

void GetWeight(double datalist[],int cellID, int radius); // 读出数据的函数
vector<string> splitStr(const string& src, const string& delimiter);
int main(){
	double *weightList = new double[fNofcell]; // 根据谁分配谁释放的原则，故应该声明好数组传进去修改
    GetWeight(weightList, cellID,radius); // 从csv读进数组，CellID依次从 00 - 99 （第一位是x，第二位是y）
    for(int i =0;i<fNofcell;i++){
        cout <<"i = " << i << "\t weightList[i] = "<< weightList[i] << endl;
    }
	delete[] weightList; // delete数组用delte[]
}


// dataList[fNofcell] 传入的数组指针
// cellID 待查询中心点的位置
// raidus 待查询的半径大小，单位mm，为int
void GetWeight(double dataList[fNofcell],int cellID, int radius){
    // 通过传入一个数组对象，直接修改这个数组对象
	// 故不需要返回值
    fstream fin;
    vector<string>  dataListstring; //创建一个vector<string>对象
    string filename = "WeightResultList/WeightResult" + to_string(radius)+".csv";
    fin.open(filename);
    string line; //保存读入的每一行
    // double *dataList = new double[fNofcell];
    for(int i=0;i<cellID+1;i++){ // +1 是为了把标题行扔掉
        getline(fin,line); //会自动把\n换行符去掉，将不要的数据读进tmp
    }
    getline(fin,line);
	fin.close();
    dataListstring = splitStr(line,"\t"); // 字符串分割
    // cout << "len of dataListstring = " << dataListstring.size() << endl;
    for(int i=0+2;i<fNofcell+2;i++){
        dataList[i-2] = stod(dataListstring[i]);
		// cout <<"dataListstring["<<i<<"] = " << dataListstring[i]
		// 	 <<"\tstod(dataListstring["<<i<<"]) = "<<stod(dataListstring[i])
		// 	 <<"\tdataList["<<i-2<<"] = " << dataList[i-2] << endl; 
    }    
	
}

//@brief: 指定单个分隔符（单个字符或子串）分割字符串
//@param: src 原字符串；delimiter 分隔符，单个字符或子串
vector<string> splitStr(const string& src, const string& delimiter) {
	std::vector<string> vetStr;
	
	// 入参检查
	// 1.原字符串为空或等于分隔符，返回空 vector
	if (src == "" || src == delimiter) {
		return vetStr;
	}
	// 2.分隔符为空返回单个元素为原字符串的 vector
	if (delimiter == "") {
		vetStr.push_back(src);
		return vetStr;
	}

	string::size_type startPos = 0;
	auto index = src.find(delimiter);
	while (index != string::npos) {
		auto str = src.substr(startPos, index - startPos);
		if (str != "") {
			vetStr.push_back(str);
		}
		startPos = index + delimiter.length();
		index = src.find(delimiter, startPos);
	}
	// 取最后一个子串
	auto str = src.substr(startPos);
	if (str != "") {
		vetStr.push_back(str);
	}

	return vetStr;
}