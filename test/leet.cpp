#include <iostream>

#include <vector>
#include <set>
#include <string>

using namespace std;

class Solution {
    public:
    int lengthOfLongestSubstring(string s) {
        int n = s.length();
        set<char> myset;
        int ans = 0, i = 0, j = 0;
        while (i < n && j < n) {
            // try to extend the range [i, j]
            if (myset.find(s[j]) == myset.end()){
                myset.insert(s[j++]);
                ans = max(ans, j - i);
            }
            else {
                myset.erase(s[i++]);
            }
        }
        return ans;
    }
};


int main() {
    vector<int> arr1 = vector<int>();
    arr1.push_back(3);
    arr1.push_back(2);
    arr1.push_back(4);

    cout << Solution().lengthOfLongestSubstring(string("pwwkew")) << endl;
    cout << Solution().lengthOfLongestSubstring(string("abcabcabc")) << endl;
    cout << Solution().lengthOfLongestSubstring(string("")) << endl;
    cout << Solution().lengthOfLongestSubstring(string(" ")) << endl;
}
