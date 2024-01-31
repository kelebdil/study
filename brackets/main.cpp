#include <deque>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>

std::deque<std::string> parse(std::string const & str) {
    std::map<std::size_t, std::deque<std::string>> map;
    std::stack<std::string::const_iterator> stack;
    for (auto it = str.begin(); it != str.end();) {
        if (*it == '(') {
            stack.push(it++);
        } else if (*it == ')') {
            if (stack.empty()) {
                std::stringstream ss;
                ss << "Unexpected ')' in string '" << str << "' at position "
                   << std::distance(str.begin(), it);
                throw std::logic_error(ss.str());
            }
            auto start = stack.top();
            stack.pop();
            map[stack.size()].push_back(std::string{ start, ++it });
        } else {
            it++;
        }
    }
    std::deque<std::string> result;
    for (auto const & p : map) {
        for (auto const & s : p.second) {
            result.push_back(s);
        }
    }
    return result;
}

int main(int, char **) {
    std::string input = "(if (a = b) (a) (b))";
    auto output = parse(input);
    for (auto const & s : output) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    return 0;
}
