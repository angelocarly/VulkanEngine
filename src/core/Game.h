#ifndef VULKANENGINE_GAME_H
#define VULKANENGINE_GAME_H

#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "../vks/vks_input.h"

class Game
{

    struct Link;

    struct Node
    {
        int id;
        std::string name;
        std::list<Link> causes;
    };

    struct Link
    {
        int id;
        Node *a;
        Node *b;
        float weight;
    };

public:
    Game()
    {
//        std::map<int, Node> nodes = readFile();
    }

    std::map<int, Node> readFile()
    {
        std::map<int, Node> nodes;
//        std::map<int, Link> links;

        std::fstream infile("wow");
        std::string line;

        // Store all nodes
        while (getline(infile, line))
        {
            if (line.empty() || line.rfind('#', 0) == 0) continue;

            Node n;
            std::vector<std::string> data;
            boost::split(data, line, boost::is_any_of(","));
            n.id = std::stoi(data[0]);
            n.name = data[1];
            nodes[n.id] = Node{};

            // Store links
            std::vector<std::string> linkids;
            boost::split(linkids, data[3], boost::is_any_of("|"));
            for (std::string i : linkids)
            {
                if (i.empty()) continue;
                int lid = std::stoi(i);

                Link l;
                if (nodes.count(lid))
                {
                    l.a = &nodes.at(lid);
                } else
                {
                    nodes[lid] = Node{};
                    l.a = &nodes.at(lid);
                }

                l.b = &nodes.at(n.id);
                l.weight = 1;
                n.causes.push_back(l);
            }

            nodes[n.id] = n;
        }

        std::cout << nodes[5].causes.size() << std::endl;

        return nodes;
    }

private:
    VksInput inputHandler();
};

#endif //VULKANENGINE_GAME_H
