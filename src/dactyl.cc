#include <cstring>
#include <filesystem>
#include <glm/glm.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "key.h"
#include "key_data.h"
#include "scad.h"
#include "transform.h"

#define IS_OPT(ARG, LONG, SHORT) (strcmp(LONG, ARG) == 0 || strcmp(SHORT, ARG) == 0)

using namespace scad;

bool generateTestKeys = false;
// Add the caps into the stl for testing.
bool addCaps = false;
bool addPCB = false;
bool addBattery = false;

enum class Direction { UP, DOWN, LEFT, RIGHT };

void AddShapes(std::vector<Shape>* shapes, std::vector<Shape> to_add) {
    for (Shape s : to_add) {
        shapes->push_back(s);
    }
}

Shape ConnectMainKeys(KeyData& d);

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (IS_OPT(argv[i], "--gen-test-keys", "-k")) {
            generateTestKeys = true;
        } else if (IS_OPT(argv[i], "--add-caps", "-c")) {
            addCaps = true;
        } else if (IS_OPT(argv[i], "--add-pcb", "-p")) {
            addPCB = true;
        } else if (IS_OPT(argv[i], "--add-battery", "-b")) {
            addBattery = true;
        } else if (IS_OPT(argv[i], "--help", "-h")) {
            std::cout << "Usage: dactyl [<options>]\n" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -k, --write-test-keys           Write test keys for debug purposes"
                      << std::endl;
            std::cout << "  -c, --add-caps                  Add caps for debug purposes"
                      << std::endl;
            std::cout << "  -h, --help                      Display help" << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown argument " << argv[i] << std::endl;
            return -1;
        }
    }

    // Create "output" directory
    try {
        std::filesystem::create_directories("output/scad");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating 'output' directory: " << e.what() << std::endl;
        return 1;
    }

    TransformList key_origin;
    key_origin.Translate(-20, -40, 3);

    // This is where all of the logic to position the keys is done. Everything
    // below is cosmetic trying to build the case.
    KeyData d(key_origin);

    if (generateTestKeys) {
        std::cout << "Generating test keys..." << std::endl;
        std::vector<Shape> test_shapes;
        std::vector<Key*> test_keys = {&d.key_3, &d.key_e, &d.key_4, &d.key_5, &d.key_d};
        for (Key* key : test_keys) {
            key->add_side_nubs = false;
            key->extra_z = 4;
            test_shapes.push_back(key->GetSwitch());
            if (addCaps) {
                test_shapes.push_back(key->GetCap().Color("red"));
            }
        }
        UnionAll(test_shapes).WriteToFile("output/scad/test_keys.scad");
        std::cout << "Done." << std::endl;
    }

    std::cout << "Generating cases..." << std::endl;

    // Set all of the widths here. This must be done before calling any of
    // GetTopLeft etc.

    d.key_backspace.extra_width_bottom = 11;
    d.key_backspace.extra_width_left = 3;
    d.key_delete.extra_width_bottom = 11;
    d.key_end.extra_width_bottom = 3;
    d.key_ctrl.extra_width_top = 3;
    d.key_alt.extra_width_top = 3;
    d.key_alt.extra_width_right = 3;
    d.key_alt.extra_width_left = 3;
    d.key_home.extra_width_right = 3;
    d.key_home.extra_width_left = 3;
    d.key_home.extra_width_top = 3;
    d.key_end.extra_width_top = 3;
    d.key_end.extra_width_right = 3;
    d.key_end.extra_width_left = 3;

    // left wall
    for (Key* key : d.grid.column(0)) {
        if (key) {
            key->extra_width_left = 4;
        }
    }

    d.key_5.extra_width_right = 4;
    d.key_t.extra_width_right = 4;
    d.key_g.extra_width_right = 4;

    for (Key* key : d.grid.row(0)) {
        // top row
        if (key) {
            key->extra_width_top = 2;
        }
    }
    d.key_b.extra_width_bottom = 3;

    std::vector<Shape> shapes;

    //
    // Thumb plate
    //

    shapes.push_back(Union(ConnectHorizontal(d.key_ctrl, d.key_alt),
                           ConnectHorizontal(d.key_backspace, d.key_delete),
                           ConnectVertical(d.key_ctrl, d.key_delete),
                           Tri(d.key_end.GetBottomLeft(),
                               d.key_delete.GetBottomRight(),
                               d.key_backspace.GetBottomLeft())));

    shapes.push_back(ConnectMainKeys(d));

    shapes.push_back(TriFan(d.key_ctrl.GetTopLeft(),
                            {
                                d.key_b.GetBottomRight(),
                                d.key_b.GetTopRight(),
                                d.key_g.GetBottomRight(),
                            })

    );

    // These transforms with TranslateFront are moving the connectors down in
    // the z direction to reduce the vertical jumps.
    TransformList v_bottom_right = d.key_z.GetBottomLeft().TranslateFront(0, -2, -3);

    // shapes.push_back(TriFan(v_bottom_right,
    //                         {
    //                             d.key_x.GetBottomRight().TranslateFront(0, 0, -1),
    //                             d.key_x.GetBottomLeft(),
    //                             d.key_z.GetBottomRight().TranslateFront(0, 0, -1),
    //                         }));
    // shapes.push_back(TriFan(d.key_backspace.GetBottomLeft(),
    //                         {
    //                             v_bottom_right,
    //                             d.key_left_arrow.GetBottomRight().TranslateFront(0, 0, -1),
    //                             d.key_right_arrow.GetBottomLeft().TranslateFront(0, 0, -1),
    //                             d.key_right_arrow.GetBottomRight(),
    //                         }));
    // shapes.push_back(TriFan(d.key_tilde.GetBottomRight(),
    //                         {
    //                             d.key_slash.GetBottomLeft(),
    //                             d.key_slash.GetBottomRight().TranslateFront(0, 0, -1),
    //                             v_bottom_right,
    //                         }));
    shapes.push_back(TriFan(d.key_delete.GetTopLeft(),
                            {
                                d.key_ctrl.GetTopLeft(),
                                d.key_b.GetBottomRight(),
                                d.key_backspace.GetTopLeft(),
                            }));
    shapes.push_back(TriFan(d.key_b.GetBottomLeft(),
                            {
                                d.key_b.GetBottomRight(),
                                d.key_backspace.GetTopLeft(),
                                d.key_backspace.GetTopLeft(),
                                d.key_v.GetBottomRight(),
                                d.key_v.GetTopRight(),
                                d.key_v.GetBottomRight(),
                            }));

    // Bottom right corner.
    shapes.push_back(TriFan(d.key_shift.GetBottomRight(),
                            {
                                d.key_z.GetBottomLeft(),
                                v_bottom_right,
                                d.key_shift.GetBottomLeft(),
                            }));

    shapes.push_back(TriFan(d.key_z.GetBottomRight().TranslateFront(0, 0, -2),
                            {
                                d.key_backspace.GetBottomLeft(),
                                v_bottom_right,
                                d.key_z.GetBottomLeft(),
                            }));
    shapes.push_back(TriFan(d.key_z.GetBottomRight().TranslateFront(0, 0, -2),
                            {
                                d.key_x.GetBottomLeft(),
                                d.key_backspace.GetBottomLeft(),
                            }));
    shapes.push_back(TriFan(d.key_x.GetBottomLeft(),
                            {
                                d.key_x.GetBottomRight(),
                                d.key_c.GetBottomLeft(),
                                d.key_backspace.GetBottomLeft(),
                            }));
    shapes.push_back(TriFan(d.key_c.GetBottomLeft(),
                            {
                                d.key_c.GetBottomRight(),
                                d.key_backspace.GetBottomLeft(),
                            }));
    shapes.push_back(TriFan(d.key_c.GetBottomRight(),
                            {
                                d.key_v.GetBottomLeft(),
                                d.key_v.GetBottomRight(),
                                d.key_backspace.GetBottomLeft(),
                            }));
    shapes.push_back(TriFan(d.key_v.GetBottomRight(),
                            {
                                d.key_backspace.GetTopLeft(),
                                d.key_backspace.GetBottomLeft(),
                            }));

    // Connecting top wall to keys
    TransformList key_plus_top_right_wall = d.key_plus.GetTopRight().TranslateFront(0, 3, -3);
    TransformList key_2_top_left_wall = d.key_2.GetTopLeft().TranslateFront(0, 3.75, 0);
    TransformList key_2_top_right_wall = d.key_2.GetTopRight().TranslateFront(0, 4, -1);
    TransformList key_3_top_right_wall = d.key_3.GetTopRight().TranslateFront(0, 3.5, 0);
    TransformList key_4_top_right_wall = d.key_4.GetTopRight().TranslateFront(0, 2.2, 0);

    shapes.push_back(TriFan(key_4_top_right_wall,
                            {
                                d.key_5.GetTopRight(),
                                d.key_5.GetTopLeft(),
                                d.key_4.GetTopRight(),
                                d.key_4.GetTopLeft(),
                            }));
    shapes.push_back(TriFan(key_3_top_right_wall,
                            {
                                key_4_top_right_wall,
                                d.key_4.GetTopLeft(),
                                d.key_3.GetTopRight(),
                                d.key_3.GetTopLeft(),
                                key_2_top_right_wall,
                            }));
    shapes.push_back(TriFan(key_2_top_right_wall,
                            {
                                key_2_top_left_wall,
                                d.key_2.GetTopRight(),
                                d.key_3.GetTopLeft(),
                            }));
    shapes.push_back(TriFan(key_2_top_left_wall,
                            {
                                d.key_1.GetTopRight(),
                                d.key_2.GetTopLeft(),
                                d.key_2.GetTopRight(),
                            }));
    shapes.push_back(TriFan(d.key_plus.GetTopRight(),
                            {
                                d.key_1.GetTopLeft(),
                                d.key_1.GetTopRight(),
                                key_2_top_left_wall,
                            }));
    shapes.push_back(TriFan(key_plus_top_right_wall,
                            {
                                key_2_top_left_wall,
                                d.key_plus.GetTopRight(),
                                d.key_plus.GetTopLeft(),
                            }));
    //
    // Make the wall
    //
    {
        struct WallPoint {
            WallPoint(TransformList transforms,
                      Direction out_direction,
                      float extra_distance = 0,
                      float extra_width = 0)
                : transforms(transforms),
                  out_direction(out_direction),
                  extra_distance(extra_distance),
                  extra_width(extra_width) {
            }
            TransformList transforms;
            Direction out_direction;
            float extra_distance;
            float extra_width;
        };

        Direction up = Direction::UP;
        Direction down = Direction::DOWN;
        Direction left = Direction::LEFT;
        Direction right = Direction::RIGHT;

        std::vector<WallPoint> wall_points = {
            // Start top left and go clockwise
            {d.key_plus.GetTopLeft(), up},
            {key_plus_top_right_wall, up, 0, .3},

            {key_2_top_left_wall, up, 0, .3},
            {key_2_top_right_wall, up},

            //{d.key_3.GetTopLeft(), up},
            {key_3_top_right_wall, up},

            // {d.key_4.GetTopLeft(), up},
            {key_4_top_right_wall, up},
            {d.key_5.GetTopRight(), up},
            {d.key_5.GetTopRight(), right},
            {d.key_5.GetBottomRight(), right},

            {d.key_t.GetTopRight(), right},
            {d.key_t.GetBottomRight(), right},

            {d.key_g.GetTopRight(), right},
            {d.key_g.GetBottomRight(), right, 1, .5},

            {d.key_ctrl.GetTopLeft().RotateFront(0, 0, -15), up, 1, .5},
            {d.key_ctrl.GetTopRight(), up},

            {d.key_alt.GetTopLeft(), up},
            {d.key_alt.GetTopRight(), up, 0, .5},
            {d.key_alt.GetTopRight(), right, 0, .5},
            {d.key_alt.GetBottomRight(), right},

            {d.key_home.GetTopRight(), right},
            {d.key_home.GetBottomRight(), right},

            {d.key_end.GetTopRight(), right},
            {d.key_end.GetBottomRight(), right, 0, .5},
            {d.key_end.GetBottomRight(), down, 0, .5},
            {d.key_end.GetBottomLeft(), down},

            {d.key_backspace.GetBottomLeft(), down},

            {v_bottom_right, down},

            // {d.key_tilde.GetBottomRight(), down},
            // {d.key_tilde.GetBottomLeft(), down},

            {d.key_shift.GetBottomLeft(), down, 0, .75},
            {d.key_shift.GetBottomLeft(), left, 0, .5},
            {d.key_shift.GetTopLeft(), left, 0, .5},

            {d.key_caps.GetBottomLeft(), left},
            {d.key_caps.GetTopLeft(), left},

            {d.key_tab.GetBottomLeft(), left},
            {d.key_tab.GetTopLeft(), left},

            {d.key_plus.GetBottomLeft(), left},
            {d.key_plus.GetTopLeft(), left},
        };

        std::vector<std::vector<Shape>> wall_slices;
        for (WallPoint point : wall_points) {
            Shape s1 = point.transforms.Apply(GetPostConnector());

            TransformList t = point.transforms;
            float distance = 4.8 + point.extra_distance;
            switch (point.out_direction) {
                case Direction::UP:
                    t.AppendFront(TransformList().Translate(0, distance, 0).RotateX(-20));
                    break;
                case Direction::DOWN:
                    t.AppendFront(TransformList().Translate(0, -1 * distance, 0).RotateX(20));
                    break;
                case Direction::LEFT:
                    t.AppendFront(TransformList().Translate(-1 * distance, 0, 0).RotateY(-20));
                    break;
                case Direction::RIGHT:
                    t.AppendFront(TransformList().Translate(distance, 0, 0).RotateY(20));
                    break;
            }

            // Make sure the section extruded to the bottom is thick enough.
            // With certain angles the projection is very small if you just use
            // the post connector from the transform. Compute an explicit shape.
            const glm::vec3 post_offset(0, 0, -4);
            const glm::vec3 p = point.transforms.Apply(post_offset);
            const glm::vec3 p2 = t.Apply(post_offset);

            glm::vec3 out_v = p2 - p;
            out_v.z = 0;
            const glm::vec3 in_v = -1.f * glm::normalize(out_v);

            float width = 3.3 + point.extra_width;
            Shape s2 = Hull(Cube(.1).Translate(p2), Cube(.1).Translate(p2 + (width * in_v)));

            std::vector<Shape> slice;
            slice.push_back(Hull(s1, s2));
            slice.push_back(Hull(s2, s2.Projection().LinearExtrude(.1).TranslateZ(.05)));

            wall_slices.push_back(slice);
        }

        for (size_t i = 0; i < wall_slices.size(); ++i) {
            auto& slice = wall_slices[i];
            auto& next_slice = wall_slices[(i + 1) % wall_slices.size()];
            for (size_t j = 0; j < slice.size(); ++j) {
                shapes.push_back(Hull(slice[j], next_slice[j]));
                // Uncomment for testing. Much faster and easier to visualize.
                // shapes.push_back(slice[j]);
            }
        }
    }

    for (Key* key : d.all_keys()) {
        shapes.push_back(key->GetSwitch());
        if (addCaps) {
            shapes.push_back(key->GetCap().Color("red"));
        }
    }

    // Add all the screw inserts.
    std::vector<Shape> screw_holes;
    double screw_height = 6;
    double screw_radius = 4.55 / 2.0;
    Shape screw_hole = Cylinder(screw_height + 2, screw_radius, 30);
    Shape screw_insert =
        Cylinder(screw_height, screw_radius + 1.65, 30).TranslateZ(screw_height / 2);

    glm::vec3 screw_left_bottom = d.key_shift.GetBottomLeft().Apply(kOrigin);
    screw_left_bottom.z = 0;
    screw_left_bottom.x += 3.2;

    glm::vec3 screw_left_top = d.key_plus.GetTopLeft().Apply(kOrigin);
    screw_left_top.z = 0;
    screw_left_top.x += 2.8;
    screw_left_top.y += -.5;

    glm::vec3 screw_right_bottom = d.key_end.GetBottomRight().Apply(kOrigin);
    screw_right_bottom.z = 0;
    screw_right_bottom.y += 5.5;
    screw_right_bottom.x -= 2.5;

    glm::vec3 screw_right_mid_mid = d.key_ctrl.GetTopLeft().Apply(kOrigin);
    screw_right_mid_mid.z = 0;
    screw_right_mid_mid.x -= 3.5;
    screw_right_mid_mid.y += 5;

    glm::vec3 screw_right_mid = d.key_alt.GetTopRight().Apply(kOrigin);
    screw_right_mid.z = 0;
    screw_right_mid.x -= 6;
    screw_right_mid.y -= 0;

    glm::vec3 screw_right_top = d.key_4.GetTopLeft().Apply(kOrigin);
    screw_right_top.z = 0;
    screw_right_top.x += 8;
    screw_right_top.y += 3;

    glm::vec3 screw_mid_bottom = d.key_x.GetBottomRight().Apply(kOrigin);
    screw_mid_bottom.z = 0;
    screw_mid_bottom.x += 0;
    screw_mid_bottom.y -= 28;

    shapes.push_back(Union(screw_insert.Translate(screw_left_top),
                           screw_insert.Translate(screw_right_top),
                           screw_insert.Translate(screw_right_mid_mid),
                           screw_insert.Translate(screw_right_mid),
                           screw_insert.Translate(screw_right_bottom),
                           screw_insert.Translate(screw_mid_bottom),
                           screw_insert.Translate(screw_left_bottom)));
    screw_holes = {
        screw_hole.Translate(screw_left_top),
        screw_hole.Translate(screw_right_top),
        screw_hole.Translate(screw_right_mid_mid),
        screw_hole.Translate(screw_right_mid),
        screw_hole.Translate(screw_right_bottom),
        screw_hole.Translate(screw_mid_bottom),
        screw_hole.Translate(screw_left_bottom),
    };

    std::vector<Shape> negative_shapes;
    AddShapes(&negative_shapes, screw_holes);
    // Cut off the parts sticking up into the thumb plate.
    // negative_shapes.push_back(
    //     d.key_backspace.GetTopLeft().Apply(Cube(50, 50, 6).TranslateZ(3)).Color("red"));

    // Cut out hole for type-c
    Shape cylinderHole = Cylinder(20, 1.5, 30).RotateX(90);

    float type_c_cylinder_hole_offset_x = 3.1;
    float type_c_cylinder_hole_offset_z = 0.2;
    Shape type_c_hole = Hull(cylinderHole.TranslateX(-type_c_cylinder_hole_offset_x)
                                 .TranslateZ(-type_c_cylinder_hole_offset_z),
                             cylinderHole.TranslateX(type_c_cylinder_hole_offset_x)
                                 .TranslateZ(-type_c_cylinder_hole_offset_z),
                             cylinderHole.TranslateX(-type_c_cylinder_hole_offset_x)
                                 .TranslateZ(type_c_cylinder_hole_offset_z),
                             cylinderHole.TranslateX(type_c_cylinder_hole_offset_x)
                                 .TranslateZ(type_c_cylinder_hole_offset_z))
                            .RotateZ(-8.7)
                            .TranslateX(1.9);
    glm::vec3 type_c_location = d.key_4.GetTopLeft().Apply(kOrigin);
    type_c_location.z = 4.8;
    type_c_location.x += 25;
    negative_shapes.push_back(type_c_hole.Translate(type_c_location));

    Shape buttonHole = Union(Cylinder(10, 2.5, 30), Cube(2, 10, 5))
                           .RotateY(90)
                           .RotateZ(14.175295)
                           .Translate(type_c_location)
                           .TranslateX(16)
                           .TranslateY(-11.3);
    Shape button = Union(Cylinder(10, 2.3, 30), Cube(1.8, 9.8, 4.8))
                       .RotateY(90)
                       .RotateZ(14.175295)
                       .Translate(type_c_location)
                       .TranslateX(15)
                       .TranslateY(-5);
    negative_shapes.push_back(buttonHole);

    Shape led_hole = Cylinder(10, 0.7, 30)
                         .RotateY(90)
                         .RotateZ(14.175295)
                         .Translate(type_c_location)
                         .TranslateX(16)
                         .TranslateY(-4)
                         .TranslateZ(-0.8);
    negative_shapes.push_back(led_hole);

    Shape result = UnionAll(shapes);
    // Subtracting is expensive to preview and is best to disable while testing.
    result = result.Subtract(UnionAll(negative_shapes));

    if (addPCB) {
        Shape pcb = Import("../../extra/pcb.stl", 10)
                        .Color("gray")
                        .Translate(31.2, 37.8, 2.1)
                        .RotateZ(-8.7);
        result = Union(result, pcb);
    }

    if (addBattery) {
        Shape battery =
            Cube(55, 65, 10).RotateZ(61.82389562).TranslateZ(5).TranslateX(42).TranslateY(-40);
        result = Union(result, battery.Color("orange"));
    }

    glm::vec3 boardScrewMountLocation = d.key_5.GetTopRight().Apply(kOrigin);
    boardScrewMountLocation.z = 9.55;
    boardScrewMountLocation.x -= 2.7;
    boardScrewMountLocation.y -= 0.42;
    Shape boardScrewMount = Union(Cylinder(13, 2.8, 30), Cube(5.6, 5, 13).TranslateY(2.5))
                                .RotateZ(-11)
                                .Subtract(Cylinder(8, 2.025, 30).TranslateZ(-6.5));
    Shape boardScrewMount1 = boardScrewMount.Translate(boardScrewMountLocation);

    boardScrewMountLocation.x -= 15.93;
    boardScrewMountLocation.y += 2.45;
    Shape boardScrewMount2 = boardScrewMount.Translate(boardScrewMountLocation);

    boardScrewMountLocation = d.key_g.GetBottomRight().Apply(kOrigin);
    boardScrewMountLocation.z = 9.55;
    boardScrewMountLocation.x -= 2.58;
    boardScrewMountLocation.y += 5.19;
    Shape boardScrewMount3 = boardScrewMount.RotateZ(-80).Translate(boardScrewMountLocation);

    result = Union(result, boardScrewMount1, boardScrewMount2, boardScrewMount3);

    std::vector<Shape> bottom_plate_shapes = {result};
    for (Key* key : d.all_keys()) {
        bottom_plate_shapes.push_back(Hull(key->GetSwitch()));
    }

    Shape bottom_plate =
        UnionAll(bottom_plate_shapes).Projection().LinearExtrude(2).Subtract(UnionAll(screw_holes));
    Shape screw_head_hole = Cylinder({
        .h = 1.75,
        .r1 = 2.9,
        .r2 = 2.6,
        .fn = 30,
    });

    Shape rubber_feet_hole = Cylinder(0.3, 2.5, 30);

    // screw_left_bottom.z = -0.9;
    // bottom_plate = bottom_plate.Subtract(rubber_feet_hole.Translate(screw_left_bottom));
    bottom_plate =
        bottom_plate.Subtract(screw_head_hole.Translate(screw_left_bottom).TranslateZ(-0.25 / 2));

    result.WriteToFile("output/scad/left.scad");
    result.MirrorX().WriteToFile("output/scad/right.scad");

    std::cout << "Done." << std::endl;

    // Bottom plate
    {
        std::cout << "Generating bottom plates..." << std::endl;
        bottom_plate.WriteToFile("output/scad/bottom_left.scad");
        bottom_plate.MirrorX().WriteToFile("output/scad/bottom_right.scad");
        std::cout << "Done." << std::endl;
    }

    return 0;
}

Shape ConnectMainKeys(KeyData& d) {
    std::vector<Shape> shapes;
    for (size_t r = 0; r < d.grid.num_rows(); ++r) {
        for (size_t c = 0; c < d.grid.num_columns(); ++c) {
            Key* key = d.grid.get_key(r, c);
            if (!key) {
                // No key at this location.
                continue;
            }
            Key* left = d.grid.get_key(r, c - 1);
            Key* top_left = d.grid.get_key(r - 1, c - 1);
            Key* top = d.grid.get_key(r - 1, c);

            if (left) {
                shapes.push_back(ConnectHorizontal(*left, *key));
            }
            if (top) {
                shapes.push_back(ConnectVertical(*top, *key));
                if (left && top_left) {
                    shapes.push_back(ConnectDiagonal(*top_left, *top, *key, *left));
                }
            }
        }
    }
    return UnionAll(shapes);
}
