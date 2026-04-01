//
// Created by zykov on 3/27/2026.
//

#include "ObjImporter.h"
#include <fstream>
#include <iostream>
#include <string>
#include <strstream>

Mesh ObjImporter::Import() {
    Mesh mesh;

    std::vector<Vector3> verts{};
    std::vector<Vector3> normals{};

    if (std::ifstream file(fileName); file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;

            std::strstream s;
            s << line;

            char junk;

            if (line.substr(0, 1) == "v") {
                if (line.substr(0, 2) == "vn") {
                    Vector3 vn{};
                    std::string prefix;

                    s >> prefix >> vn.x >> vn.y >> vn.z;
                    normals.push_back(vn);
                } else if (line.substr(0, 2) == "vt") {
                    //
                } else {
                    Vector3 v{};
                    s >> junk >> v.x >> v.y >> v.z;
                    verts.push_back(v);
                }
            } else if (line.substr(0, 1) == "f") {
                int v_idx[3], vt_idx[3], vn_idx[3];
                char slash;

                s >> junk;

                // if file contains vt and vn else just taking v
                if (line.contains("/")) {
                    for (int i = 0; i < 3; i++) {
                        s >> v_idx[i] >> slash >> vt_idx[i] >> slash >> vn_idx[i];
                    }
                } else {
                    s >> v_idx[0] >> v_idx[1] >> v_idx[2];
                }

                std::vector<Vector3> vn{};

                // if file does not contain normal then calculate and add them
                if (normals.empty()) {
                    Vector3 line1 = verts[v_idx[1]-1] - verts[v_idx[0]-1];
                    Vector3 line2 = verts[v_idx[2]-1] - verts[v_idx[0]-1];
                    Vector3 v = Vector3::CrossProduct(line1, line2).Normalize();

                    vn.push_back(v);
                    vn.push_back(v);
                    vn.push_back(v);
                } else {
                    vn.push_back(normals[vn_idx[0]-1]);
                    vn.push_back(normals[vn_idx[1]-1]);
                    vn.push_back(normals[vn_idx[2]-1]);
                }

                Vertex vertex1 { verts[v_idx[0]-1], vn[0] };
                Vertex vertex2 { verts[v_idx[1]-1], vn[1] };
                Vertex vertex3 { verts[v_idx[2]-1], vn[2] };

                mesh.vertices.push_back(vertex1);
                mesh.vertices.push_back(vertex2);
                mesh.vertices.push_back(vertex3);

                Triangle triangle = {
                    vertex1,
                    vertex2,
                    vertex3
                    };
                mesh.tries.push_back(triangle);
            }
        }
        file.close();
    } else {
        std::cerr << "Failed open file" << std::endl;
    }

    return mesh;
}