import os
from string import Template
import argparse


parser = argparse.ArgumentParser(
    description="Python codegen script for Islands game data")

parser.add_argument("source_dir", action="store", type=str)
parser.add_argument("output_file", action="store", type=str)
parser.add_argument("--deps", action="store", dest="deps", type=str)

args = parser.parse_args()


game_data_path: str = args.source_dir

game_data_dir = os.fsencode(game_data_path)

cwd = os.fsencode(os.getcwd())

source_dir_relative_to_cwd = os.path.relpath(game_data_dir, cwd)

directories_with_files: dict[str, list[str]] = dict()


def get_paths(dir):
    if dir not in directories_with_files:
        directories_with_files[dir] = []
    with os.scandir(dir) as it:
        for entry in it:
            if entry.is_file():
                directories_with_files[dir].append(os.path.basename(entry.path))
            elif entry.is_dir():
                get_paths(os.fsencode(entry.path))


get_paths(game_data_dir)


source: list[str] = []

for directory, dir_contents in directories_with_files.items():
    vars: str = ''
    for file_path in dir_contents:
        name = os.fsdecode(file_path.upper()).replace(
            "\\", "/").replace("/", "_").replace(".", "_")

        file_path_relative_to_gamedata = os.path.relpath(os.fsencode(
            os.path.join(directory, file_path)), os.fsencode(source_dir_relative_to_cwd))
        file_path_relative_to_gamedata = os.path.join(
            source_dir_relative_to_cwd, file_path_relative_to_gamedata)

        variable_def = """
            inline constexpr const char* {var_name} = "{var_value}";
        """.format(var_name=name, var_value=os.fsdecode(file_path_relative_to_gamedata).replace(
            "\\", "/"))
        vars = vars + variable_def

    base = os.path.relpath(directory, cwd)

    directory_path_var = """
            inline constexpr const char* __DIRECTORY_PATH = "{var_value}";
    """.format(var_value=os.fsdecode(base)).replace(
        "\\", "/")
    vars = vars + directory_path_var

    ns = os.fsdecode(base).replace(
        "\\", "/").replace("/", "::")
    namespace = """
        #ifndef {guard}_HPP
        #define {guard}_HPP
        namespace {ns} {{
            {variable_def}
        }}
        #endif
    """.format(guard=ns.replace("::", "_").upper(), ns=ns, variable_def=vars)
    source.append(namespace)

out_template = Template(
    """
    /* PYTHON AUTOGEN */
    namespace files {
        $source
    }
    """
)

source = ''.join([source for source in source])

autogen = out_template.substitute(source=source)

with open(args.output_file, "w") as out:
    out.write(autogen)
