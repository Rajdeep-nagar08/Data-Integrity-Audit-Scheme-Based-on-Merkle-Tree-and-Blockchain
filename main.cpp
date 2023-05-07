#include <bits/stdc++.h>

// #include "C:/Program Files/OpenSSL-Win64/include/openssl/sha.h"

//   #include<openssl/sha.h>

using namespace std::chrono;

using namespace std;

int p = 19260817;

#define d 26

// std::string sha256(const std::string str){
//   unsigned char hash[SHA256_DIGEST_LENGTH];

//   SHA256_CTX sha256;
//   SHA256_Init(&sha256);
//   SHA256_Update(&sha256, str.c_str(), str.size());
//   SHA256_Final(hash, &sha256);

//   std::stringstream ss;

//   for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
//     ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( hash[i] );
//   }
//   return ss.str();
// }

string findHash(string text)
{
    int lt = text.size();

    vector<int> powr(lt, 1);

    for (int i = 1; i < lt; i++)
        powr[i] = (powr[i - 1] * 26) % p;

    int i, j, k;
    int hp = 0;
    int hw = 0;

    for (i = 0; i < lt; i++)
    {
        hw = ((hw * d) % p + (text[i] - 'a')) % p;
    }

    string hash = to_string(hw);

    for (char &ch : hash)
    {
        ch = (ch - '0' + 'a');
    }

    return hash;
}


void insertSMT(string &t, vector<string> &trans, unordered_map<int, vector<string>> &mp, string &root)
{
    trans.push_back(t);
    int size = trans.size();
    string hash = findHash(t);
    int levels = ceil(log2(size)) + 1;
    mp[1].push_back(hash);
    int i = 2;
    while (i <= levels)
    {
        int prevSz = mp[i - 1].size();
        if (prevSz % 2 == 1)
        { // odd hashes at prev level, no merging keep going as it is (similar to last hash of previous level)
            int currSz = mp[i].size();
            int neededSz = (prevSz / 2) + 1;
            if (currSz == neededSz)
            {
                mp[i][mp[i].size() - 1] = mp[i - 1][prevSz - 1];
            }
            else
            {
                mp[i].push_back(mp[i - 1][prevSz - 1]);
            }
        }
        else
        { // even hashes at prev level, merge last two hashes of prev level and replace that with the last hash of this level
            string h1 = mp[i - 1][prevSz - 2];
            string h2 = mp[i - 1][prevSz - 1];
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            string newHash = findHash(h2 + h1);
            if (mp[i].size() > 0)
                mp[i][mp[i].size() - 1] = newHash;
            else
                mp[i].push_back(newHash);
        }
        i++;
    }
    if (mp[levels].size() == 1)
    {
        root = mp[levels][0];
    }
    else
    {
        string h1 = mp[levels][0];
        string h2 = mp[levels][1];
        reverse(h1.begin(), h1.end());
        reverse(h2.begin(), h2.end());
        string newHash = findHash(h2 + h1);
        root = newHash;
        mp[levels + 1].push_back(root);
    }
    return;
}







//  verifySMT(t,pos,trans,mp,root);

bool verifySMT(string t, int pos, vector<string> &trans, unordered_map<int, vector<string>> mp, string &root)
{
    string hash = findHash(t);
    int size = trans.size();
    int levels = ceil(log2(size)) + 1;
    int i = 2;
    while (i <= levels)
    {
        int prevSz = mp[i - 1].size();
        int prevPos = pos;
        if (prevPos % 2 == 0)
        {
            string h1 = mp[i - 1][prevPos - 2];
            string h2 = hash;
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            hash = findHash(h2 + h1);
            pos = prevPos / 2;
        }
        else
        {
            if (prevPos == prevSz)
            {
                pos = (prevPos / 2) + 1;
            }
            else
            {
                string h1 = hash;
                string h2 = mp[i - 1][prevPos];
                reverse(h1.begin(), h1.end());
                reverse(h2.begin(), h2.end());
                hash = findHash(h2 + h1);
                pos = (prevPos + 1) / 2;
            }
        }
        i++;
    }

    if (hash == root)
    {
        return true;
    }
    return false;
}



void insertQuad(string &t, vector<string> &trans, unordered_map<int, vector<string>> &mp, string &root)
{
    trans.push_back(t);
    int size = trans.size();
    string hash = findHash(t);
    int levels = ((ceil(log2(size) / 2)) + 1);
    mp[1].push_back(hash);
    int i = 2;
    while (i <= levels)
    {
        int prevSz = mp[i - 1].size();
        int currSz = mp[i].size();
        int neededSz = (prevSz / 4) + (prevSz % 4 == 0 ? 0 : 1);
        string newVal = "";
        vector<string> v = mp[i - 1];
        int k = v.size() / 4 + (v.size() % 4 == 0 ? -1 : 0);
        int idx = k * 4;
        int cnt = 0;
        for (int i = v.size() - 1; i >= idx; i--)
        {
            string s = v[i];
            reverse(s.begin(), s.end());
            newVal += s;
            cnt++;
        }
        string newHash;
        if (cnt == 1)
            newHash = hash;
        else
            newHash = findHash(newVal);
        if (currSz == neededSz) // replacement by newHash
            mp[i][mp[i].size() - 1] = newHash;
        else
        { // push new Hash
            mp[i].push_back(newHash);
        }
        i++;
    }
    // making new root
    if (mp[levels].size() == 1)
    {
        root = mp[levels][0];
    }
    else // level increase
    {
        string h1 = mp[levels][0];
        string h2 = mp[levels][1];
        reverse(h1.begin(), h1.end());
        reverse(h2.begin(), h2.end());
        string newHash = findHash(h2 + h1);
        root = newHash;
        mp[levels + 1].push_back(root);
    }
    return;
}

bool verifyQuad(string t, int pos, vector<string> &trans, unordered_map<int, vector<string>> mp, string &root)
{
    string hash = findHash(t);
    int size = trans.size();
    int levels = ((ceil(log2(size) / 2)) + 1);
    int i = 2;
    while (i <= levels)
    {
        int prevSz = mp[i - 1].size();
        int prevPos = pos;
        if (prevPos % 4 == 1) // 1st node of the group
        {
            string h1 = hash;
            string h2 = "", h3 = "", h4 = "";
            if (prevPos + 1 <= mp[i - 1].size())
                h2 = mp[i - 1][prevPos];
            if (prevPos + 2 <= mp[i - 1].size())
                h3 = mp[i - 1][prevPos + 1];
            if (prevPos + 3 <= mp[i - 1].size())
                h4 = mp[i - 1][prevPos + 2];
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            reverse(h3.begin(), h3.end());
            reverse(h4.begin(), h4.end());
            string str = "";
            str += h4;
            str += h3;
            str += h2;
            str += h1;
            hash = findHash(str);
            pos = (prevPos / 4) + (prevPos % 4 != 0 ? 1 : 0);
        }
        else if (prevPos % 4 == 2)
        {
            string h1 = mp[i - 1][prevPos - 2];
            string h2 = hash;
            string h3 = "";
            string h4 = "";
            if (prevPos + 1 <= mp[i - 1].size())
                h3 = mp[i - 1][prevPos];
            if (prevPos + 2 <= mp[i - 1].size())
                h4 = mp[i - 1][prevPos + 1];
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            reverse(h3.begin(), h3.end());
            reverse(h4.begin(), h4.end());
            string str = "";
            str += h4;
            str += h3;
            str += h2;
            str += h1;
            hash = findHash(str);
            pos = (prevPos / 4) + (prevPos % 4 != 0 ? 1 : 0);
        }
        else if (prevPos % 4 == 3)
        {
            string h1 = mp[i - 1][prevPos - 3];
            string h2 = mp[i - 1][prevPos - 2];
            string h3 = hash;
            string h4 = "";
            if (prevPos + 1 <= mp[i - 1].size())
                h4 = mp[i - 1][prevPos];
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            reverse(h3.begin(), h3.end());
            reverse(h4.begin(), h4.end());
            string str = "";
            str += h4;
            str += h3;
            str += h2;
            str += h1;
            hash = findHash(str);
            pos = (prevPos / 4) + (prevPos % 4 != 0 ? 1 : 0);
        }
        else if (prevPos % 4 == 0)
        {
            string h1 = mp[i - 1][prevPos - 4];
            string h2 = mp[i - 1][prevPos - 3];
            string h3 = mp[i - 1][prevPos - 2];
            string h4 = hash;
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            reverse(h3.begin(), h3.end());
            reverse(h4.begin(), h4.end());
            string str = "";
            str += h4;
            str += h3;
            str += h2;
            str += h1;
            hash = findHash(str);
            pos = (prevPos / 4) + (prevPos % 4 != 0 ? 1 : 0);
        }
        i++;
    }
    if (hash == root)
    {
        return true;
    }
    return false;
}

double log3(int a, int b)
{

    double x = log2(a);

    double y = log2(b);

    double ans = double(double(x) / double(y));

    return ans;
}

void insertHybrid(string &t, vector<string> &trans, unordered_map<int, vector<string>> &mp, string &root)
{
    trans.push_back(t);
    int size = trans.size();
    string hash = findHash(t);
    int levels = ((ceil(log3(size, 3))) + 1);
    mp[1].push_back(hash);
    int i = 2;
    while (i <= levels)
    {
        int prevSz = mp[i - 1].size();
        int currSz = mp[i].size();
        int neededSz = (prevSz / 3) + (prevSz % 3 == 0 ? 0 : 1);
        string newVal = "";
        vector<string> v = mp[i - 1];
        int k = v.size() / 3 + (v.size() % 3 == 0 ? -1 : 0);
        int idx = k * 3; // [idx.......v.size()-1] all these hashes combine and form new parent hash
        int cnt = 0;
        for (int i = v.size() - 1; i >= idx; i--)
        {
            string s = v[i];
            reverse(s.begin(), s.end());
            newVal += s;
            cnt++;
        }
        string newHash;
        if (cnt == 1)
            newHash = hash;
        else
            newHash = findHash(newVal);
        if (currSz == neededSz) // replacement by newHash
            mp[i][mp[i].size() - 1] = newHash;
        else
        { // push new Hash
            mp[i].push_back(newHash);
        }
        i++;
    }
    // making new root
    if (mp[levels].size() == 1)
    {
        root = mp[levels][0];
    }
    else // level increase
    {
        string h1 = mp[levels][0];
        string h2 = mp[levels][1];
        reverse(h1.begin(), h1.end());
        reverse(h2.begin(), h2.end());
        string newHash = findHash(h2 + h1);
        root = newHash;
        mp[levels + 1].push_back(root);
    }
    return;
}

bool verifyHybrid(string t, int pos, vector<string> &trans, unordered_map<int, vector<string>> mp, string &root)
{
    string hash = findHash(t);
    int size = trans.size();
    int levels = ((ceil(log3(size, 3))) + 1);
    int i = 2;
    while (i <= levels)
    {
        int prevSz = mp[i - 1].size();
        int prevPos = pos;
        if (prevPos % 3 == 1) // 1st node of the group
    {
            string h1 = hash;
            string h2 = "", h3 = "";
            if (prevPos + 1 <= mp[i - 1].size())
                h2 = mp[i - 1][prevPos];
            if (prevPos + 2 <= mp[i - 1].size())
                h3 = mp[i - 1][prevPos + 1];
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            reverse(h3.begin(), h3.end());
            string str = "";
            str += h3;
            str += h2;
            str += h1;
            hash = findHash(str);
            pos = (prevPos / 3) + (prevPos % 3 != 0 ? 1 : 0);
        }
        else if (prevPos % 3 == 2) // 2nd node of the group
        {
            string h1 = mp[i - 1][prevPos - 2];
            string h2 = hash;
            string h3 = "";
            if (prevPos + 1 <= mp[i - 1].size())
                h3 = mp[i - 1][prevPos];
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            reverse(h3.begin(), h3.end());
            string str = "";
            str += h3;
            str += h2;
            str += h1;
            hash = findHash(str);
            pos = (prevPos / 3) + (prevPos % 3 != 0 ? 1 : 0);
        }
        else if (prevPos % 3 == 0) // 3rd node of the group
        {
            string h1 = mp[i - 1][prevPos - 3];
            string h2 = mp[i - 1][prevPos - 2];
            string h3 = hash;
            reverse(h1.begin(), h1.end());
            reverse(h2.begin(), h2.end());
            reverse(h3.begin(), h3.end());
            string str = "";
            str += h3;
            str += h2;
            str += h1;
            hash = findHash(str);
            pos = (prevPos / 3) + (prevPos % 3 != 0 ? 1 : 0);
        }
        i++;
    }
    if (hash == root)
    {
        return true;
    }
    return false;
}

int main()
{

    // string str=findHash("jfhcdkjvkjf");

    cout << "Select Tree Type :(smt/quad/hybrid)" << endl;

    string mytree;

    cin >> mytree;

    if (mytree == "smt")
    {

        vector<string> trans; // transactions or leaf nodes

        unordered_map<int, vector<string>> mp; // level => hash value

        string root; // root hash of SMT

        bool choice = true;

        while (choice)
        {

            cout << "What function you wants to perform in SMT ?" << endl;

            cout << "1. Insert transaction" << endl;

            cout << "2. Verify transaction" << endl;

            int op;

            cin >> op;

            if (op == 1)
            {

                ifstream in("input_insert_SMT.txt");

                string t;

                ofstream out("output_insert_SMT.txt");

                auto start = high_resolution_clock::now();

                while (in >> t)
                {

                    

                    insertSMT(t, trans, mp, root);

                }

            

                    auto stop = high_resolution_clock::now();

                    auto duration = duration_cast<microseconds>(stop - start);

                    out << "Time taken to insert transactions in SMT: " << duration.count() << " milliseconds"
                        << "\n";

                    cout << "Transactions inserted successfully !!!" << endl;

                    out << "Transactions added successfully!!!" << endl;
                    
                out << "Tree Is" << endl
                    << endl;

                for (int j = ceil(log2(trans.size())) + 1; j >= 1; j--)
                {
                    for (auto it : mp[j])
                    {
                        out << it << " ";
                    }
                    out << endl
                        << endl;
                }
            }

            // verifying transaction (SMT)

            else
            {

                ifstream in("input_verify_SMT.txt");

                string t;

                int pos;

                vector<bool> verify;

                auto start = high_resolution_clock::now();

                while (in >> t >> pos)
                {

                    bool flag = verifySMT(t, pos, trans, mp, root);

                    verify.push_back(flag);
                }

                auto stop = high_resolution_clock::now();

                auto duration = duration_cast<microseconds>(stop - start);

                ofstream out("output_verify_SMT.txt");

                cout << "Transactions verified successfully !!!" << endl
                     << endl;

                out << "Transactions verified successfully !!!" << endl
                    << endl;

                for (auto it : verify)
                {
                    if (it)
                    {
                        out << "Valid Transaction !!!" << endl;
                    }
                    else
                    {
                        out << "Invalid Transaction !!!" << endl;
                    }
                }

                out << "Time taken to Verify transactions in SMT: " << duration.count() << " milliseconds"
                    << "\n";
            }

            cout << "Do you wants to continue ? (Yes/No) : " << endl;

            string s;

            cin >> s;

            if (s == "No")
                choice = false;
        }
    }

    // Insert in Quad Tree

    else if (mytree == "quad")
    {

        vector<string> trans; // transactions or leaf nodes

        unordered_map<int, vector<string>> mp; // level => hash value

        string root; // root hash of Quad

        bool choice = true;

        while (choice)
        {

            cout << "What function you wants to perform in Quad ?" << endl;

            cout << "1. Insert transaction" << endl;

            cout << "2. Verify transaction" << endl;

            int op;

            cin >> op;

            if (op == 1)
            {

                ifstream in("input_insert_Quad.txt");

                string t;

                int x = 0;

                auto start = high_resolution_clock::now();

                while (in >> t)
                {

                    insertQuad(t, trans, mp, root);
                }

                auto stop = high_resolution_clock::now();

                auto duration = duration_cast<microseconds>(stop - start);

                ofstream out("output_insert_Quad.txt");

                cout << "Transactions inserted successfully !!!" << endl
                     << endl;

                out << "Transactions added successfully!!!" << endl;

                out << "Quad Tree :::" << endl
                    << endl;

                for (int j = ceil(log2(trans.size()) / 2) + 1; j >= 1; j--)
                {
                    for (auto it : mp[j])
                    {
                        out << it << " ";
                    }
                    out << endl
                        << endl;
                }

                out << "Time taken to insert transactions in Quad: " << duration.count() << " milliseconds"
                    << "\n";
            }

            // verifying transaction in Quad Tree

            else
            {

                //  cout<<trans.size();

                ifstream in("input_verify_Quad.txt");

                string t;

                int pos;

                vector<bool> verify;

                auto start = high_resolution_clock::now();

                while (in >> t >> pos)
                {

                    bool flag = verifyQuad(t, pos, trans, mp, root);


                    verify.push_back(flag);
                }

                auto stop = high_resolution_clock::now();

                auto duration = duration_cast<microseconds>(stop - start);

                ofstream out("output_verify_Quad1.txt");

                cout << "Transactions verified successfully !!!" << endl
                     << endl;

                out << "Transactions verified successfully !!!" << endl
                    << endl;

                for (auto it : verify)
                {
                    if (it)
                    {
                        out << "Valid Transaction !!!" << endl;
                    }
                    else
                    {
                        out << "Invalid Transaction !!!" << endl;
                    }
                }

                out << "Time taken to Verify transactions in Quad Tree: " << duration.count() << " milliseconds"
                    << "\n";
            }

            cout << "Do you wants to continue ? (Yes/No) : " << endl;

            string s;

            cin >> s;

            if (s == "No")
                choice = false;
        }
    }

    // mixed (smt + quad)

    else
    {

        vector<string> trans; // transactions or leaf nodes

        unordered_map<int, vector<string>> mp; // level => hash value

        string root; // root hash of mixed

        bool choice = true;

        while (choice)
        {

            cout << "What function you wants to perform in Hybrid Tree ?" << endl;

            cout << "1. Insert transaction" << endl;

            cout << "2. Verify transaction" << endl;

            int op;

            cin >> op;

            if (op == 1)
            {

                ifstream in("input_insert_hybrid.txt");

                string t;

                int x = 0;

                auto start = high_resolution_clock::now();

                while (in >> t)
                {

                    insertHybrid(t, trans, mp, root);
                }

                auto stop = high_resolution_clock::now();

                auto duration = duration_cast<microseconds>(stop - start);

                ofstream out("output_insert_hybrid.txt");

                cout << "Transactions inserted successfully !!!" << endl
                     << endl;

                out << "Transactions added successfully!!!" << endl;

                out << "Hybrid Tree :::" << endl
                    << endl;

                for (int j = ceil(log3(trans.size(), 3)) + 1; j >= 1; j--)
                {
                    for (auto it : mp[j])
                    {
                        out << it << " ";
                    }
                    out << endl
                        << endl;
                }

                out << "Time taken to insert transactions in Hybrid Tree: " << duration.count() << " milliseconds"
                    << "\n";
            }

            // verifying transaction in Hybrid Tree

            else
            {

                ifstream in("input_verify_hybrid.txt");

                string t;

                int pos;

                vector<bool> verify;

                auto start = high_resolution_clock::now();

                while (in >> t >> pos)
                {

                    bool flag = verifyHybrid(t, pos, trans, mp, root);

                    // cout << t << " " << pos << endl;

                    verify.push_back(flag);
                }

                auto stop = high_resolution_clock::now();

                auto duration = duration_cast<microseconds>(stop - start);

                ofstream out("output_verify_hybrid.txt");

                cout << "Transactions verified successfully !!!" << endl
                     << endl;

                out << "Transactions verified successfully !!!" << endl
                    << endl;

                for (auto it : verify)
                {
                    if (it)
                    {
                        out << "Valid Transaction !!!" << endl;
                    }
                    else
                    {
                        out << "Invalid Transaction !!!" << endl;
                    }
                }

                out << "Time taken to Verify transactions in Hybrid Tree: " << duration.count() << " milliseconds"
                    << "\n";
            }

            cout << "Do you wants to continue ? (Yes/No) : " << endl
                 << endl;

            string s;

            cin >> s;

            if (s == "No")
                choice = false;
        }
    }

    return 0;
}
