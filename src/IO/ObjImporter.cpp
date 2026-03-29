//
// Created by zykov on 3/27/2026.
//

#include "ObjImporter.h"

#include <fstream>
#include <iostream>
#include <string>
#include <strstream>


Mesh ObjImporter::Import(const char *fileName) {
    Mesh mesh;

    std::vector<Vector3> verts{};

    if (std::ifstream file(fileName); file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;

            std::strstream s;
            s << line;

            char junk;

            if (line.substr(0, 2) == "v ") {
                Vector3 v{0, 0, 0};
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            } else if (line.substr(0, 2) == "f ") {
                int f[3];
                s >> junk >> f[0] >> f[1] >> f[2];

                Vector3 normal{}, line1{}, line2{};

                line1 = verts[f[1]-1] - verts[f[0]-1];
                line2 = verts[f[2]-1] - verts[f[0]-1];
                normal = Vector3::CrossProduct(line1, line2).Normalize();

                Triangle triangle = {
                    verts[f[0]-1],
                    normal,
                    verts[f[1]-1],
                    normal,
                    verts[f[2]-1],
                    normal,
                    };
                mesh.tries.push_back(triangle);
            }
        }
        file.close();
    } else {
        std::cerr << "Failed open file" << std::endl;
    }

    std::cout << "Mesh" << std::endl;

    for (int i = 0; i < mesh.tries.size(); i++) {
        std::cout << mesh.tries[i].vertices->position.x << " " << mesh.tries[i].vertices->position.y << " " << mesh.tries[i].vertices->position.z << std::endl;
    }

    return mesh;
}