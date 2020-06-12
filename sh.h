#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>

using namespace std;

#define MAX_THREADS std::thread::hardware_concurrency() - 1;
atomic<bool> key_tr;
condition_variable triger;
mutex mtx;
vector<int> key;
vector<int> test_key_size;
vector<bool> test_key_size_ch;
vector<string> alph;
vector<string> encr;

void swap(int* a, int i, int j)
{
	
	int s = a[i];
	a[i] = a[j];
	a[j] = s;
}
bool NextSet(int* a, int n)
{
	int j = n - 2;
	while (j != -1 && a[j] >= a[j + 1]) j--;
	if (j == -1)
		return false; 
	int k = n - 1;
	while (a[j] >= a[k]) k--;
	swap(a, j, k);
	int l = j + 1, r = n - 1; 
	while (l < r)
		swap(a, l++, r--);
	return true;
}

bool check(int* a, int size) {
	vector<string> unencr;
	for (int i = 0; i < encr.size(); i++) {
		string norm_elem;
		for (int j = 0; j < encr[i].size() / size; j++) {
			for (int k = 0; k < size; k++) {
				int el = j * size;
				int pos;
				for (int kk = 0; kk < size; kk++) {
					if (k + 1 == a[kk]) {
						pos = kk;
					}
				}
				norm_elem.push_back(encr[i][el + pos]);
			}
		}
		unencr.push_back(norm_elem);
		/*string word;
		bool prob = false;
		for (int j = 0; j < norm_elem.size(); j++) {
			if (norm_elem[j] == ' ' || norm_elem[j] == '#') {
				if (prob) {
					break;
				}
			}
			else {
				if (!prob) {
					prob = true;
				}
				word.push_back(norm_elem[j]);
			}
		}
		for (int i = 0; i < alph.size(); i++) {
			if (word == alph[i]) {
				return true;
			}
		}
		return false;*/
	}
	int corect = 0;
	int uncorect = 0;
	for (int i = 0; i < unencr.size(); i++) {
		bool end = true;
		string word;
		for (int j = 0; j < unencr[i].size(); j++) {
			if (unencr[i][j] == ' ' || unencr[i][j] == '#') {
				if (end) {
				}
				else {
					bool w_cor = false;
					for (int k = 0; k < alph.size(); k++) {
						if (word == alph[k]) {
							w_cor = true;
						}
					}
					if (w_cor) {
						corect++;
					}
					else {
						uncorect++;
					}
					int ss = word.size();
					for (int k = 0; k < ss; k++) {
						word.pop_back();
					}
					end = true;
				}			
			}
			else {
				if (end) {
					end = false;
				}
				word.push_back(unencr[i][j]);
			}
		}
	}
	/*if (corect > uncorect) {
		return true;
	}
	else if (corect = uncorect && corect != 0) {
		return true;
	}
	else {
		return false;
	}*/
	if (corect > uncorect && uncorect == 0) {
		return true;
	}
	return false;
}

void func(int size) {
//635412
	
	bool tr = false;
	int* arr = new int[size];
	for (int i = 0; i < size; i++) {
		arr[i] = i + 1;
	}
	
	if (check(arr,size)) {		
		tr = true;
	}
	else {
		while (NextSet(arr, size)) {
			if (check(arr, size)) {
				tr = true;
				break;
			}
		}
	}
	if (tr) {
		if (key_tr) {
			mtx.lock();
			for (int i = 0; i < size; i++) {
				key.push_back(arr[i]);
			}
			key_tr = false;
			mtx.unlock();
			triger.notify_one();
		}
	}



}

class Th_pool {
private:
	vector<thread> Pool;
	mutex taskMutex;
	void thread_while() {
		bool ex = true;
		while (ex) {
			int el = -1;
			for (int i = 0; i < test_key_size_ch.size(); i++) {
				if (test_key_size_ch[i] == 0) {
					el = i;
					taskMutex.lock();
					test_key_size_ch[i] = 1;
					taskMutex.unlock();
					break;
				}
			}
			if (el == -1) {
				ex = false;
			}
			else {
				func(test_key_size[el]);
			}


		}
	
	
	}
public:
	void push() {
		int tmp = MAX_THREADS;
		for (int i = 0; i < tmp; i++) {
			Pool.push_back(thread(&Th_pool::thread_while, this));
			Pool.back().detach();
		}
	}
};

void test() {
	setlocale(LC_CTYPE, "Russian");
	key_tr = true;
	string path_alp;
	string path;
	cout << "Введите путь к алфавиту: "; cin >> path_alp; cout << endl;
	cout << "Введите путь к зашифрованому файлу: "; cin >> path; cout << endl;
	//vector<string> alph;
	ifstream alph_txt(path_alp);
	while (alph_txt)
	{
		string elem;
		getline(alph_txt, elem);
		if (elem.size() == 1 && elem[0] == ' ') {		}
		else if (elem.size() == 0) {}
		else {alph.push_back(elem);}
	}
	alph_txt.close();
	//vector<string> encr;
	ifstream encr_txt(path);
	while (encr_txt)
	{
		string elem;
		getline(encr_txt, elem);
		if (elem.size() == 1 && elem[0] == ' ') {}
		else if (elem.size() == 0) {}
		else { encr.push_back(elem); }
	}
	encr_txt.close();
	if (encr.size() != 0) {
		//vector<int> test_key_size;		
		for (int i = 1; i < encr[0].size()+1; i++) {
			if (i == 1) {
			}
			else {
				int r = encr[0].size() % i;
				if (r == 0) {
					test_key_size.push_back(i);
					test_key_size_ch.push_back(0);
				}
			}
		}
		int tmp = MAX_THREADS;
		//string en_el;
		//if (encr[0].size() == 0 || encr[0].size() == 1) {
		//	en_el = encr[1];
		//}
		//else {
		//	en_el = encr[0];
		//}
		Th_pool pool;
		
		pool.push();			
		
		/*if (test_key_size.size() <= tmp) {
			
			for (int i = 0; i < test_key_size.size(); i++) {
				thread th(func, test_key_size[i], alph, en_el);
				th.detach();
			}
		}
		else if (test_key_size.size() > tmp) {
			thread th1(func, test_key_size[0], alph, en_el);
			thread th2(func, test_key_size[1], alph, en_el);
			thread th3(func, test_key_size[2], alph, en_el);
			thread th4(func, test_key_size[3], alph, en_el);
			thread th5(func, test_key_size[4], alph, en_el);
			th1.join();
			th2.join();
			th3.join();
			th4.join();
			th5.join();
			for (int i = 5; i < test_key_size.size(); i++) {
				thread th(func, test_key_size[i], alph, en_el);
				th.detach();
			}
		}*/
		
		unique_lock<mutex> lock(mtx);
		triger.wait(lock);
		vector<int> key_r = key;
		string path_s;
		cout << "Введите путь где нужно сохранить файл: "; cin >> path_s; cout << endl;
		ofstream save_txt(path_s);
		string st_k;
		for (int i = 0; i < key_r.size(); i++) {
			st_k.push_back(to_string(key_r[i])[0]);
		}
		save_txt << "Ключ: " << st_k << endl;
		for (int i = 0; i < encr.size(); i++) {
			string norm_elem;
			for (int j = 0; j < encr[i].size()/key_r.size(); j++) {
				for (int k = 0; k < key_r.size(); k++) {
					int el = j * key_r.size();
					int pos;
					for (int kk = 0; kk < key_r.size(); kk++) {
						if (k+1 == key_r[kk]) {
							pos = kk;
						}
					}
					
					norm_elem.push_back(encr[i][el+pos]);

				}
			}
			save_txt << norm_elem << endl;
		}
		save_txt.close();
		cout << "Файл расшифрован \n";
	}
	else {
		cout << "Зашифрованный файл пустой!! \n";
	}
}


