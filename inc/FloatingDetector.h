// FloatingDetector.h
#pragma once
#include <string>
#include <unordered_map>
#include <algorithm>

class FloatingBitPairDetector
{
public:
    void addExample(const std::string &expected, const std::string &received)
    {
        if (expected.size() != received.size())
            return;

        for (int i = 0; i < expected.size(); ++i)
        {
            if (expected[i] != received[i])
            {
                for (int j = 0; j < expected.size(); ++j)
                {
                    if (i != j && expected[j] == received[i])
                    {
                        // i 原本是 A，現在變成 B（B 是 j 的值）→ 推測 i 被 j 短路
                        pair_counter[keyOf(i, j)]++;
                    }
                }
            }
        }
    }
    void finalize()
    {
        has_pair = false;
        int max_count = 0;
        for (const auto &[key, count] : pair_counter)
        {
            if (count > max_count)
            {
                max_count = count;
                best_key = key;
                has_pair = true;
            }
        }
        if (has_pair)
        {
            auto dash = best_key.find('-');
            best_b1 = std::stoi(best_key.substr(0, dash));
            best_b2 = std::stoi(best_key.substr(dash + 1));
        }
    }

    bool found() const { return has_pair; }
    int bit1() const { return best_b1; }
    int bit2() const { return best_b2; }

private:
    std::string keyOf(int a, int b) const
    {
        return std::to_string(std::min(a, b)) + "-" + std::to_string(std::max(a, b));
    }

    std::unordered_map<std::string, int> pair_counter;
    std::string best_key;
    int best_b1 = -1, best_b2 = -1;
    bool has_pair = false;
};
