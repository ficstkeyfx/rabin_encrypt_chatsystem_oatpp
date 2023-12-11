#ifndef RABIN
#define RABIN

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
using namespace std;

class RabinCryptosystem {
private:
    int p;
    int q;
    int n;
    vector<int> l;
    string s;

    int modulo(int a, int b) {
        return a >= 0 ? a % b : (b - abs(a % b)) % b;
    }

    int mod(int k, int b, int m) {
        int i = 0;
        int a = 1;
        std::vector<int> t;
        while (k > 0) {
            t.push_back(k % 2);
            k = (k - t[i]) / 2;
            i++;
        }
        for (int j = 0; j < i; j++) {
            if (t[j] == 1) {
                a = (a * b) % m;
                b = (b * b) % m;
            } else {
                b = (b * b) % m;
            }
        }
        return a;
    }

    std::vector<int> eea(int a, int b) {
        if (b > a) {
            int temp = a;
            a = b;
            b = temp;
        }
        int x = 0;
        int y = 1;
        int lastx = 1;
        int lasty = 0;
        while (b != 0) {
            int q = a / b;
            int temp1 = a % b;
            a = b;
            b = temp1;
            int temp2 = x;
            x = lastx - q * x;
            lastx = temp2;
            int temp3 = y;
            y = lasty - q * y;
            lasty = temp3;
        }
        std::vector<int> arr(3);
        arr[0] = lastx;
        arr[1] = lasty;
        arr[2] = 1;
        return arr;
    }

public:
    RabinCryptosystem(int prime1, int prime2, string plaintext) : p(prime1), q(prime2), s(plaintext) {
        n = p * q;
    }

    int encrypt(int m) {
        return (m * m) % n;
    }

    int decrypt(int c) {
        int mp = mod((p + 1) / 4, c, p);
        int mq = mod((q + 1) / 4, c, q);
        std::vector<int> arr = eea(p, q);
        int pp = arr[0] * p * mq;
        int qq = arr[1] * q * mp;
        double r = modulo((pp + qq), n);
        if (r < 128)
            return r;
        int negative_r = n - r;
        if (negative_r < 128)
            return negative_r;
        int s = modulo((pp - qq), n);
        if (s < 128)
            return s;
        int negative_s = n - s;
        if (negative_s < 128)
            return negative_s;
    }

    string encode() {
        std::vector<int> l_;
        for (char c : s) {
            l_.push_back((int)this->encrypt(c));
        }
        l = l_;
        std::ostringstream oss;
        std::copy(l.begin(), l.end(), std::ostream_iterator<int>(oss, ""));
        return oss.str();
    }

    string decode() {
        std::vector<char> l_;
        for (int i : l) {
            l_.push_back(char(this->decrypt(i)));
        }
        std::ostringstream oss;
        std::copy(l_.begin(), l_.end(), std::ostream_iterator<char>(oss, ""));
        return oss.str();
    }
};

#endif // RABIN


// int main() {
//     RabinCryptosystem cryptosystem(167, 151, "Rabin Cryptosystem");

//     std::string test = "Rabin Cryptosystem";
//     std::cout << "Message: " << test << std::endl;

//     string encryptedMessage = cryptosystem.encode();

//     std::cout << "Encryption: ";
//     cout << encryptedMessage;

//     std::cout << "\nDecoded message: ";
//     string result = cryptosystem.decode();

//     std::cout << result << std::endl;

//     return 0;
// }
