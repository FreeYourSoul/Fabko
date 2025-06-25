// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <fil/cli/command_line_interface.hh>

int main(int argc, char** argv) {

    fil::sub_command command_1 {
        "test", //
        []() { fmt::print("Command 1 executed\n");              },
        "Lorem ipsum dolor sit amet consectetur adipiscing elit. Quisque faucibus ex sapien vitae pellentesque sem placerat. In id cursus mi pretium tellus duis convallis. Tempus "
        "leo eu aenean sed diam urna tempor. Pulvinar quentin\nvivamus fringilla lacus nec metus bibendum egestas. Iaculis massa nisl malesuada lacinia integer nunc posuere. Ut "
        "hendrerit "
        "semper vel class aptent taciti sociosqu. Ad litora torquent per conubia nostra inceptos himenaeos.",
        {},
        {
              fil::option {"--option1",
                "-o1",
                []() { fmt::print("Option 1 executed\n"); },
                "This is option 1 "
                "Lorem ipsum dolor sit amet consectetur adipiscing elit. Quisque faucibus ex sapien vitae pellentesque sem placerat. In id cursus mi pretium tellus duis "
                "convallis. Tempus leo eu aenean sed diam urna tempor. Pulvinar vivamus fringilla lacus nec metus bibendum egestas. Iaculis massa nisl malesuada lacinia integer "
                "nunc posuere. Ut hendrerit semper vel class aptent taciti sociosqu. Ad litora torquent per conubia nostra inceptos himenaeos. Lorem ipsum dolor sit amet "
                "consectetur adipiscing elit. Quisque faucibus ex sapien vitae pellentesque sem placerat. In id cursus mi pretium tellus duis convallis. Tempus leo eu aenean sed "
                "diam urna tempor. Pulvinar vivamus fringilla lacus nec metus bibendum egestas. Iaculis massa nisl malesuada lacinia integer nunc posuere. Ut hendrerit semper vel "
                "class aptent taciti sociosqu. qfqfqwfqwfqwfqwfqwfqwfqwfqfqwf\n---Ad litora torquent quentin\n lol\nper conubia nostra inceptos himenaeos. Lorem ipsum dolor sit "
                "amet consectetur "
                "adipiscing elit. Quisque ex sapien vitae pellentesque sem placerat. In id cursus mi pretium tellus duis convallis. Tempus leo eu aenean sed diam urna tempor. "
                "Pulvinar vivamus fringilla "
                "lacus nec metus bibendum egestas. Iaculis massa nisl malesuada lacinia integer nunc posuere. Ut hendrerit semper vel class aptent taciti sociosqu. Ad litora "
                "torquent per conubia nostra inceptos himenaeos. Lorem ipsum dolor sit amet consectetur adipiscing elit. Quisque faucibus ex sapien vitae pellentesque sem "
                "placerat. In id cursus mi pretium tellus duis convallis. Tempus leo eu aenean sed diam urna tempor. Pulvinar vivamus fringilla lacus nec metus bibendum egestas. "
                "Iaculis massa nisl malesuada lacinia integer nunc posuere. Ut hendrerit semper vel class aptent taciti sociosqu. Ad litora torquent per conubia nostra inceptos "
                "himenaeos."},
              fil::option {"--option2", "-o2", []() { fmt::print("Option 2 executed\n"); }, "This is option 2"},
              }
    };

    fil::command_line_interface cli({command_1}, [] {});

    if (cli.parse_command_line(argc, argv)) {
        fmt::print("Command executed successfully\n");
    } else {
        fmt::print("No command executed\n");
    }

    return 0;
}
