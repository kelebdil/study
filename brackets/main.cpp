    #include <iostream>
    #include <stack>
    #include <string>
    #include <deque>
    #include <map>

    std::deque<std::string> parse(const std::string &str)
    {
        std::map<std::size_t, std::deque<std::string>> map;
        std::stack<std::string::const_iterator> stack;
        for ( auto it = str.begin(); it != str.end();) {
            if (*it == '(') {
                stack.push(it++);
            } else if (*it == ')') {
                auto start = stack.top(); stack.pop();
                map[stack.size()].push_back(std::string{start, ++it});
            } else {
                it++;
            }
        }
        std::deque<std::string> result;
        for (const auto & p : map) {
            for (const auto & s : p.second) {
                result.push_back(s);
            }
        }
        return result;
    }

    int main(int , char **) {
        std::string input = "(if (a = b) (a) (b))";
        auto output = parse(input);
        for(const  auto & s:output) {
            std::cout << s << " ";
        }
        std::cout <<std::endl;
        return 0;
    }
